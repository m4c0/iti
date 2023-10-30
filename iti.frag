#version 450

layout(location = 0) out vec4 frag_colour;

layout(location = 0) in float instance;
layout(location = 1) in float ceil_h;
layout(location = 2) in float floor_h;
layout(location = 3) in float h;

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
  if (h > ceil_h) {
    frag_colour = ceil_c();
  } else if (h > floor_h) {
    frag_colour = wall();
  } else {
    frag_colour = floor_c();
  }
}
