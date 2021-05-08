#include "Random.h"

Random::Random()
{
	Init();
}

void Random::Init()
{
	std::srand(time(0));
	x = std::rand();
	y = std::rand();
	z = std::rand();
}

unsigned long Random::Xorshf96()
{
	// Period 2^96-1
	unsigned long t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

float Random::Xorshf96_01()
{
	return (float)Xorshf96() / (float)std::numeric_limits<unsigned long>::max();
}
