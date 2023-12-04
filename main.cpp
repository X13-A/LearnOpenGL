#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
char* readShaderSource(const char* shaderPath)
{
    std::string shaderCode;
    std::ifstream shaderFile;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // Open files
        shaderFile.open(shaderPath);
        std::stringstream shaderStream;

        shaderStream << shaderFile.rdbuf();

        // Close file handler
        shaderFile.close();
        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure& e) 
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        return nullptr;
    }

    char* shaderSource = new char[shaderCode.size() + 1];
    std::copy(shaderCode.begin(), shaderCode.end(), shaderSource);
    shaderSource[shaderCode.size()] = '\0';

    return shaderSource;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main()
{
    unsigned int BASE_WIDTH = 800;
    unsigned int BASE_HEIGHT = 600;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(BASE_WIDTH, BASE_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, BASE_WIDTH, BASE_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    float vertices[] =
    {
        // Triangle 1
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,

         // Triangle 2
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f    
    };

    unsigned int indices[] = 
    {
        0, 1, 3,
        1, 2, 3 
    };

    // Indice buffer
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // Vertex buffers
    unsigned int VBO_1;
    glGenBuffers(1, &VBO_1);

    unsigned int VBO_2;
    glGenBuffers(1, &VBO_2);

    // Vertex array object (stores how to draw the vertices)
    unsigned int VAO_1;
    glGenVertexArrays(1, &VAO_1);

    unsigned int VAO_2;
    glGenVertexArrays(1, &VAO_2);

    // As the VAO is bound, it will store all settings related to the VBO and EBO
    glBindVertexArray(VAO_1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO_2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    #pragma region shaders

    char* vertexShaderSource = readShaderSource("C:/Git Projects/LearnOpenGL/shaders/vertex_shader.glsl");
    char* fragmentShaderBlueSource = readShaderSource("C:/Git Projects/LearnOpenGL/shaders/fragment_shader_1.glsl");
    char* fragmentShaderRedSource = readShaderSource("C:/Git Projects/LearnOpenGL/shaders/fragment_shader_2.glsl");

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShaderRed;
    fragmentShaderRed = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderRed, 1, &fragmentShaderRedSource, NULL);
    glCompileShader(fragmentShaderRed);

    unsigned int fragmentShaderBlue;
    fragmentShaderBlue = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderBlue, 1, &fragmentShaderBlueSource, NULL);
    glCompileShader(fragmentShaderBlue);

    int  success;
    char vertexInfoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, vertexInfoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vertexInfoLog << std::endl;
    }
    char redFragmentInfoLog[512];
    glGetShaderiv(fragmentShaderRed, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderRed, 512, NULL, redFragmentInfoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << redFragmentInfoLog << std::endl;
    }
    char blueFragmentInfoLog[512];
    glGetShaderiv(fragmentShaderBlue, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderBlue, 512, NULL, blueFragmentInfoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << blueFragmentInfoLog << std::endl;
    }

    unsigned int shaderProgramBlue;
    shaderProgramBlue = glCreateProgram();
    glAttachShader(shaderProgramBlue, vertexShader);
    glAttachShader(shaderProgramBlue, fragmentShaderBlue);
    glLinkProgram(shaderProgramBlue);

    unsigned int shaderProgramRed;
    shaderProgramRed = glCreateProgram();
    glAttachShader(shaderProgramRed, vertexShader);
    glAttachShader(shaderProgramRed, fragmentShaderRed);
    glLinkProgram(shaderProgramRed);

    delete vertexShaderSource;
    delete fragmentShaderBlueSource;
    delete fragmentShaderRedSource;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaderBlue);

    int timeUniformLocation = glGetUniformLocation(shaderProgramBlue, "time");


#pragma endregion

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgramBlue);
        glUniform1f(timeUniformLocation, glfwGetTime());
        glBindVertexArray(VAO_1);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*) 0);


        glUseProgram(shaderProgramRed);
        glUniform1f(timeUniformLocation, glfwGetTime());
        glBindVertexArray(VAO_2);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*) (3 * sizeof(unsigned int)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}