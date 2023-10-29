#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 xz0;
layout(location = 2) in vec2 xz1;
layout(location = 3) in float ceil_h;
layout(location = 4) in float floor_h;

layout(location = 0) out float instance;

vec2 world() {
  return mix(xz0, xz1 + vec2(0.1), position);
}

vec2 world2camera(vec2 p) {
  p -= vec2(2.0, 6.0);

  float t = 0.7;
  return mat2(cos(t), -sin(t), sin(t), cos(t)) * p;
}

vec4 camera() {
  vec2 p = world2camera(world());
  return vec4(p.x, position.y, p.y, 1.0);
}

void main2() {
  vec4 p = camera();

  instance = gl_InstanceIndex;

  const float f = 1000.0;
  const float n = 0.1;
  const float fn = f - n;

  p *= mat4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, -f/fn, -1,
    0, 0, -f*n/fn, 0
  );
  p /= p.w;
  p.z = clamp(p.z, 0.0, 1.0);
  gl_Position = p;
}

void main() {
  instance = gl_InstanceIndex;

  vec2 p = camera().xz;

  p /= 20.0;

  gl_Position = vec4(p, 0, 1);
}
