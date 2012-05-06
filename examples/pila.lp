int f()
{
	return 10;
}

void main()
{
	int i = 0;
	while (i < 100)
	{
		f(); //El codigo generado quita el resultado de la pila automaticamente si nadie lo asigna.
		i = i + 1;
	}
}
