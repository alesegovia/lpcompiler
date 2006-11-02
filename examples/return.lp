int gimmeanint(int i)
{
	if (i > 2)
	{
		int g = gimmeanint(i - 1); //si nunca sacamos el tos, se rompe
	}
	return 2;
}

void main()
{
	int param = gimmeanint(3);
	print(toString(param));
}
