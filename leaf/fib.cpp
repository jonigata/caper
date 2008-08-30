// 2008/08/14 Naoyuki Hirayama

#include <cstdio>
#include <cstdlib>

int fib( int n )
{
	switch(n){
	case 1:
	case 2:
		return 1;
	default:
		return fib(n-1)+fib(n-2);
	}
}

int main( int argc, char** argv )
{
	int n = atoi( argv[1] );
	printf( "%d\n", fib(n) );
}
