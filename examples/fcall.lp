void f(int s1, bool s2, float s3, string s4)
{
	print(toString(s1) + s4);
	return;
}

void g(string s1, string s2)
{
	print(s1 + s2);
	return;
}

void main()
{
	f(10, true, 2.1, "Hola");
	g("Hola ", "Mundo");
}
