#include "HelperMath.h"

ofVec3f uvToWorld(ofImage depthTex, ofVec2f uv, ofCamera cam, float minDepth, float maxDepth)
{
	int x = int(float(depthTex.getWidth()) * uv.x);
	int y = int(float(depthTex.getHeight()) * uv.y);

	float d = depthTex.getColor(x, y).r / 255.0;

	float xFov = cam.getFov();
	float yFov = xFov / cam.getAspectRatio();

	float zCam = minDepth + d * (maxDepth - minDepth);

	float focalLenX = 0.5 / tan(0.5 * xFov);
	float focalLenY = 0.5 / tan(0.5 * yFov);

	float pxTanX = (uv.x - 0.5) / focalLenX;
	float pxTanY = (uv.y - 0.5) / focalLenY;

	ofVec3f camPos = zCam * ofVec3f(pxTanX, pxTanY, 1.0);

	return cam.cameraToWorld(camPos);
}

void helper::genMesh(ofVboMesh &mesh, ofImage depthImage, ofImage colorImage, ofCamera cam, float minDepth, float maxDepth)
{
	int cw = colorImage.getWidth();
	int ch = colorImage.getHeight();

	int dw = depthImage.getWidth();
	int dh = depthImage.getHeight();

	float da = float(dw) / float(dh);

	float xFov = ofDegToRad(cam.getFov());
	float yFov = xFov;// xFov / cam.getAspectRatio();

	float focalLenX = 0.5 / tan(0.5 * xFov);
	float focalLenY = 0.5 / tan(0.5 * yFov);

	glm::mat4 invCamMat = inverse(cam.getModelViewMatrix());

	for (int i = 0; i < dw; ++i)
	{
		for (int j = 0; j < dh; ++j)
		{

			ofVec2f uv(float(i) / float(dw), 1.f - float(j) / float(dh));

			float d = depthImage.getColor(i, j).r / 255.f;

			float zCam = 10;// minDepth + d * (maxDepth - minDepth);

			float pxTanX = (uv.x - 0.5) / focalLenX;
			float pxTanY = (uv.y - 0.5) / focalLenY;

			ofVec3f camPos = zCam * ofVec3f(pxTanX, pxTanY, -1.0);

			ofVec3f worldPos = camPos * invCamMat;

			ofColor c = colorImage.getColor(i * cw / dw, j * ch / dh);
			
			mesh.addVertex(worldPos);
			mesh.addColor(c);
		}
	}
}
