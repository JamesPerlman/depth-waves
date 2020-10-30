#include "ofApp.h"
#include "HelperMath.h"

//--------------------------------------------------------------
void ofApp::setup(){

	computeParticlesShader.setupShaderFromFile(GL_COMPUTE_SHADER, "shaders/compute-particles.glsl");
	computeParticlesShader.linkProgram();

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
void ofApp::update() {
	ofClear(0, 0, 0, 0);

	/** Update Waves */

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
	float camFov[2] = { xFov, xFov / cam.getAspectRatio() };

	auto waveBuf = ofBufferObject();
	waveBuf.allocate(waves.size() * sizeof(waves), waves.data(), GL_UNIFORM_BUFFER);

	// start compute shader
	computeParticlesShader.begin();

	colorImage.getTexture().bindAsImage(0, GL_READ_ONLY);
	depthImage.getTexture().bindAsImage(1, GL_READ_ONLY);

	vertBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 2);
	waveBuf.bindBase(GL_SHADER_STORAGE_BUFFER, 3);

	computeParticlesShader.setUniform1f("minDepth", 0.f);
	computeParticlesShader.setUniform1f("maxDepth", 1000.f);
	computeParticlesShader.setUniformMatrix4f("cameraModelViewMatrix", cam.getModelViewMatrix());
	computeParticlesShader.setUniform2fv("cameraFov", camFov);

	computeParticlesShader.dispatchCompute(384, 384, 1);
	computeParticlesShader.end();
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
	// start wave
	emittingWave = true;
	waves.emplace_back(glm::vec3(0, 0, 0), 0.f, 0.f, 1.f, 1.f, 0.1f);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	// end wave
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
