#pragma once

#define AMPLITUDE_EPSILON 0.001f

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
			glm::vec4 pos;
			ofFloatColor color;
		};

		struct Wave {
			glm::vec3 pos;
			float outerRadius, innerRadius, amplitude, velocity, decay;
			
			Wave(glm::vec3 pos, float outerRadius, float innerRadius, float amplitude, float velocity, float decay)
			{
				this->pos = pos; this->outerRadius = outerRadius; this->innerRadius = innerRadius; this->amplitude = amplitude, this->velocity = velocity; this->decay = decay;
			};
		};

		ofVboMesh colorMesh, depthMesh;
		ofImage colorImage, depthImage;
		ofEasyCam cam;
		ofShader renderShader, computeParticlesShader;
		ofVbo vbo;
		ofBufferObject vertBuffer;
		int numVerts;

		vector<Wave> waves;

		bool emittingWave;
};
