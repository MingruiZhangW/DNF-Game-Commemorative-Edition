#version 330

uniform float alpha = 0.4;

uniform vec3 colour;
uniform sampler2D theTexture;

in vec2 TextureCoor;

out vec4 fragColor;

void main() {
	// Make the texture black with some transparency
	vec4 previous_color = texture(theTexture, TextureCoor);
	vec4 block_color = vec4(0.0, 0.0, 0.0, previous_color.a * alpha);

    fragColor = block_color;
}
