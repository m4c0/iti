#version 450

layout(push_constant) uniform upc {
  vec2 window;
  float aspect;
  float time;
} pc;

layout(location = 0) out vec4 frag_colour;

layout(location = 0) in float instance;
layout(location = 1) in float h;
layout(location = 2) in vec4 ccff;

const vec3 camera = vec3(2.0, 0.0, 6.0);
const float camera_angle = -0.6;

mat3 cam() {
  float t = camera_angle + pc.time * 0.25;
  return mat3(
    cos(t), 0, sin(t),
    0, 1, 0,
    -sin(t), 0, cos(t)
  );
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

vec4 floor_c(vec2 p) {
  mat3 cam = cam();
  
  const float fl = 1.5; // focal len
  vec3 ro = camera;
  vec3 rd = cam * vec3(p, fl);
  
  float t = (ccff.w - ro.y) / rd.y;
  vec3 pos = ro + t*rd;

  vec2 s = sign(fract(pos.xz * 0.5) - 0.5);
  float pat = 0.5 - 0.5 * s.x * s.y;
  return vec4(vec3(pat * 0.3 + 0.2), 1.0);
}

void main() {
  vec2 p = gl_FragCoord.xy / pc.window * 2.0 - 1.0;
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
