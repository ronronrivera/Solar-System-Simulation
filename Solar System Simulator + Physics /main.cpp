#include <iostream>
#include  <memory>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SHADER.h"
#include "CAMERA.h"

#include "HEADER.h"
#include "CELESTIAL_OBJECTS.h"

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 800;


Camera camera(glm::vec3(0.0f, 20.0f, -1000.0f));

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
bool pause = true;
bool spacePressedLastFrame = false;


float constant = 1.0f;
float linear = 0.00045f;


//timing
float deltaTime = 0.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window){


    bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if(spacePressed && !spacePressedLastFrame){
        pause = !pause;
    }
    spacePressedLastFrame = spacePressed;

    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);


    if(glfwGetKey(window , GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.ProcessKeyboard(LEFT, deltaTime);
   }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn){
    
    float xPos = static_cast<float>(xposIn);
    float yPos = static_cast<float>(yposIn);

    if(firstMouse){
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xoffset = xPos - lastX;
    float yoffset = lastY - yPos; // reversed since y-coordinates go from bottom to top

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

GLFWwindow *window;

GLFWwindow* STARTGLFW(){

    if(!glfwInit()){
        std::cout << "FAILED TO INITIALIZE OPENGL, PANI!!!\n";
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(WIDTH, HEIGHT, "Solar System Project By Ron-ron Aspe Rivera :) " ,nullptr, nullptr);

    if(!window){
        std::cout << "FAILED TO CREATE WINDOW!! PANIC!\n";
        glfwTerminate();
        return nullptr;
    }


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetScrollCallback(window, scroll_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "GLAD FAILED TO INITIALIZE, PANIC!!\n";
    }

    return window;
}

int main(){
    
    STARTGLFW();

    std::vector<std::unique_ptr<CelestialBody>> celestialBodies;

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float simulationTime = 0.0f;
    float lastActualTime = glfwGetTime();
    
    Shader planetShader("SHADERS/vertexShader_Planet.glsl", "SHADERS/fragmentShader_Planet.glsl");
    Shader starShader("SHADERS/vertexShader_Stars.glsl", "SHADERS/fragmentShader_Stars.glsl");
    Shader moonShader("SHADERS/vertexShader_moon.glsl", "SHADERS/fragmentShader_moon.glsl");
    
    //Star
    glm::vec3 sunPos = glm::vec3(0.0f, 0.0f, 0.0f);
    float sunScale = 300.0f;
    celestialBodies.push_back(std::make_unique<Star>(
    starShader,
    sunPos,
    "textures/sun.png",
    sunScale
));

    //mercury
    float mercury_orbitRadius = 600.0f;
    float mercury_orbitSpeed = 0.5f;
    float mercury_spinSpeed = 0.5f;
    

    float mercury_axialTilt = glm::radians(0.0f);
    float mercury_scale = 3.8f;

    float mercury_quadratic = 1.0f / (mercury_orbitRadius * mercury_orbitRadius);

    glm::vec3 mercuryPos = glm::vec3(mercury_orbitRadius, 0.0f, 0.0f);

    celestialBodies.push_back(std::make_unique<Planet>(
            planetShader,
            mercuryPos,
            "textures/mercury.jpg",
            mercury_orbitSpeed,
            mercury_spinSpeed,
            mercury_axialTilt,
            mercury_scale,
            sunPos,
            camera.Position,
            constant,
            linear,
            mercury_quadratic
                ));
    //vemus
    float venus_orbitRadius = 900.0f;
    float venus_orbitSpeed = 0.6f;
    float venus_spinSpeed = 0.6f;
    
    float venus_axialTilt = 177.36f;
    float venus_scale = 9.5f;
    
    float venus_quadratic = 1 / (venus_orbitRadius * venus_orbitRadius);

    glm::vec3 venusPos = glm::vec3(venus_orbitRadius, 0.0f, 0.0f);

    celestialBodies.push_back(std::make_unique<Planet>(
        planetShader,
        venusPos,
        "textures/venus.jpg",
        venus_orbitSpeed,
        venus_spinSpeed,
        venus_axialTilt,
        venus_scale,
        sunPos,
        camera.Position,
        constant,
        linear,
        venus_quadratic
    ));

    //earth
    float earth_orbitRadius = 1200.0f;
    float earth_orbitSpeed = 0.7f;
    float earth_spinSpeed = 0.7f;
    float axialTilt = glm::radians(23.5f);
    float earthScale = 10.0f;

    float quadratic = 1.0f / (earth_orbitRadius * earth_orbitRadius);
    
    glm::vec3 earthPos = glm::vec3(earth_orbitRadius, 0.0f, 0.0f);
    celestialBodies.push_back(std::make_unique<Planet>(
       planetShader,
       earthPos,
       "textures/earth.png",
       earth_orbitSpeed,
       earth_spinSpeed,
       axialTilt,
       earthScale,
       sunPos,
       camera.Position,
       constant,
       linear,
       quadratic
    ));

    float moonDistanceFromEarth = 20.0f;
    float moonScale = 1.0f;
    float moon_OrbitSpeed = 2.7f;
    
    float axialTiltMoon = glm::radians(5.0f);

    float moonQuadratic = 1.0f / (moonDistanceFromEarth * moonDistanceFromEarth);

    //earth moon
    celestialBodies.push_back(std::make_unique<Moon>(
        moonShader,
        glm::vec3(moonDistanceFromEarth, 0.0f, 0.0f),
        "textures/moon.jpg",
        moon_OrbitSpeed,
        axialTiltMoon,
        moonScale,
        earthPos,
        camera.Position,
        celestialBodies[3].get(),
        constant,
        linear,
        moonQuadratic
    ));   
    
    //mars
    float mars_orbitRadius = 1500.0f;
    float mars_orbitSpeed = 0.8f;
    float mars_spinSpeed = 0.8f;
    float mars_axialTilt = glm::radians(25.5f);
    float marsScale = 5.3f;

    float mars_quadratic = 1.0f / (mars_orbitRadius * mars_orbitRadius);
    
    glm::vec3 marsPos = glm::vec3(mars_orbitRadius, 0.0f, 0.0f);
 
    celestialBodies.push_back(std::make_unique<Planet>(
        planetShader,
        marsPos,
        "textures/mars.jpg",
        mars_orbitSpeed,
        mars_spinSpeed,
        mars_axialTilt,
        marsScale,
        sunPos,
        camera.Position,
        constant,
        linear,
        mars_quadratic
    ));
    
    //jupiter
    float jupiter_orbitRadius = 1800.0f;
    float jupiter_orbitSpeed = 0.9f;
    float jupiter_spinSpeed = 0.9f;

    float jupiter_axialTilt = glm::radians(3.13);
    float jupiterScale = 110.0f;

    float jupiter_quadratic = 1.0f / (jupiter_orbitRadius * jupiter_orbitRadius);

    glm::vec3 jupiterPos = glm::vec3(jupiter_orbitRadius, 0.0f, 0.0f);

    celestialBodies.push_back(std::make_unique<Planet>(
        planetShader,
        jupiterPos,
        "textures/jupiter.jpg",
        jupiter_orbitSpeed,
        jupiter_spinSpeed,
        jupiter_axialTilt,
        jupiterScale,
        sunPos,
        camera.Position,
        constant,
        linear,
        jupiter_quadratic
    ));

   while(!glfwWindowShouldClose(window)){
        
        float actualTime = glfwGetTime();

        deltaTime = actualTime - lastActualTime;
        lastActualTime = actualTime;
        float FPS = 1.0f / deltaTime;
        
//        std::cout << "FPS: " << FPS << std::endl;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(!pause)
            simulationTime += deltaTime;


        float znear = 0.1f;
        float zfar  = 3000000000.0f;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, znear, zfar);
        
        for(auto &obj: celestialBodies){
            obj->Draw(view, projection, simulationTime);
        }

        processInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
