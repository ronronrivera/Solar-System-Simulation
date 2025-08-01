
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


class Planets{

    public:
        Planets(glm::vec3 pos):position(pos) {
            SphereMesh();
            setupMesh();
        }

        void Draw(Shader &shader, glm::mat4 model, int value,const char* texture = "") {
            
            shader.setInt(texture, value); 
            shader.setMat4("model", model);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            

        }
        
        void addTexture(Shader shader,const char* path){
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


        private:
        glm::vec3 position;
        
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        unsigned int VBO = 0, VAO = 0, EBO = 0, textureID;;
        int indexCount;


        void SphereMesh(){
            
            int X_SEGMENTS = 64;
            int Y_SEGMENTS = 64;
            const float PI = 3.14159;
            
            for(unsigned int y = 0; y <= Y_SEGMENTS; y++){
                for(unsigned int x = 0; x <= X_SEGMENTS; x++){
                    float yStack = ((float)y / (float)Y_SEGMENTS);
                    float xSector = ((float)x / (float)X_SEGMENTS);
                   
                    float phi = (PI / 2) - PI * yStack;
                    float theta = 2 * PI * xSector;

                    float xPos = std::cos(phi) * std::sin(theta);
                    float yPos = std::cos(phi) * std::cos(theta);
                    float zPos = std::sin(phi);

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
};

#endif 
