#include <iostream>
#include  <memory>


#include "SHADER.h"
#include "CAMERA.h"
#include "CELESTIAL_OBJECTS.h"
#include "ASSIMP.h"

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 800;


Camera camera(glm::vec3(0.0f, 20.0f, -10000.0f));

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

bool altPressed = false;
bool altPressedLastFrame = false;
glm::vec3 shipPosition;
float orbitDistance = 100.0f;
float orbitAngle = 0.0f;


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
    altPressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
    if(altPressed && !altPressedLastFrame){
        shipPosition = camera.Position + camera.Front * 100.0f + camera.Up * -10.0f;
        orbitDistance = glm::distance(camera.Position, shipPosition);
    }
    altPressedLastFrame = altPressed;

}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn){
    
    if(altPressed) return;

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
    
    Shader shipShader("SHADERS/vertexShader_model.glsl", "SHADERS/fragmentShader_model.glsl");
    Model shipModel("models/ship.obj");

    //Star
    glm::vec3 sunPos = glm::vec3(0.0f, 0.0f, 0.0f);
    float sunScale = 3000.0f;
    celestialBodies.push_back(std::make_unique<Star>(
    starShader,
    sunPos,
    "textures/sun.png",
    sunScale
));

    //mercury
    float mercury_orbitRadius = 5000.0f;
    float mercury_orbitSpeed = 0.05f;
    float mercury_spinSpeed = 0.5f;
    

    float mercury_axialTilt = glm::radians(0.0f);
    float mercury_scale = 3.8 * 20.0f;

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
    float venus_orbitRadius = 10000.0f;
    float venus_orbitSpeed = 0.06f;
    float venus_spinSpeed = 0.6f;
    
    float venus_axialTilt = 177.36f;
    float venus_scale = 9.5f * 20.0f;
    
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
    float earth_orbitRadius = 15000.0f;
    float earth_orbitSpeed = 0.07f;
    float earth_spinSpeed = 0.7f;
    float axialTilt = glm::radians(23.5f);
    float earthScale = 10.0f * 20;

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

    float moonDistanceFromEarth = 500.0f;
    float moonScale = 1.0f * 20.0f;
    float moon_OrbitSpeed = 0.27f;
    
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
    float mars_orbitRadius = 20000.0f;
    float mars_orbitSpeed = 0.08f;
    float mars_spinSpeed = 0.8f;
    float mars_axialTilt = glm::radians(25.5f);
    float marsScale = 5.3f * 20.0f;

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
    float jupiter_orbitRadius = 25000.0f;
    float jupiter_orbitSpeed = 0.09f;
    float jupiter_spinSpeed = 0.9f;

    float jupiter_axialTilt = glm::radians(3.13);
    float jupiterScale = 110.0 * 20.0f;

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
    
    float planetX_orbitRadius = 30000.0f;
    float planetX_orbitSpeed = 0.1f;
    float planetX_spinSpeed = 1.0f;
    
    float planetX_axialTilt = glm::radians(0.0f);
    float planetXScale = 10 * 20.0f;

    float planetX_quadratic = 1.0f / (planetX_orbitRadius * planetX_orbitRadius);

    glm::vec3 planetXpos = glm::vec3(planetX_orbitRadius, 0.0f, 0.0f);

    celestialBodies.push_back(std::make_unique<Planet>(
        planetShader,
        planetXpos,
        "textures/planetX.jpg",
        planetX_orbitSpeed,
        planetX_spinSpeed,
        planetX_axialTilt,
        planetXScale,
        sunPos,
        camera.Position,
        constant,
        linear,
        planetX_quadratic
    ));
    
    while(!glfwWindowShouldClose(window)){
        
        float actualTime = glfwGetTime();

        deltaTime = actualTime - lastActualTime;
        lastActualTime = actualTime;
        float FPS = 1.0f / deltaTime;

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
        
        shipShader.use();
        shipShader.setMat4("view", view);
        shipShader.setMat4("projection", projection);


        shipShader.setVec3("dirLight.direction", sunPos);
        shipShader.setVec3("dirLight.ambient",  glm::vec3(0.4));
        shipShader.setVec3("dirLight.diffuse",  glm::vec3(1.0f));
        shipShader.setVec3("dirLight.specular", glm::vec3(1.0f));

        // Material shininess
        shipShader.setFloat("material.shininess", 32.0f);
        
        if(altPressed){
            orbitAngle += 0.5f * deltaTime;

            float x = shipPosition.x + orbitDistance * cos(orbitAngle);
            float z = shipPosition.z + orbitDistance * sin(orbitAngle);

            camera.Position = glm::vec3(x, shipPosition.y + 20.0f, z);

            camera.Front = glm::normalize(shipPosition - camera.Position);
            camera.Right = glm::normalize(glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f)));
            camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
        }
        else{
            shipPosition = camera.Position + camera.Front * 100.0f + camera.Up * -10.0f;
//            camera.Position = shipPosition - camera.Front * 10.0f;
        }

        glm::mat4 model = glm::mat4(1.0f);
        
        // Offset the ship a bit in front of the camera
        model = glm::translate(model, camera.Position + camera.Front * 100.0f + camera.Up *  -10.0f);
        //Rotate the ship to match the camera orientation
        model = glm::rotate(model, glm::radians(-camera.Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-camera.Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        
        //scale the ship down
        model = glm::scale(model, glm::vec3(1.0f));
        
        shipShader.setMat4("model", model);

        shipModel.Draw(shipShader);

        processInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
