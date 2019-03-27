#version 440

uniform sampler2D particleTex;

in vec2 texCoords;
out vec4 finalColor;

void main(){
	finalColor = texture(particleTex, texCoords);
}