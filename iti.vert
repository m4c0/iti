#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 xz0;
layout(location = 2) in vec2 xz1;
layout(location = 3) in float ceil_h;
layout(location = 4) in float floor_h;

layout(location = 0) out vec2 frag_coord;

void main() {
  vec2 p = position * 2.0 - 1.0;
  gl_Position = vec4(p, 0, 1);
  frag_coord = p;
}
