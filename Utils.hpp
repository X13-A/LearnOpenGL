#ifndef UTILS_HPP
#define UTILS_HPP
#include <vector>

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
}

#endif