#version 450

layout(push_constant) uniform upc {
  float aspect;
} pc;

layout(location = 0) in vec2 frag_coord;

layout(location = 0) out vec4 frag_colour;

void main() {
  vec2 p = frag_coord * 0.5 + 0.5;
  
  frag_colour = vec4(p, 0, 1);
}
