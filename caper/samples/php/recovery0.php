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

require_once('recovery0_parser.php');

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
                return \rec\Token::token_eof;
            }
        } while (preg_match('/\s/', $c));

        switch ($c)
        {
        case '(':
            return \rec\Token::token_LParen;
        case ')':
            return \rec\Token::token_RParen;
        case ',':
            return \rec\Token::token_Comma;
        case '*':
            return \rec\Token::token_Star;
        default:
            if ($c == NULL) {
                return \rec\Token::token_eof;
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
                return \rec\Token::token_Number;
            }
        }
        trigger_error(sprintf("bad input char '%s'(%d)\n", $c, ord($c)), E_USER_ERROR);
        return \rec\Token::token_eof;
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
    
    function PackList($x)
    {
        global $output;
        $output .= "list: " . (string)$x . "\n";
        return $x;
    }

    function PackListError()
    {
        global $output;
        trigger_error("catching error");
        return -1;
    }

    function MakeList($n)
    {
        return $n;
    }

    function AddToList($m, $n)
    {
        return $m + $n;
    }
}

header('Content-Type: text/html');

if (isset($_POST['input'])) {
    $input = $_POST['input'];
    $s = new Scanner($input);
    $parser = new rec\Parser(new SemanticAction());

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
<title>recovery0</title>
</head>
<body>
    <h1>recovery0</h1>
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
    <form action="recovery0.php" method="post">
        <p>Input: <input type="text" name="input" value="" /></p>
        <p><input type="submit" value="Send" /></p>
    </form>
</body></html>
