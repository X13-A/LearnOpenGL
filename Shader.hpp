#ifndef SHADER_H
#define SHADER_H

#include "Commons.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:
    const char* _vertexPath;
    const char* _fragmentPath;
    GLuint _vertexID;
    GLuint _fragmentID;

public:
    GLuint ID;

    Shader(const char* vertexPath, const char* fragmentPath) : _vertexPath(vertexPath), _fragmentPath(fragmentPath)
    {
        compileShaders();
    }

    void compileShaders() 
    {
        // Compile vertex shader
        std::string vShaderString = readShaderSource(_vertexPath);
        const char* vShaderCode = vShaderString.c_str();
        _vertexID = compileShader(vShaderCode, GL_VERTEX_SHADER);

        // Compile fragment shader
        std::string fShaderString = readShaderSource(_fragmentPath);
        const char* fShaderCode = fShaderString.c_str();
        _fragmentID = compileShader(fShaderCode, GL_FRAGMENT_SHADER);

        // Link shaders
        ID = glCreateProgram();
        glAttachShader(ID, _vertexID);
        glAttachShader(ID, _fragmentID);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        // Detach shaders after linking (optional but often done)
        glDetachShader(ID, _vertexID);
        glDetachShader(ID, _fragmentID);
    }

    GLuint compileShader(const char* shaderCode, GLenum shaderType)
    {
        unsigned int shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderCode, NULL);
        glCompileShader(shader);
        checkCompileErrors(shader, shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
        return shader;
    }

    void reload()
    {
        std::cout << "Reloading shader : " << _vertexPath << ", " << _fragmentPath << std::endl;
        // Detach and delete old shaders
        glDetachShader(ID, _vertexID);
        glDetachShader(ID, _fragmentID);
        glDeleteShader(_vertexID);
        glDeleteShader(_fragmentID);

        // Recompile shaders
        compileShaders();
    }

    void use()
    {
        glUseProgram(ID);
    }
    
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec4(const std::string& name, glm::vec4 value) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), value.r, value.g, value.b, value.a);
    }

    void setVec2(const std::string& name, glm::vec2 value) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), value.r, value.g);
    }


    void setVec3(const std::string& name, glm::vec3 value) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value.r, value.g, value.b);
    }

    void setMat4(const std::string& name, glm::mat4 value)
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
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

    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n" << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n" << std::endl;
            }
        }
    }
};
#endif