#include "GL/glew.h"
#include "GL/glut.h"
#include <iostream>
#include <cstring>
#include <vector>
#include <functional>



void createContext(int argc, char* argv[], const char* windowTitle) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize(400, 300);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(windowTitle);
}

void initializeGL() {
	glewExperimental = GL_TRUE;
	glewInit();
}

GLuint initializeBuffers(float* vertices, size_t dataSize) {
    GLuint vbo = 0;
    glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, dataSize, vertices, GL_STATIC_DRAW);
    return vbo;
}

GLuint initShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    GLint len[1] = {(GLint)strlen(source)};
	glShaderSource(shader, 1, &source, len);
	glCompileShader(shader);

    GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint logSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
		std::vector<char> errorLog(logSize+1, 0);
		glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]);
		std::string s(errorLog.begin(), errorLog.end());
		std::cout << "Error compiling shader[" << s << "]\n";
        glDeleteShader(shader);
	}
    return shader;
}

GLuint initializeProgram() {
    const char* vertexSource = 
    "#version 330\n"
    "in vec2 position;\n"
    "void main()\n"
    "{\n"
        "gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\n";

    const char* fragmentSource = 
	"#version 330\n"
	"in vec2 position\n;"
	"void main() {\n"
	"	gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
	"}\n";

    GLuint vertexShader = initShader(vertexSource, GL_VERTEX_SHADER), 
        fragmentShader = initShader(fragmentSource, GL_FRAGMENT_SHADER), 
        shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	GLint isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE) {
		GLint logSize = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logSize);
		std::vector<char> errorLog(logSize+1, 0);
		glGetProgramInfoLog(shaderProgram, logSize, &logSize, &errorLog[0]);
		std::string s(errorLog.begin(), errorLog.end());
		std::cout << s << "\n";	
	}
    return shaderProgram;
}

namespace globals {
    GLuint program = 0;
    void draw() {
        glUseProgram(program);	
        GLint posAttrib = glGetAttribLocation(program, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glutSwapBuffers();
    }
}

int main(int argc, char* argv[]) {
		
    // create OpenGL context
	createContext(argc, argv, "Hello Modern OpenGL");

    // initialize GLEW
    initializeGL();

    // initialize buffers
    float vertices[] = {
        0.0f,  0.5f, // Vertex 1 (X, Y)
        0.5f, -0.5f, // Vertex 2 (X, Y)
        -0.5f, -0.5f  // Vertex 3 (X, Y)
    };
    GLuint vbo = initializeBuffers(vertices, sizeof(vertices));

    // initialize shader program
    globals::program = initializeProgram();

    // set display function
	glutDisplayFunc(globals::draw);
	glutMainLoop();

	return 0;
}
