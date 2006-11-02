void main()
{
	//Hola soy un comentario
	bool bEnd = false;
	int nIndex = 7;

	while (!bEnd)
	{
		bool bLocal = true;
		nIndex = nIndex + 2;
		if (nIndex > 20 && nIndex < 30)
		{
			print("It ends here");
			bEnd = true;			
		}
		else
		{
			nIndex = nIndex - 1;
		}
	}
	print (toString(nIndex));
	//bEnd = bLocal;
	//bEnd = nIndex + 3;
	//bStart = !bEnd;
	//bool bEnd;
	//int bEnd;
}
