#include "ofApp.h"
#include "HelperMath.h"

//--------------------------------------------------------------
void ofApp::setup(){

	depthToVertShader.setupShaderFromFile(GL_COMPUTE_SHADER, "shaders/compute-point-from-depth.glsl");
	depthToVertShader.linkProgram();

	ofEnableDepthTest();
	cam.setDistance(0);
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
		v.color = ofFloatColor(1, 0, 1, 1);
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

	// Compute Shader Info

	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	cout << "max global (total) work group counts x: " << work_grp_cnt[0] << ", y: " << work_grp_cnt[1] << ", z: " << work_grp_cnt[2] << endl;

	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	cout << "max local (in one shader) work group sizes x: " << work_grp_size[0] << ", y: " << work_grp_size[1] << ", z: " << work_grp_size[2] << endl;

}

//--------------------------------------------------------------
void ofApp::update(){
	ofClear(0, 0, 0, 0);

	depthToVertShader.begin();
	//depthToVertShader.setUniformMatrix4f("modelViewProjectionMatrix", cam.getModelViewProjectionMatrix());
	//depthToVertShader.setUniform1f("minDepth", 0.f);
	//depthToVertShader.setUniform1f("maxDepth", 1000.f);
	//depthToVertShader.setUniformTexture("depthTex", depthImage.getTexture(), 0);
	//depthToVertShader.setUniformTexture("colorTex", colorImage.getTexture(), 1);

	vertBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
	colorImage.getTexture().bindAsImage(1, GL_READ_ONLY);
	depthImage.getTexture().bindAsImage(2, GL_READ_ONLY);

	depthToVertShader.dispatchCompute(384, 384, 1);

	depthToVertShader.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofVec2f imageSize(colorImage.getWidth(), colorImage.getHeight());

	cam.begin();

	renderShader.begin();
	renderShader.setUniform1f("size", 1.0);
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

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
