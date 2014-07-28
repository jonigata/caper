require './hello2_parser.rb'

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
    
    def Greet x, y
        print x.to_s + y.to_s + "\n"
        ""
    end
end

parser = Hello_world::Parser.new(SemanticAction.new)

parser.post :token_Hello, "Guten Tag, "
parser.post :token_World, "Welt"
parser.post :token_eof, ""
