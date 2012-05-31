#!/usr/bin/sw

class TestClass
{
	Integer	$v;	

	void __constructor()
	{
		// Do something
		$this->v = 0;
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
}

TestClass $tc = new TestClass();
$tc->method_1(100, 1, 1);
IO.Print("Final = " + $tc->method_1(0, 10, 5) + "\n");

$tc = null;

