#!/bin/sw
#<?php

Real pow(Real $value, Integer $power)
{
	Real $ret;
	
	if( $value == 0.0 )	return 0.0;
	if( $power == 0 )	return 1.0;
	
	$ret = 1.0;
	if( $power > 0 )
	{
		do {
			$ret *= $value;
		} while($power -= 1);
	}
	else
	{
		do {
			$ret /= $value;
		} while($power += 1);
	}
	
//	IO.Print("$ret = " + (String)$ret + "\n");
	return $ret;
}

Integer $i;
Integer $pow;
Real	$val;

for( $i = 1; $i <= 100; $i += 1 )
{
	//IO.Print( "$i = " + (String)$i + "\n" );
	for( $pow = -3; $pow <= 3; $pow += 1 )
	//for( $pow = 0; $pow <= 5; $pow += 1 )
	{
		$val = pow((Real)$i, $pow);
		IO.Print( " " + (String)$i + "^" + (String)$pow + " = " + (String)$val + "\n" );
	}
}

#?>
