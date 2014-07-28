class Node
    def calc
    end
    def to_s
        ""
    end
end

class Expr < Node
end

class Term < Node
end

class Number < Node
    attr_accessor :number
    def initialize num
        @number = num
    end
    def calc
        @number
    end
    def to_s
        @number.to_s
    end
end

class AddExpr < Expr
    attr_accessor :lhs, :rhs
    def initialize x, y
        @lhs = x
        @rhs = y
    end
    def calc
        @lhs.calc + @rhs.calc
    end
    def to_s
        "(" + @lhs.to_s + " + " + @rhs.to_s + ")"
    end
end

class SubExpr < Expr
    attr_accessor :lhs, :rhs
    def initialize x, y
        @lhs = x
        @rhs = y
    end
    def calc
        @lhs.calc - @rhs.calc
    end
    def to_s
        "(" + @lhs.to_s + " - " + @rhs.to_s + ")"
    end
end

class TermExpr < Expr
    attr_accessor :term
    def initialize t
        @term = t
    end
    def calc
        @term.calc
    end
    def to_s
        @term.to_s
    end
end

class MulTerm < Term
    attr_accessor :lhs, :rhs
    def initialize x, y
        @lhs = x
        @rhs = y
    end
    def calc
        @lhs.calc * @rhs.calc
    end
    def to_s
        "(" + @lhs.to_s + " * " + @rhs.to_s + ")"
    end
end

class DivTerm < Term
    attr_accessor :lhs, :rhs
    def initialize x, y
        @lhs = x
        @rhs = y
    end
    def calc
        @lhs.calc / @rhs.calc
    end
    def to_s
        "(" + @lhs.to_s + " / " + @rhs.to_s + ")"
    end
end

class NumberTerm < Term
    attr_accessor :number
    def initialize num
        @number = num
    end
    def calc
        @number.calc
    end
    def to_s
        @number.to_s
    end
end
