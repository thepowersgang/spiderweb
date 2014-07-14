#!/bin/sw
#<?php	// Hijack PHP's syntax hilighting

#
# This file is mostly to test the language features of SpiderScript
# - And some of the SpiderWeb bindings
#

// Test function 1
void printl(String $string)
{
	IO.Print($string);
	IO.Print("\n");
}

// Test function 2
Real some_random_function(Integer $func, Real $x)
{
	if( $func == 0 )
		return 8.0;
	else if ($func == 1)
		return $x / 4.0;
	else if ($func == 2)
		return $x * ($x + 4.0);
	else
		return ($x - 1.0) * ($x + 4.0) * ($x - 6.0);
}

// Testing Integer printing
Integer	$val = 1337;
IO.Print("$val = " + (String)$val + "\n");

// Testing Integer operations and function calls
$val = $val ^ 0x1337;
printl("$val = " + (String)$val);

// Test loops
IO.Print("$val =");
for ( $val = 0; $val < 10; $val ++ )
{
	IO.Print(" " + (String)$val);
}
IO.Print("\n");

// Test complex functions
{
	Real $x = 10.0;
	// Testing implicit casting
	printl(
		"f("
		+ $x
		+ ") = "
		+ some_random_function(3, $x)
		);
}

// Test objects and File IO
IO.File $fp("1.txt", "r");
if( $fp )
{
	IO.Print("First 100 bytes of '1.txt':\n");
	IO.Print( $fp->Read(100) );
	IO.Print("\n----\n");
}
else
{
	IO.Print("Oops, no file 1.txt\n");
}
delete $fp;

// Test garbage collection of objects
IO.Print("Writing a value to 2.txt...");
$fp = new IO.File("2.txt", "w");
$fp->Write("Hello There\n");
delete $fp;
IO.Print(" Done\n");

for<outer>( Integer $i = 0; $i < 10; $i ++ )
{
	IO.Print("\n$i = " + $i + ", ");
	// Test break and continue in loops
	for( $val = 0; $val < 20; $val ++ )
	{
		if($i == 8 && $val == 2)
			continue outer;
		
		if( $val > $i*2 )	break;
		if( $val > 5 && $val < 8)	continue;
		IO.Print(" " + $val);
	}
}
IO.Print("\n");

$val = 1234;
switch($val)
{
case 1233:
	IO.Print("un deux trois trois\n");
	break;
case 1234:
	IO.Print("un deux "+"trois 4\n");
case 1235:
	IO.Print("one two three five\n");
	break;
}
switch($val)
{
case 0:
	IO.Print("Oops.\n");
	break;
default:
	IO.Print("I care not\n");
}

// Test string optimisation
// - Literal grouping
IO.Print("Hello" + " " + "World");

// Test integer and float optimsations
$val = 1234 + 4321 / 5 - 100 * 10;
Real $fval = 1234.0 + 4321.0 / 5.0 - 100.0 * 10.0;
$val = 1234 + 4321.0 / 5 - 100.0 * 10;
$fval = 12.0 + 11;

//IO.Print("%i %i %i\n" % (1, 2, 3));

//namespace Lang
//{
	class Format
	{
		void __constructor(String $fmt)
		{
		}
		
		Format operator %(String $str)
		{
			return $this;
		}
		Format operator %(Integer $int)
		{
			return $this;
		}
	}
//}

String $formatted_string = new /*Lang.*/Format("Str %s int %i") % "hello" % 13;

// Test global return value
return 20;

#?>
# vim: ft=php
