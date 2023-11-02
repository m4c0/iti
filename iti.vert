#version 450

layout(push_constant) uniform upc {
  vec4 camera;
  vec2 window;
  float aspect;
  float fov;
  float time;
} pc;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 xz0;
layout(location = 2) in vec2 xz1;
layout(location = 3) in vec4 ccff;

layout(location = 0) out float out_x;
layout(location = 1) out float out_h;
layout(location = 2) out vec4 out_ccff;

vec4 model() {
  vec2 d = mix(xz0, xz1, position);
  d -= pc.camera.xz;
  return vec4(d.x, 0, d.y, 1.0);
}

vec4 view_model() {
  float t = pc.camera.w;
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

  const float p10 = -(f + n) / fn;
  const float p14 = -2.0 * n * f / fn;
  const float tan_fov = tan(pc.fov);

  vec4 vm = view_model();
  vec4 res;
  res.x = vm.x / (tan_fov * pc.aspect);
  res.y = vm.y / tan_fov;
  res.z = vm.z * p10 + p14;
  res.w = -vm.z;
  return res;
}

void main() {
  vec4 p = frustum();
  p.y = xz0.y == xz1.y ? position.y : position.x;
  p.y = mix(-1.0, 1.0, p.y) * p.w;
  gl_Position = p;

  float xzd = length(xz1 - xz0);
  xzd *= xz0.y == xz1.y ? position.x : position.y;
  out_x = xzd;
  out_h = -p.y + pc.camera.y;
  out_ccff = ccff;
}
