#!/bin/sw

void myPrint(String $string)
{
	IO.Print($string);
}

void printl(String $string)
{
	IO.Print($string);
	IO.Print("\n");
}

Integer	$val;

$val = 1337;
IO.Print("$val = " + (String)$val + "\n");

$val = $val ^ 0x1337;

printl("$val = " + (String)$val);

IO.Print("$val =");
for( $val = 0; $val < 10; $val += 1 )
{
	myPrint(" " + $val);
}
IO.Print("\n");

return 20;
