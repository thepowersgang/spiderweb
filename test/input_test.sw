#!/usb/bin/sw
#<?php

if( 0 )
{
	String $line = "";
	
	while( $line != "Hello\n" )
	{
		$line = IO.ReadLine();
		IO.Print("You said: " + $line);
	}
}

if( 1 )
{
	String $input = "";
	Integer $output;

	IO.Print("Please enter a number: ");
	$input = IO.ReadLine();
	$output = (Integer)$input;
	$output *= 4;
	IO.Print("4 * your input is " + $output + "\n");
}

#?>
