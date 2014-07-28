require './recovery1_parser.rb'

class Scanner
    attr_accessor :input

    def initialize input = $stdin
        @input = input
    end
    
    def get
        begin
            c = @input.getc
            return :token_NewLine, 0 if c == ?\n
        end while c =~ /\s/

        case c
        when ?-
            return :token_Minus, 0
        when ?+
            return :token_Plus, 0
        when ?*
            return :token_Star, 0
        when ?/
            return :token_Slash, 0
        when ?(
            return :token_LParen, 0
        when ?)
            return :token_RParen, 0
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

	def DoLine1
		0
	end
	
	def DoLine2 exp
		print "Exp: " + exp.to_s + "\n"
		exp
	end
	
	def DoLine3
		print "catched\n"
		-1
	end
	
	def DoAddExp1 exp1
		exp1
	end

	def DoAddExp2 exp1, exp2
		exp1 + exp2
	end

	def DoAddExp3 exp1, exp2
		exp1 - exp2
	end

	def DoMulExp1 exp1
		exp1
	end

	def DoMulExp2 exp1, exp2
		exp1 * exp2
	end

	def DoMulExp3 exp1, exp2
		exp1 / exp2
	end
	
	def DoUnaryExp1 exp1
		exp1
	end
	
	def DoUnaryExp2 exp1
		-exp1
	end
	
	def DoPrimExp1 num1
		num1
	end
	
	def DoPrimExp2 exp1
		exp1
	end
end

s = Scanner.new
parser = Calc::Parser.new(SemanticAction.new)

$stdout.print ">"

begin
    token, v = s.get
    #p Calc::token_label(token)
end while !parser.post(token, v)

if parser.error
    $stderr.print "error occured: " + Calc::token_label(token) + "\n"
    exit 1
end

if v = parser.accept
    print "accepted\n"
else
    print "failed\n"
end
