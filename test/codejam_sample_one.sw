#!/usr/bin/sw
#<?php

// This script is to solve the first sample problem in the google CodeJam competion
// http://code.google.com/codejam/contest/351101/dashboard#s=p0

Integer $test_case_count = (Integer)IO.ReadLine();
IO.Print((String)$test_case_count + " text cases\n");

Integer	$case;

for($case = 1; $case <= $test_case_count; $case ++ )
{
	Integer $credits, $item_count;

	Integer	$i, $j;

	IO.Print("Case #" + (String)$case);

	$credits = (Integer)IO.ReadLine();
	$item_count = (Integer)IO.ReadLine();
	
	String	$item_costs_s[] = Lang.Strings.Split( IO.ReadLine(), " " );

	Integer	$item_costs[$item_count];
	for( $i = 0; $i < $item_count; $i ++ )
	{
		$item_costs[$i] = (Integer)$item_costs_s[$i];
	}
	
	for( $i = 0; $i < $item_count; $i ++ )
	{
		// Start from $i because the rest has been covered
		for( $j = $i + 1; $j < $item_count; $j ++ )
		{
			if( $item_costs[$i] + $item_costs[$j] == $credits )
			{
				IO.Print(": " + (String)($i+1) + " " + (String)($j+1) + "\n");
			}
		}
	}
}

#?>

