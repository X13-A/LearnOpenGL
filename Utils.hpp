#ifndef UTILS_HPP
#define UTILS_HPP
#include <vector>
#include <cstdlib>  // for rand() and srand()
#include <ctime>    // for time()

namespace Utils
{
	/// <summary>
	/// Does not work on pointer vectors
	/// </summary>
	template <typename T>
	static T clone(const std::vector<T>& vec)
	{
		return std::vector<T>(vec);
	}

	static void printVec3(const glm::vec3 vec)
	{
		std::cout << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
	}

	static void initRandom()
	{
		srand(static_cast<unsigned int>(time(nullptr)));
	}

	static float randomFloat(float min, float max)
	{
		float res = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		res *= max - min;
		res += min;
		return res;
	}
}

#endif