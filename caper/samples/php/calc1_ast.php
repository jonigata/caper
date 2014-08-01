<?php

class Node
{
    function calc()
    {
        return 0;
    }

    function __toString()
    {
        return "";
    }
}

class Expr extends Node
{
}

class Term extends Node
{
}

class Number extends Node
{
    public $number;

    function __construct($num)
    {
        $this->number = $num;
    }

    function calc()
    {
        return $this->number;
    }

    function __toString()
    {
        return (string)$this->number;
    }
}

class AddExpr extends Expr {
    public $lhs;
    public $rhs;

    function __construct($x, $y)
    {
        $this->lhs = $x;
        $this->rhs = $y;
    }

    function calc()
    {
        return $this->lhs->calc() + $this->rhs->calc();
    }

    function __toString()
    {
        return "(" . (string)$this->lhs . " + " . (string)$this->rhs . ")";
    }
};

class SubExpr extends Expr
{
    public $lhs;
    public $rhs;

    function __construct($x, $y)
    {
        $this->lhs = $x;
        $this->rhs = $y;
    }

    function calc()
    {
        return $this->lhs->calc() - $this->rhs->calc();
    }

    function __toString()
    {
        return "(" . (string)$this->lhs . " - " . (string)$this->rhs . ")";
    }
};

class TermExpr extends Expr
{
    public $term;

    function __construct($x)
    {
        $this->term = $x;
    }

    function calc()
    {
        return $this->term->calc();
    }

    function __toString()
    {
        return $this->term->__toString();
    }
};

class MulTerm extends Term
{
    public $lhs;
    public $rhs;

    function __construct($x, $y)
    {
        $this->lhs = $x;
        $this->rhs = $y;
    }

    function calc()
    {
        return $this->lhs->calc() * $this->rhs->calc();
    }

    function __toString()
    {
        return "(" . (string)$this->lhs . " * " . (string)$this->rhs . ")";
    }
};

class DivTerm extends Term
{
    public $lhs;
    public $rhs;

    function __construct($x, $y)
    {
        $this->lhs = $x;
        $this->rhs = $y;
    }

    function calc()
    {
        return $this->lhs->calc() / $this->rhs->calc();
    }

    function __toString()
    {
        return "(" . (string)$this->lhs . " / " . (string)$this->rhs . ")";
    }
};

class NumberTerm extends Term
{
    public $number;

    function __construct($x)
    {
        $this->number = $x;
    }

    function calc()
    {
        return $this->number;
    }

    function __toString()
    {
        return (string)$this->number;
    }
};
