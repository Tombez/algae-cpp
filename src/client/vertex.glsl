#version 130

uniform vec2 camera;
uniform vec2 scale;

in vec2 position;
in vec4 color;

out vec4 col;

void main() {
	col = color / 255.0;
	gl_Position = vec4((position - camera) * scale, 0.0, 1.0);
}
