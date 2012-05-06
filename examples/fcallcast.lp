void func (float f)
{
	 print(toString(trunc(f)));
}

void func2(float f, int i)
{
}

void main()
{
	int i = 1;
	func(i);
	//func2(2); //Error, el numero de argumentos no coincide con la llamada
	func2(1,2);
}
