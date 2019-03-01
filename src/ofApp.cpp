#include "ofApp.h"
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    rollCam.setup();//rollCam's setup.
    rollCam.setCamSpeed(0.1);//rollCam's speed set;
    ofBackground(0);
    lig.setup();
    
    float r=ofGetHeight()*0.8;
    for (int i=0; i<300; i++) {
        ofVec3f newPos;
        newPos.set(ofRandomf()*r,ofRandomf()*r,ofRandomf()*r);
        pos.push_back(newPos);
    }
    
    receiver.setup(PORT);
    
    //GUI
    gui.setup();
    gui.add(threshold_1.setup("threshold_1", 1.6, 0, 2));
    gui.add(threshold_2.setup("threshold_2", 1.3, 0, 2));
    
    //3Dmodel
//    //we need to call this for textures to work on models
//    ofDisableArbTex();
//
//    //this makes sure that the back of the model doesn't show through the front
//    ofEnableDepthTest();
    
//    //now we load our model
//    model.loadModel("suisui.stl");
//    model.setPosition(ofGetWidth()/2, ofGetHeight()/2, -100);
//
//    light.enable();
//    light.setPosition(model.getPosition() + ofPoint(0, 0, 1600));
}

//--------------------------------------------------------------
void ofApp::update(){
    rollCam.update();   //rollCam's rotate update.
    
    // OSCの個数分繰り返す
    int index = 0;
    while(receiver.hasWaitingMessages()) {
        
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        string value = m.getArgAsString(index);
        // msg += "getAddress: " + m.getAddress() + "\n";
        // msg += "getArgAsString " + ofToString(index) + ": " + value + "\n";
        
        if( index == 0 ){
            getFFT = ofMap( ofToFloat( m.getArgAsString(index) ), -90, 0, 0, 2 );
            if(getFFT > threshold_1){
                int num = ofRandom(3);
                if(num == 0){
                    rollCam.setRandomScale(1.0, 5.5);
                    rollCam.setRandomPos(270);
                }else if(num == 1){
                    rollCam.setRandomPos(270);
                }else if(num == 2){
                    rollCam.setPos(-20, -10, -180);
                    rollCam.setScale(0.3);
                }else{
                    rollCam.setRandomScale(1.0, 2.5);
                }
            }
            
        }
        
        index++;
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    switch(sceneFLG){
            
        case 1:
            rolling();
            break;
        
        case 2:
            //first let's just draw the model with the model object
            //drawWithModel();
            //rollCam.begin(); //rollCam begin
            //then we'll learn how to draw it manually so that we have more control over the data
            drawWithMesh();
            //rollCam.end();  //rollCam end
            break;
    }
    gui.draw();
}
//--------------------------------------------------------------
void ofApp::rolling(){
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetWidth()/2);
    ofEnableLighting();
    lig.setPosition(0, -ofGetHeight()/2, ofGetHeight()/3);
    //lig.draw();
    lig.enable();
    ofPopMatrix();
    
    rollCam.begin(); //rollCam begin
    ofEnableDepthTest();
    ofNoFill();
    ofSetLineWidth(3);
    ofSetColor(150,150, 255);
    ofDrawBox(0, 0, 0, ofGetHeight()/3 *getFFT );
    
    for (int i=0; i<pos.size(); i++) {
        ofNoFill();
        if (i%3==0)ofNoFill();
        ofSetColor(ofColor::fromHsb(150,150, 255));
        ofSetLineWidth(3);
        ofDrawBox(pos[i].x, pos[i].y, pos[i].z, ofGetHeight()/20 *getFFT );
    }
    
    ofNoFill();
    ofSetLineWidth(3);
    ofDrawBox(0, 0, 0, ofGetHeight()/3*1 *getFFT );
    rollCam.end();  //rollCam end
    
    lig.disable();
    ofDisableDepthTest();
    ofSetColor(255,255,255);
    
    ofPopMatrix();
    ofDisableLighting();
//    if(!hide){
//        string info = "";
//        info += "PressKey '1' : set camera all random.\n";
//        info += "PressKey '2' : set random camera rotate.\n";
//        info += "PressKey '3' : set camera position (0,0,0)\n";
//        info += "PressKey '4' : set random camera distance.\n";
//        info += "PressKey '5' : set camera distance by window height.\n";
//        info += "PressKey 'f' : toggle fullscreen.\n";
//        info += "PressKey 'd' : hide explanation.\n";
//        info += "Now Angle    : "+ofToString(rollCam.posN)+"\n";
//        ofDrawBitmapString(info, 10,10);
//    }
    gui.draw();
    
}
//--------------------------------------------------------------
//draw the model the built-in way
void ofApp::drawWithModel(){
    
    //get the position of the model
    ofVec3f position = model.getPosition();
    
    //save the current view
    ofPushMatrix();
    
    //center ourselves there
    ofTranslate(position);
    ofRotate(-ofGetMouseX(), 0, 1, 0);
    ofRotate(0,90,90,0);
    ofTranslate(-position);
    
    //draw the model
    model.drawWireframe();
    
    //restore the view position
    ofPopMatrix();
}
//--------------------------------------------------------------
//draw the model manually
void ofApp::drawWithMesh(){
    
    //get the model attributes we need
    ofVec3f scale = model.getScale();
    ofVec3f position = model.getPosition();
    float normalizedScale = model.getNormalizedScale();
    ofVboMesh mesh = model.getMesh(0);
    ofTexture texture;
    ofxAssimpMeshHelper& meshHelper = model.getMeshHelper( 0 );
    bool bHasTexture = meshHelper.hasTexture();
    if( bHasTexture ) {
        texture = model.getTextureForMesh(0);
    }
    
    ofMaterial material = model.getMaterialForMesh(0);
    
    ofPushMatrix();
    
    //translate and scale based on the positioning.
    ofTranslate(position);
    ofRotate( ofGetElapsedTimeMillis()/20, 0, 1, 0);
    ofRotate(180,0,0,0);
    
    
    
    ofScale(normalizedScale, normalizedScale, normalizedScale);
    ofScale(scale.x,scale.y,scale.z);
    
    //modify mesh with some noise
    float liquidness = 5;
    //float amplitude = mouseY/100.0;
    float amplitude = 0;
    if(getFFT<threshold_2){
        amplitude = getFFT/10;
    }else{
         amplitude = getFFT*3;
    }


    float speedDampen = 5;
    vector<ofVec3f>& verts = mesh.getVertices();
    for(unsigned int i = 0; i < verts.size(); i++){
        verts[i].x += ofSignedNoise(verts[i].x/liquidness, verts[i].y/liquidness,verts[i].z/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
        verts[i].y += ofSignedNoise(verts[i].z/liquidness, verts[i].x/liquidness,verts[i].y/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
        verts[i].z += ofSignedNoise(verts[i].y/liquidness, verts[i].z/liquidness,verts[i].x/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
    }
    
    //draw the model manually
    if(bHasTexture) texture.bind();
    material.begin();
    //mesh.drawWireframe(); //you can draw wireframe too
    mesh.drawFaces();
    material.end();
    if(bHasTexture) texture.unbind();
    
    ofPopMatrix();
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='q') {//All Random.
        rollCam.setRandomScale(0.5, 1.5);
        rollCam.setRandomPos(270);
    }
    if (key=='w') {//Random rotate.
        rollCam.setRandomPos(270);
    }
    if (key=='e') {//Inputting optional rotate.
        rollCam.setPos(0, 0, 0);
    }
    if (key=='r') {//Random distance.
        rollCam.setRandomScale(0.5, 1.5);
    }
    if (key=='t') {//Inputting optional distance.
        rollCam.setScale(1);
    }

    if (key=='y') {
        ofToggleFullscreen();
    }
    if (key=='u') {
        hide= !hide;
    }
    
    if (key=='1') {
        sceneFLG = 1;
        
    }
    if (key=='2') {
        sceneFLG = 2;
        //now we load our model
        model.loadModel("suisui.stl");
        model.setPosition(ofGetWidth()/2, ofGetHeight()/2, -500);
        //we need to call this for textures to work on models
        ofDisableArbTex();
        //this makes sure that the back of the model doesn't show through the front
        ofEnableDepthTest();
        light.enable();
        light.setPosition(model.getPosition() + ofPoint(0, 0, 1600));
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
