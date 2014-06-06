import System
import calc

class SemanticAction (ISemanticAction):
  def syntax_error() as void:
    pass
  def stack_overflow() as void:
    pass

  def MakeSub( ref arg0 as int, arg1 as int, arg2 as int) as void:
    Console.Write("$arg0 $arg1 $arg2\n")
    arg0 = arg1 - arg2
  def MakeMul( ref arg0 as int, arg1 as int, arg2 as int) as void:
    Console.Write("$arg0 $arg1 $arg2\n")
    arg0 = arg1 * arg2
  def MakeDiv( ref arg0 as int, arg1 as int, arg2 as int) as void:
    Console.Write("$arg0 $arg1 $arg2\n")
    arg0 = arg1 / arg2
  def MakeAdd( ref arg0 as int, arg1 as int, arg2 as int) as void:
    Console.Write("$arg0 $arg1 $arg2\n")
    arg0 = arg1 + arg2
  def Identity( ref arg0 as int, arg1 as int) as void:
    Console.Write("$arg0 $arg1\n")
    arg0 = arg1



def Main():
  sa = SemanticAction()
  a as int
  sa.MakeSub(a, 1, 2)
  parser = calc.Parser(sa)
  parser.post(calc.Token.token_Number, 1)
  parser.post(calc.Token.token_Add, 0)
  parser.post(calc.Token.token_Number, 1)
  parser.post(calc.Token.token_Mul, 0)
  parser.post(calc.Token.token_Number, 1)
  parser.post(calc.Token.token_eof, 0)
  v as object
  if parser.accept(v):
    Console.Write("answer = ${v cast int}\n")
