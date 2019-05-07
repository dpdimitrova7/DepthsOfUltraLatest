#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	smallFont.loadFont("selena.otf", 16); //http://openfontlibrary.org/en/font/selena
	largeFont.loadFont("selena.otf", 48);

	//ofSetBackgroundColor(0);
	bubble.load("bubblepng.png");
	cone.set(20, 200);
	//seabed.load("Seabed2-thesequel.mov");
	//seabed.play();
	//The Kinect here is just an OSC receiver and parser
	//It just needs a port number and font for the debug text
	kinect.setup(12345, smallFont);

	//Here we get a pointer to the list of skeletons it has parsed
	//from OSC
	skeletons = kinect.getSkeletons();

	//We could inspect the skeletons and draw them here in ofApp
	//but for now let's pass the list to a default renderer class
	renderer.setup(skeletons, largeFont);

	//KINECT END

	ofEnableDepthTest();
	ofDisableArbTex();

	myCamera.tiltRad(1.6);
	scl = 10; // Set Scale of grids

	terrainW = 800;
	terrainH = 600;


	cols = terrainW / scl; // set rows per scl
	rows = terrainH / scl; // set columns per scl


	cout << "columns numbers: " << cols << endl;
	cout << "rows numbers: " << cols << endl;

	//mesh.setMode(OF_PRIMITIVE_POINTS);
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);


	for (int j = 0; j < rows; j++) {
		xoff = 0;
		for (int i = 0; i < cols; i++) {

			float x = i * scl;
			float y = j * scl;
			//float z = ofRandom(-10,10);
			float z = ofMap(ofNoise(yoff, xoff), 0, 1, -50, 50);
			mesh.addVertex(ofVec3f(x, y, z));
			mesh.addColor(ofFloatColor(0.1, 0.3, 1.2));
			mesh.addColor(ofFloatColor(ofMap(z, 50, 50, 0, 1), ofMap(z, -115, 10, 0, 1), ofMap(z, -150, 176, 0, 1), 1));

			xoff += 0.1;
		}
		yoff += 0.1;
	}


	// Generate order of indices to set triangles per rows and column
	for (int j = 0; j < rows - 1; j++) {
		for (int i = 0; i < cols - 1; i++) {

			mesh.addIndex(i + j * cols);         // 0
			mesh.addIndex((i + 1) + j * cols);     // 1
			mesh.addIndex(i + (j + 1)*cols);     // 6

			mesh.addIndex((i + 1) + j * cols);     // 1
			mesh.addIndex((i + 1) + (j + 1)*cols); // 7
			mesh.addIndex(i + (j + 1)*cols);     // 6
		}
	}
	
	//setup arduino communication
	serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	serial.setup("COM4", 9600);

	
//	loadFrames(125);
	count = 0;
	//ofSetFrameRate(5);
	
}

float ofApp::getZNoiseValue(int _x, int _y, float n1_freq, float n2_freq, float n1_amp, float n2_amp) {

	int x = _x;
	int y = _y + ofGetFrameNum();
	float n1 = ofNoise(x * n1_freq, y * n1_freq) * n1_amp;
	float n2 = ofNoise(x * n2_freq, y * n2_freq) * n2_amp;
	float ret = n1 + n2;

	return ret;
}
//--------------------------------------------------------------
void ofApp::update() {
	//KINECT
	//Each frame check for new Kinect OSC messages
	kinect.update();


	light.setPosition(ofVec3f(0, sin(ofGetElapsedTimef()) * 100, 0));
	for (int i = 0; i < mesh.getNumVertices(); i++) {
		ofVec3f vec;
		vec = mesh.getVertex(i);
		vec.z = getZNoiseValue(vec.x, vec.y, 0.02, 0.02, 0.5, 10);
		mesh.setVertex(i, vec);
	}


	while (serial.available()) {
		char byteReturned;
		byteReturned = serial.readByte();
		if (byteReturned != '.') {
			recieved += byteReturned;
		}
		else {
			currentVal = ofToInt(recieved);
			currentValSmooth += (currentVal - currentValSmooth)*0.05;
			recieved = "";
			std::cout << currentVal << std::endl;
		}
	}

	//seabed.update();

}


