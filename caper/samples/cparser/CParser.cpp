// Written by Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>.
// This file is public domain software.

#include "CScanner.hpp"      // cparser::Scanner
#include "CParserSite.hpp"   // cparser::ParserSite
#include "CParserAST.hpp"    // cparser::Node, cparser::TokenInfo
#include "CParser.hpp"       // cparser::Parser

#include <cstring>      // std::strlen
#include <vector>       // std::vector
#include <istream>      // std::ifstream
#include <fstream>      // std::ifstream
#include <iterator>     // std::istreambuf_iterator

namespace cparser
{
    template <class Iterator>
    bool parse(Iterator begin, Iterator end)
    {
        using namespace cparser;
        ParserSite ps;
        Scanner<Iterator, ParserSite> scanner(ps);

        std::vector<TokenValue > infos;
        scanner.scan(infos, begin, end);
        //scanner.show_tokens(infos.begin(), infos.end());

        //std::cout << std::endl << "--------------" << std::endl;
        Parser<shared_ptr<Node>, ParserSite> parser(ps);
        std::vector<TokenValue >::iterator it, end2 = infos.end();
        for (it = infos.begin(); it != end2; ++it)
        {
            //std::cout << scanner.token_to_string(*it) << std::endl;
            if (parser.post(it->m_token, make_shared<TokenValue >(*it)))
            {
                if (parser.error())
                {
                    ps.location() = it->location();
                    ps.message(std::string("ERROR: syntax error near ") +
                        scanner.token_to_string(*it));
                }
                break;
            }
        }

        shared_ptr<Node> node;
        if (parser.accept(node))
        {
            std::cerr << "Accepted!" << std::endl;
            shared_ptr<TransUnit> trans_unit;
            trans_unit = static_pointer_cast<TransUnit, Node>(node);
            return ps.compile(*trans_unit.get());
        }

        return false;
    }

    bool parse_string(const char *s)
    {
        return parse(s, s + std::strlen(s));
    }

    bool parse_string(const std::string& str)
    {
        return parse(str.begin(), str.end());
    }

    bool parse_file(const char *filename)
    {
        std::ifstream file(filename);
        if (file.is_open())
        {
            std::istreambuf_iterator<char> begin(file), end;
            bool ok = parse(begin, end);
            file.close();
            return ok;
        }
        return false;
    }
} // namespace cparser

int main(int argc, char **argv)
{
    using namespace std;
    if (argc <= 1 || strcmp(argv[1], "/?") == 0 || strcmp(argv[1], "--help") == 0)
    {
        cout << "cparser parses preprocessed C source." << endl;
        cout << endl;
        cout << "    Usage: cparser input-file.i" << endl;
        return 0;
    }

    if (strcmp(argv[1], "--version") == 0)
    {
        cout << "cparser 0.0 by Katayama Hirofumi MZ" << endl;
        return 0;
    }

    cparser::parse_file(argv[1]);

    return 0;
}
