#ifndef PERFORMANCE_TRACKER_H
#define PERFORMANCE_TRACKER_H

#include <GLFW/glfw3.h>
using namespace std;

class PerformanceTracker
{
	private: 
		float frameTime = 0;
		float delay = 1;
		std::size_t frames = 0;

	public:
		bool verbose = false;
		bool overrideTitle = false;
		float averageFPS;

		void update(GLFWwindow* window)
		{
			++frames;

			float time = glfwGetTime();
			if (time - frameTime >= delay)
			{
				averageFPS = frames / delay;
				frames = 0;
				if (verbose)
				{
					std::cout << "Average framerate: " << averageFPS << std::endl;
				}
				if (overrideTitle)
				{
					glfwSetWindowTitle(window, ("Average framerate: " + to_string((int)averageFPS)).c_str());
				}
				frameTime = glfwGetTime();
			}
		}
};

#endif
