#version 440
#extension GL_ARB_separate_shader_objects : enable

in vec4 fragColor;

out vec4 outColor;

void main (void)  
{  
   outColor = fragColor;
}
