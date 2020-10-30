#version 440

struct Vertex {
	vec4 pos;
	vec4 color;
};

struct Wave {
	vec3 pos;
	float outerRadius;
	float innerRadius;
	float amplitude;
	float velocity;
	float decay;
};

layout(binding = 0, rgba8) uniform readonly image2D colorTex;
layout(binding = 1, rgba8) uniform readonly image2D depthTex;

layout(std430, binding = 2) buffer vertex {
	Vertex v[];
};

layout(std430, binding = 3) buffer wave {
	Wave w[];
};

uniform float minDepth;
uniform float maxDepth;
uniform mat4x4 cameraModelViewMatrix;
uniform vec2 cameraFov;

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// apply 5x5 convolution kernel to smooth depth edges
float getSmoothedDepth()
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

vec4 getWorldPosition()
{
	vec2 uv = vec2(gl_GlobalInvocationID.xy) / vec2(imageSize(depthTex));

	float d = 1.0;//getSmoothedDepth();

	float zCam = minDepth + d * (maxDepth - minDepth);

	vec2 focalLen = 0.5f / tan(0.5f * cameraFov);

	vec2 pixelTans = (uv - 0.5f) / focalLen;

	vec3 pos = zCam * vec3(pixelTans, 1.f);

	mat4 m = cameraModelViewMatrix;

	return vec4(pos, 1.0);
}

void main()
{
	// Step 1: Get point in space where particle is supposed to be
	vec4 point = getWorldPosition();

	// Set vertex coordinate

	uint idx = 384 * gl_GlobalInvocationID.x + gl_GlobalInvocationID.y;

	vec2 depthSizef = vec2(imageSize(depthTex));
	vec2 colorSizef = vec2(imageSize(colorTex));

	ivec2 px = ivec2(vec2(gl_GlobalInvocationID.xy) / depthSizef * colorSizef);
	v[idx].pos = point;
	v[idx].color = imageLoad(colorTex, px);
}
