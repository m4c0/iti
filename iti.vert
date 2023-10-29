#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 xz0;
layout(location = 2) in vec2 xz1;
layout(location = 3) in float ceil_h;
layout(location = 4) in float floor_h;

layout(location = 0) out float instance;

void main() {
  vec3 xyz;
  xyz.xz = mix(xz0, xz1, position);
  xyz.y = position.y * 2.0 - 1.0;

  instance = gl_InstanceIndex;

  gl_Position = vec4(xyz, 1);
}
