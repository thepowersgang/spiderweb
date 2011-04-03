#!/bin/sw
#<?php	// Hijack PHP's syntax hilighting

void printl(String $string)
{
	IO.Print($string);
	IO.Print("\n");
}

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
	printl("f(" + $x + ") = " + some_random_function(3, $x));
}

// Test objects
Object $fp;	// TODO: Replace "Object" with a class definition
// Maybe: "Object(IO.File) $fp"
$fp = new IO.File("1.txt", "r");

IO.Print("First 100 bytes of '1.txt':\n");
IO.Print( $fp->Read(100) );
IO.Print("\n----\n");

// Test garbage collection of objects
IO.Print("Writing a value to 2.txt...");
$fp = new IO.File("2.txt", "w");
$fp->Write("Hello There\n");
$fp = null;
IO.Print(" Done\n");

Integer $i;
for<outer>( $i = 0; $i < 10; $i ++ )
{
	IO.Print("\n$i = " + $i + ", ");
	// Test break and continue in loops
	for( $val = 0; $val < 20; $val ++ )
	{
		if($i == 8 && $val == 2)
			continue outer;
		
		if( $val > 10 )	break;
		if( $val > 5 && $val < 8)	continue;
		IO.Print(" " + $val);
	}
}
IO.Print("\n");

// Test global return value
return 20;

#?>
