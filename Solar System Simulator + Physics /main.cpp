
#include "HEADER.h"
#include "CELESTIAL_OBJECTS.h"
const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 800;


Camera camera(glm::vec3(0.0f, 0.0f, -20.0f));

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

//timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window){

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

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while(!glfwWindowShouldClose(window)){
        
        float currentFrame = glfwGetTime();

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

   
        //sun        
        sunShader.use();
        sunShader.setMat4("projection", projection);
        sunShader.setMat4("view", view);
        sunShader.setVec3("sunColor",glm::vec3(1.0f, 1.0f, 0.0f));        
        glm::mat4 sun_model = glm::mat4(1.0f);
        sun_model = glm::scale(sun_model, glm::vec3(5.0f));
        sun_model = glm::rotate(sun_model, glm::radians(90.0f), glm::vec3(1.0f , 0.0f, 0.0f));
        sun.Draw(sunShader, sun_model);

        //earth
        float orbitRadius = 15.0f;
        float orbitSpeed = 0.5f;
        float spinSpeed = 2.0f;
        float currentTime = glfwGetTime();
        float axialTilt = glm::radians(23.5f);
        float earthScale = 0.05f;

        earthShader.use();
        earthShader.setMat4("projection", projection);
        earthShader.setMat4("view", view);
        earthShader.setVec3("earthColor", glm::vec3(0.0f, 0.0f, 1.0f));    

        glm::mat4 earth_model = glm::mat4(1.0f);
        earth_model = glm::rotate(earth_model, currentTime * orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        earth_model = glm::translate(earth_model, glm::vec3(orbitRadius, 0.0f, 0.0f));
        earth_model = glm::rotate(earth_model, axialTilt, glm::vec3(0.0f, 0.0f, 1.0f));
        earth_model = glm::rotate(earth_model, currentTime * spinSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        earth_model = glm::scale(earth_model, glm::vec3(1.0f));
        earth.Draw(earthShader,earth_model);

        //mons
        float moonDistanceFromEarth = 3.0f;
        float moonScale = 0.1f;
        float moonOrbitSpeed = 0.05f;
        moonShader.use();
        moonShader.setMat4("projection", projection);
        moonShader.setMat4("view", view);
        moonShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 mons_model = glm::mat4(1.0f);
        mons_model = glm::rotate(mons_model, currentTime * moonOrbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        mons_model = glm::translate(mons_model, glm::vec3(moonDistanceFromEarth, 0.0f, 0.0f));
        mons_model = glm::scale(mons_model, glm::vec3(moonScale));
        mons_model = earth_model * mons_model;
        mons.Draw(moonShader, mons_model);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

   

}
