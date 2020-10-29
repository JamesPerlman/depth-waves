#version 440

struct Vertex {
	vec4 pos;
	vec4 color;
};

layout(std430, binding=0) buffer vertex {
	Vertex v[];
};

layout(binding = 1, rgba8) uniform readonly image2D colorTex;
layout(binding = 2, rgba8) uniform readonly image2D depthTex;

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


// apply 5x5 convolution kernel to smooth depth edges
float getDepth()
{
	float kernel[5][5] = {
		{ 1,  4,  6,  4, 1 },
		{ 4, 16, 24, 16, 4 },
		{ 6, 24, 36, 24, 6 },
		{ 4, 16, 24, 16, 4 },
		{ 1,  4,  6,  4, 1 },
	};
	float c = 256.f;

	float depth = 0.f;

	ivec2 maxPos = imageSize(depthTex) - 1;
	ivec2 minPos = ivec2(0, 0);
	
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			ivec2 pos = ivec2(gl_GlobalInvocationID.xy) - 2 + ivec2(i, j);
			pos = clamp(pos, minPos, maxPos);
			depth += kernel[i][j] * imageLoad(depthTex, pos).r;
		}
	}

	return depth / c;
}

void main()
{
	uint idx = 384 * gl_GlobalInvocationID.x + gl_GlobalInvocationID.y;
	
	float depth = getDepth();

	v[idx].pos = vec4(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, 1000.f * depth, 1.f);
	vec4 vertex = v[idx].color;
	v[idx].color = imageLoad(colorTex, ivec2(gl_GlobalInvocationID.xy));
}
