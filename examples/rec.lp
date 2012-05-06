void f(int i)
{
	if (i < 10)
	{
		print(toString(i));
		f(i + 1);
	}
	return;
}

void main()
{
	f(0);
}
