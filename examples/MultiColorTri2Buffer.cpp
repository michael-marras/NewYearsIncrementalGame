// Thid Party Libraries
#include <SDL3/SDL.h>
#include <glad/glad.h>

//C++ Standard Template Library (STL)
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

// -------------Globals-------------
// Globals generally are prefixed with 'g' in this application

std::string LoadShaderAsString(const std::string& filename) {
    // Rersulting shader program loaded as a single string
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if(myFile.is_open()) {
        while(std::getline(myFile, line)) {
            result += line + '\n';
        }
        myFile.close();
    }
    return result;
}

// Screen Dimensions
int gScreenHeight                      = 480; 
int gScreenWidth                       = 640;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext           = nullptr;

// Main loop flag
bool gRunning = true;

/* shader
 *The following stores a unique id for the graphics pipeline
 * program object that will be used for our OpenGl draw calls.
 */
GLuint gGraphicsPipelineShaderProgram  = 0;

/* OpenGL Objects
 * Vertex Array Object (VAO)
 * Vertex Array objects encapsulate all of the items needed to render an object.
 * For example, we may have multiple vertex buffer objects (VBO) related to rendering one object.
 * The VAO allows us to setup the OpenGL state to render that object that object using
 * the correct layout and correct buffers with one call after being setup.
 */ 
GLuint gVertexArrayObject              = 0;
/* Vertex Buffer Object (VBO)
 * Vertex Buffer Objects store information relating to vertices (e.g. positions normals,
 * and textures)
 * VBOs are our mechanism for arranging geometry on the GPU.
 */
GLuint gVertexBufferObject             = 0;
GLuint gVertexBufferObject2            = 0;



GLuint CompileShader(GLuint type, const std::string& source) {
    GLuint shaderObject;

    if(type == GL_VERTEX_SHADER){
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    }
    else if(type == GL_FRAGMENT_SHADER) {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    const char* src = source.c_str();
    glShaderSource(shaderObject,1, &src, nullptr);
    glCompileShader(shaderObject);

    return shaderObject;
}

GLuint CreateShaderProgram(const std::string& vertexshadersource,
                        const std::string& fragmentshadersource) {
    GLuint programObject = glCreateProgram();

    GLuint myVertexShader   = CompileShader(GL_VERTEX_SHADER, vertexshadersource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);

    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    //validate our program
    glValidateProgram(programObject);
    // glDetachShader, glDeleteShader

    return programObject;
}

void CreateGraphicsPipeline() {

    std::string vertexShaderSource   = LoadShaderAsString("../shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("../shaders/frag.glsl");

    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void GetOpenGlVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

/**
 * Setup geometry during the vertex specification step
 *
 * @return void 
 */
void VertexSpecification() {

    /* Geometry Data
     * Here we are going to store x,y, and z position attributes within vertex
     * positions for the data. For now, this information is just stored in the CPU, and we
     * are going to store this data on the GPU shortly, in a call to glBufferData which will
     * store this information into a vertex buffer object.
     */

    //Lives on the cpu
    const std::vector<GLfloat> vertexPosition = 
    {
        //x     y     z
        -0.8f, -0.8f, 0.0f, //vertex 1 (left)
        0.8f, -0.8f, 0.0f,  //vertex 2 (right)
        0.0f, 0.8f, 0.0f    //vertex 3 (top)
    };

    const std::vector<GLfloat> vertexColors = 
    {
        //x     y     z
        1.0f, 0.0f, 0.0f, //Red
        0.0f, 1.0f, 0.0f,  //Green
        0.0f, 0.0f, 1.0f    //Blue
    };

    /* Vertex Arrays Object (VAO) Setu[]
     * Note: We can think of the VAO as a 'wrapper around' all of the Vertex Buffer Objects
     * In the sense that it encapsulates all VBO state that we are setting up.
     * This, it is also important that we glBindVertexArray (i.e. select the VAO we want to use)
     * before our vertex buffer object operations.
     */ 
    
    // Start settings things up on the gpu
    glGenVertexArrays(1, &gVertexArrayObject);

    // We bind (i.e select) to the Vertex Array Object (VAO) that we want to work within.
    glBindVertexArray(gVertexArrayObject);

    /* Vertex Buffer Object (VBO) creation
     * Create a new vertex buffer object
     * Note: Wel'' see this pattern of code often in OpenGL of creating and binding to a buffer
     */ 
    glGenBuffers(1, &gVertexBufferObject);

    /* Next we will do glBindBuffer.
     * Bind is equivalent to selecting the active buffer object that we want to work with in OpenGL.
     */
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    /* Now in our currently binded buffer, we populate the data from our 'vertex positions' (which is on the CPU)
     * onto a buffer that will live on the GPU.
     */
    glBufferData(GL_ARRAY_BUFFER,                         // Kind of Buffer
                 vertexPosition.size() * sizeof(GLfloat), // Size of data in bytes
                 vertexPosition.data(),                   // Raw arra of data
                 GL_STATIC_DRAW);                         // How we intend to use the data

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                              // 0 corresponds to glEnableVertexAttribArray
                          3,                              // The number of components (e.g. x,y,z )
                          GL_FLOAT,                       // Type
                          GL_FALSE,                       // Is the data normalized  
                          0,                              // Stride
                          (void*)0                        // Offset
            );

    // Setting up our colors
    glGenBuffers(1, &gVertexBufferObject2);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject2);
    glBufferData(GL_ARRAY_BUFFER, 
                vertexColors.size() * sizeof(GL_FLOAT),
                vertexColors.data(),
                GL_STATIC_DRAW);

    // Linking up the attributes in our VAO
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3, // r,g,b
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0
            );

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
}

