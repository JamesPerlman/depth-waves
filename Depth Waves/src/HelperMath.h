#pragma once
#pragma once
#include "ofMain.h"

namespace helper {
	void genMesh(ofVboMesh &mesh, ofImage depthImage, ofImage colorImage, ofCamera cam, float minDepth, float maxDepth);
}
