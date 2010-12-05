#!/bin/sw

void myPrint(String $string)
{
	IO.Print($string);
}

Integer	$val;

$val = 1337;
IO.Print("$val = " + (String)$val + "\n");

$val = $val ^ 0x1337;

myPrint("$val = " + (String)$val + "\n");
