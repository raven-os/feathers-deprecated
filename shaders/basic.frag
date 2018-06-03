#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D image;

void main() {
  outColor.rgb = texture(image, fragTexCoord).rgb;
  outColor.rg = fragTexCoord;
  outColor.b = 0.0f;
  outColor.a = 1.0f;
}