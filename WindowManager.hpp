#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include "Commons.hpp"
#include <iostream>
#include "Color.hpp"
#include "Singleton.hpp"
#include "InputManager.hpp"

class WindowManager : public Singleton<WindowManager>
{
    private:
        unsigned int width;
        unsigned int height;
        GLFWwindow* window;

    public:
        GLFWwindow* getWindow();

        unsigned int getWidth() const;

        unsigned int getHeight() const;

        static void init(unsigned int width, unsigned int height, const char* name);

        void setTitle(const char* title);

        void clear(Color color);

        void checkForEvents();

        void close();

        void update();
};

#endif