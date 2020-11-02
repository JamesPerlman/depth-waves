#include "ofApp.h"
#include "HelperMath.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);

	computeParticlesShader.setupShaderFromFile(GL_COMPUTE_SHADER, "shaders/compute-particles.glsl");
	computeParticlesShader.linkProgram();

	ofEnableDepthTest();
	depthImage.load("depth.png");
	depthImage.setImageType(OF_IMAGE_COLOR_ALPHA);
	colorImage.load("color-small.png");
	colorImage.setImageType(OF_IMAGE_COLOR_ALPHA);

	depthImageWidth = depthImage.getWidth();
	depthImageHeight = depthImage.getHeight();

	numVerts = depthImageWidth * depthImageHeight;

	vector<Vertex> verts;
	verts.resize(numVerts);
	for (auto &v : verts) {
		v.color = ofFloatColor(0, 0, 0, 0);
	}

	vertBuffer.allocate(sizeof(Vertex) * numVerts, GL_DYNAMIC_DRAW);
	vbo.setVertexBuffer(vertBuffer, 4, sizeof(Vertex), 0);
	vbo.setColorBuffer(vertBuffer, sizeof(Vertex), sizeof(glm::vec4));
	vbo.enableColors();

	renderShader.setGeometryInputType(GL_POINTS);
	renderShader.setGeometryOutputType(GL_TRIANGLE_STRIP);
	renderShader.setGeometryOutputCount(14);
	renderShader.load("shaders/render-blocks.vert", "shaders/render-blocks.frag", "shaders/render-blocks.geom");

	// Set up compute shader

	vertBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);

	// Set up GUI

}

//--------------------------------------------------------------
void ofApp::update() {
	ofClear(0, 0, 0, 0);

	/** Update Waves */

	if (isEmittingWave)
	{

	}

	for (auto &wave : waves)
	{
		wave.outerRadius += wave.velocity;
		wave.innerRadius += wave.velocity;
		wave.amplitude *= wave.decay;
	}

	// Remove waves whose magnitudes are close to zero
	if (waves.size() > 0)
	{
		auto deadWaves = std::remove_if(waves.begin(), waves.end(), [](Wave wave) { return fabs(wave.amplitude) <= AMPLITUDE_EPSILON; });
		waves.erase(deadWaves, waves.end());
	}

	// prepare data to pass to shader
	float xFov = ofDegToRad(cam.getFov());
	ofVec2f camFov(xFov, xFov);
	glm::vec3 camPos = cam.getPosition();
	glm::vec3 camRot = cam.getOrientationEulerRad();
	glm::vec3 camDir = cam.getLookAtDir();

	auto waveBuf = ofBufferObject();
	waveBuf.allocate(waves.size() * sizeof(waves), waves.data(), GL_UNIFORM_BUFFER);

	// start compute shader
	computeParticlesShader.begin();

	colorImage.getTexture().bindAsImage(0, GL_READ_ONLY);
	depthImage.getTexture().bindAsImage(1, GL_READ_ONLY);

	vertBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 2);
	waveBuf.bindBase(GL_SHADER_STORAGE_BUFFER, 3);

	computeParticlesShader.setUniform1f("minDepth", -1000.f);
	computeParticlesShader.setUniform1f("maxDepth", -100.f);
	computeParticlesShader.setUniform3f("cameraRot", camRot.x, camRot.y, camRot.z);
	computeParticlesShader.setUniform3f("cameraPos", camPos.x, camPos.y, camPos.z);
	computeParticlesShader.setUniform2f("cameraFov", camFov.x, camFov.y);

	ofLog(OF_LOG_NOTICE, ofToString(cam.getPosition()));

	computeParticlesShader.dispatchCompute(384, 384, 1);
	computeParticlesShader.end();

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofVec2f imageSize(colorImage.getWidth(), colorImage.getHeight());

	cam.begin();

	renderShader.begin();
	renderShader.setUniform1f("size", .5);
	renderShader.setUniform2f("imageSize", imageSize * 4);
	renderShader.setUniformMatrix4f("modelViewProjectionMatrix", cam.getModelViewProjectionMatrix());
	
	vbo.draw(GL_POINTS, 0, numVerts);
	
	renderShader.end();

	cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	// start wave
	isEmittingWave = true;
	waves.emplace_back(glm::vec3(0, 0, 0), 0.f, 0.f, 1.f, 1.f, 0.1f);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	// end wave
	isEmittingWave = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
