#version 450

layout(location = 0) out vec4 frag_colour;

layout(location = 0) in float instance;
layout(location = 1) in float ceil_h;
layout(location = 2) in float floor_h;
layout(location = 3) in float h;

void main() {
  if (ceil_h > h && h > floor_h) discard;

  float r = mod(instance, 4.0) / 4.0;
  float g = mod(floor(instance / 4.0), 2.0) / 2.0;
  float b = floor(instance / 8.0) / 2.0;
  frag_colour = vec4(r, g, b, 1);
}
