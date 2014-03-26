using System;
 
class PrintTest
{
    public static void Main()
    {
        Console.WriteLine("文字数を数えます、何か入力して Enter");
        var s = Console.ReadLine();
        Console.WriteLine("{0} は {1} 文字です", s, s.Length);
    }
}
