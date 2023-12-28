#ifndef PERFORMANCE_TRACKER_H
#define PERFORMANCE_TRACKER_H

#include "Commons.hpp"
#include "Singleton.hpp"

using namespace std;

class PerformanceTracker : public Singleton<PerformanceTracker>
{
	private: 
		float frameTime = 0;
		float delay = 1;
		float averageFPS;
		std::size_t frames = 0;

	public:
		bool verbose = false;
		bool overrideTitle = false;

		float getAverageFPS()
		{
			return averageFPS;
		}

		void update()
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
					std::string title = "Average framerate: " + to_string((int) averageFPS);
					WindowManager::getInstance().setTitle(title.c_str());
				}
				frameTime = glfwGetTime();
			}
		}
};

#endif
