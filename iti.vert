#version 450

layout(push_constant) uniform upc {
  float aspect;
} pc;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 xz0;
layout(location = 2) in vec2 xz1;
layout(location = 3) in vec4 ccff;

layout(location = 0) out float instance;
layout(location = 1) out float out_h;
layout(location = 2) out vec4 out_ccff;

const vec3 camera = vec3(2.0, 0.0, 6.0);
const float camera_angle = -0.6;

vec4 model() {
  vec2 d = mix(xz0, xz1, position);
  d -= camera.xz;
  return vec4(d.x, 0, d.y, 1.0);
}

vec4 view_model() {
  const float t = camera_angle;
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

  const float ymax = n * fov;
  const float xmax = ymax * pc.aspect;

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
  p.y = xz0.y == xz1.y ? position.y : position.x;
  p.y = mix(-1.0, 1.0, p.y) * p.w;
  gl_Position = p;

  out_h = p.y;
  out_ccff = ccff;
}
