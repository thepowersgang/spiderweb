#!/usr/bin/sw
#<?php

class TestClass
{
	Integer	$v;	

	void __constructor()
	{
		// Do something
		$this->v = 99;
	}
	
	void __destructor()
	{
		// Do something else
	}
	
	Integer method_1(Integer $a, Integer $b, Integer $c)
	{
		$this->v += $a;
		$this->v *= $b;
		$this->v /= $c;
		IO.Print("$this->v = " + (String)$this->v + "\n");
		return $this->v;
	}
	
	Integer operator %(Integer $other) const
	{
		return $this->v % $other;
	}
}

//Integer operator /(const TestClass $v1, const Integer $other)
//{
//	return $v1->v / $other;
//}

TestClass $tc();
$tc->method_1(100, 1, 1);
IO.Print("Final = " + $tc->method_1(0, 10, 5) + "\n");
IO.Print("Operator override = " + $tc % 12 + "\n");

//$tc = null;

# vim: ft=php

