#version 330 core

out vec4 FragColor;

uniform vec3 earthColor;

void main(){
	FragColor = vec4(earthColor, 1.0);
}
