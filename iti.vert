#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 xz0;
layout(location = 2) in vec2 xz1;
layout(location = 3) in float ceil_h;
layout(location = 4) in float floor_h;

layout(location = 0) out float instance;

vec4 model() {
  vec2 d = vec2(0.0);
  d = mix(xz0, xz1 + d, position);
  d -= vec2(2.0, 6.0);

  float y = xz0.y == xz1.y ? position.y : position.x;
  y = mix(ceil_h, floor_h, y);
  return vec4(d.x, y, d.y, 1.0);
}

vec4 view_model() {
  const float t = -0.6;
  const mat4 view = mat4(
    cos(t), 0, sin(t), 0,
    0, 1, 0, 0,
    -sin(t), 0, cos(t), 0,
    0, 0, 0, 1
  );
  return view * model();
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
  const mat4 proj = mat4(
    p0, 0, 0, 0,
    0, p5, 0, 0,
    0, 0, p10, -1,
    0, 0, p14, 0
  );
  return proj * view_model();
}

void main() {
  instance = gl_InstanceIndex;

  vec4 p = frustum();
  gl_Position = p;
}
