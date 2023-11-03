#version 450

layout(push_constant) uniform upc {
  vec4 camera;
  vec2 window;
  float aspect;
  float fov;
  float time;
} pc;

layout(location = 0) out vec4 frag_colour;

layout(location = 0) in float out_x;
layout(location = 1) in float h;
layout(location = 2) in vec4 ccff;

mat3 cam() {
  float t = pc.camera.w;
  return mat3(
    cos(t), 0, sin(t),
    0, 1, 0,
    -sin(t), 0, cos(t)
  );
}
vec2 sector_uv(vec2 p) {
  mat3 cam = cam();
  
  const float fl = 1.0 / tan(pc.fov); // focal len
  vec3 ro = pc.camera.xyz;
  ro.x *= -1;
  vec3 rd = cam * vec3(p, fl);
  
  float t = (ccff.w - ro.y) / abs(rd.y);
  vec3 pos = ro + t*rd;
  return pos.xz;
}

vec4 wall() {
  vec2 uv = vec2(out_x, h);
  vec2 s = sign(fract(uv * 0.5) - 0.5);
  float pat = 0.5 - 0.5 * s.x * s.y;
  return vec4(0.2, vec2(pat * 0.3 + 0.2), 1.0);
}

vec4 ceil_c(vec2 p) {
  vec2 uv = sector_uv(p);
  vec2 s = sign(fract(uv * 0.5) - 0.5);
  float pat = 0.5 - 0.5 * s.x * s.y;
  return vec4(vec2(pat * 0.3 + 0.2), 0.2, 1.0);
}

vec4 floor_c(vec2 p) {
  vec2 uv = sector_uv(p);
  vec2 s = sign(fract(uv * 0.5) - 0.5);
  float pat = 0.5 - 0.5 * s.x * s.y;
  return vec4(vec3(pat * 0.3 + 0.2), 1.0);
}

void main() {
  vec2 p = (2.0 * gl_FragCoord.xy - pc.window) / pc.window.y;

  if (h > ccff.x) {
    frag_colour = ceil_c(p);
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
