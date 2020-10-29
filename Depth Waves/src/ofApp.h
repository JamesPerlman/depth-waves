#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		int depthImageWidth, depthImageHeight;

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		struct Vertex {
			glm::vec3 pos;
			ofFloatColor color;
		};

		struct Wave {
			glm::vec3 pos;
			float radius;
			float wavelength;
			float amplitude;
			float decay;
		};

		ofVboMesh colorMesh, depthMesh;
		ofImage colorImage, depthImage;
		ofEasyCam cam;
		ofShader renderShader, depthToVertShader;
		ofVbo vbo;
		ofBufferObject vertBuffer;
		int numVerts;

		vector<Wave> waves;

};
