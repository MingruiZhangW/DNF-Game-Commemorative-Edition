#version 330

uniform vec3 colour;
uniform sampler2D theTexture;

in vec2 TextureCoor;

out vec4 fragColor;

void main() {
	fragColor = texture(theTexture, TextureCoor);
}
