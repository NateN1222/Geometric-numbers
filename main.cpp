// Author Nate N 11/4/2016

// CONTROLS:
// shift/ctrl: world space scale
// up/down: local space scale
// W/A/S/D: camera translate

// linker options: -lglfw3 -lGLU -lGL -lrt -lm -ldl -lXrandr -lXinerama -lXxf86vm -lXcursor -lXext -lXrender -lXfixes -lX11 -lpthread -lxcb -lXau -lXdmcp -lGLEW
// settings: -std=gnu++11

#include <iostream>
#include <chrono>
#include <cmath>
#include <stdio.h>
#include <vector>
#include <algorithm>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLEW_STATIC
#include <GL/glew.h>

#define WIDTH 800
#define HEIGHT 800

GLuint shaderProgram;
GLuint vertexShader;
GLuint fragmentShader;
GLuint geometryShader;

typedef struct {

    float x,y;
    float r,g,b;
    float scale;
    float vertexCount;


} vertex;
GLuint createShader(GLenum type, const GLchar* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if(type == GL_GEOMETRY_SHADER){

        printf("Geometry shader status:");

    }

    if(type == GL_VERTEX_SHADER){

        printf("Vertex shader status:");

    }

    if(type == GL_FRAGMENT_SHADER){
        printf("Fragment shader status:");
    }


    if(status == GL_TRUE){
        printf(" SUCCESS\n");

    } else{
        printf(" FAILED\n");
        GLint logSize = 0;
        glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&logSize);
        char buffer[logSize];
        glGetShaderInfoLog(shader, logSize, NULL, buffer);
        printf("LOG:\n%s\n",buffer);

    }


    return shader;
}
void compileShaders(){
    // Shader sources

    printf("---OpenGL version is (%s)---\n", glGetString(GL_VERSION));

    const GLchar* vertexSource =
        "#version 150 core\n"
        "in vec2 position;"

        "in vec3 color;"
        "out vec3 Color;"

        "in float vertexCount;"
        "out float sides;"

        "in float scale;"
        "out float Scale;"

        "uniform float camX;"
        "uniform float camY;"
        "uniform float camScale;"
        "uniform float worldScale;"
        "void main()"
        "{"
            "Scale = scale;"
            "sides = vertexCount;"
            "Color = color;"
            "vec4 pos = vec4((vec2(position.x,position.y)*camScale)+vec2(camX,camY), 0.0, 1.0);"
            //"gl_Position = vec4((vec2(position.x,position.y)*camScale)+vec2(camX,camY), 0.0, 1.0);"
            "gl_Position = worldScale*pos;"
        "}";

    const GLchar* fragmentSource =
        "#version 150 core\n"
        "in vec3 color;"
        "out vec4 outColor;"
        "void main()"
        "{"
            "outColor = vec4(1.0, 1.0, 0.0, 1.0);"
        "}";

    const GLchar* geometrySource =
        "#version 150 core\n"
        "layout(points) in;"
        "layout(line_strip, max_vertices = 1000) out;" // 1000 = arbitrary limit, shapes look like circles at this point anyway

        "in vec3 Color[];"
        "out vec3 color;"

        "in float sides[];"

        "in float Scale[];"

        "const float PI = 3.1415926;"

        "void main()"
        "{"
            "float sidez;"
            "if(sides[0] >= 1000){" // hopefull nobody tries to count them
                "sidez = 1000;"
            "} else {"
                "sidez = sides[0];"
            "}"
            "color = Color[0];"
            "float angle = 0.0;"

            "float increment = (2.0*PI)/sidez;"
            "for(int i = 0; i <= sidez; i++)"
            "{"
                "vec4 offset = vec4(cos(angle) * Scale[0],sin(angle) * Scale[0],0.0,0.0);"
                //"vec4 offset = vec4(cos(angle),sin(angle),0.0,0.0);"
                //"offset = offset * Scale[0];"
                "gl_Position = gl_in[0].gl_Position + offset;"
                "EmitVertex();"
                "angle = angle + increment;"
            "}"
            "EndPrimitive();"
        "}";
    // Create and compile the vertex shader
    vertexShader = createShader(GL_VERTEX_SHADER,vertexSource);
    fragmentShader = createShader(GL_FRAGMENT_SHADER,fragmentSource);
    geometryShader = createShader(GL_GEOMETRY_SHADER,geometrySource);


    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, geometryShader);

    glBindFragDataLocation(shaderProgram, 0, "outColor");

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

}
bool w_pressed = false;
bool a_pressed = false;
bool s_pressed = false;
bool d_pressed = false;
bool shift_pressed = false;
bool ctrl_pressed = false;
bool up_pressed = false;
bool down_pressed = false;
bool escape_pressed = false;
bool space_pressed = false;
bool first = true;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){

    if( (key == GLFW_KEY_W) && (action == GLFW_PRESS)){
        w_pressed = true;

    }
    if( (key == GLFW_KEY_A) && (action == GLFW_PRESS)){
        a_pressed = true;

    }
    if( (key == GLFW_KEY_S) && (action == GLFW_PRESS)){
        s_pressed = true;

    }
    if( (key == GLFW_KEY_D) && (action == GLFW_PRESS)){
        d_pressed = true;

    }
    if( (key == GLFW_KEY_W) && (action == GLFW_RELEASE)){
        w_pressed = false;
        first = true;
    }
    if( (key == GLFW_KEY_A) && (action == GLFW_RELEASE)){
        a_pressed = false;
        first = true;
    }
    if( (key == GLFW_KEY_S) && (action == GLFW_RELEASE)){
        s_pressed = false;
        first = true;
    }
    if( (key == GLFW_KEY_D) && (action == GLFW_RELEASE)){
        d_pressed = false;
        first = true;
    }
    if( (key == GLFW_KEY_LEFT_SHIFT) && (action == GLFW_PRESS)){
        shift_pressed = true;
    }
    if( (key == GLFW_KEY_LEFT_CONTROL) && (action == GLFW_PRESS)){
        ctrl_pressed = true;
    }
    if( (key == GLFW_KEY_LEFT_SHIFT) && (action == GLFW_RELEASE)){
        shift_pressed = false;
        first = true;
    }
    if( (key == GLFW_KEY_LEFT_CONTROL) && (action == GLFW_RELEASE)){
        ctrl_pressed = false;
        first = true;
    }
    if( (key == GLFW_KEY_UP) && (action == GLFW_PRESS)){
        up_pressed = true;
    }
    if( (key == GLFW_KEY_DOWN) && (action == GLFW_PRESS)){
        down_pressed = true;
    }
    if( (key == GLFW_KEY_UP) && (action == GLFW_RELEASE)){
        up_pressed = false;
        first = true;
    }
    if( (key == GLFW_KEY_DOWN) && (action == GLFW_RELEASE)){
        down_pressed = false;
        first = true;
    }
    if( (key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)){
        escape_pressed = true;
        first = true;
    }
    if( (key == GLFW_KEY_SPACE) && (action == GLFW_PRESS)){
        space_pressed = true;
        first = true;
    }
    if( (key == GLFW_KEY_SPACE) && (action == GLFW_RELEASE)){
        space_pressed = false;
        first = true;
    }

}
float scaleFunction(float scale){
    return (scale/2);
}
std::vector<int> primeDecomp(int input){



    std::vector<int> factors;
    int i = 2;
    printf("factors: \n");
    while (true){

        if ( ((input % i) == 0) && (i != input) ){
            input = input / i;
            factors.push_back(i);
            printf("%i\n",i);
            i = 2;


        } else{
            if(input == i){

                factors.push_back(i);
                printf("%i\n",i);
                return factors;
            } else{
                i++;
            }
        }
    }


}
std::vector<vertex> generatePoly(float x, float y, float scale, int n, int nextN,float r, float g, float b){

    float angle = 0.0f;
    float increment = (2*M_PI)/n;
    std::vector<vertex> poly;
    vertex tempVertex;


    for(int i = 0; i < n; i++){
        tempVertex = {
            .x= x+(cos(angle)*scale),
            .y= y+(sin(angle)*scale),
            .r= r,
            .g= g,
            .b= b,
            .scale= scale,
            .vertexCount= (float) nextN
        };

        poly.push_back(tempVertex);

        angle += increment;
    }

    return poly;
}
std::vector<vertex> join(std::vector<vertex> A,std::vector<vertex> B){
    std::vector<vertex> AB = A;
    AB.insert(AB.end(), B.begin(), B.end());
    return AB;
}

