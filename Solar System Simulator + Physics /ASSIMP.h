#ifndef MODEL_H
#define MODEL_H


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SHADER.h"


unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

struct Vertex{

    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    glm::vec2 TexCoords;
};

struct Texture{
    
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh{

    public:

        //Mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> texture;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> texture){

            this->vertices = vertices;
            this->indices = indices;
            this->texture = texture;

            setupMesh();

        }

        void Draw(Shader &shader){

            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;

            for(unsigned int i = 0; i<texture.size(); i++){

                glActiveTexture(GL_TEXTURE0 + i);
                
                std::string number;
                std::string name = texture[i].type;

                if(name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++);
            
                shader.setInt(("material." + name + number).c_str(), i);
                glBindTexture(GL_TEXTURE_2D, texture[i].id);
            }
        
            glActiveTexture(GL_TEXTURE0);

            //draw mesh
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        
        }
        
    private:
        //render data
        unsigned int VAO, VBO, EBO;

        void setupMesh(){

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        
            //vertex position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

            //vertex normal
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

            //vertex texture
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        
            glBindVertexArray(0);
        }
              
};

class Model{

    public:
        
        bool gammaCorrection;

        Model(const char* path){
            loadModel(path);
        }

        void Draw(Shader &shader){
            for(int i = 0; i<meshes.size(); i++)
                meshes[i].Draw(shader);
        }

    private:
        //model data
        std::vector<Mesh> meshes;
        std::string directory;
       
        std::vector<Texture> textures_loaded; //stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once
        
        void loadModel(std::string path){
            
            Assimp::Importer import;
            
            const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
            
            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ){
                std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << "\n";

                return;
            }

            directory =  path.substr(0, path.find_last_of('/'));

            processNode(scene->mRootNode, scene);
    
        }
        void processNode(aiNode *node, const aiScene *scene){
            
            //process all node's meshes (if any)
            for(unsigned int i = 0; i<node->mNumMeshes; i++){
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(processMesh(mesh, scene));
            }
            //do the same 
            for(unsigned int i = 0; i<node->mNumChildren; i++){
                processNode(node->mChildren[i], scene);
            }

        }
        Mesh processMesh(aiMesh *mesh, const aiScene *scene){
            
            //data to fill
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;
            //walk through each of of the mesh vertices
            for(unsigned int i = 0; i < mesh->mNumVertices; i++){
                
                Vertex vertex;

                glm::vec3 vector;
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.Position = vector;
                
                //normals
                if(mesh->HasNormals()){

                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    vertex.Normal = vector;
                }

                //texture coordinates

                if(mesh->mTextureCoords[0]) //does the mesh contains texture coordinates?
                {
                    glm::vec3 vec;

                    //a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                    //use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = vec;
                    
                    if(mesh->HasTangentsAndBitangents()){
                    
                        //tangent
                        vector.x = mesh->mTangents[i].x;
                        vector.y = mesh->mTangents[i].y;
                        vector.z = mesh->mTangents[i].z;
                        vertex.Tangent = vector;

                        //bitangen
                        vector.x = mesh->mBitangents[i].x;
                        vector.y = mesh->mBitangents[i].y;
                        vector.z = mesh->mBitangents[i].z;
                        vertex.Bitangent = vector;
                    }
                }
                else
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);

                vertices.push_back(vertex);
                
                
            }
            //now walk through eacg of the mesh's faces (a face is a mesh its triangle ) and retrieve the corresponding vertex indices
            for(unsigned int i = 0; i<mesh->mNumFaces; i++){
                aiFace face = mesh->mFaces[i];
                //retrieve all indices of the face and store them in the indices vector
                for(unsigned int j = 0; j<face.mNumIndices; j++){
                    indices.push_back(face.mIndices[j]);
                }
            }
            
            //process materials 
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN
        
            // 1. Diffuse maps
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. Specular maps
            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. Normal maps
            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. Height maps
            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
            
            //return a mesh object created from the extracted mesh data
            return Mesh(vertices, indices, textures);
            
        }
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName){

            std::vector<Texture> textures;
            for(unsigned int i = 0; i < mat->GetTextureCount(type); i++){
                aiString str;
                mat->GetTexture(type, i, &str);
                //check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
                bool skip = false;
                for(unsigned int j = 0; j < textures_loaded.size(); j++){
                    if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0){
                        textures.push_back(textures_loaded[j]);
                        skip = true;
                        break;
                    }
                }
                if(!skip){
                    //if texture hasn't been loaded already, load it
                    Texture texture;
                    texture.id = TextureFromFile(str.C_Str(), this->directory);
                    texture.type = typeName;
                    texture.path = str.C_Str();
                    textures.push_back(texture);
                    textures_loaded.push_back(texture);  //store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
                }

            }
            return textures;
        }
};

unsigned int TextureFromFile(const char* path, const std::string &directory, bool gamma){
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;

    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if(data){
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
    else{
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}

#endif
