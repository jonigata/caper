// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.


// $Id$

#if !defined(ZW_FASTLALR_HPP)
#define ZW_FASTLALR_HPP

// module: LALR
//   LALR(1)�\�̍쐬(������)

#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <functional>
#include "grammar.hpp"
#include "lr.hpp"

//#define ZW_PARSER_LIVECAST

namespace zw {

namespace gr {

/*============================================================================
 *
 * ��O
 *
 * 
 *
 *==========================================================================*/

class syntax_error : public std::exception {};

class unconnected_rule_base : public std::exception {};

template <class Token, class Traits>
class unconnected_rule : public unconnected_rule_base {
public:
    unconnected_rule(std::set<rule<Token, Traits>>& remains) {
        std::stringstream ss;
        ss << "unconnected rules: ";
        bool first = true;
        for (const auto& x: remains) {
            if (first) { first = false; } else { ss << ", "; }
            ss << x;
        }
        message = ss.str();
    }

    const char* what() const throw() { return message.c_str(); }

    std::string message;
};

/*============================================================================
 *
 * check_reachable
 *
 * �ڑ��`�F�b�N
 *
 *==========================================================================*/
template <class Token, class Traits>
void check_reachable(const grammar<Token, Traits>& g) {
    typedef symbol<Token, Traits>         symbol_type; 
    typedef rule<Token, Traits>           rule_type; 
    typedef symbol_set<Token, Traits>     symbol_set_type;

    symbol_set_type symbols;

    typedef std::set<rule_type> remains_type;
    remains_type remains;
    for (const rule_type& rule: g) {
        remains.insert(rule);
    }
    symbols.insert(symbol_type(g.root_rule().left()));

    bool iterate = true;
    while (iterate) {
        iterate = false;

        typedef std::vector<typename std::set<rule_type>::iterator> erased_type;
        erased_type erased;

        for (auto i = remains.begin(); i != remains.end(); ++i) {
            const rule_type& rule = *i;

            if (symbols.find(symbol_type(rule.left())) != symbols.end()) {
                symbols.insert(rule.right().begin(), rule.right().end());
                erased.push_back(i);
                iterate = true;
            }
        }

        for (const auto& x: erased) {
            remains.erase(x);
        }
    }

    if (!remains.empty()) {
        throw unconnected_rule<Token, Traits>(remains);
    }
}

/*============================================================================
 *
 * make_lalr_table
 *
 * LALR(1)�\�̍쐬
 *
 *==========================================================================*/
template <class Token, class Traits, class SRReporter, class RRReporter>
void 
make_lalr_table(
    parsing_table<Token, Traits>&   table,
    const grammar<Token, Traits>&   g,
    Token                           error_token,
    SRReporter                      srr,
    RRReporter                      rrr) {
    typedef symbol<Token, Traits>                       symbol_type; 
    typedef terminal<Token, Traits>                     terminal_type; 
    typedef rule<Token, Traits>                         rule_type; 
    typedef lr0_collection<Token, Traits>               lr0_collection_type; 
    typedef symbol_set<Token, Traits>                   symbol_set_type; 
    typedef terminal_set<Token, Traits>                 terminal_set_type; 
    typedef nonterminal_set<Token, Traits>              nonterminal_set_type; 
    typedef core<Token, Traits>                         core_type; 
    typedef item<Token, Traits>                         item_type; 
    typedef item_set<Token, Traits>                     item_set_type; 
    typedef core_set<Token, Traits>                     core_set_type; 
    typedef parsing_table<Token, Traits>                parsing_table_type;
    typedef typename parsing_table_type::state          state_type;
    typedef typename parsing_table_type::states_type    states_type;
    typedef typename parsing_table_type::action         action_type;
    typedef typename state_type::propagate_type         propagate_type;

    // �L���̎��W
    terminal_set_type terminals;    
    nonterminal_set_type nonterminals;    
    symbol_set_type all_symbols;
    collect_symbols(terminals, nonterminals, all_symbols, g);

    terminal_type dummy("#", Token(-1));
    terminals.insert(dummy);
    all_symbols.insert(dummy);

    terminal_type eof("$", Traits::eof());
    terminals.insert(eof);
    all_symbols.insert(eof);
        
    // �ڑ��`�F�b�N
    check_reachable(g);

    // FIRST, FOLLOW�̍쐬
    first_collection<Token, Traits> first;
    make_first(first, terminals, g);

    follow_collection<Token, Traits> follow;
    make_follow(follow, first, g, eof);

    // �\�̍쐬
    table.set_grammar(g);

    // p.271

    // 1. Construct the kernels of the sets of LR(0) items for G.
    // If space is not a premium, the simplest way is to construct
    // the LR(0) sets of items, as in Section 4.6.2, and then
    // remove the nonkernel items.  If space is serverely
    // constrained, we may wish instead to store only the kernel
    // items for each set, and compute GOTO for a set of items I
    // by first computing the closure of I.

    // simplest way�̂ق�
    lr0_collection_type I;
    make_lr0_collection(I, g);

    // states
    states_type& states = table.states();

    // kernels: I(kernel)��state�̍���
    typedef std::map<core_set_type, int> kernels_type;
    kernels_type kernels;

    // state, kernels�����
    core_type root_core(g.root_rule(), 0);
    for (const auto& i: I) {
        // �V�������
        state_type& s = table.add_state();
        s.no = int(table.states().size()- 1);
        s.cores = i;
        choose_kernel(s.kernel, s.cores, g);
        kernels[s.kernel] = s.no;

        if (s.kernel.count(root_core)) {
            table.first_state(s.no);
            s.generate_map[root_core].insert(eof);
        }
    }

    // goto_table�����
    for (auto& s: states) {
        for (const auto& symbol: all_symbols) {
            core_set_type gotoIX;
            make_lr0_goto(gotoIX, s.cores, symbol, g);
            if (gotoIX.empty()) { continue; }

            core_set_type gotoIX2;
            choose_kernel(gotoIX2, gotoIX, g);
            s.goto_table[symbol] = kernels[gotoIX2];
        }
    }

    // 2. Apply Algorithm 4.62 to the kernel of each set of LR(0)
    // items and grammar symbol X to determine which lookaheads
    // are spontaneously generated for kernel items in GOTO( I, X
    // ), and from which items in I lookaheads are propagated to
    // kernel items in GOTO( I, X ) .
        
    // 3. Initialize a table that gives, for each kernel item in
    // each set of items, the associated lookaheads.  Initially,
    // each item has associated with it only those lookaheads that
    // we determined in step(2) were generated spontaneously.

    // determine lookahead p.296
    for (auto& s: states) {
        for (const auto& k: s.kernel) {
            item_set_type J;
            J.insert(item_type(k, dummy));
            make_lr1_closure(J, first, g);

            for (const auto& j: J) {
                if (j.over()) { continue; }

                const symbol_type& X = j.curr();

                int goto_state = s.goto_table[X];
                const core_set_type& gotoIX = states[goto_state].kernel;

                for (const auto& l: gotoIX) {
                    if (!(l.rule() == j.rule())) { continue; }
                    if (l.cursor() != j.cursor()+ 1) { continue; }

                    if (j.lookahead() == dummy) {
                        // ��ǂݓ`�d
                        s.propagate_map[k].insert(
                            std::make_pair(goto_state, l));
                    } else {
                        // ��������
                        states[goto_state].generate_map[l].insert(
                            j.lookahead());
                    }
                }                                
            }
        }
    }        
        
    // 4. Make repeated passes over the kernel items in all sets.
    // When we visit an item /i/, we look up the kernel items to
    // which /i/ propagates its lookaheads, using information
    // tabulated in step (2).  The current set of lookaheads for
    // /i/ is added to those already associated with each of the
    // items to which items until no more new lookaheads are
    // propagated.

    bool iterate = true;
    while (iterate) {
        iterate = false;

        for (const auto& s: states) {
            for (const auto& j: s.kernel) {
                auto f0 = s.generate_map.find(j);
                if (f0 == s.generate_map.end()) { continue; }
                auto f1 = s.propagate_map.find(j);
                if (f1 == s.propagate_map.end()) { continue; }

                const terminal_set_type& sg = (*f0).second;
                const propagate_type& propagate = (*f1).second;

                for (const auto& k: propagate) {
                    terminal_set_type& dg =
                        states[k.first].generate_map[k.second];

                    size_t n = dg.size();
                    dg.insert(sg.begin(), sg.end());
                    if (dg.size() != n) { iterate = true; }
                }
            }
        }
    }

    // kernel lr0 collection�ɐ�ǂ݂�^����closure�����
    for (auto& s: states) {
        for (const auto& x: s.kernel) {
            for (const auto& symbol: s.generate_map[x]) {
                s.items.insert(item_type(x, symbol));
            }
        }

        make_lr1_closure(s.items, first, g);
    }

    // ���i�ɂ�����\����͓����J(i)������B
    // �����A���̓���\�ɋ���������΁A�^����ꂽ���@��
    // LALR(1)�łȂ��A�������\����̓��[�`�������o�����Ƃ͂ł��Ȃ��B
    for (auto& s: states) {
        // p287
        // a) ��[A�����Ea��,b]��J(i)�̗v�f�ł���A
        // goto(J(i),a)=J(j)�ł���΁A
        // action[i,a]�ɓ���"shift j"������B
        // �����ŁAa�͏I�[�L���łȂ���΂Ȃ�Ȃ��B
        for (const item_type& x: s.items) {
            if (x.over()) { continue; }

            const symbol_type& a = x.curr();
            if (!a.is_terminal()) { continue; }

            auto gt = s.goto_table.find(a);
            if (gt == s.goto_table.end()) { continue; }

            int next = (*gt).second;
                        
            // terminal��goto_table����폜
            s.goto_table.erase(gt);

            // shift
            auto k = s.action_table.find(a.token());
            if (k != s.action_table.end()) {
                if ((*k).second.type == action_reduce) {
                    srr(x.rule(), (*k).second.rule);
                }
            }

            s.action_table[a.token()] = action_type(
                action_shift, next, x.rule());
        }

        // b), c)�͓����ɍs��
        for (const item_type& x: s.items) {
            if (!x.over()) { continue; }

            // conflict����ł�accept��reduce�̈��Ƃ݂Ȃ�
            bool add_action = true;

            auto k = s.action_table.find(x.lookahead().token());
            if (k != s.action_table.end()) {
                const rule_type& krule = (*k).second.rule;
                if ((*k).second.type == action_shift) {
                    srr(krule, x.rule());
                    add_action = false; // shift��D��
                }
                if ((*k).second.type == action_reduce &&
                    !(krule == x.rule())) {
                    rrr(krule, x.rule());
                    // �Ⴂ����D��
                    add_action = x.rule().id() < (*k).second.rule.id(); 
                }
            }

            if (!add_action) { continue; }

            if (x.rule() == g.root_rule()) {
                // c)��[S'��S�E, $]��Ji�̗v�f�Ȃ�΁A
                // action[i, $]��"accept"������B

                s.action_table[Traits::eof()] = action_type(
                    action_accept, 0xdeadbeaf, g.root_rule());
            } else {
                // b)��[A�����E, a]��Ji�̗v�f�ł���A
                // A��S�Ȃ�΁Aaction[i, a]��
                // "reduce A����"������B

                s.action_table[x.lookahead().token()] = action_type(
                    action_reduce, 0xdeadbeaf, x.rule());
            }                    
        }

        // ���i�ɑ΂���s����֐��́A
        // ���̋K�������ׂĂ̔�I�[�L��A�ɓK�p���č쐬����B
        // ���Ȃ킿�Agoto(I(i),A)=I(j)�ł���΁Agoto[i,A]=j�Ƃ���B
        for (const auto& A: nonterminals) {
            item_set_type gt2;
            make_lr1_goto(gt2, s.items, symbol_type(A), first, g);

            core_set_type gt;
            items_to_cores(gt, gt2);

            auto next = kernels.find(gt);
            if (next == kernels.end()) { continue; }
            s.goto_table[A] = (*next).second;
        }
    }

    // ������Ԃ̌���
    for (const auto& s: table.states()) {
        // ���[�g���@���ǂ����̃`�F�b�N
        for (const auto& item: s.items) {
            if (item.cursor() == 0 && item.rule() == g.root_rule()) {
                table.first_state(s.no);
            }
        }
    }

    // �G���[������Ԃ��ǂ����̔���
    for (auto& s: table.states()) {
        // ���[�g���@���ǂ����̃`�F�b�N
        for (const auto& pair: s.action_table) {
            if (pair.first == error_token) {
                s.handle_error = true;
                break;
            }
        }
    }

}

template <class Token, class Traits>
void
make_lalr_table(
    parsing_table<Token, Traits>&   table,
    const grammar<Token, Traits>&   g,
    Token                           error_token) {
    make_lalr_table(
        table,
        g,
        error_token,
        null_reporter<Token, Traits>(),
        null_reporter<Token, Traits>());
}

/*============================================================================
 *
 * class parser
 *
 * LALR�\����̓G���W��
 *
 *==========================================================================*/

template < class Table, class Value >
class parser {
public:
    typedef Table                                   table_type;
    typedef Value                                   value_type;
    typedef typename table_type::token_type         token_type;
    typedef typename table_type::traits_type        traits_type;
    typedef typename table_type::state              state_type;
    typedef typename table_type::action             action_type;
    typedef typename table_type::rule_type          rule_type;

private:
    struct stack_frame {
        int         state;
        value_type  value;