int permutationCount = 0;
std::vector<int> primes;
std::vector<vertex> generateShape(int input,bool next){

    if(next){
        permutationCount++;
        std::next_permutation(primes.begin(),primes.end());
    }
    // idea: make shapes with first vertex normal to a unit circle?

    float scale = 1.0f;
    vertex vert1 = { // this is just the first vertex (the center)
        .x= 0.0f,
        .y= 0.0f,
        .r= rand() % 2 + 1,
        .g= rand() % 2 + 1,
        .b= rand() % 2 + 1,
        .scale= scale,
        .vertexCount= (float) primes[0]
    };
    std::vector<vertex> lastIteration = {vert1};
    std::vector<vertex> newIteration;
    std::vector<vertex> output = lastIteration;
    std::vector<vertex> test;

    int currentN;
    int nextN;

    // todo: implement colors
    float r = 1.0f;
    float g = 1.0f;
    float b = 0.0f;

    for(int i = 0; i < primes.size()-1; i++){
        currentN = primes[i];
        nextN = primes[i+1];
        newIteration.clear();
        scale = scaleFunction(scale);
        for(int j = 0; j < lastIteration.size(); j++){ // iterate through the last iteration and create a new one
            vertex tempVertex = lastIteration[j];
            newIteration = join(
            newIteration,
            generatePoly(tempVertex.x,tempVertex.y,scale,currentN,nextN,r,g,b)
            );

        }
        output.insert(std::end(output), std::begin(newIteration), std::end(newIteration));

        lastIteration = newIteration;

    }
    return output;
}
auto lastTime = std::chrono::high_resolution_clock::now();
float d_time(){
        if(!first){
            auto newTime = std::chrono::high_resolution_clock::now();
            auto dtime = std::chrono::duration_cast<std::chrono::duration<float>>(newTime - lastTime).count();
            lastTime = newTime;
            return dtime;
        } else{
            lastTime = std::chrono::high_resolution_clock::now();
            first = false;
            return 0.0f;
        }
}



