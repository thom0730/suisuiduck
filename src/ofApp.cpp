#include "ofApp.h"
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetCircleResolution(64);
    
    
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
    minDistance = 10;
    maxDistance = 500;
    gui.add(minDistance.setup("minDistance", minDistance, 0, 500));
    gui.add(maxDistance.setup("maxDistance", maxDistance, 0, 500));
    gui.add(x.setup("x", -10, -180, 180));
    gui.add(y.setup("y", 180, -180, 180));
    gui.add(z.setup("z", 180, -180, 180));
    gui.add(scale.setup("scale", 5, 0, 10));
    
    vbomesh.setUsage(GL_DYNAMIC_DRAW);
    vbomesh.setMode(OF_PRIMITIVE_LINES);
    
    //Kinect
    kinect.open(true, true, 0);
    kinect.start();
    


}

//--------------------------------------------------------------
void ofApp::update(){
    rollCam.update();   //rollCam's rotate update.
    kinect.update();
    
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
            
            switch(sceneFLG){
                    
                case 1:
                    RollingCam();;
                    break;
                    
                case 2:
                    RollingCam();
                    break;
                case 3:
                    RollingCam();
                    break;
                case 4:
                    AroundCam();
                    break;
                case 5:
                    AroundCam();
                    break;
                case 6:
                    RollingCam();
                    break;
            }
        }
        
        index++;
    }
    
    
    //Kinect UPDATE
    if(sceneFLG == 4){
        FrameNewKinect();
    }
    if(sceneFLG == 5){
        drawCamMeshUpdate();
    }
    
    myFbo.begin();
    ofClear(0);
    switch(sceneFLG){
            
        case 1:
            drawRolling();
            break;
            
        case 2:
            //first let's just draw the model with the model object
            //drawWithModel();
            //rollCam.begin(); //rollCam begin
            //then we'll learn how to draw it manually so that we have more control over the data
            drawWithMesh(3);
            //rollCam.end();  //rollCam end
            break;
        case 3:
            rollCam.begin();
            drawNoiseLine();
            rollCam.end();
            break;
        case 4:
            drawKinect();
            break;
            
        case 5:
            drawCamMesh();
            break;
        case 6:
            drawWithMesh(20);;
            break;
    }
    myFbo.end();

}

