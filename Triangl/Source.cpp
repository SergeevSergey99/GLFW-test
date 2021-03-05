#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

std::string extractSource(const char* filename)
{
    std::string ShaderCode;
    std::ifstream ShaderStream(filename, std::ios::in);

    if (ShaderStream.is_open())
    {
        std::string line = "";
        while (std::getline(ShaderStream, line))
        {
            ShaderCode += "\n" + line;
        }
        ShaderStream.close();
    }
    return ShaderCode;
}
unsigned int compileShader(unsigned int type, std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = new char[lenght];
        glGetShaderInfoLog(id, lenght, &lenght, message);

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "verte" : "fragment") << " shader" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        delete [] message;
    }
    return id;
}

unsigned int loadShader(std::string vertexFile, std::string fragmentFile)
{
    unsigned int program = glCreateProgram();

    std::string vsCode = extractSource(vertexFile.c_str());
    std::string fsCode = extractSource(fragmentFile.c_str());

    unsigned int vsId = compileShader(GL_VERTEX_SHADER, vsCode);
    unsigned int fsId = compileShader(GL_FRAGMENT_SHADER, fsCode);

    glAttachShader(program, vsId);
    glAttachShader(program, fsId);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vsId);
    glDeleteShader(fsId);

    return program;
}
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        return - 1;
    }

    std::vector<glm::vec3> verticles;
    verticles.push_back(glm::vec3(-0.5, -0.5, 0.0));
    verticles.push_back(glm::vec3(0.5, -0.5, 0.0));
    verticles.push_back(glm::vec3(0.0, 0.5, 0.0));
/*    float positions[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
    };*/

    unsigned int bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, verticles.size() * sizeof(glm::vec3), &verticles[0], GL_STATIC_DRAW);
   //glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, verticles.size(), GL_FLOAT, GL_FALSE, 0, (void*)0);

    unsigned int id = loadShader("shaderV", "shaderF");
    glUseProgram(id);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, verticles.size());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glUseProgram(0);
    glDeleteProgram(id);
    glfwTerminate();
    return 0;
}