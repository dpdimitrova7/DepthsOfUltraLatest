#pragma once

#include "ofMain.h"
#include "ofxKinectV2OSC.h"

class ofApp : public ofBaseApp{

	public:
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

		float getZNoiseValue(int _x, int _y, float n1_freq, float n2_freq, float n1_amp, float n2_amp);


		float xoff, yoff;
		int cols, rows;
		int terrainW, terrainH;
		int scl;

		string recieved;
		int currentVal;
		int currentValSmooth;
		float floating;

		ofMesh mesh;

		bool showIndices;

		float terrainZPos;
		float terrainZPosOff;

		//light
		ofLight light;

		bool over = false;
		ofCamera myCamera;

		int byteData;
		ofSerial serial;
		string msg;
		//float msg2;


		//KINECT VARS
		ofxKinectV2OSC kinect;
		Skeleton* skeleton;
		vector<Skeleton>* skeletons;
		ofTrueTypeFont smallFont, largeFont;

		BodyRenderer renderer;
		
		ofVideoPlayer seabed;

		//void loadFrames(int n);
		//vector, type of vector, name of the vector
		vector <ofImage> frames;
		int shrink;
		int count;
		ofImage bubble;
		ofConePrimitive cone;
	
};
