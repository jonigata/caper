#include "grammar.hpp"
//#include "fastlalr.hpp"
//#include "lalr.hpp"
#include "honalee.hpp"

typedef std::char_traits<char> traits;
typedef zw::gr::grammar<char,traits>          grammar;
typedef zw::gr::rule<char,traits>             rule;
typedef zw::gr::nonterminal<char,traits>      nonterminal;
typedef zw::gr::terminal<char,traits>         terminal;
typedef zw::gr::epsilon<char,traits>          epsilon;
typedef zw::gr::parsing_table<char,traits>    parsing_table;

#if 1
void honalee1 ()
{
	terminal add("+",1), mul("*",2), lpar("(",3), rpar(")",4), id("id", 'i');
        nonterminal S("S"), E("E"), T("T"), F("F");

        rule r0(S); r0 << E;
        rule r1(E); r1 << E << add << T;
        rule r2(E); r2 << T;
        rule r3(T); r3 << T << mul << F;
        rule r4(T); r4 << F;
        rule r5(F); r5 << lpar << E << rpar;
        rule r6(F); r6 << id;

        grammar g(r0); g << r1 << r2 << r3 << r4 << r5 << r6;

        parsing_table table;
        zw::gr::make_lr1_table( table, g );
        
}

void honalee2 ()
{
	terminal a("a",'a'), b("b",'b'), c("c",'c'), d("d",'d'), e("e",'e');
        nonterminal SS("S'"), S("S"), A("A"), B("B");

        rule r0(SS); r0 << S;
        rule r1(S); r1 << a << A << d;
        rule r2(S); r2 << a << B << e;
        rule r3(S); r3 << b << A << e;
        rule r4(S); r4 << b << B << d;
        rule r5(A); r5 << c;
        rule r6(B); r6 << c;

        grammar g(r0); g << r1 << r2 << r3 << r4 << r5 << r6;
        parsing_table table;
        zw::gr::make_lr1_table( table, g );
        
}

void honalee3 ()
{
        // LR(2) Grammar
	terminal d("d",'d'), e("e",'e'), f("f",'f'), x("x",'x'), y("y",'y');
        nonterminal S("S"), A("A"), B("B"), C("C");

        rule r0(S); r0 << A;
        rule r1(A); r1 << B << d << e;
        rule r2(A); r2 << C << d << f;
        rule r3(B); r3 << x << y;
        rule r4(C); r4 << x << y;

        grammar g(r0); g << r1 << r2 << r3 << r4;
        parsing_table table;
        zw::gr::make_lr1_table( table, g );
        
}
#endif

#if 0
void p295()
{
        // •¶–@ p295

        nonterminal sdash("S'"),s("S"),l("L"),r("R");
	terminal id( "id", 'i' ), star( "*", '*' ), equal( "=", '=' );

        rule r0(sdash); r0 << s;
        rule r1(s);     r1 << l << equal << r;
        rule r2(s);     r2 << r;
        rule r3(l);     r3 << star << r;
        rule r4(l);     r4 << id;
        rule r5(r);     r5 << l;

        grammar g(r0); g << r1 << r2 << r3 << r4 << r5;

#if 0
        std::cerr
                 << rule0 << std::endl
                 << rule1 << std::endl
                 << rule2 << std::endl
                 << rule3 << std::endl
                 << rule4 << std::endl
                 << rule5 << std::endl;
#endif

        parsing_table table;
        zw::gr::make_lalr_table( table, g );
        std::cerr << table;
}
#endif

void reduce_reduce_conflict()
{
        terminal a("a",'a'), b("b",'b'), c("c",'c'), d("d",'d');
        nonterminal X("X"), A("A"), B("B"), C("C");

        rule rule0( X ); rule0 << A;
        rule rule1( A ); rule1 << B;
        rule rule2( A ); rule2 << C;
        rule rule3( B ); rule3 << c << d;
        rule rule4( C ); rule4 << c << d;
        
        grammar g(rule0);
        g << rule0
          << rule1
          << rule2
          << rule3
          << rule4;

        //parsing_table table=zw::gr::make_lalr_table(g);
	std::cout << "\n[LALR parsing table]\n";
        //std::cout << table;
}

void shift_reduce_conflict()
{
        terminal a("a",'a'), b("b",'b'), c("c",'c'), d("d",'d');
        nonterminal X("X"), A("A"), B("B"), C("C");

        rule rule0( X ); rule0 << A << d;
        rule rule1( A ); rule1 << c;
        rule rule2( A ); rule2 << c << d;
        
        grammar g(rule0);
        g << rule0
          << rule1
          << rule2;

        //parsing_table table=zw::gr::make_lalr_table(g);
	std::cout << "\n[LALR parsing table]\n";
        //std::cout << table;
}

void list()
{
        terminal a("a",'a'), b("b",'b'), c("c",'c'), d("d",'d');
        nonterminal X("X"), A("A"), B("B"), C("C");

        rule rule0( X ); rule0 << A << d;
        rule rule1( A ); rule1 << a;
        rule rule2( A ); rule2 << A << a;
        
        grammar g(rule0);
        g << rule0
          << rule1
          << rule2;

        //parsing_table table=zw::gr::make_lalr_table(g);
	std::cout << "\n[LALR parsing table]\n";
        //std::cout << table;
}

