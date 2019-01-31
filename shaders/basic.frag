#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) flat in uint textureId;

layout(location = 0) out vec4 outColor;

layout(constant_id = 2) const uint TextureBatchSize = 1;

layout(set = 0, binding = 0) uniform sampler windowSampler;
layout(set = 1, binding = 0) uniform texture2D windowTextures[TextureBatchSize];

void main() {
  outColor = vec4(texture(sampler2D(windowTextures[textureId % TextureBatchSize], windowSampler), fragTexCoord).gba, 1.0f);
}