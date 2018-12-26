#include "GL/glew.h"
#include "GL/glut.h"

// This is a new block for including GLM for our camera matrices
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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
    "uniform mat4 mvp;\n"
    "void main()\n"
    "{\n"
        "gl_Position = mvp * vec4(position, 0.0, 1.0);\n"
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


struct camera_t {
    glm::mat4 modelMatrix;
    glm::mat4 projMatrix;
    glm::mat4 viewMatrix;

    camera_t():
        modelMatrix(glm::identity<glm::mat4>()),
        projMatrix(glm::perspective(50.0, 1.3, .01, 10000.0)),
        viewMatrix(glm::lookAt(glm::vec3(0.0, 0.0, -20.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)))
    {
    }

    void update() {
        const float theta = 0.005f;
        modelMatrix = glm::rotate(modelMatrix, theta, glm::vec3(1.0, 0.0, 0.0));
    }

    glm::mat4 get() const {
        return projMatrix * viewMatrix * modelMatrix;
    }
};


namespace globals {
    GLuint program = 0;
    camera_t cam;
    
   
    void draw() {
        // bind program
        glUseProgram(program);	
        
        // copy model-view-projection matrix
        GLuint loc = glGetUniformLocation(program, "mvp");
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(cam.get()));

        // associate vertex attribute 
        GLint posAttrib = glGetAttribLocation(program, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glutSwapBuffers();

        cam.update();
        glutPostRedisplay();
        glClear(GL_COLOR_BUFFER_BIT);
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