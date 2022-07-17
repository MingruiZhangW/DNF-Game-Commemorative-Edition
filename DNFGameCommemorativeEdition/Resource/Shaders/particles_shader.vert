#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

// Position here is the origin of the particle system
in vec3 position;
// PosOffset is the particle position offset, this is in world space
in vec3 posOffset;
in vec2 textureCoor;

out vec2 TextureCoor;

void main() {
	vec4 finalPos;

	finalPos = M * vec4(position, 1.0);
	finalPos.x = finalPos.x + posOffset.x;
	finalPos.y = finalPos.y + posOffset.y;

	gl_Position = P * V * finalPos;

	TextureCoor = textureCoor;
}
