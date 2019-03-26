#version 440
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 texCoords;

const float size = 0.2;
const vec2 offsets[4] = { vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0) };

//void CreateVertex(vec3 offset){
//	vec4 finalOffset = vec4(offset * size, 0.0);
//	gl_Position = gl_in[0].gl_Position * finalOffset;
//	EmitVertex();
//}

void main(){
//	CreateVertex(vec3(-1.0, 1.0, 1.0));
//	CreateVertex(vec3(-1.0, -1.0, 1.0));
//	CreateVertex(vec3(1.0, 1.0, 1.0));
//	CreateVertex(vec3(1.0, -1.0, 1.0));
	for(int i = 0; i < 4; i++){
		texCoords = offsets[i];
		vec4 pos = gl_in[0].gl_Position;
		pos.xy += size * (offsets[i] - vec2(size));

		EmitVertex();
	}
	EndPrimitive();
}