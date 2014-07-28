require './recovery0_parser.rb'

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
        when ?(
            return :token_LParen, 0
        when ?)
            return :token_RParen, 0
        when ?,
            return :token_Comma, 0
        when ?*
            return :token_Star, 0
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
                return :token_Number, n
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

    def PackList x
        print "list: " + x.to_s + "\n"
        x
    end
    
    def PackListError
        $stderr.print "catching error\n"
        -1
    end
    
    def MakeList n
        n
    end
    
    def AddToList m, n
        m + n
    end
end

s = Scanner.new
parser = Rec::Parser.new(SemanticAction.new)

$stdout.print ">"

begin
    token, v = s.get
    #p Rec::token_label(token)
end while !parser.post(token, v)

if parser.accept
    print "accepted\n"
else
    print "failed\n"
end
