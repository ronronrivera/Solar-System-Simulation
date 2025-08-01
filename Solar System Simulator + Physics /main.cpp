

#include <iostream>

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


Camera camera(glm::vec3(20.0f, 0.0f, -100.0f));

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
bool pause = false;
bool spacePressedLastFrame = false;

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
    
    Planets sun(glm::vec3(0.0f, 0.0f, 0.0f));
    Planets earth(glm::vec3(0.0f, 0.0f, 0.0f));
    Planets mons(glm::vec3(0.0f, 0.0f, 0.0f));

    Shader sunShader("vertexShader_sun.glsl", "fragmentShader_sun.glsl");
    Shader earthShader("vertexShader_earth.glsl", "fragmentShader_earth.glsl");
    Shader moonShader("vertexShader_moon.glsl", "fragmentShader_moon.glsl");


    earth.addTexture(earthShader, "textures/earth.jpg");
    sun.addTexture(sunShader, "textures/sun.png");
    mons.addTexture(moonShader, "textures/moon.jpg");    
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float simulationTime = 0.0f;
    float lastActualTime = glfwGetTime();


    while(!glfwWindowShouldClose(window)){
        
        float actualTime = glfwGetTime();

        deltaTime = actualTime - lastActualTime;
        lastActualTime = actualTime;
        float FPS = 1.0f / deltaTime;
        
//        std::cout << "FPS: " << FPS << std::endl;

        if(!pause)
            simulationTime += deltaTime;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

   
        //sun        
        sunShader.use();
        glm::vec3 sunPos = glm::vec3(0.0f, 0.0f, 0.0f);
        sunShader.setMat4("projection", projection);
        sunShader.setMat4("view", view);
        sunShader.setVec3("sunColor",glm::vec3(1.0f, 1.0f, 0.0f));        
        glm::mat4 sun_model = glm::mat4(1.0f);        
        sun_model = glm::rotate(sun_model, glm::radians(90.0f), glm::vec3(1.0f , 0.0f, 0.0f));
        sun_model = glm::scale(sun_model, glm::vec3(15.0f));
        sun_model = glm::translate(sun_model, sunPos);
        sun.Draw(sunShader, sun_model, 0,"sunTexture");
    

        //kalibutan
        
        float orbitRadius = 60.0f;
        float earth_orbitSpeed = 0.5f;
        float earth_spinSpeed = 0.7f;
        float axialTilt = glm::radians(23.5f);
        float earthScale = 0.05f;

        float constant = 1.0f;
        float linear = 0.00045f;
        float quadratic = 1.0f / (orbitRadius * orbitRadius);

        earthShader.use();
        earthShader.setMat4("projection", projection);
        earthShader.setMat4("view", view);
        earthShader.setVec3("lightPos", sunPos);
        earthShader.setVec3("viewPos", camera.Position);
        
        earthShader.setFloat("constant", constant);
        earthShader.setFloat("linear", linear);
        earthShader.setFloat("quadratic", quadratic);
        
        

         
        // earth nga wala nag spin para sa moon 
        glm::mat4 earth_noSpin_model = glm::mat4(1.0f);
        earth_noSpin_model = glm::rotate(earth_noSpin_model, simulationTime * earth_orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        earth_noSpin_model = glm::translate(earth_noSpin_model, glm::vec3(orbitRadius, 0.0f, 0.0f));
        earth_noSpin_model = glm::rotate(earth_noSpin_model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        earth_noSpin_model = glm::rotate(earth_noSpin_model, axialTilt, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 earth_model = earth_noSpin_model;
        earth_model = glm::rotate(earth_model, simulationTime * earth_spinSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        earth_model = glm::scale(earth_model, glm::vec3(1.0f));


       earth.Draw(earthShader,earth_model, 0,"earthTexture");

        //mons

        float moonDistanceFromEarth = 3.0f;
        float moonScale = 0.1f;
        float moon_OrbitSpeed = 5.0f;

        moonShader.use();
        moonShader.setMat4("projection", projection);
        moonShader.setMat4("view", view);
        moonShader.setVec3("moonColor", glm::vec3(1.0f, 1.0f, 1.0f));
        moonShader.setVec3("viewPos", camera.Position);
        moonShader.setVec3("lightPos", sunPos);
       
        glm::mat4 mons_model = glm::mat4(1.0f);
        mons_model = glm::rotate(mons_model, simulationTime * moon_OrbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        mons_model = glm::translate(mons_model, glm::vec3(moonDistanceFromEarth, 0.0f, 0.0f));
        mons_model = glm::scale(mons_model, glm::vec3(moonScale));
        mons_model = earth_noSpin_model * mons_model;
        mons.Draw(moonShader, mons_model, 0, "moonTexture");
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

   

}
