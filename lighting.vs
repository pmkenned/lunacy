#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 color;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec4 vertColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool invertTexCoords;

void main()
{
    gl_Position = projection * view *  model * vec4(position, 1.0f);
	vertColor = vec4(color, 1.0f);
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;  
	if(invertTexCoords)
		TexCoords = vec2(texCoords.x, 1.0f - texCoords.y);
	else
		TexCoords = texCoords;
} 