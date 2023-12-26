#version 330 core

in vec3 color;
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D diffuse;

void main()
{
    FragColor = texture(diffuse, texCoords) * vec4(color, 1.0);
}