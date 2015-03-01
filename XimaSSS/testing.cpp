#include "StdAfx.hpp"
#include "Globals.func.hpp"

void test(string str)
{
	MessageBox(0, str.c_str(), "Note", MB_OK);
}

void test(int i)
{
	test(itos(i));
}