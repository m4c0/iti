#version 450

layout(push_constant) uniform upc {
  vec2 window;
  float aspect;
} pc;

layout(location = 0) out vec4 frag_colour;

layout(location = 0) in float instance;
layout(location = 1) in float h;
layout(location = 2) in vec4 ccff;

const vec3 camera = vec3(2.0, 0.0, 6.0);
const float camera_angle = -0.6;

vec3 cam_ray(vec2 xy) {
  const float t = camera_angle;
  const mat2 mat = mat2(
    cos(t), sin(t),
    -sin(t), cos(t)
  );
  vec3 p = vec3(mat * vec2(xy.x, 1.0), xy.y);
  return normalize(p.xzy);
}

vec4 wall() {
  float r = mod(instance, 4.0) / 4.0;
  float g = mod(floor(instance / 4.0), 2.0) / 2.0;
  float b = floor(instance / 8.0) / 2.0;
  return vec4(r, g, b, 1);
}

vec4 ceil_c() {
  return vec4(0.5, 0.5, 0.5, 1.0);
}

mat2 view_model() {
  const float t = camera_angle;
  return mat2(
    cos(t), sin(t),
    -sin(t), cos(t)
  );
}

vec4 floor_c(vec2 p) {
  mat2 vmm = view_model();
  
  vec2 vm = vmm * vec2(0, 1);
  vec3 vmd = normalize(vec3(vm.x, p.y, vm.y));
  float t = (2.5 - camera.y) / vmd.y;
  
  vec2 rd0 = vmm * vec2(p.x, 1);
  vec3 rd = normalize(vec3(rd0.x, p.y, rd0.y));
  vec2 xz = camera.xz + rd.xz * t;
  return vec4(fract(xz) * 0.5 + 0.5, 0.0, 1.0);
}

/*
vec4 floor_c(vec2 p) {
  vec3 vm = cam_ray(vec2(0, 0));
  float t = (ccff.w - camera.y) / vm.y;

  vec3 rd = cam_ray(p);
  vec2 xz = camera.xz + rd.xz * t;

  return vec4(fract(xz) * 0.5 + 0.5, 0.2, 1.0);
}
*/

void main() {
  vec2 p = gl_FragCoord.xy / pc.window * 2.0 - 1.0;
  if (h > ccff.x) {
    frag_colour = ceil_c();
  } else if (h > ccff.y) {
    frag_colour = wall();
  } else if (h > ccff.z) {
    discard;
  } else if (h > ccff.w) {
    frag_colour = wall();
  } else {
    frag_colour = floor_c(p);
  }
}