        stack_frame(int s, const value_type& v)
            : state(s), value(v) {}
    };

    typedef std::vector<stack_frame> stack_type;
    stack_type stack_;

public:
    class arguments {
    public:
        typedef typename stack_type::const_iterator const_iterator;
    public:
        arguments(const_iterator b, const_iterator e) : b_(b), e_(e) {}

        const value_type& operator[](size_t n) const {
            assert(b_ + n < e_);
            return(*(b_ + n)).value;
        }

        size_t size() const { return e_ - b_; }
    private:
        const_iterator b_;
        const_iterator e_;
    };

    typedef std::function<Value (const arguments&)> semantic_action_type;
    typedef std::unordered_map<
        rule_type,
        semantic_action_type,
        typename rule_type::hash>
        semantic_actions_type;
    
public:
    parser() {}
    parser(const table_type& x) { reset(x); }

    void reset(const table_type& x) {
        stack_.clear();

        table_ = x;
        push_stack(table_.first_state(), value_type());
    }

    template <class F>
    void set_semantic_action(const rule_type& rule, F f) {
        semantic_actions_[rule] = semantic_action_type(f);
    }

    void set_semantic_actions(const semantic_actions_type& m) {
        semantic_actions_ = m;
    }

    bool push(const token_type& x, const value_type& v) {
        bool ate = false;

        while (!ate) {
            const state_type* state = stack_top();
            auto i = state->action_table.find(x);
            if (i == state->action_table.end()) { throw syntax_error(); }

            const action_type* action = &((*i).second);
            switch (action->type) {
                case action_shift:
                    push_stack(action->dest_index, v);
                    ate = true;
                    break;
                case action_reduce: {
                    const auto& rule = action->rule;
                    value_type v;
                    run_semantic_action(v, rule);
                    pop_stack(rule.right().size());
                    state = stack_top();
                    {
                        auto i = state->goto_table.find(rule.left());
                        assert(i != state->goto_table.end());
                        push_stack((*i).second, v);
                    }
                    break;
                }
                case action_accept: {
                    const auto& rule = action->rule;
                    run_semantic_action(accept_value_, rule);
                    return true;
                }
                case action_error:
                default:
                    throw syntax_error();
            }
        }
        return false;
    }

