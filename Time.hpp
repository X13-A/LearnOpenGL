#ifndef TIME_H
#define TIME_H

#include "Commons.hpp"

class Time 
{
    private:
        static double lastTime;
        static double _deltaTime;

    public:
        /// <summary>
        /// Returns the time elapsed between the current and the last frame
        /// </summary>
        static double deltaTime() 
        {
            return _deltaTime;
        }

        /// <summary>
        /// Updates the time variables. Call this at the beginning of every frame.
        /// </summary>
        static void update() 
        {
            double currentTime = glfwGetTime();
            _deltaTime = currentTime - lastTime;
            lastTime = currentTime;
        }
};

double Time::lastTime = 0;
double Time::_deltaTime = 0;

#endif