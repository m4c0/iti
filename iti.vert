#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 xz0;
layout(location = 2) in vec2 xz1;
layout(location = 3) in float ceil_h;
layout(location = 4) in float floor_h;

layout(location = 0) out float instance;

vec2 world() {
  vec2 d = vec2(0.1) * 0.0;
  return mix(xz0, xz1 + d, position);
}

vec2 world2camera(vec2 p) {
  p -= vec2(2.0, 6.0);

  float t = 0.6;
  return mat2(cos(t), -sin(t), sin(t), cos(t)) * p;
}

vec4 camera() {
  vec2 p = world2camera(world());
  float y = (position.y - 0.5) * 1000.0;
  return vec4(p.x, y, p.y, 1.0);
}

vec4 frustum() {
  const float f = 1000.0;
  const float n = 0.1;
  const float fn = f - n;

  const float fov = 1.0; // tan(45 deg)
  const float aspect = 16.0 / 9.0;

  const float ymax = n * fov;
  const float xmax = ymax * aspect;

  const float p0 = 2.0 * n / (xmax - -xmax);
  const float p5 = 2.0 * n / (ymax - -ymax);
  const float p10 = (-f - n) / fn;
  const float p14 = -2.0 * n * f / fn;
  const mat4 fr_mat = mat4(
    p0, 0, 0, 0,
    0, p5, 0, 0,
    0, 0, p10, -1,
    0, 0, p14, 0
  );

  vec4 p = camera();
  p = fr_mat * p;
  return p;
}

void main() {
  instance = gl_InstanceIndex;

  vec4 p = frustum();
  //p /= p.w;

  //gl_Position = vec4(p.x, p.z / 10.0, 0, 1);
  //gl_Position = vec4(camera().xz / 20.0, 0, 1);

  //p.y = position.y;
  gl_Position = p;
}
