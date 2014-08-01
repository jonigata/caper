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

require_once('calc0_parser.php');

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
            if ($c == NULL || $c == "\n") {
                return \calc\Token::token_eof;
            }
        } while (preg_match('/\s/', $c));

        switch ($c)
        {
        case '+':
            return \calc\Token::token_Add;
        case '-':
            return \calc\Token::token_Sub;
        case '*':
            return \calc\Token::token_Mul;
        case '/':
            return \calc\Token::token_Div;
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
    
    function Identity($x)
    {
        return $x;
    }

    function MakeAdd($x, $y)
    {
        global $output;
        $output .= sprintf("%d + %d\n", $x, $y);
        return $x + $y;
    }

    function MakeSub($x, $y)
    {
        global $output;
        $output .= sprintf("%d - %d\n", $x, $y);
        return $x - $y;
    }

    function MakeMul($x, $y)
    {
        global $output;
        $output .= sprintf("%d * %d\n", $x, $y);
        return $x * $y;
    }

    function MakeDiv($x, $y)
    {
        global $output;
        $output .= sprintf("%d / %d\n", $x, $y);
        return $x / $y;
    }
}

header('Content-Type: text/html');

if (isset($_POST['input'])) {
    $input = $_POST['input'];
    $s = new Scanner($input);
    $parser = new calc\Parser(new SemanticAction());

    do
    {
        $token = $s->get($v);
    } while (!$parser->post($token, $v));

    if ($parser->accept($v)) {
        $result = "accepted " . $v;
    } else {
        $result = "failed";
    }
}

?><html><head>
<title>calc0</title>
</head>
<body>
    <h1>calc0</h1>
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
    <form action="calc0.php" method="post">
        <p>Input: <input type="text" name="input" value="" /></p>
        <p><input type="submit" value="Send" /></p>
    </form>
</body></html>
