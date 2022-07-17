#version 330

uniform sampler2D theTexture;

in vec2 TextureCoor;

out vec4 fragColor;

void main() {
	fragColor = texture(theTexture, TextureCoor);
}