void MCIMLp47()
{
        nonterminal X("X"),Y("Y"),Z("Z"),ZZ("Z'");
        terminal a("a",'a'), b("b",'b'), c("c",'c'), d("d",'d');

        rule rule0(ZZ); rule0 << Z;
        rule rule1(Z);  rule1 << d;
        rule rule2(Z);  rule2 << X << Y << Z;
        rule rule3(X);  rule3 << Y;
        rule rule4(X);  rule4 << a;
        rule rule5(Y);
        rule rule6(Y);  rule6 << c;

        grammar g(rule0);
        g << rule1
          << rule2
          << rule3
          << rule4
          << rule5
          << rule6;
        
        // parsing table‚Ìì¬
        //parsing_table table=zw::gr::make_lalr_table(g);
	std::cout << "\n[LALR parsing table]\n";
        //std::cout << table;
}

void p283()
{
        nonterminal Sdash("S'"),S("S"),C("C");
        terminal    c("c",'c'),d("d",'d'),eof("$",std::char_traits<char>::eof());

        rule rule0(Sdash);
        rule0 << S;

        rule rule1(S);
        rule1 << C << C;

        rule rule2(C);
        rule2 << c << C;

        rule rule3(C);
        rule3 << d;

        grammar g(rule0);
        g << rule1
          << rule2
          << rule3;
        
#if 0
        // canonical collection‚Ìì¬
        canonical_collection cc=zw::gr::make_lalr_table(g);

	std::cout << "\n[canonical collection]\n";
        int n=0;
        const_foreach(canonical_collection,cc,i){
                std::cout << "<" << n << ">\n";
        
                const_foreach(set_of_items,*i,j){
                        std::cout << *j;
                }
		
		n++;
        }
#else
        // parsing table‚Ìì¬
        //parsing_table table=zw::gr::make_lalr_table(g);
	std::cout << "\n[LALR parsing table]\n";
        //std::cout << table;

#endif
}

void p266()
{
        // •¶–@ p266

        nonterminal edash("E'"),e("E"),t("T"),f("F");
	terminal plus("+",'+'),star("*",'*'),lpar("(",'('),rpar(")",')'),id("n",'n');

        rule rule0(edash);
        rule0 << e;

        rule rule1(e);
        rule1 << e << plus << t;

        rule rule2(e);
        rule2 << t;

        rule rule3(t);
        rule3 << t << star << f;

        rule rule4(t);
        rule4 << f;

        rule rule5(f);
        rule5 << lpar << e << rpar;
    
        rule rule6(f);
        rule6 << id;

        grammar g(rule0);
        g << rule1
          << rule2
          << rule3
          << rule4
          << rule5
          << rule6;

#if 0
        // closure‚Ìì¬
        set_of_items items;
        items.insert(term(rule0,0));
        set_of_items c=zw::gr::make_closure(g,items);

	std::cout << "[closure]\n";
        const_foreach(set_of_items,c,i){
                std::cout << *i;
        }

	// goto‚Ìì¬
	items.clear();
	items.insert(term(rule0,1));
	items.insert(term(rule1,1));

	c=zw::gr::make_goto(g,items,plus);

	std::cout << "\n[goto]\n";
        const_foreach(set_of_items,c,i){
                std::cout << *i;
        }

        // canonical collection‚Ìì¬
        canonical_collection cc=zw::gr::make_canonical_collection(g);

	std::cout << "\n[canonical collection]\n";
        int n=0;
        const_foreach(canonical_collection,cc,i){
                std:: cout << "<" << (n++) << ">\n";
        
                const_foreach(set_of_items,*i,j){
                        std::cout << *j;
                }
        }
        slr_table table=zw::gr::make_slr_table(g);
#endif
}

#if 0
void p228()
{
        // •¶–@ p.228
        nonterminal edash("E'"),e("E"),tdash("T'"),t("T"),f("F");
	terminal plus('+'),star('*'),lpar('('),rpar(')'),id('n');

        rule rule0(e);
        rule0 << t << edash;

        rule rule1(edash);
        rule1 << plus << t << edash;

        rule rule2(edash);
        rule2 << epsilon();

        rule rule3(t);
        rule3 << f << tdash;

        rule rule4(tdash);
        rule4 << star << f << tdash;

        rule rule5(tdash);
        rule5 << epsilon();
    
        rule rule6(f);
        rule6 << lpar << e << rpar;

        rule rule7(f);
        rule7 << id;

        grammar g(rule0);
        g << rule1
          << rule2
          << rule3
          << rule4
          << rule5
          << rule6
          << rule7;

        // follow‚Ìì¬
        set_of_symbols follow=zw::gr::make_follow(g,f);
        for( set_of_symbols::const_iterator i = follow.begin() ; i != follow.end() ; ++i ) {
                std::cout << *i;
        }


}
#endif

int main(int argc,char** argv)
{
#if 1
        honalee3();
#else
        p295();
        MCIMLp47();
        list();
        shift_reduce_conflict();
        p283();
	p266();
	p228();
#endif
    
        return 0;
}
