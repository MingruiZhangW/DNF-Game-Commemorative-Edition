#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float tilt = 0.4;
uniform float scale = 0.3;

in vec3 position;
in vec2 textureCoor;

out vec2 TextureCoor;

void main() {
	// Do shear transformation
	mat3 mat_scale = mat3(vec3(1.0,0.0,0.0),vec3(0.0,1.0,0.0),vec3(0.0,0.0,1.0));
	mat3 mat_tilt = mat3(vec3(1.0, tilt, 0.0), vec3(0.0, scale, 0.0), vec3(0.0, 0.0, 1.0));
	mat3 mul = mat_scale * mat_tilt;
	vec3 temp = position * mul;
	
	gl_Position =  P * V * M * vec4(temp, 1.0);

	TextureCoor = textureCoor;
}
