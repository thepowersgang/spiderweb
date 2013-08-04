
void a(){}

Integer $i;
Integer $niter = 1000000;

for( $i = 0; $i < $niter; $i ++ )
{
	a();
}

IO.Print("Performed "+$niter+" function calls\n");

