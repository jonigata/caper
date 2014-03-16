#ifndef CAPER_SCANNER_HPP
#define CAPER_SCANNER_HPP

#include "caper_ast.hpp"

////////////////////////////////////////////////////////////////
// scanner
template <class It>
class scanner {
public:
    typedef int char_type;

public:
    scanner(It b, It e) : b_(b), e_(e), c_(b), unget_(EOF) {
        addr_ = 0;
        dirdic_["token"] = token_directive_token;
        dirdic_["token_prefix"] = token_directive_token_prefix;
        dirdic_["external_token"] = token_directive_external_token;
        dirdic_["access_modifier"] = token_directive_access_modifier;
        dirdic_["namespace"] = token_directive_namespace;
        dirdic_["dont_use_stl"] = token_directive_dont_use_stl;
        lines_.push_back(0);
    }
    ~scanner() {}

    int addr() { return addr_; }

    int lineno(int addr) {
        std::vector<int>::const_iterator i = std::upper_bound(lines_.begin(), lines_.end(), addr);
        assert(i != lines_.begin());
        return int(i - lines_.begin());
    }
    int column(int addr) {
        std::vector<int>::const_iterator i = std::upper_bound(lines_.begin(), lines_.end(), addr);
        assert(i != lines_.begin());
        --i;
        return addr - *i;
    }

    Token get(value_type& v) {
      retry:
        int c;
        do {
            c = sgetc();
        } while (isspace(c));

        int b = addr_ - 1;

        // コメント
        if (c == '/') {
            int k = sgetc();
            if (k == '/') {
                while ((k = sgetc()) != '\n')
                    ;
                goto retry;
            } else {
                sungetc(k);
            }
        }

        // 記号類
        switch (c) {
            case ':': v = Value(range(b, addr_), Operator(':')); return token_colon;
            case ';': v = Value(range(b, addr_), Operator(';')); return token_semicolon;
            case '|': v = Value(range(b, addr_), Operator('|')); return token_pipe;
            case '(': v = Value(range(b, addr_), Operator('(')); return token_lparen;
            case ')': v = Value(range(b, addr_), Operator(')')); return token_rparen;
            case '[': v = Value(range(b, addr_), Operator('[')); return token_lbracket;
            case ']': v = Value(range(b, addr_), Operator(']')); return token_rbracket;
            case EOF: v = Value(range(b, addr_), Operator('$')); return token_eof;
        }

        // 識別子
        if (isalpha(c)) {
            std::stringstream ss;
            while (c != EOF &&(isalpha(c)|| isdigit(c)|| c == '_' || c == '.')) {
                ss <<(char)c;
                c = sgetc();
            }
            sungetc(c);
            v = Value(range(b, addr_), Identifier(ss.str()));
            return token_identifier;
        }

        // 整数
        if (isdigit(c)) {
            int n = 0;
            while (c != EOF && isdigit(c)) {
                n *= 10;
                n += c - '0';
                c = sgetc();
            }
            sungetc(c);
            v = Value(range(b, addr_), Integer(n));
            return token_integer;
        }

        //ディレクティブ
        if (c == '%') {
            std::stringstream ss;
            c = sgetc();
            while (c != EOF &&(isalpha(c)|| isdigit(c)|| c == '_')) {
                ss <<(char)c;
                c = sgetc();
            }
            sungetc(c);

            dirdic_type::const_iterator  i = dirdic_.find(ss.str());
            if (i != dirdic_.end()) {
                v = Value(range(b, addr_), Directive(ss.str()));
                return(*i).second;
            }
            throw bad_directive(addr_, ss.str());
        }

        // 型タグ
        if (c == '<') {
            std::vector<char> stack;
            stack.push_back('<');
            std::stringstream ss;

            while (!stack.empty()) {
                c = sgetc();
                switch (c) {
                    case '<': push_paren(stack, c); ss << char(c); break;
                    case '(': push_paren(stack, c); ss << char(c); break;
                    case '[': push_paren(stack, c); ss << char(c); break;
                    case '>':
                        pop_paren(stack, c);
                        if (!stack.empty()) {
                            ss << char(c);
                        }
                        break;
                    case ')': pop_paren(stack, c); ss << char(c); break;
                    case ']': pop_paren(stack, c); ss << char(c); break;
                    case EOF: throw mismatch_paren(addr_, c);
                    default:
                        if (c == '*' || c == ':' || c == ',' || c == '_' ||
                            isspace(c)|| isalpha(c)|| isdigit(c)) {
                            ss << char(c);
                            break;
                        } else {
                            throw unexpected_char(addr_, c);
                        }
                }
            }
            v = Value(range(b, addr_), TypeTag(ss.str()));
            return token_typetag;
        }

        throw unexpected_char(addr_, c);
    }

private:
    char_type sgetc() {
        int c;
        if (unget_ != EOF) {
            c = unget_;
            unget_ = EOF;
        } else if (c_ == e_) {
            c = EOF; 
        } else {
            c = *c_++;
            if (c == '\n') {
                lines_.push_back(addr_+1);
            }
        }
        addr_++;
        return c;
    }

    void sungetc(char_type c) {
        if (c != EOF) {
            addr_--;
            unget_ = c;
        }
    }

    void push_paren(std::vector<char>& stack, int c) {
        assert(c == '<' || c == '(' || c == '[');
        stack.push_back(c);
    }

    void pop_paren(std::vector<char>& stack, int c) {
        assert(!stack.empty());
        switch (c) {
            case '>': if (stack.back() != '<') { throw mismatch_paren(addr_, c); } break;
            case ')': if (stack.back() != '(') { throw mismatch_paren(addr_, c); } break;
            default: assert(0);
        }
        stack.pop_back();
    }

private:
    Range range(int b, int e) { return Range(b, e); }

private:
    It              b_;
    It              e_;
    It              c_;
    char_type       unget_;
    int             addr_;

    typedef std::unordered_map<std::string, Token> dirdic_type;
    dirdic_type dirdic_;

    std::vector<int> lines_;
};

#endif // CAPER_SCANNER_HPP
