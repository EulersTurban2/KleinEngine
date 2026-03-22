#ifndef __APP_HPP
#define __APP_HPP

#include <core/window.hpp>
#include <core/shaders.hpp>
#include <core/platform.hpp>

// Poenta jeste da APP bude samo petlja u kojoj se crtaju i definisu stvari, van toga ne bi trebalo ista da se nalazi!
namespace App {
    class App {
        public:
        App(){
            //TODO: read from a json file, everything 
            window = new Engine::Resources::Window(800,600,"test");
            window->init();   
        }

        ~App(){
            delete window;
        }

        void run(){
            GLFWwindow* ref = window->return_ref();

            Engine::Resources::Shader* vertexShader = new Engine::Resources::Shader("../resources/shaders/vertex/vert1.glsl", Engine::Resources::VERTEX_SHADER);
            Engine::Resources::Shader* fragmentShader = new Engine::Resources::Shader("../resources/shaders/fragment/frag1.glsl", Engine::Resources::FRAGMENT_SHADER);
            std::vector<Engine::Resources::Shader*> shaders = {vertexShader, fragmentShader};
            Engine::Resources::ShaderProgram* shaderProgramObj = new Engine::Resources::ShaderProgram(shaders);
            

            unsigned int VBO,VAO,EBO;

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
    

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);            

            glBindBuffer(GL_ARRAY_BUFFER,0);
            glBindVertexArray(0);

            while (!glfwWindowShouldClose(ref))
            {
                Engine::Platform::processInput(ref);
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                
                shaderProgramObj->use();
                glBindVertexArray(VAO);
                //glDrawArrays(GL_TRIANGLES, 0, 6);
                glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                
                glfwSwapBuffers(ref);
                glfwPollEvents();
            }

            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            delete shaderProgramObj;

            glfwTerminate();
        }
        private:
        Engine::Resources::Window* window;

        // tmp
        float vertices[12] = {
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left 
        };
        unsigned int indices[6] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };
    };    
}


#endif // __APP_HPP