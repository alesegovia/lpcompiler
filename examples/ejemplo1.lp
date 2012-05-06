int factorial(int n)
{
	if (n <= 1)
		return 1;
	else
		return (n * factorial(n-1));
}

void main()
{
	int f = factorial(10);
	print("Factorial(10) = " + toString(f));

	if (f != 3628800)
	{
		print("Error!");
	}

}
