#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texCoord;

layout(constant_id = 0) const float screenXSize = 1.0; // window x scaling
layout(constant_id = 1) const float screenYSize = 1.0; // window y scaling

layout(location = 0) out vec2 fragTexCoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
  const vec2 screenSize = vec2(screenXSize, screenYSize);

  fragTexCoord = texCoord / screenSize;
  gl_Position = vec4(pos / screenSize, 0.0, 1.0);
}
