#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{             
    FragColor = texture(ourTexture, TexCoord);
}