    // push��true��Ԃ������ɗL���ɂȂ�
    const value_type& accept_value() { return accept_value_; }

private:
    void run_semantic_action(value_type& v, const rule_type& rule) {
        if (const auto& f = semantic_actions_[rule]) {
            v = f(arguments(
                      stack_.end() - rule.right().size(),
                      stack_.end()));
        }
    }

    void push_stack(int stack_index, const Value& value) {
        stack_.push_back(stack_frame(stack_index, value));
    }

    void pop_stack(size_t n) {
        stack_.erase(stack_.end() - n, stack_.end());
    }

    typename table_type::state* stack_top() {
        return &table_.states()[stack_.back().state];
    }

public:
    Table                   table_;
    semantic_actions_type   semantic_actions_;
    value_type              accept_value_;

};

template < class Token, class Traits, class Value >
struct package {
    typedef zw::gr::rule<Token, Traits>              rule;
    typedef zw::gr::epsilon<Token, Traits>           epsilon;
    typedef zw::gr::nonterminal<Token, Traits>       nonterminal;
    typedef zw::gr::terminal<Token, Traits>          terminal;
    typedef zw::gr::symbol<Token, Traits>            symbol;
    typedef zw::gr::grammar<Token, Traits>           grammar;
    typedef zw::gr::parsing_table<Token, Traits>     parsing_table;
    typedef zw::gr::parser<parsing_table, Value>     parser;

    static void make_lalr_table(
        parsing_table&  table,
        const grammar&  g,
        Token           error_token) {
        zw::gr::make_lalr_table(table, g, error_token);
    }

    template < class Reporter > static
    void make_lalr_table(
        parsing_table&  table,
        const grammar&  g,
        Token           error_token,
        Reporter        srr,
        Reporter        rrr) {
        zw::gr::make_lalr_table(table, g, error_token, srr, rrr);
    }
};

} // namespace gr

} // namespace zw

#endif