//--------------------------------------------------------------
void ofApp::draw(){
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);

	ofFill();
	ofSetColor(0, 0, 225, 230);
	ofPushMatrix();
	
	ofTranslate(300, -(currentValSmooth*0.3 + ofGetMouseY()-1000), currentValSmooth);
	ofRotateY(30);
	cone.draw();
	ofPopMatrix();
	//ofDrawRectangle(0, 0, ofGetWidth(),ofGetHeight()/2);

	//ofSetColor(255);

	
	//bubble.draw(sin(ofGetFrameNum()*0.04) * ofRandom(0, 3.5), sin(ofGetFrameNum()*0.04) * 300, 200, 200);
/*
	if (count == frames.size()) {
		count = 0;
	}
	frames[count].draw(0, 0, ofGetWidth(), ofGetHeight());
	if (ofGetFrameNum() % 2 == 0){
		count++;
	}

	*/
	
	//seabed.draw(0, 0, ofGetWidth(), ofGetHeight());
	//ofBackgroundGradient(ofColor(40, 40, 40), ofColor(0, 0, 0), OF_GRADIENT_CIRCULAR);
	//KINECT

	kinect.drawDebug();

	//We passed the skeleton list pointer to the renderer earlier,
	//now we tell it to draw them
	renderer.draw();

	//If you want to stop using the default renderer and start
	//drawing your own graphics, uncomment this for a starting point:
	for (int i = 0; i < skeletons->size(); i++) {
		ofSetColor(ofColor::fromHsb(ofGetFrameNum() % 255, 255, 255));
		Joint handLeft = skeletons->at(i).getHandLeft();
		ofCircle(handLeft.x(), handLeft.y(), 60);
		Joint handRight = skeletons->at(i).getHandRight();
		ofCircle(handRight.x(), handRight.y(), 60);
		std::cout << i << " : " << handRight.z() << std::endl;
		if (handRight.z() > 12) {
			ofSetColor(0);
			ofCircle(handRight.x(), handRight.y(), 100);
		}
	}

	//Print out commands and text
	string commands = "COMMANDS\n\n";
	commands.append("d = debug\n");
	commands.append("j = joints\n");
	commands.append("b = bones\n");
	commands.append("h = hands\n");
	commands.append("r = ranges\n");

	ofSetColor(ofColor::white);
	smallFont.drawString(commands, 20, 40);
	largeFont.drawString("fps:\n" + ofToString(ofGetFrameRate()), 20, ofGetHeight() - 100);


	//KINECTEND


	

	ofPushMatrix();
	ofTranslate(ofGetWidth() - terrainW + scl, ofGetHeight()*0.5 - terrainH * 0.5 + scl * 0.5);
	ofPopMatrix();
	myCamera.begin();
	ofEnableLighting();
	light.enable();

	ofSetColor(255);
	ofRotateX(currentValSmooth+ofGetMouseY());
	ofTranslate(-300, 0, 0);
	mesh.draw();

	light.disable();
	ofDisableLighting();

	if (showIndices) {
		for (int j = 0; j < rows; j++) {
			for (int i = 0; i < cols; i++) {
				ofDrawBitmapStringHighlight(ofToString(i + j * cols), ofPoint(mesh.getVertex(i + j * cols).x - 10, mesh.getVertex(i + j * cols).y + 5));
			}
		}
	}

	

	myCamera.end();



	//ofDrawRectangle(-1500,- currentValSmooth, ofGetWidth()+800, ofGetHeight() + 100);
	//ofPopMatrix();
	//draw a string which presents the value sent from arduino

	ofDrawBitmapString(msg, 50, 20);


}
/*
void ofApp::loadFrames(int n) {
	for (int i = 0; i < n; i++) {
		//strings are objects that represent sequences of characters;
		std::ostringstream localOSS;
		localOSS << "seabed" << setw(3) << setfill('0') << i << ".png";
		string path = "seabedAsStills/" + localOSS.str();
		std::cout << path << std::endl;
		ofImage temp;
		//loading a path of all images that end with .gif
		temp.load(path);
		//pushing the images back to the vector
		frames.push_back(temp);
	}
	
}*/

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	//KINECT

	if (key == 'd') kinect.toggleDebug();
	if (key == 'j') renderer.toggleJoints();
	if (key == 'b') renderer.toggleBones();
	if (key == 'h') renderer.toggleHands();
	if (key == 'r') renderer.toggleRanges();
	//KINECTEND
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == 'i') {
		showIndices = !showIndices;
	}
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
