#include <iostream>
#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader{

    public:
        //program ID
        unsigned int ID;
        
        Shader(const char* vertexPath, const char* fragmentPath){

            //1. Retrieve the vertex/frgament source code from filePath
            std::string vertexCode;
            std::string fragmentCode;
            std::ifstream vShaderFile;
            std::ifstream fshaderFile;
            //ensure ifstream objects can throw exceptions
            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fshaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try{
                //open files
                vShaderFile.open(vertexPath);
                fshaderFile.open(fragmentPath);
                std::stringstream vShaderStream, fshaderStream;
                //read file's buffer contents into streams
                vShaderStream << vShaderFile.rdbuf();
                fshaderStream << fshaderFile.rdbuf();
                //close file handlers
                vShaderFile.close();
                fshaderFile.close();
                //convert stream intro string
                vertexCode = vShaderStream.str();
                fragmentCode = fshaderStream.str();
            }
            catch(std::ifstream::failure e){
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ"
                    <<std::endl;
            }
            const char* vShaderCode = vertexCode.c_str();
            const char* fShaderCode = fragmentCode.c_str();
            //2.compile shaders
            GLuint vertex,fragment;
            int success;
            char infoLog[512];

            //vertex shader
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex,1,&vShaderCode, NULL);
            glCompileShader(vertex);

            //print compile errors if any
            glGetShaderiv(vertex, GL_COMPILE_STATUS,&success);
            if(!success){
                glGetShaderInfoLog(vertex,512,NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"<< infoLog << std::endl;
                
            }
            //Similar for Fragment Shader
            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);

            glGetShaderiv(fragment, GL_COMPILE_STATUS,&success);
            if(!success){
                glGetShaderInfoLog(fragment,512,NULL, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"<< infoLog << std::endl;
            }
            //shader program
            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);
            glLinkProgram(ID);
            //print linking errors if any
            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if(!success){
                glGetProgramInfoLog(ID, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                    << infoLog << std::endl;
 }
            glDeleteShader(vertex);
            glDeleteShader(fragment);
        }
        //use/activate the Shader
        void use(){
            glUseProgram(ID);
        }
        //utility uniform functions
        void setBool(const std::string &name, bool value) const{
            glUniform1i(glGetUniformLocation(ID, name.c_str()),(int)value);
        }
        void setInt(const std::string &name, int value) const{
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }
        void setFloat(const std::string &name, float value) const{
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }
        void setMat4(const std::string &name, glm::mat4 model){
           unsigned int modelLoc = glGetUniformLocation(ID, name.c_str());
           glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        }
        /*
        void setVec3(const std::string &name, float x, float y, float z){
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        }
        */
    void setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }
 
};




#endif


