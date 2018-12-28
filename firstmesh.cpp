#include "GL/glew.h"
#include "GL/glut.h"

// This is a new block for including GLM for our camera matrices
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// to read a mesh
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <functional>


struct mesh_t {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    Assimp::Importer importer;
    explicit mesh_t(const std::string& scenePath) {
        const aiScene* scene = importer.ReadFile(scenePath.c_str(), 
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

        auto pz = 0;
        auto nindices = 0;
        if (scene->HasMeshes()) {
            for (unsigned int i(0); i < scene->mNumMeshes; ++i) {
                pz += scene->mMeshes[i]->mNumVertices * 3;
                nindices += scene->mMeshes[i]->mNumFaces* 3;
            }
            vertices.resize(pz);
            indices.resize(nindices);
            size_t q = 0, c = 0;
            for (unsigned int i(0); i < scene->mNumMeshes; ++i) {
                if (scene->mMeshes[i]->mNumVertices > 0) {
                    for (unsigned int v(0); v < scene->mMeshes[i]->mNumVertices; ++v) {
                        aiVector3D const& p = scene->mMeshes[i]->mVertices[v];
                        vertices[q++] = p.x;
                        vertices[q++] = p.y;
                        vertices[q++] = p.z;
                    }
                    for (unsigned int f(0); f < scene->mMeshes[i]->mNumFaces; ++f) {
                        const aiFace& face = scene->mMeshes[i]->mFaces[f];
                        indices[c++] = face.mIndices[0];
                        indices[c++] = face.mIndices[1];
                        indices[c++] = face.mIndices[2];
                    }
                }
            }
        }
    }
};

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
    glEnable(GL_DEPTH_TEST);
}

std::pair<GLuint, GLuint> initializeBuffers(
                            float* vertices, 
                            size_t vertexArraySize, 
                            unsigned int* vertexIndices, 
                            size_t vertexIndexArraySize) {
    // vertex buffer object
    GLuint vbo = 0;
    glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glNamedBufferData(vbo, vertexArraySize, vertices, GL_STATIC_DRAW);

    // element buffer object
    GLuint ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glNamedBufferData(ebo, vertexIndexArraySize, vertexIndices, GL_STATIC_DRAW);

    return std::make_pair(vbo, ebo);
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
    "#version 330 core\n"
    "in vec3 position;\n"
    "out vec3 rgb;\n"
    "uniform mat4 mvp;\n"
    "void main()\n"
    "{\n"
        "gl_Position = mvp * vec4(position, 1.0);\n"
        "vec3 p = position;\n"
        "if (p.z > 0) {\n"
            "rgb = vec3(1.0, 1.0, p.z);\n"
        "} else {\n"
            "rgb = vec3(-p.z, 0.0, 0.0);\n"
        "}\n"
    "}\n";

    const char* fragmentSource = 
	"#version 330 core\n"
    "in vec3 vpos;\n"
    "in vec3 rgb;\n"
    "out vec4 color;\n"
	"void main() {\n"
	"	color = vec4(rgb, 1.0);\n"
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
        projMatrix(glm::perspective(50.0, 1.3, .001, 10000.0)),
        viewMatrix(glm::lookAt(glm::vec3(0.0, 0.0, -20.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)))
    {
    }

    void update() {
        const float theta = 0.005f;
        modelMatrix = glm::rotate(modelMatrix, theta, glm::vec3(0.3, 0.3, 0.0));
    }

    glm::mat4 get() const {
        return projMatrix * viewMatrix * modelMatrix;
    }
};


namespace globals {
    GLuint program = 0;
    camera_t cam;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLint numElems = 0;    
   
    void draw() {
        // bind program
        glUseProgram(program);

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // copy model-view-projection matrix
        GLuint loc = glGetUniformLocation(program, "mvp");
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(cam.get()));

        // associate vertex attribute 
        GLint posAttrib = glGetAttribLocation(program, "position");
        glEnableVertexAttribArray(posAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // bind element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);  

        GLint isize = 0;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &isize);
        numElems = isize / sizeof(unsigned int);

        // draw elements  
        glDrawElements(GL_TRIANGLES, numElems, GL_UNSIGNED_INT, 0);
        cam.update();
        glutSwapBuffers();
        glutPostRedisplay();

        glUseProgram(0);
    }

}

int main(int argc, char* argv[]) {
		
    // create OpenGL context
	createContext(argc, argv, "Hello Modern OpenGL");

    // initialize GLEW
    initializeGL();

    // initialize buffers
    mesh_t mesh("data/cube.obj");
    auto bufs = initializeBuffers(
        mesh.vertices.data(),
        mesh.vertices.size() * sizeof(float),
        &mesh.indices[0],
        mesh.indices.size() * sizeof(unsigned int)
    );

    globals::ebo = bufs.second;
    globals::vbo = bufs.first;

    // initialize shader program
    globals::program = initializeProgram();

    // set display function
	glutDisplayFunc(globals::draw);
	glutMainLoop();

	return 0;
}