int main(int argc, char* argv[]){
    // for some reason this only works if it is up here..
    int number = 0;
    if(argc == 2){
        printf("Geometric shape generator - Author: Nate Nichols 11/4/2016 \n");
        long lnum = strtol(argv[1],nullptr,10);
        if (lnum > INT_MAX){
            printf("Number too large\n");
            return 0;
        } else{
            int number = (int) lnum;
            primes = primeDecomp(number);
        }
        printf("generating number: %i.....\n",number);
    } else{
        printf("Invalid args.\n");
        return 0;
    }

    srand(time(NULL));
    std::vector<vertex> final_shape = generateShape(number,false);


    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL",NULL,NULL); // Windowed
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();
    glfwSetKeyCallback(window, key_callback);


    compileShaders();

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Specify the layout of the vertex data
    // float X, float Y, float R, float G, float B, float SCALE, float NUMSIDES

    int vertexSize = (7*sizeof(float));

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, vertexSize, 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*) (2*sizeof(GL_FLOAT)));


    glBindAttribLocation(shaderProgram, 2, "scale");
    GLint scaleAttrib = glGetAttribLocation(shaderProgram, "scale");
    glEnableVertexAttribArray(scaleAttrib);
    glVertexAttribPointer(scaleAttrib, 1, GL_FLOAT, GL_FALSE, vertexSize, (void*) (5*sizeof(GL_FLOAT)));


    glBindAttribLocation(shaderProgram, 3, "vertexCount");
    GLint vertexCountAttrib = glGetAttribLocation(shaderProgram, "vertexCount");
    glEnableVertexAttribArray(vertexCountAttrib);
    glVertexAttribPointer(vertexCountAttrib, 1, GL_FLOAT, GL_FALSE, vertexSize, (void*) (6*sizeof(GL_FLOAT)));

    GLint uniCamX = glGetUniformLocation(shaderProgram, "camX");
    GLint uniCamY = glGetUniformLocation(shaderProgram, "camY");
    GLint uniCamScale = glGetUniformLocation(shaderProgram, "camScale");
    GLint uniWorldScale = glGetUniformLocation(shaderProgram, "worldScale");

    int data_size = final_shape.size()*vertexSize;
    vertex buffer_data[final_shape.size()];
    std::copy(final_shape.begin(), final_shape.end(), buffer_data);
    glBufferData(GL_ARRAY_BUFFER, final_shape.size()*vertexSize,buffer_data, GL_STATIC_DRAW);

    float camY = 0.0f;
    float camX = 0.0f;
    float cam_v = 1.0f;
    float camScale_v = 0.5f;
    float camScale = 1.0f;
    float worldScale_v = 0.5f;
    float worldScale = 1.0f;
    glUniform1f(uniCamScale, camScale);
    glUniform1f(uniWorldScale, worldScale);

    while(!glfwWindowShouldClose(window) && (escape_pressed == false)){

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black

        if(w_pressed){
            camY -= d_time()*cam_v;
            glUniform1f(uniCamY, camY);

        }
        if(a_pressed){
            camX += d_time()*cam_v;
            glUniform1f(uniCamX, camX);

        }
        if(s_pressed){
            camY += d_time()*cam_v;
            glUniform1f(uniCamY, camY);
        }
        if(d_pressed){
            camX -= d_time()*cam_v;
            glUniform1f(uniCamX, camX);
        }
        if(shift_pressed){
            camScale += d_time()*camScale_v;
            glUniform1f(uniCamScale, camScale);
        }
        if(ctrl_pressed){
            camScale -= d_time()*camScale_v;
            glUniform1f(uniCamScale, camScale);
        }
        if(up_pressed){
            worldScale += d_time()*worldScale_v;
            glUniform1f(uniWorldScale, worldScale);
        }
        if(down_pressed){
            worldScale -= d_time()*worldScale_v;
            glUniform1f(uniWorldScale, worldScale);
        }

        glDrawArrays(GL_POINTS, 0, final_shape.size());

        glfwSwapBuffers(window);
        glfwPollEvents();

        if(space_pressed){
            final_shape = generateShape(number,true);
            int data_size = final_shape.size()*vertexSize;
            vertex buffer_data[final_shape.size()];
            std::copy(final_shape.begin(), final_shape.end(), buffer_data);
            glBufferData(GL_ARRAY_BUFFER, final_shape.size()*vertexSize,buffer_data, GL_STATIC_DRAW);
        }

    }
    //glDeleteBuffers(1,vbo);
    return 0;
}
