#pragma once

#include <random>
#include <time.h>
#include <limits>


/// <summary>
/// Small random class, providing some faster/better random functions.
/// </summary>
class Random
{
public:
	Random();

	/// <summary>
	/// Initialises the RNG with random values (using std::random)
	/// </summary>
	void Init();

	/// <summary>
	/// Faster random function than rand(): https://stackoverflow.com/a/1640399
	/// </summary>
	unsigned long Xorshf96();

	/// <summary>
	/// Returns a random number between 0 and 1 using xorshf96().
	/// </summary>
	/// <returns></returns>
	float Xorshf96_01();

private:
	// Random start values
	unsigned long x = 0;
	unsigned long y = 0;
	unsigned long z = 0;
};


