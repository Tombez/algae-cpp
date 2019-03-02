#version 130

in vec2 position;
in vec4 color;

out vec4 col;

void main() {
	col = color / 255.0;
	gl_Position = vec4(position, 0.0, 1.0);
}
