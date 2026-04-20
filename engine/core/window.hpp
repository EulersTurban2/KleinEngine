#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine{
    namespace Core{

        enum WINDOW_STATE {
            FAILURE = -1,
            SUCCESS = 0
        };

        class Window {

            public:
                Window(int width, int height, const std::string& title): _width(width), _height(height), _title(title) {};
                ~Window() {};


                // zabranjujemo kopiranje prozora, jer ne zelimo da se desi da imamo 
                // vise instanci prozora koje dele isti GLFWwindow* pointer, sto bi dovelo do problema sa resursima i stabilnoscu
                Window(const Window&) = delete;
                Window& operator=(const Window&) = delete;

                WINDOW_STATE init();
                GLFWwindow* return_ref() const;

            private:

                std::string _title;
                int _width, _height;

                GLFWwindow* window;
        };
    }
}
#endif // __WINDOW_HPP__