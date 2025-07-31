
#ifndef CELESTIAL_H
#define CELESTIAL_H


#include "HEADER.h"


class Planets{

    public:
        Planets(glm::vec3 pos):position(pos) {
            SphereMesh();
            setupMesh();
        }

        void Draw(Shader &shader, glm::mat4 model) {
            
            shader.setMat4("model", model);


            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
       
        private:
        
        glm::vec3 position;

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        unsigned int VBO = 0, VAO = 0, EBO = 0;
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

                    //positions
                    vertices.push_back(xPos);
                    vertices.push_back(yPos);
                    vertices.push_back(zPos);
 /*                   
                    //normals
                    vertices.push_back(xPos);
                    vertices.push_back(yPos);
                    vertices.push_back(zPos);
*/

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
           
           glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
           glEnableVertexAttribArray(0);
        
/*         will add normals later      
           glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
           glEnableVertexAttribArray(1);
*/           
           glBindBuffer(GL_ARRAY_BUFFER, 0);
           glBindVertexArray(0);
        }
};

#endif 
