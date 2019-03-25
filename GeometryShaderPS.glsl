#version 440

void main(){
	for(int i = 0; i < gl_in.length(), i++){
		gl_Position = gl_in[i].gl_Position;
	}
}