#version 460

smooth in vec2 fragTC;		// Interpolated texture coordinates

uniform sampler2D tex;		// Texture sampler

out vec3 outCol;	// Final pixel color

void main() {
	outCol = texture(tex, fragTC).rgb;
}