//--------------------------------------------------------------
void ofApp::draw(){
    /* Apply effects */
    myGlitch.generateFx();
    
    /* draw effected view */
    ofSetColor(255);
    ofFill();
    myFbo.draw(0, 0);

    gui.draw();
    ofDrawBitmapStringHighlight("fps: " + ofToString(ofGetFrameRate()), ofGetWidth() - 120, 20);
}
//--------------------------------------------------------------
void ofApp::RollingCam(){
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
//--------------------------------------------------------------
void ofApp::AroundCam(){
    rollCam.setPos(x, y, z);
    rollCam.setScale(scale);
    
}
//--------------------------------------------------------------
void ofApp::drawRolling(){
    vbomesh.clear();
    vbomesh.setMode(OF_PRIMITIVE_LINES);
    
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
    ofSetLineWidth(2);
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
    ofSetLineWidth(2);
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
    //gui.draw();
    
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
void ofApp::drawWithMesh(int _i){
    
    vbomesh.clear();
    
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
    
    if(_i<15){
        if(getFFT<threshold_2){
            amplitude = getFFT/10;
        }else{
            amplitude = getFFT*_i;
        }
    }else{
        amplitude = getFFT*_i;
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
    //gui.draw();
}
//--------------------------------------------------------------
void ofApp::drawNoiseLine(){
    vbomesh.clear();
    for(int i=0;i<NUM;i++){
        float fft = getFFT*5;
        if(i%3 == 0){
            fft = getFFT * 0.2;
        }else if(i%4 == 0){
            fft = getFFT * 20;
        }
        vbomesh.addVertex(vec[i]*fft);
    }
    glLineWidth(1);
    glPointSize(5);
    ofSetColor(150,150, 255);
    vbomesh.draw();
    
}
//--------------------------------------------------------------
void ofApp::FrameNewKinect(){
    if (kinect.isFrameNew()) {
        
        vbomesh.clear();
        
        int st = 2;
        int w = kinect.getDepthPixelsRef().getWidth();
        int h = kinect.getDepthPixelsRef().getHeight();
        int minD = minDistance;
        int maxD = maxDistance;
        
        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                float d = kinect.getDistanceAt(x, y);
                if (d > minD && d < maxD) { // adjust depth range as you like
                    ofVec3f p = kinect.getWorldCoordinateAt(x, y, d);
                    vbomesh.addColor(ofColor(255));
                    vbomesh.addVertex(p);
                    vbomesh.addColor(ofFloatColor(0.4, 0.8, 1.0));
                }
            }
        }
    }
    
}
//--------------------------------------------------------------
void ofApp::drawKinect(){
//    x=-10;
//    y=180;
//    z=180;
//    scale=5;
    
    ofClear(0);
    ofSetColor(ofFloatColor(0.4, 0.8, 1.0));
    if (vbomesh.getVertices().size()) {
        ofPushStyle();
        glPointSize(1);
        rollCam.begin(); //rollCam begin
        ofPushMatrix();
        ofTranslate(0, 0, -100);
        vbomesh.draw();
        ofPopMatrix();
        rollCam.end(); //rollCam begin
        ofPopStyle();
    }
    //ofDrawBitmapStringHighlight("fps: " + ofToString(ofGetFrameRate()), ofGetWidth() - 120, 20);
    gui.draw();
    
}
//--------------------------------------------------------------
void ofApp::drawCamMeshSetUp(){
//    x=70;
//    y=0;
//    z=0;
//    scale=0.5;

    vbomesh.clear();

    
    int width = kinect.getColorPixelsRef().getWidth();
    int height = kinect.getColorPixelsRef().getHeight();

    //add one vertex to the mesh for each pixel
    for (int y = 0; y < height; y+=2){
        for (int x = 0; x<width; x+=2){
             vbomesh.addVertex(ofPoint(x,y,0));    // mesh index = x + y*width
            // this replicates the pixel array within the camera bitmap...
            vbomesh.addColor(ofFloatColor(0,0,0));  // placeholder for colour data, we'll get this from the camera
        }
    }


    for (int y = 0; y<height-1; y+=2){
        for (int x=0; x<width-1; x+=2){
            vbomesh.addIndex(x+y*width);                // 0
            vbomesh.addIndex((x+1)+y*width);            // 1
            vbomesh.addIndex(x+(y+1)*width);            // 10
            
            vbomesh.addIndex((x+1)+y*width);            // 1
            vbomesh.addIndex((x+1)+(y+1)*width);        // 11
            vbomesh.addIndex(x+(y+1)*width);            // 10
        }
    }
    //this determines how much we push the meshes out
    extrusionAmount = 300.0;
}
//--------------------------------------------------------------
void ofApp::drawCamMeshUpdate(){
    
    if (kinect.isFrameNew()) {
        
        int width = kinect.getColorPixelsRef().getWidth();
        int height = kinect.getColorPixelsRef().getHeight();

        //this determines how far we extrude the mesh
        for (int i=0; i< width*height-4; i+=2){
            
            ofFloatColor sampleColor(kinect.getColorPixelsRef()[i*3]/255.f,                // r
                                     kinect.getColorPixelsRef()[i*3+1]/255.f,            // g
                                     kinect.getColorPixelsRef()[i*3+2]/255.f);            // b
            
            //now we get the vertex aat this position
            //we extrude the mesh based on it's brightness
            ofVec3f tmpVec = vbomesh.getVertex(i);
            tmpVec.z = sampleColor.getBrightness() * extrusionAmount;
            vbomesh.setVertex(i, tmpVec);
            
            vbomesh.setColor(i, sampleColor);
        }
    }
}
//--------------------------------------------------------------
void ofApp::drawCamMesh(){
     ofPushMatrix();
    //we have to disable depth testing to draw the video frame
    ofDisableDepthTest();
    //    vidGrabber.draw(20,20);
    
    //but we want to enable it to show the mesh
    ofEnableDepthTest();
    rollCam.begin();
    
    //You can either draw the mesh or the wireframe
    ofTranslate(-ofGetWidth()/2, ofGetHeight()/2);
    //vbomesh.drawWireframe();
    vbomesh.drawFaces();
    rollCam.end();
    ofPopMatrix();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==' ') {//Inputting optional rotate.
        rollCam.setPos(x, y, z);
        rollCam.setScale(scale);
    }
    if (key=='z') {//All Random.
        rollCam.setRandomScale(0.5, 1.5);
        rollCam.setRandomPos(270);
    }
    if (key=='x') {//Random rotate.
        rollCam.setRandomPos(270);
    }
    if (key=='v') {//Random distance.
        rollCam.setRandomScale(0.5, 1.5);
    }


    
    
    //=======scene==========
    if (key=='0') {
        myFbo.allocate(ofGetWidth(), ofGetHeight());
        myGlitch.setup(&myFbo);
        
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
    if (key=='3') {
        for(int i=0;i<NUM;i++){
            int x = ofRandom(-ofGetHeight()/5,ofGetHeight()/5);
            int y = ofRandom(-ofGetHeight()/5,ofGetHeight()/5);
            int z = ofRandom(-ofGetHeight()/5,ofGetHeight()/5);
            vec[i] = ofVec3f(x,y,z);
            
            vbomesh.addVertex(ofVec3f(x,y,z));
        }
        sceneFLG = 3;
    }
    if (key=='4') {
        vbomesh.setMode(OF_PRIMITIVE_POINTS);
        sceneFLG = 4;
    }
    if (key=='5') {
        drawCamMeshSetUp();
        sceneFLG = 5;
    }
    if (key=='6') {
        sceneFLG = 6;
        //now we load our model
        model.loadModel("suisui.stl");
        model.setPosition(ofGetWidth()/2, ofGetHeight()/2, -1000);
        //we need to call this for textures to work on models
        ofDisableArbTex();
        //this makes sure that the back of the model doesn't show through the front
        ofEnableDepthTest();
        light.enable();
        light.setPosition(model.getPosition() + ofPoint(0, 0, 1600));
    }
    
    
    //=======glitch==========
    
    if (key == 'q') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE    , true);
    if (key == 'w') myGlitch.setFx(OFXPOSTGLITCH_GLOW            , true);
    if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_SHAKER            , true);
    if (key == 'r') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER        , true);
    if (key == 't') myGlitch.setFx(OFXPOSTGLITCH_TWIST            , true);
    if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE        , true);
    if (key == 'u') myGlitch.setFx(OFXPOSTGLITCH_NOISE            , true);
    if (key == 'i') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN        , true);
    if (key == 'o') myGlitch.setFx(OFXPOSTGLITCH_SWELL            , true);
    if (key == 'p') myGlitch.setFx(OFXPOSTGLITCH_INVERT            , true);
    
    if (key == 'a') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
    if (key == 's') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE    , true);
    if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE    , true);
    if (key == 'f') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE    , true);
    if (key == 'g') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT    , true);
    if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT    , true);
    if (key == 'j') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT    , true);
    
    
    //=======mesh==========
    if (key=='m') {
        vbomesh.setMode(OF_PRIMITIVE_POINTS);
    }
    if (key=='n') {
        vbomesh.setMode(OF_PRIMITIVE_LINES);
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 'q') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE    , false);
    if (key == 'w') myGlitch.setFx(OFXPOSTGLITCH_GLOW            , false);
    if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_SHAKER            , false);
    if (key == 'r') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER        , false);
    if (key == 't') myGlitch.setFx(OFXPOSTGLITCH_TWIST            , false);
    if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE        , false);
    if (key == 'u') myGlitch.setFx(OFXPOSTGLITCH_NOISE            , false);
    if (key == 'i') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN        , false);
    if (key == 'o') myGlitch.setFx(OFXPOSTGLITCH_SWELL            , false);
    if (key == 'p') myGlitch.setFx(OFXPOSTGLITCH_INVERT            , false);
    
    if (key == 'a') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
    if (key == 's') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE    , false);
    if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE    , false);
    if (key == 'f') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE    , false);
    if (key == 'g') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT    , false);
    if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT    , false);
    if (key == 'j') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT    , false);

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
