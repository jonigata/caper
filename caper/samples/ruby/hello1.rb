require './hello1_parser.rb'

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
    
    def Greet
        print "hello world\n"
    end
end

parser = Hello_world::Parser.new(SemanticAction.new)

parser.post :token_Hello, 0
parser.post :token_World, 0
parser.post :token_eof, 0
