#version 430

uniform sampler2D u_TextureSampler;

out vec4 fs_Color;

void main() {
  const vec2 viewport_size = textureSize(u_TextureSampler, 0);
  const vec2 texture_coordinates = gl_FragCoord.xy / viewport_size;
  fs_Color = texture(u_TextureSampler, texture_coordinates);
}
