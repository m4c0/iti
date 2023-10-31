#version 450

layout(push_constant) uniform upc {
  vec2 window;
  float aspect;
} pc;

layout(location = 0) out vec4 frag_colour;

layout(location = 0) in float instance;
layout(location = 1) in float h;
layout(location = 2) in vec4 ccff;

vec4 wall() {
  float r = mod(instance, 4.0) / 4.0;
  float g = mod(floor(instance / 4.0), 2.0) / 2.0;
  float b = floor(instance / 8.0) / 2.0;
  return vec4(r, g, b, 1);
}

vec4 ceil_c() {
  return vec4(0.5, 0.5, 0.5, 1.0);
}

vec4 floor_c() {
  return vec4(0.2, 0.2, 0.2, 1.0);
}

void main() {
  if (h > ccff.x) {
    frag_colour = ceil_c();
  } else if (h > ccff.y) {
    frag_colour = wall();
  } else if (h > ccff.z) {
    discard;
  } else if (h > ccff.w) {
    frag_colour = wall();
  } else {
    frag_colour = floor_c();
  }
}
