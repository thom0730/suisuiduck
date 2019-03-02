#pragma once

#include "ofMain.h"
#include "ofxRollingCam.h"
#include "ofxOsc.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include "ofxPostGlitch.h"
#include "ofxMultiKinectV2.h"
#include "ofxSyphon.h"

#define HOST "127.0.0.1"
#define PORT 7401
#define NUM 800 //Noiseの頂点数

//FOR Kinect
#define STRINGIFY(x) #x
#define MAX 100
#define DIFF 40
#define POINT 50000

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    ofxRollingCam rollCam;
    ofLight lig;
    vector <ofVec3f> pos;
    bool hide;
    
    //GUI
    ofxPanel gui;
    ofxFloatSlider threshold_1;
    ofxFloatSlider threshold_2;
    
    int sceneFLG = 1;
    
    //カメラの動き方
    void RollingCam();
    void AroundCam();
    
    //DRAWING FUNCTION
    //1
    void drawRolling();
    //2
    void drawWithModel();
    void drawWithMesh(int _i);
    //3
    void drawNoiseLine();
    //4
    void FrameNewKinect();
    void drawKinect();
    
    //OSC
    ofxOscReceiver receiver;
    float getFFT = 0;
    
    //3D model
    //this is our model we'll draw
    ofxAssimpModelLoader model;
    
    ofLight light;
    
    //glitch
    ofFbo myFbo;
    ofxPostGlitch myGlitch;
    
    //drawNoiseLine()
    ofVboMesh vbomesh;
    ofVec3f vec[NUM];

    //drawCamMesh()
    ofCamera cam; // add mouse controls for camera movement
    float extrusionAmount;
    ofVboMesh mainMesh;
    ofVideoGrabber vidGrabber;
    //Kinect
    ofxMultiKinectV2 kinect;
    ofxFloatSlider minDistance;
    ofxFloatSlider maxDistance;
    ofxFloatSlider x;
    ofxFloatSlider y;
    ofxFloatSlider z;
    ofxFloatSlider scale;
    
    
};
