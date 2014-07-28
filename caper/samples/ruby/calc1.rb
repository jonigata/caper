require './calc1_ast.rb'
require './calc1_parser.rb'

class Scanner
    attr_accessor :input

    def initialize input = $stdin
        @input = input
    end
    
    def get
        begin
            c = @input.getc
            return :token_eof, 0 if c.nil? || c == ?\n
        end while c =~ /\s/

        case c
        when ?+
            return :token_Add, 0
        when ?-
            return :token_Sub, 0
        when ?*
            return :token_Mul, 0
        when ?/
            return :token_Div, 0
        else
            return :token_eof, 0 if c.nil?
            if c =~ /\d/
                n = 0
                begin
                    n *= 10
                    n += c.ord - ?0.ord
                    c = @input.getc
                end while !c.nil? && c =~ /\d/
                @input.ungetc c if !c.nil?
                return :token_Number, Number.new(n)
            end
        end
        fail format("bad input char '%s'(%d)\n", c, c.ord)
    end
end

class SemanticAction
    def syntax_error
    end

    def stack_overflow
    end
    
    def downcast v
        v
    end

    def upcast v
        v
    end

    def MakeExpr x
        TermExpr.new(x)
    end

    def MakeAdd(x, y)
        $stderr.print x.to_s + " + " + y.to_s + "\n"
        AddExpr.new(x, TermExpr.new(y))
    end

    def MakeSub(x, y)
        $stderr.print x.to_s + " - " + y.to_s + "\n"
        SubExpr.new(x, TermExpr.new(y))
    end

    def MakeTerm x 
        NumberTerm.new(x)
    end

    def MakeMul(x, y)
        $stderr.print x.to_s + " * " + y.to_s + "\n"
        MulTerm.new(x, NumberTerm.new(y))
    end

    def MakeDiv(x, y)
        $stderr.print x.to_s + " / " + y.to_s + "\n"
        DivTerm.new(x, NumberTerm.new(y))
    end
end

s = Scanner.new
parser = Calc::Parser.new(SemanticAction.new)

$stdout.print ">"

begin
    token, v = s.get
    #p Calc::token_label(token)
end while !parser.post(token, v)

if v = parser.accept
    printf "accepted %d\n", v.calc
    print v.to_s + "\n"
else
    print "failed\n"
end
