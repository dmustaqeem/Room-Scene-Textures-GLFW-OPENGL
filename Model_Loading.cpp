
#include <glad/glad.h>
//#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>
#include <FreeImage.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
void loadTextures();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

const int numTextures = 6;

GLuint texID[numTextures];

const char* textureFileNames[numTextures];

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader("cube.vs", "cube.fs");
    Shader skyboxShader("sky.vs", "sky.fs");
    Shader modelShader("Vertex.vs", "Fragment.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };


    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };



    float floor[] =
    {
        -10.0, -1.0, -10.5, 1, 0, 0, 0,1,
         10.0, -1.0, -10.5, 1, 0, 0, 1,1,
        -10.0, -1.0,  4.5, 1, 0, 0,  0,0,

        -10.0, -1.0,  4.5, 1, 0, 0,  0,0,
         10.0, -1.0,  4.5, 1, 0, 0,  1,0,
         10.0, -1.0, -10.5, 1, 0, 0, 1,1
    };

    float roof[] =
    {
        -10.0, 5.0, -10.5, 1, 0, 0, 0,1,
         10.0, 5.0, -10.5, 1, 0, 0, 1,1,
        -10.0, 5.0,  4.5, 1, 0, 0,  0,0,

        -10.0, 5.0,  4.5, 1, 0, 0,  0,0,
         10.0, 5.0,  4.5, 1, 0, 0,  1,0,
         10.0, 5.0, -10.5, 1, 0, 0, 1,1
    };

    float wallLeft[] = 
    {
        -10.0, -1.0, -10.5, 1, 0, 0, 1,0,
        -10.0,  5.0, -10.5, 1, 0, 0, 1,1,
        -10.0,  5.0,  4.5,  1, 0, 0, 0,1,

        -10.0,  5.0,  4.5, 1, 0, 0,  0,1,
        -10.0, -1.0,  4.5, 1, 0, 0,  0,0,
        -10.0, -1.0, -10.5, 1, 0, 0, 1,0
    };
    float wallRight[] =
    {
         10.0, -1.0, -10.5, 1, 0, 0, 1,0,
         10.0,  5.0, -10.5, 1, 0, 0, 1,1,
         10.0,  5.0, -4.0,  1, 0, 0, 0,1,

         10.0,  5.0, -4.0, 1, 0, 0,  0,1,
         10.0, -1.0, -4.0, 1, 0, 0,  0,0,
         10.0, -1.0, -10.5, 1, 0, 0, 1,0,


         10.0, -1.0,  0.0, 1, 0, 0, 1,0,
         10.0,  5.0,  0.0, 1, 0, 0, 1,1,
         10.0,  5.0,  4.5,  1, 0, 0, 0,1,

         10.0,  5.0,  4.5, 1, 0, 0,  0,1,
         10.0, -1.0,  4.5, 1, 0, 0,  0,0,
         10.0, -1.0,  0.0, 1, 0, 0, 1,0,

         //Below Window
         10.0, -1.0,  -4.0, 1, 0, 0, 1,0,
         10.0,  1.0,  -4.0, 1, 0, 0, 1,1,
         10.0,  1.0,   0.0,  1, 0, 0, 0,1,

         10.0,  1.0,  0.0, 1, 0, 0,  0,1,
         10.0, -1.0,  0.0, 1, 0, 0,  0,0,
         10.0, -1.0,  -4.0, 1, 0, 0, 1,0,
         //Above Window
         10.0,  3.0,  -4.0, 1, 0, 0, 1,0,
         10.0,  5.0,  -4.0, 1, 0, 0, 1,1,
         10.0,  5.0,   0.0,  1, 0, 0, 0,1,

         10.0,  5.0,  0.0, 1, 0, 0,  0,1,
         10.0,  3.0,  0.0, 1, 0, 0,  0,0,
         10.0,  3.0,  -4.0, 1, 0, 0, 1,0,

    };

    float windowRight[] = 
    {
         10.0,  1.0,  -4.0, 1, 0, 0, 1,0,
         10.0,  3.0,  -4.0, 1, 0, 0, 1,1,
         10.0,  3.0,   0.0,  1, 0, 0, 0,1,

         10.0,  3.0,  0.0, 1, 0, 0,  0,1,
         10.0,  1.0,  0.0, 1, 0, 0,  0,0,
         10.0,  1.0,  -4.0, 1, 0, 0, 1,0,
    };

    float wallBack[] =
    {
          0.0, -1.0, -10.5, 1, 0, 0, 1,0,
          0.0,  5.0, -10.5, 1, 0, 0, 1,1,
        -10.0,  5.0, -10.5,  1, 0, 0, 0,1,

        -10.0,  5.0, -10.5, 1, 0, 0,  0,1,
        -10.0, -1.0, -10.5, 1, 0, 0,  0,0,
          0.0, -1.0, -10.5, 1, 0, 0, 1,0,

         10.0, -1.0, -10.5, 1, 0, 0, 1,0,
         10.0,  5.0, -10.5, 1, 0, 0, 1,1,
          5.0,  5.0, -10.5,  1, 0, 0, 0,1,

          5.0,  5.0, -10.5, 1, 0, 0,  0,1,
          5.0, -1.0, -10.5, 1, 0, 0,  0,0,
         10.0, -1.0, -10.5, 1, 0, 0, 1,0,

         //Below Window
          5.0, -1.0, -10.5, 1, 0, 0, 1,0,
          5.0,  1.0, -10.5, 1, 0, 0, 1,1,
          0.0,  1.0, -10.5,  1, 0, 0, 0,1,

          0.0,  1.0, -10.5, 1, 0, 0,  0,1,
          0.0, -1.0, -10.5, 1, 0, 0,  0,0,
          5.0, -1.0, -10.5, 1, 0, 0, 1, 0,

          //Above Window
          5.0,  3.0, -10.5, 1, 0, 0, 1,0,
          5.0,  5.0, -10.5, 1, 0, 0, 1,1,
          0.0,  5.0, -10.5,  1, 0, 0, 0,1,

          0.0,  5.0, -10.5, 1, 0, 0,  0,1,
          0.0,  3.0, -10.5, 1, 0, 0,  0,0,
          5.0,  3.0, -10.5, 1, 0, 0, 1, 0

    };

    float windowBack[] = 
    {
          5.0,  3.0, -10.5, 1, 0, 0, 1,0,
          5.0,  1.0, -10.5, 1, 0, 0, 1,1,
          0.0,  1.0, -10.5,  1, 0, 0, 0,1,

          0.0,  1.0, -10.5, 1, 0, 0,  0,1,
          0.0,  3.0, -10.5, 1, 0, 0,  0,0,
          5.0,  3.0, -10.5, 1, 0, 0, 1, 0
    };

    float wallFront[] =
    {
         10.0, -1.0, 4.5, 1, 0, 0, 1,0,
         10.0,  5.0, 4.5, 1, 0, 0, 1,1,
        -8.0,  5.0, 4.5,  1, 0, 0, 0,1,

        -8.0,  5.0, 4.5, 1, 0, 0,  0,1,
        -8.0, -1.0, 4.5, 1, 0, 0,  0,0,
         10.0, -1.0, 4.5, 1, 0, 0, 1,0,

         -8.0,  3.0, 4.5, 1, 0, 0, 1,0,
         -8.0,  5.0, 4.5, 1, 0, 0, 1,1,
        -10.0,  5.0, 4.5,  1, 0, 0, 0,1,

        -10.0,  5.0, 4.5, 1, 0, 0,  0,1,
        -10.0,  3.0, 4.5, 1, 0, 0,  0,0,
         -8.0,  3.0, 4.5, 1, 0, 0, 1,0

    };

    float door[] =
    {
         -8.0, -1.0, 4.5, 1, 0, 0, 1,0,
         -8.0,  3.0, 4.5, 1, 0, 0, 1,1,
        -10.0,  3.0, 4.5,  1, 0, 0, 0,1,

        -10.0,  3.0, 4.5, 1, 0, 0,  0,1,
        -10.0, -1.0, 4.5, 1, 0, 0,  0,0,
         -8.0, -1.0, 4.5, 1, 0, 0, 1,0
    };

    float tableLegs[] = 
    {
        //Front
         1.0, -0.9, -5.0, 1, 0, 0, 0, 0,
         1.0,  0.3, -5.0, 1, 0, 0, 0, 1,
         1.2,  0.3, -5.0, 1, 0, 0, 1, 1,

         1.2,  0.3, -5.0, 1, 0, 0, 1, 1,
         1.2, -0.9, -5.0, 1, 0, 0, 1, 0,
         1.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        //Back
         1.0, -0.9, -5.2, 1, 0, 0, 0, 0,
         1.0,  0.3, -5.2, 1, 0, 0, 0, 1,
         1.2,  0.3, -5.2, 1, 0, 0, 1, 1,

         1.2,  0.3, -5.2, 1, 0, 0, 1, 1,
         1.2, -0.9, -5.2, 1, 0, 0, 1, 0,
         1.0, -0.9, -5.2, 1, 0, 0, 0, 0,
        //Left
         1.0, -0.9, -5.0, 1, 0, 0, 0, 0,
         1.0,  0.3, -5.0, 1, 0, 0, 0, 1,
         1.0,  0.3, -5.2, 1, 0, 0, 0, 1,

         1.0,  0.3, -5.2, 1, 0, 0, 0, 1,
         1.0, -0.9, -5.2, 1, 0, 0, 0, 0,
         1.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        //Right
         1.2, -0.9, -5.0, 1, 0, 0, 0, 0,
         1.2,  0.3, -5.0, 1, 0, 0, 0, 1,
         1.2,  0.3, -5.2, 1, 0, 0, 0, 1,

         1.2,  0.3, -5.2, 1, 0, 0, 0, 1,
         1.2, -0.9, -5.2, 1, 0, 0, 0, 0,
         1.2, -0.9, -5.0, 1, 0, 0, 0, 0,

         //Front RIght
             //Front
         3.0, -0.9, -5.0, 1, 0, 0, 0, 0,
         3.0,  0.3, -5.0, 1, 0, 0, 0, 1,
         3.2,  0.3, -5.0, 1, 0, 0, 1, 1,

         3.2,  0.3, -5.0, 1, 0, 0, 1, 1,
         3.2, -0.9, -5.0, 1, 0, 0, 1, 0,
         3.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        //Back
         3.0, -0.9, -5.2, 1, 0, 0, 0, 0,
         3.0,  0.3, -5.2, 1, 0, 0, 0, 1,
         3.2,  0.3, -5.2, 1, 0, 0, 1, 1,

         3.2,  0.3, -5.2, 1, 0, 0, 1, 1,
         3.2, -0.9, -5.2, 1, 0, 0, 1, 0,
         3.0, -0.9, -5.2, 1, 0, 0, 0, 0,
        //Left
         3.0, -0.9, -5.0, 1, 0, 0, 0, 0,
         3.0,  0.3, -5.0, 1, 0, 0, 0, 1,
         3.0,  0.3, -5.2, 1, 0, 0, 0, 1,

         3.0,  0.3, -5.2, 1, 0, 0, 0, 1,
         3.0, -0.9, -5.2, 1, 0, 0, 0, 0,
         3.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        //Right
         3.2, -0.9, -5.0, 1, 0, 0, 0, 0,
         3.2,  0.3, -5.0, 1, 0, 0, 0, 1,
         3.2,  0.3, -5.2, 1, 0, 0, 0, 1,

         3.2,  0.3, -5.2, 1, 0, 0, 0, 1,
         3.2, -0.9, -5.2, 1, 0, 0, 0, 0,
         3.2, -0.9, -5.0, 1, 0, 0, 0, 0,

         //Left Back
        //Front
        1.0, -0.9, -6.0, 1, 0, 0, 0, 0,
        1.0,  0.3, -6.0, 1, 0, 0, 0, 1,
        1.2,  0.3, -6.0, 1, 0, 0, 1, 1,

        1.2,  0.3, -6.0, 1, 0, 0, 1, 1,
        1.2, -0.9, -6.0, 1, 0, 0, 1, 0,
        1.0, -0.9, -6.0, 1, 0, 0, 0, 0,
       //Back
        1.0, -0.9, -6.2, 1, 0, 0, 0, 0,
        1.0,  0.3, -6.2, 1, 0, 0, 0, 1,
        1.2,  0.3, -6.2, 1, 0, 0, 1, 1,

        1.2,  0.3, -6.2, 1, 0, 0, 1, 1,
        1.2, -0.9, -6.2, 1, 0, 0, 1, 0,
        1.0, -0.9, -6.2, 1, 0, 0, 0, 0,
       //Left
        1.0, -0.9, -6.0, 1, 0, 0, 0, 0,
        1.0,  0.3, -6.0, 1, 0, 0, 0, 1,
        1.0,  0.3, -6.0, 1, 0, 0, 0, 1,

        1.0,  0.3, -6.2, 1, 0, 0, 0, 1,
        1.0, -0.9, -6.2, 1, 0, 0, 0, 0,
        1.0, -0.9, -6.0, 1, 0, 0, 0, 0,
       //Right
        1.2, -0.9, -6.0, 1, 0, 0, 0, 0,
        1.2,  0.3, -6.0, 1, 0, 0, 0, 1,
        1.2,  0.3, -6.2, 1, 0, 0, 0, 1,

        1.2,  0.3, -6.2, 1, 0, 0, 0, 1,
        1.2, -0.9, -6.2, 1, 0, 0, 0, 0,
        1.2, -0.9, -6.0, 1, 0, 0, 0, 0,


        //Front RIght
             //Front
         3.0, -0.9, -6.0, 1, 0, 0, 0, 0,
         3.0,  0.3, -6.0, 1, 0, 0, 0, 1,
         3.2,  0.3, -6.0, 1, 0, 0, 1, 1,

         3.2,  0.3, -6.0, 1, 0, 0, 1, 1,
         3.2, -0.9, -6.0, 1, 0, 0, 1, 0,
         3.0, -0.9, -6.0, 1, 0, 0, 0, 0,
        //Back
         3.0, -0.9, -6.2, 1, 0, 0, 0, 0,
         3.0,  0.3, -6.2, 1, 0, 0, 0, 1,
         3.2,  0.3, -6.2, 1, 0, 0, 1, 1,

         3.2,  0.3, -6.2, 1, 0, 0, 1, 1,
         3.2, -0.9, -6.2, 1, 0, 0, 1, 0,
         3.0, -0.9, -6.2, 1, 0, 0, 0, 0,
        //Left
         3.0, -0.9, -6.0, 1, 0, 0, 0, 0,
         3.0,  0.3, -6.0, 1, 0, 0, 0, 1,
         3.0,  0.3, -6.2, 1, 0, 0, 0, 1,

         3.0,  0.3, -6.2, 1, 0, 0, 0, 1,
         3.0, -0.9, -6.2, 1, 0, 0, 0, 0,
         3.0, -0.9, -6.0, 1, 0, 0, 0, 0,
        //Right
         3.2, -0.9, -6.0, 1, 0, 0, 0, 0,
         3.2,  0.3, -6.0, 1, 0, 0, 0, 1,
         3.2,  0.3, -6.2, 1, 0, 0, 0, 1,

         3.2,  0.3, -6.2, 1, 0, 0, 0, 1,
         3.2, -0.9, -6.2, 1, 0, 0, 0, 0,
         3.2, -0.9, -6.0, 1, 0, 0, 0, 0,


    };


    float chair[] = 
    {
        //Front
        -2.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        -2.0,  0.0, -5.0, 1, 0, 0, 0, 1,
        -1.8,  0.0, -5.0, 1, 0, 0, 1, 1,
        
        -1.8,  0.0, -5.0, 1, 0, 0, 1, 1,
        -1.8, -0.9, -5.0, 1, 0, 0, 1, 0,
        -2.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        //Back
        -2.0, -0.9, -5.2, 1, 0, 0, 0, 0,
        -2.0,  0.0, -5.2, 1, 0, 0, 0, 1,
        -1.8,  0.0, -5.2, 1, 0, 0, 1, 1,

        -1.8,  0.0, -5.2, 1, 0, 0, 1, 1,
        -1.8, -0.9, -5.2, 1, 0, 0, 1, 0,
        -2.0, -0.9, -5.2, 1, 0, 0, 0, 0,
        //Left
        -2.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        -2.0,  0.0, -5.0, 1, 0, 0, 0, 1,
        -2.0,  0.0, -5.2, 1, 0, 0, 0, 1,
        
        -2.0,  0.0, -5.2, 1, 0, 0, 0, 1,
        -2.0, -0.9, -5.2, 1, 0, 0, 0, 0,
        -2.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        //Right
        -1.8, -0.9, -5.0, 1, 0, 0, 0, 0,
        -1.8,  0.0, -5.0, 1, 0, 0, 0, 1,
        -1.8,  0.0, -5.2, 1, 0, 0, 0, 1,

        -1.8,  0.0, -5.2, 1, 0, 0, 0, 1,
        -1.8, -0.9, -5.2, 1, 0, 0, 0, 0,
        -1.8, -0.9, -5.0, 1, 0, 0, 0, 0,

        //Right Front
                //Front
        -1.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        -1.0,  0.0, -5.0, 1, 0, 0, 0, 1,
        -0.8,  0.0, -5.0, 1, 0, 0, 1, 1,

        -0.8,  0.0, -5.0, 1, 0, 0, 1, 1,
        -0.8, -0.9, -5.0, 1, 0, 0, 1, 0,
        -1.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        //Back
        -1.0, -0.9, -5.2, 1, 0, 0, 0, 0,
        -1.0,  0.0, -5.2, 1, 0, 0, 0, 1,
        -0.8,  0.0, -5.2, 1, 0, 0, 1, 1,

        -0.8,  0.0, -5.2, 1, 0, 0, 1, 1,
        -0.8, -0.9, -5.2, 1, 0, 0, 1, 0,
        -1.0, -0.9, -5.2, 1, 0, 0, 0, 0,
        //Left
        -1.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        -1.0,  0.0, -5.0, 1, 0, 0, 0, 1,
        -1.0,  0.0, -5.2, 1, 0, 0, 0, 1,

        -1.0,  0.0, -5.2, 1, 0, 0, 0, 1,
        -1.0, -0.9, -5.2, 1, 0, 0, 0, 0,
        -1.0, -0.9, -5.0, 1, 0, 0, 0, 0,
        //Right
        -0.8, -0.9, -5.0, 1, 0, 0, 0, 0,
        -0.8,  0.0, -5.0, 1, 0, 0, 0, 1,
        -0.8,  0.0, -5.2, 1, 0, 0, 0, 1,

        -0.8,  0.0, -5.2, 1, 0, 0, 0, 1,
        -0.8, -0.9, -5.2, 1, 0, 0, 0, 0,
        -0.8, -0.9, -5.0, 1, 0, 0, 0, 0,


        //Left Back
         //Front
        -2.0, -0.9, -6.0, 1, 0, 0, 0, 0,
        -2.0,  0.0, -6.0, 1, 0, 0, 0, 1,
        -1.8,  0.0, -6.0, 1, 0, 0, 1, 1,

        -1.8,  0.0, -6.0, 1, 0, 0, 1, 1,
        -1.8, -0.9, -6.0, 1, 0, 0, 1, 0,
        -2.0, -0.9, -6.0, 1, 0, 0, 0, 0,
        //Back
        -2.0, -0.9, -6.2, 1, 0, 0, 0, 0,
        -2.0,  0.0, -6.2, 1, 0, 0, 0, 1,
        -1.8,  0.0, -6.2, 1, 0, 0, 1, 1,

        -1.8,  0.0, -6.2, 1, 0, 0, 1, 1,
        -1.8, -0.9, -6.2, 1, 0, 0, 1, 0,
        -2.0, -0.9, -6.2, 1, 0, 0, 0, 0,
        //Left
        -2.0, -0.9, -6.0, 1, 0, 0, 0, 0,
        -2.0,  0.0, -6.0, 1, 0, 0, 0, 1,
        -2.0,  0.0, -6.0, 1, 0, 0, 0, 1,

        -2.0,  0.0, -6.2, 1, 0, 0, 0, 1,
        -2.0, -0.9, -6.2, 1, 0, 0, 0, 0,
        -2.0, -0.9, -6.0, 1, 0, 0, 0, 0,
        //Right
        -1.8, -0.9, -6.0, 1, 0, 0, 0, 0,
        -1.8,  0.0, -6.0, 1, 0, 0, 0, 1,
        -1.8,  0.0, -6.2, 1, 0, 0, 0, 1,

        -1.8,  0.0, -6.2, 1, 0, 0, 0, 1,
        -1.8, -0.9, -6.2, 1, 0, 0, 0, 0,
        -1.8, -0.9, -6.0, 1, 0, 0, 0, 0,


        //Right Back
            //Front
            -1.0, -0.9, -6.0, 1, 0, 0, 0, 0,
            -1.0, 0.0, -6.0, 1, 0, 0, 0, 1,
            -0.8, 0.0, -6.0, 1, 0, 0, 1, 1,

            -0.8, 0.0, -6.0, 1, 0, 0, 1, 1,
            -0.8, -0.9, -6.0, 1, 0, 0, 1, 0,
            -1.0, -0.9, -6.0, 1, 0, 0, 0, 0,
            //Back
            -1.0, -0.9, -6.2, 1, 0, 0, 0, 0,
            -1.0, 0.0, -6.2, 1, 0, 0, 0, 1,
            -0.8, 0.0, -6.2, 1, 0, 0, 1, 1,

            -0.8, 0.0, -6.2, 1, 0, 0, 1, 1,
            -0.8, -0.9, -6.2, 1, 0, 0, 1, 0,
            -1.0, -0.9, -6.2, 1, 0, 0, 0, 0,
            //Left
            -1.0, -0.9, -6.0, 1, 0, 0, 0, 0,
            -1.0, 0.0, -6.0, 1, 0, 0, 0, 1,
            -1.0, 0.0, -6.0, 1, 0, 0, 0, 1,

            -1.0, 0.0, -6.2, 1, 0, 0, 0, 1,
            -1.0, -0.9, -6.2, 1, 0, 0, 0, 0,
            -1.0, -0.9, -6.0, 1, 0, 0, 0, 0,
            //Right
            -0.8, -0.9, -6.0, 1, 0, 0, 0, 0,
            -0.8, 0.0, -6.0, 1, 0, 0, 0, 1,
            -0.8, 0.0, -6.2, 1, 0, 0, 0, 1,

            -0.8, 0.0, -6.2, 1, 0, 0, 0, 1,
            -0.8, -0.9, -6.2, 1, 0, 0, 0, 0,
            -0.8, -0.9, -6.0, 1, 0, 0, 0, 0,

        //Left Front Top
            //Front
            -2.0, 0.0, -5.0, 1, 0, 0, 0, 0,
            -2.0, 1.0, -5.0, 1, 0, 0, 0, 1,
            -1.8, 1.0, -5.0, 1, 0, 0, 1, 1,

            -1.8,  1.0, -5.0, 1, 0, 0, 1, 1,
            -1.8, -0.0, -5.0, 1, 0, 0, 1, 0,
            -2.0, -0.0, -5.0, 1, 0, 0, 0, 0,
            //Back
            -2.0, -0.0, -6.2, 1, 0, 0, 0, 0,
            -2.0, 1.0, -6.2, 1, 0, 0, 0, 1,
            -1.8, 1.0, -6.2, 1, 0, 0, 1, 1,

            -1.8, 1.0, -6.2, 1, 0, 0, 1, 1,
            -1.8, -0.0, -6.2, 1, 0, 0, 1, 0,
            -2.0, -0.0, -6.2, 1, 0, 0, 0, 0,
            //Left
            -2.0, -0.0, -5.0, 1, 0, 0, 0, 0,
            -2.0, 1.0, -5.0, 1, 0, 0,  0, 1,
            -2.0, 1.0, -6.2, 1, 0, 0,  1, 1,

            -2.0, 1.0, -6.2, 1, 0, 0,  1, 1,
            -2.0, -0.0, -6.2, 1, 0, 0, 1, 0,
            -2.0, -0.0, -5.0, 1, 0, 0, 0, 0,
            //Right
            -1.8, -0.0, -5.0, 1, 0, 0, 0, 0,
            -1.8, 1.0, -5.0, 1, 0, 0,  0, 1,
            -1.8, 1.0, -6.2, 1, 0, 0,  1, 1,

            -1.8, 1.0, -6.2, 1, 0, 0,  1, 1,
            -1.8, -0.0, -6.2, 1, 0, 0, 1, 0,
            -1.8, -0.0, -5.0, 1, 0, 0, 0, 0,
            //Top
            -2.0, 1.0, -5.0, 1, 0, 0, 0, 0,
            -1.8, 1.0, -5.0, 1, 0, 0, 1, 0,
            -1.8, 1.0, -6.2, 1, 0, 0, 1, 1,
            
            -1.8, 1.0, -6.2, 1, 0, 0, 1, 1,
            -2.0, 1.0, -6.2, 1, 0, 0, 0, 1,
            -2.0, 1.0, -5.0, 1, 0, 0, 0, 0,
            

            //Seat
            //Front
            -2.0, 0.0, -5.0, 1, 0, 0, 0, 0,
            -2.0, 0.2, -5.0, 1, 0, 0, 0, 1,
            -0.8, 0.2, -5.0, 1, 0, 0, 1, 1,

            -0.8, 0.2, -5.0, 1, 0, 0, 1, 1,
            -0.8, -0.0, -5.0, 1, 0, 0, 1, 0,
            -2.0, -0.0, -5.0, 1, 0, 0, 0, 0,
            //Back
            -2.0, -0.0, -6.2, 1, 0, 0, 0, 0,
            -2.0, 0.2, -6.2, 1, 0, 0, 0, 1,
            -0.8, 0.2, -6.2, 1, 0, 0, 1, 1,

            -0.8, 0.2, -6.2, 1, 0, 0, 1, 1,
            -0.8, -0.0, -6.2, 1, 0, 0, 1, 0,
            -2.0, -0.0, -6.2, 1, 0, 0, 0, 0,
            //Left
            -2.0, -0.0, -5.0, 1, 0, 0, 0, 0,
            -2.0, 0.2, -5.0, 1, 0, 0, 0, 1,
            -2.0, 0.2, -6.2, 1, 0, 0, 0, 1,

            -2.0, 0.2, -6.2, 1, 0, 0, 0, 1,
            -2.0, -0.0, -6.2, 1, 0, 0, 0, 0,
            -2.0, -0.0, -5.0, 1, 0, 0, 0, 0,
            //Right
            -0.8, -0.0, -5.0, 1, 0, 0, 0, 0,
            -0.8, 0.2, -5.0, 1, 0, 0, 0, 1,
            -0.8, 0.2, -6.2, 1, 0, 0, 0, 1,

            -0.8, 0.2, -6.2, 1, 0, 0, 0, 1,
            -0.8, -0.0, -6.2, 1, 0, 0, 0, 0,
            -0.8, -0.0, -5.0, 1, 0, 0, 0, 0,
            //Top
            -2.0, 0.2, -5.0, 1, 0, 0, 0, 0,
            -0.8, 0.2, -5.0, 1, 0, 0, 1, 0,
            -0.8, 0.2, -6.2, 1, 0, 0, 1, 1,

            -0.8, 0.2, -6.2, 1, 0, 0, 1, 1,
            -2.0, 0.2, -6.2, 1, 0, 0, 1, 0,
            -2.0, 0.2, -5.0, 1, 0, 0, 0, 0,
    };


    float tableTop[] =
    {
        //Front
         0.5,  0.3, -4.5, 1, 0, 0, 0, 0,
         0.5,  0.4, -4.5, 1, 0, 0, 0, 1,
         3.7,  0.4, -4.5, 1, 0, 0, 1, 1,

         3.7,  0.4, -4.5, 1, 0, 0, 1, 1,
         3.7,  0.3, -4.5, 1, 0, 0, 1, 0,
         0.5,  0.3, -4.5, 1, 0, 0, 0, 0,
         //Back
         0.5,  0.3, -6.7, 1, 0, 0, 0, 0,
         0.5,  0.4, -6.7, 1, 0, 0, 0, 1,
         3.7,  0.4, -6.7, 1, 0, 0, 1, 1,

         3.7,  0.4, -6.7, 1, 0, 0, 1, 1,
         3.7,  0.3, -6.7, 1, 0, 0, 1, 0,
         0.5,  0.3, -6.7, 1, 0, 0, 0, 0,
          //Left
         0.5,  0.3, -4.5, 1, 0, 0, 0, 0,
         0.5,  0.4, -4.5, 1, 0, 0, 0, 1,
         0.5,  0.4, -6.7, 1, 0, 0, 0, 1,

         0.5,  0.4, -6.7, 1, 0, 0, 0, 1,
         0.5,  0.3, -6.7, 1, 0, 0, 0, 0,
         0.5,  0.3, -4.5, 1, 0, 0, 0, 0,
         //Right
         3.7,  0.3, -4.5, 1, 0, 0, 0, 0,
         3.7,  0.4, -4.5, 1, 0, 0, 0, 1,
         3.7,  0.4, -6.7, 1, 0, 0, 0, 1,

         3.7,  0.4, -6.7, 1, 0, 0, 0, 1,
         3.7,  0.3, -6.7, 1, 0, 0, 0, 0,
         3.7,  0.3, -4.5, 1, 0, 0, 0, 0,
         //Top
         0.5,  0.4, -4.5, 1, 0, 0, 0, 0,
         3.7,  0.4, -4.5, 1, 0, 0, 1, 0,
         3.7,  0.4, -6.7, 1, 0, 0, 1, 1,

         3.7,  0.4, -6.7, 1, 0, 0, 1, 1,
         0.5,  0.4, -6.7, 1, 0, 0, 0, 1,
         0.5,  0.4, -4.5, 1, 0, 0, 0, 0,

    };


    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor), &floor, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


    unsigned int leftWallVAO, leftWallVBO;
    glGenVertexArrays(1, &leftWallVAO);
    glGenBuffers(1, &leftWallVBO);
    glBindVertexArray(leftWallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, leftWallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallLeft), &wallLeft, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    unsigned int rightWallVAO, rightWallVBO;
    glGenVertexArrays(1, &rightWallVAO);
    glGenBuffers(1, &rightWallVBO);
    glBindVertexArray(rightWallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rightWallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallRight), &wallRight, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


    unsigned int backWallVAO, backWallVBO;
    glGenVertexArrays(1, &backWallVAO);
    glGenBuffers(1, &backWallVBO);
    glBindVertexArray(backWallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backWallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallBack), &wallBack, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


    unsigned int frontWallVAO, frontWallVBO;
    glGenVertexArrays(1, &frontWallVAO);
    glGenBuffers(1, &frontWallVBO);
    glBindVertexArray(frontWallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, frontWallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallFront), &wallFront, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    unsigned int roofWallVAO, roofWallVBO;
    glGenVertexArrays(1, &roofWallVAO);
    glGenBuffers(1, &roofWallVBO);
    glBindVertexArray(roofWallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, roofWallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(roof), &roof, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    unsigned int chairVAO, chairVBO;
    glGenVertexArrays(1, &chairVAO);
    glGenBuffers(1, &chairVBO);
    glBindVertexArray(chairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, chairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(chair), &chair, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


    unsigned int tableLegVAO, tableLegVBO;
    glGenVertexArrays(1, &tableLegVAO);
    glGenBuffers(1, &tableLegVBO);
    glBindVertexArray(tableLegVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tableLegVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tableLegs), &tableLegs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


    unsigned int tableTopVAO, tableTopVBO;
    glGenVertexArrays(1, &tableTopVAO);
    glGenBuffers(1, &tableTopVBO);
    glBindVertexArray(tableTopVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tableTopVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tableTop), &tableTop, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    unsigned int doorVAO, doorVBO;
    glGenVertexArrays(1, &doorVAO);
    glGenBuffers(1, &doorVBO);
    glBindVertexArray(doorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, doorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(door), &door, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


    unsigned int windowRightVAO, windowRightVBO;
    glGenVertexArrays(1, &windowRightVAO);
    glGenBuffers(1, &windowRightVBO);
    glBindVertexArray(windowRightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, windowRightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowRight), &windowRight, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


    unsigned int windowBackVAO, windowBackVBO;
    glGenVertexArrays(1, &windowBackVAO);
    glGenBuffers(1, &windowBackVBO);
    glBindVertexArray(windowBackVAO);
    glBindBuffer(GL_ARRAY_BUFFER, windowBackVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowBack), &windowBack, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    // load textures
    // -------------
    vector<std::string> faces
    {
       "resources/skybox/right.jpg",
        "resources/skybox/left.jpg",
        "resources/skybox/top.jpg",
        "resources/skybox/bottom.jpg",
        "resources/skybox/front.jpg",
        "resources/skybox/back.jpg",
    };

    unsigned int cubemapTexture = loadCubemap(faces);


    vector<std::string> windowRightTexPath
    {
       "resources/skybox/right.jpg"
    };

    unsigned int windowRightTex = loadCubemap(windowRightTexPath);
    
    textureFileNames[0] = "brick.jpg";
    textureFileNames[1] = "wallPaint.jpg";
    textureFileNames[2] = "wood.jpeg";
    textureFileNames[3] = "tableTop.jpeg";
    textureFileNames[4] = "door.jpg";
    textureFileNames[5] = "windowTex.jpg";

    loadTextures();





    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("skybox", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw scene as normal
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", camera.Position);
        //// cubes
        //glBindVertexArray(cubeVAO);
        ////glActiveTexture(GL_TEXTURE0);
        //glEnable(GL_TEXTURE_2D);
        //glBindTexture(GL_TEXTURE_2D, cubemapTexture);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        //glDisable(GL_TEXTURE_2D);
        //glBindVertexArray(0);




        modelShader.use();
        glm::mat4 model2 = glm::mat4(1.0f);
        glm::mat4 view2 = camera.GetViewMatrix();
        glm::mat4 projection2 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        modelShader.setMat4("model", model2);
        modelShader.setMat4("view", view2);
        modelShader.setMat4("projection", projection2);


        glBindVertexArray(roofWallVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindVertexArray(floorVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindVertexArray(frontWallVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[1]);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glBindVertexArray(0);

        glBindVertexArray(leftWallVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[1]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindVertexArray(rightWallVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[1]);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        glBindVertexArray(0);

        glBindVertexArray(backWallVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[1]);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        glBindVertexArray(0);

        glBindVertexArray(chairVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[2]);
        glDrawArrays(GL_TRIANGLES, 0, 156);
        glBindVertexArray(0);

        glBindVertexArray(tableLegVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[2]);
        glDrawArrays(GL_TRIANGLES, 0, 96);
        glBindVertexArray(0);

        glBindVertexArray(tableTopVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[3]);
        glDrawArrays(GL_TRIANGLES, 0, 102);
        glBindVertexArray(0);

        glBindVertexArray(doorVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[4]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

         glBindVertexArray(windowRightVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[5]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindVertexArray(windowBackVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID[5]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);




        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &skyboxVAO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


void loadTextures()
{
    int i;
    glGenTextures(numTextures, texID);  // Get the texture object IDs.
    for (i = 0; i < numTextures; i++) {
        void* imgData;  // Pointer to image color data read from the file.
        int imgWidth;   // The width of the image that was read.
        int imgHeight;  // The height.
        FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames[i]);
        if (format == FIF_UNKNOWN) {
            printf("Unknown file type for texture image file %s\n", textureFileNames);
            continue;
        }
        FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], 0);
        if (!bitmap) {
            printf("Failed to load image %s\n", textureFileNames[i]);
            continue;
        }
        FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap);
        FreeImage_Unload(bitmap);
        imgData = FreeImage_GetBits(bitmap2);
        imgWidth = FreeImage_GetWidth(bitmap2);
        imgHeight = FreeImage_GetHeight(bitmap2);
        if (imgData) {
            printf("Texture image loaded from file %s, size %dx%d\n",
                textureFileNames[i], imgWidth, imgHeight);
            int format;
            if (FI_RGBA_RED == 0)
                format = GL_RGB;
            else
                format = GL_BGR;
            glBindTexture(GL_TEXTURE_2D, texID[i]);  // Will load image data into texture object #i
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, format,
                GL_UNSIGNED_BYTE, imgData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Required since there are no mipmaps.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // (Actually, this one is the default)
            //stbi_image_free(imgData);
        }
        else {
            printf("Failed to get texture data from %s\n", textureFileNames[i]);
        }
        
    }
}
