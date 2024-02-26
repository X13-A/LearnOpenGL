#ifndef COMPUTE_SHADER_HPP
#define COMPUTE_SHADER_HPP

#include "Commons.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ComputeShader
{
public:
    GLuint ID;

    ComputeShader(const char* path)
    {
        std::string computeShaderSource = readShaderSource(path);
        ID = glCreateShader(GL_COMPUTE_SHADER);

        const char* computeShaderCode = computeShaderSource.c_str();
        glShaderSource(ID, 1, &computeShaderCode, nullptr);
        glCompileShader(ID);
        checkCompileErrors("COMPUTE");
    }

private:
    std::string readShaderSource(const char* shaderPath)
    {
        std::string shaderCodeStr;
        std::ifstream shaderFile;

        // ensure ifstream object can throw exceptions:
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open file
            shaderFile.open(shaderPath);
            std::stringstream shaderStream;
            // read file's buffer content into stream
            shaderStream << shaderFile.rdbuf();
            // close file handler
            shaderFile.close();
            // convert stream into string
            shaderCodeStr = shaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }

        return shaderCodeStr;
    }

    void checkCompileErrors(std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(ID, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n" << std::endl;
            }
        }
        else
        {
            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(ID, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n" << std::endl;
            }
        }
    }
};
#endif