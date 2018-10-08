#version 300 es

out highp vec4 outColor;

in highp vec2 fragTexCoord;

uniform sampler2D image;

void main()
{
  outColor = texture(image, fragTexCoord) + vec4(0.1f);
}
