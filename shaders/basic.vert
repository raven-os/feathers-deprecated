#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out uint textureId;

layout(constant_id = 0) const float screenXSize = 1.0; // window x scaling
layout(constant_id = 1) const float screenYSize = 1.0; // window y scaling


out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
  const vec2 screenSize = vec2(screenXSize, screenYSize);

  textureId = gl_VertexIndex / 6;
  fragTexCoord = texCoord;
  gl_Position = vec4(pos.xy / screenSize, pos.z, 1.0);
}
