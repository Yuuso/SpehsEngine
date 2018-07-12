#include "stdafx.h"
#include "SpehsEngine/Math/Mathematics.h"


namespace se
{
	int factorial(int number)
	{
		return number <= 1 ? 1 : factorial(number - 1) * number;
	}
}