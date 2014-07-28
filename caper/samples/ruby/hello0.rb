require './hello0_parser.rb'

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
end

parser = Hello_world::Parser.new(SemanticAction.new)

parser.post :token_Hello, 0
parser.post :token_World, 0
