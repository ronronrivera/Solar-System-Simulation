
#ifndef CELESTIAL_H
#define CELESTIAL_H

#include <iostream>
#include <vector>


#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SHADER.h"
#include "CAMERA.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

class CelestialBody{

    public:

        const char* path;
       
        Shader shader;

        CelestialBody(Shader& shader, const char* path):path(path), shader(shader){ 

            SphereMesh();
            setupMesh();
            addexture();
        }

        virtual void Draw(glm::mat4 view, 
                glm::mat4 projection, 
               float dt
                ) {

            shader.use();
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);

            shader.setInt("_texture", 0); 

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            
        }

        virtual glm::mat4 planetNoSpin_model() const {
            return glm::mat4(1.0f); // or throw or return dummy matrix
        }

        protected:
        
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        unsigned int VBO = 0, VAO = 0, EBO = 0, textureID;;
        int indexCount;

        void prepareDraw(glm::mat4 view, glm::mat4 projection) {
            shader.use();
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);
            shader.setInt("_texture", 0); 

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);
        }


        void SphereMesh(){
            
            int X_SEGMENTS = 64;
            int Y_SEGMENTS = 64;
            const float PI = 3.14159;
            
            for(unsigned int y = 0; y <= Y_SEGMENTS; y++){
                for(unsigned int x = 0; x <= X_SEGMENTS; x++){
                    float yStack = ((float)y / (float)Y_SEGMENTS);
                    float xSector = ((float)x / (float)X_SEGMENTS);
                   
                    float phi = PI * yStack;
                    float theta = 2 * PI * xSector;

                    float xPos = std::cos(theta) * std::sin(phi);
                    float yPos = std::cos(phi);
                    float zPos = std::sin(theta) * std::sin(phi);

                    //calculate uv
                    float xUV = (float)x/X_SEGMENTS;
                    float yUV = (float)y/Y_SEGMENTS;

                    //positions
                    vertices.push_back(xPos);
                    vertices.push_back(yPos);
                    vertices.push_back(zPos);
                   
                    //normals
                    vertices.push_back(xPos);
                    vertices.push_back(yPos);
                    vertices.push_back(zPos);

                    //texCoord
                    vertices.push_back(xUV);
                    vertices.push_back(yUV);


                }
            }
            
            for(unsigned int y = 0; y < Y_SEGMENTS; y++){
                for(unsigned int x = 0; x < X_SEGMENTS; x++){
                    unsigned int first = (y * (X_SEGMENTS + 1)) + x;
                    unsigned int second = first + X_SEGMENTS + 1;
                    indices.push_back(first);
                    indices.push_back(second);
                    indices.push_back(first + 1);
             
                    indices.push_back(second);
                    indices.push_back(second + 1);
                    indices.push_back(first + 1);
                }
            }

            indexCount = indices.size();

        }
        
        void setupMesh(){
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
           
            glBindVertexArray(VAO);
           
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
           
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
           
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
           
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
            glEnableVertexAttribArray(2);
            

           glBindBuffer(GL_ARRAY_BUFFER, 0);
           glBindVertexArray(0);
        }
        void addexture(){
            glGenTextures(1, &textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            int width, height, nrChannel;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* data = stbi_load(path, &width, &height, &nrChannel, 0);
            
            if(!data){
                std::cout << "FAILED TO LOAD TEXTURE\n";
                return;
            }
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);
           
            GLenum format = GL_RGB;
            if (nrChannel == 1) format = GL_RED;
            else if (nrChannel == 3) format = GL_RGB;
            else if (nrChannel == 4) format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
            
       }

};

class Star: public CelestialBody{

    public:

        glm::vec3 pos;
        float scale;
        Star(Shader& shader,
                glm::vec3 pos, 
                const char* path,
                float scale
                ):CelestialBody(shader, path),
                pos(pos),
                scale(scale)
                {}
        
        void Draw(glm::mat4 view, glm::mat4 projection, float dt) override{
            
            prepareDraw(view, projection);
            float rotationSpeed = 0.01f;
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, dt*rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(scale));
            shader.setMat4("model", model);
            
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
};




