// Calculate the Fibonacci number for n
int fibo(int n)
{
	if (n == 0)
	{
		return 0;
	}

	if (n == 1)
	{
		return 1;
	}

	return fibo(n-1) + fibo(n-2);
}

// Program fibo.lp
// Read a number, calculate the Fibonacci number for said number
// and print a message with the result.
void main()
{
	print("Enter number to calculate the Fibonacci number for:");
	string nStr = read();

	int f = fibo(toInt(nStr));
	string s = toString(f);

	print("The Fibonacci number for " + nStr + " is: " + s);
}

