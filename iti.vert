#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 xz0;
layout(location = 2) in vec2 xz1;
layout(location = 3) in float ceil_h;
layout(location = 4) in float floor_h;

layout(location = 0) out float instance;

void main2() {
  vec4 p;
  p.xz = mix(xz0, xz1, position);
  p.y = position.y * 2.0 - 1.0;

  p.xz -= vec2(2.0, 6.0);
  float t = 0.7;
  p.xz = mat2(cos(t), -sin(t), sin(t), cos(t)) * p.xz;

  instance = gl_InstanceIndex;

  const float f = 1000.0;
  const float n = 0.1;
  const float fn = f - n;

  p.w = 1.0;
  p *= mat4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, -f/fn, -1,
    0, 0, -f*n/fn, 0
  );
  p.z = clamp(p.z, 0.0, 1.0);
  p /= p.w;
  gl_Position = p;
}

void main() {
  instance = gl_InstanceIndex;

  vec2 p;
  p = mix(xz0, xz1 + vec2(0.1), position);
  p -= vec2(2.0, 6.0);

  float t = 0.7;
  p = mat2(cos(t), -sin(t), sin(t), cos(t)) * p;

  p /= 20.0;

  gl_Position = vec4(p, 0, 1);
}