/** 
 * Initialization of the graphics application. Typically this will involve setting up a window
 * and the OpenGL Context (with the appropriate version)
 *
 * @return void
 */
void InitializeProgram() {
    // Initialize SDL
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL3 could not initialize video subsystem" << std::endl;
        exit(1);
    }

    // Setup the OpenGL Context
    // Use OpenGL 4.1 core or greater
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //We want to request a double buffer for smooth updating.
    // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create an application window using OpenGL that supports SDL
    gGraphicsApplicationWindow = SDL_CreateWindow("OpenGL Window", gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL);

    // Check if the window did not create
    if (gGraphicsApplicationWindow == nullptr) {
        std::cout << "SDL_Window was not able to be created" << std::endl;
        exit(1);
    }

    // Create an OpenGL Graphics Context
    gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);
    if(gOpenGLContext==nullptr) {
        std::cout << "OpenGl context not available" << std::endl;
        exit(1);
    }

    // Initialize the Glad Library
    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cout << "glad was not initialized" <<std::endl;
        exit(1);
    }

    GetOpenGlVersionInfo();
}

void Input() {
    SDL_Event event;

    while(SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_EVENT_QUIT) {
            std::cout << "quit" << std::endl;
            gRunning = false;
        }
    }

}

void PreDraw() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0,0,gScreenWidth, gScreenHeight);
    glClearColor(1.f, 1.f, 0.f, 1.f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipelineShaderProgram);
}

void Draw() {
    
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    glDrawArrays(GL_TRIANGLES,0,3);

}

void MainLoop() {
    while(gRunning) {
        Input();

        PreDraw();

        Draw();

        //Update the screeen
        SDL_GL_SwapWindow(gGraphicsApplicationWindow);
    }
}

void CleanUp() {
    SDL_DestroyWindow(gGraphicsApplicationWindow);
    SDL_Quit();
}

int main() {

    // 1. Setup the graphics program
    InitializeProgram();

    // 2. Setup our Geometry
    VertexSpecification();

    // 3. Create our graphics pipeline
    // at a minimum this means the vertex and fragment shader
    CreateGraphicsPipeline();

    // 4. Call the main application loop
    MainLoop();
    
    // Call the cleanup function when our program terminates
    CleanUp();

    return 0;
}