class Planet: public CelestialBody{

    public:

        glm::vec3 pos;

        float orbitSpeed;
        float spinSpeed;
        float axialTilt;
        float scale;

        glm::vec3 lightPos;
        glm::vec3 viewPos;
        float constant;
        float linear;
        float quadratic;

        glm::mat4 noSpin_model;

        Planet(Shader& shader,
                glm::vec3 pos, 
                const char* path, 
                float orbitSpeed,
                float spinSpeed,
                float axialTilt,
                float scale,
                glm::vec3 lightPos,
                glm::vec3 viewPos,
                float constant,
                float linear,
                float quadratic
                ):CelestialBody(shader, path),
                pos(pos),
                orbitSpeed(orbitSpeed),
                spinSpeed(spinSpeed),
                axialTilt(axialTilt),
                scale(scale),
                lightPos(lightPos),
                viewPos(viewPos),
                constant(constant),
                linear(linear),
                quadratic(quadratic)
                {
                    baseModel = glm::translate(glm::mat4(1.0f), pos);
                    baseModel = glm::rotate(baseModel, axialTilt, glm::vec3(0.0f, 1.0f, 0.0f));
                    noSpin_model = baseModel;
                }           


        void Draw(glm::mat4 view, 
                glm::mat4 projection, 
               float dt
                ) override{

            shader.use();

            prepareDraw(view, projection);

            shader.setVec3("lightPos", lightPos);
            shader.setVec3("viewPos", viewPos);
          
            shader.setFloat("constant", constant);
            shader.setFloat("linear", linear);
            shader.setFloat("quadratic", quadratic);
            
            if (orbitSpeed != 0.0f) {
                noSpin_model = glm::rotate(glm::mat4(1.0f), dt * orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
                noSpin_model = glm::translate(noSpin_model, pos);  // Apply position AFTER rotation
                noSpin_model = glm::rotate(noSpin_model, axialTilt, glm::vec3(0.0f, 1.0f, 0.0f));
            } else {
                noSpin_model = baseModel;
            }

            glm::mat4 Spin_model = noSpin_model;
            Spin_model = glm::rotate(Spin_model, dt * spinSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
            Spin_model = glm::scale(Spin_model, glm::vec3(scale, scale, scale));
            shader.setMat4("model", Spin_model);



            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            
        }
        
        glm::mat4 planetNoSpin_model() const  override{
            return noSpin_model;
        }

    private:
        glm::mat4 baseModel;
};

class Moon: public CelestialBody{

    public:
        
        const CelestialBody* parentBody;

        glm::vec3 pos;

        float orbitSpeed;
        float axialTilt;
        float scale;

        glm::vec3 lightPos;
        glm::vec3 viewPos;
        float constant;
        float linear;
        float quadratic;


        Moon(Shader& shader,
                glm::vec3 pos, 
                const char* path, 
                float orbitSpeed,
                float axialTilt,
                float scale,
                glm::vec3 lightPos,
                glm::vec3 viewPos,
                const CelestialBody* parent,
                float constant,
                float linear,
                float quadratic
                ):CelestialBody(shader, path),
                parentBody(parent),
                pos(pos),
                orbitSpeed(orbitSpeed),
                axialTilt(axialTilt),
                scale(scale),
                lightPos(lightPos),
                viewPos(viewPos),
                constant(constant),
                linear(linear),
                quadratic(quadratic)
                {
        }


        void Draw(glm::mat4 view, 
                glm::mat4 projection, 
               float dt
                ) override{

            shader.use();
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);

            shader.setVec3("lightPos", lightPos);
            shader.setVec3("viewPos", viewPos);
            
            shader.setFloat("constant", constant);
            shader.setFloat("linear", linear);
            shader.setFloat("quadratic", quadratic);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, dt * orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, pos);
            model = glm::rotate(model, axialTilt, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(scale, scale, scale));
            model = parentBody->planetNoSpin_model() * model;

            shader.setMat4("model", model);

            shader.setInt("texture_diffuse", 0); 

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            
        }

};

#endif 
