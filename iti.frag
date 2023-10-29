#version 450

layout(location = 0) out vec4 frag_colour;

layout(location = 0) in float instance;

void main() {
  float r = mod(instance, 4.0) / 4.0;
  float g = mod(floor(instance / 4.0), 2.0) / 2.0;
  float b = floor(instance / 8.0) / 2.0;
  frag_colour = vec4(r, g, b, 1);
}
