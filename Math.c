#include "Math.h"

int DivRoundUp(int Dividend, int Divisor)
{
	return (Dividend + Divisor - 1) / Divisor;
}

int Log2(int Value)
{
	int Result = 0;
	while (Value >>= 1)
	{
		Result++;
	}
	return Result;
}

int IsPowerOfTwo(int Value)
{
	return Value && !(Value & (Value - 1));
}

int RoundUpToPowerOfTwo(int Value)
{
	if (IsPowerOfTwo(Value))
	{
		return Value;
	}
	int Result = 1;
	while (Result < Value)
	{
		Result <<= 1;
	}
	return Result;
}
