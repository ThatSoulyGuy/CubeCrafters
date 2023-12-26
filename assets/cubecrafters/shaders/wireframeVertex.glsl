#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 vertices[8];
    vertices[0] = vec3(-0.5045, -0.5045, -0.5045);
    vertices[1] = vec3( 0.5045, -0.5045, -0.5045);
    vertices[2] = vec3( 0.5045,  0.5045, -0.5045);
    vertices[3] = vec3(-0.5045,  0.5045, -0.5045);
    vertices[4] = vec3(-0.5045, -0.5045,  0.5045);
    vertices[5] = vec3( 0.5045, -0.5045,  0.5045);
    vertices[6] = vec3( 0.5045,  0.5045,  0.5045);
    vertices[7] = vec3(-0.5045,  0.5045,  0.5045);

    int edgeIndex = gl_InstanceID * 2;
    int edges[24];

    edges[0] = 0;
    edges[1] = 1;
    edges[2] = 1;
    edges[3] = 2;
    edges[4] = 2;
    edges[5] = 3;
    edges[6] = 3;
    edges[7] = 0;
    edges[8] = 4;
    edges[9] = 5;
    edges[10] = 5;
    edges[11] = 6;
    edges[12] = 6;
    edges[13] = 7;
    edges[14] = 7;
    edges[15] = 4;
    edges[16] = 0;
    edges[17] = 4;
    edges[18] = 1;
    edges[19] = 5;
    edges[20] = 2;
    edges[21] = 6;
    edges[22] = 3;
    edges[23] = 7;

    vec3 pos = vertices[edges[edgeIndex + gl_VertexID % 2]];

    gl_Position = projection * view * model * vec4(pos, 1.0);
}