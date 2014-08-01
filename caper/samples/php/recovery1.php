<?php

error_reporting(E_ALL);

$output = '';
$errors = '';

function user_error_handler($errno, $errmsg, $filename, $linenum, $vars)
{
    global $errors;
    
    switch ($errno) {
    case E_USER_ERROR:
        $errors .= "ERROR: ";
        break;

    case E_USER_WARNING:
        $errors .= "WARNING: ";
        break;

    case E_USER_NOTICE:
        $errors .= "NOTICE: ";
        break;
        
    default:
        break;
    }
    $errors .= $filename . "(" . $linenum . "): " . $errmsg . "\n";
}
set_error_handler('user_error_handler');

require_once('recovery1_parser.php');

class Scanner
{
    public $input;

    function __construct($data)
    {
        $this->input = $data;
    }
    
    function getch()
    {
        if (strlen($this->input) > 0)
        {
            $ch = $this->input[0];
            $this->input = substr($this->input, 1);
            return $ch;
        }
        return NULL;
    }
    
    function ungetch($ch)
    {
        $this->input = $ch . $this->input;
    }

    function get(&$v)
    {
        do
        {
            $c = $this->getch();
            if ($c == "\n") {
                return \calc\Token::token_NewLine;
            }
        } while (preg_match('/\s/', $c));

        switch ($c)
        {
        case '-':
            return \calc\Token::token_Minus;
        case '+':
            return \calc\Token::token_Plus;
        case '*':
            return \calc\Token::token_Star;
        case '/':
            return \calc\Token::token_Slash;
        case '(':
            return \calc\Token::token_LParen;
        case ')':
            return \calc\Token::token_RParen;
        default:
            if ($c == NULL) {
                return \calc\Token::token_eof;
            }
            if (preg_match('/\d/', $c)) {
                $n = 0;
                do
                {
                    $n *= 10;
                    $n += ord($c) - ord('0');
                    $c = $this->getch();
                } while ($c != NULL && preg_match('/\d/', $c));
                if ($c != NULL) {
                    $this->ungetch($c);
                }
                $v = $n;
                return \calc\Token::token_Number;
            }
        }
        trigger_error(sprintf("bad input char '%s'(%d)\n", $c, ord($c)), E_USER_ERROR);
        return \calc\Token::token_eof;
    }
}

class SemanticAction
{
    function syntax_error()
    {
    }

    function stack_overflow()
    {
    }
    
    function downcast($y)
    {
        return $y;
    }

    function upcast($y)
    {
        return $y;
    }
    
    function DoLine1()
    {
        return 0;
    }

    function DoLine2($exp)
    {
        global $output;
        $output .= "Exp: " . (string)$exp . "\n";
        return $exp;
    }

    function DoLine3()
    {
        global $output;
        $output .= "catched\n";
        return -1;
    }

    function DoAddExp1($exp1)
    {
        return $exp1;
    }

    function DoAddExp2($exp1, $exp2)
    {
        return $exp1 + $exp2;
    }

    function DoAddExp3($exp1, $exp2)
    {
        return $exp1 - $exp2;
    }

    function DoMulExp1($exp1)
    {
        return $exp1;
    }

    function DoMulExp2($exp1, $exp2)
    {
        return $exp1 * $exp2;
    }

    function DoMulExp3($exp1, $exp2)
    {
        return $exp1 / $exp2;
    }

    function DoUnaryExp1($exp1)
    {
        return $exp1;
    }

    function DoUnaryExp2($exp1)
    {
        return -$exp1;
    }
    
    function DoPrimExp1($num1)
    {
        return $num1;
    }
    
    function DoPrimExp2($num1)
    {
        return $num1;
    }
}

header('Content-Type: text/html');

if (isset($_POST['input'])) {
    $input = $_POST['input'] . "\n";
    $s = new Scanner($input);
    $parser = new calc\Parser(new SemanticAction());

    do
    {
        $token = $s->get($v);
        # $output .= calc\token_label($token) . "\n";
    } while (!$parser->post($token, $v));
    
    if ($parser->error) {
        trigger_error(sprintf("error occured: " . calc\token_label($token)));
    } else {
        if ($parser->accept($v)) {
            $result = "accepted " . $v;
        } else {
            $result = "failed";
        }
    }
}

?><html><head>
<title>recovery1</title>
</head>
<body>
    <h1>recovery1</h1>
    <p style="color: red; font-weight: bold;">
        <?php
            if (isset($errors) && strlen($errors) > 0) {
                echo "ERROR: " . nl2br(htmlspecialchars($errors));
            }
        ?>
    </p>
    <p>
        <?php
            if (isset($input)) {
                echo "Input: " . htmlspecialchars($input);
            }
        ?>
    </p>
    <p>
        <?php
            if (isset($output) && strlen($output) > 0) {
                echo "Output: <br />" . nl2br(htmlspecialchars($output));
            }
        ?>
    </p>
    <p>
        <?php
            if (isset($result)) {
                echo "Result: " . htmlspecialchars($result);
            }
        ?>
    </p>
    <form action="recovery1.php" method="post">
        <p>Input:<br /><textarea name="input" rows="5" cols="40"></textarea></p>
        <p><input type="submit" value="Send" /></p>
    </form>
</body></html>
