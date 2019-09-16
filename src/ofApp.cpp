#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSeedRandom(42);
    
    //set up gui
    setupGui();

    //tsne-data json file path
    string file = ofToDataPath("tsne-data.json");

    //parse json
    bool parsingSuccessful = result.open(file);
    if(!parsingSuccessful) throw;
    
    int numPoints = result.size();
    cout<< numPoints <<" sounds in total"  <<endl;
    
    //set up vector of pointers to sample objects
    //each containing file name and xy pos
    for (int i = 0; i < numPoints; i++) {
        string path = ofToDataPath(result[i]["path"].asString());
        float x = ofMap(result[i]["point"][0].asFloat(), 0, 1, 24, ofGetWidth()-guiWidth-24);
        float y = ofMap(result[i]["point"][1].asFloat(), 0, 1, 24, ofGetHeight()-24);
        //push back pointer to new sample
        sounds.push_back(shared_ptr<Sample>(new Sample(path, x, y, 8)));
    }
    
    //set up optional link (off by default)
    //set bpm
    link.setBPM(115);
    bpm = link.getBPM();
    bps =((double)bpm / 60) /  link.getQuantum();
    link.disableLink();
    
    //audio settings for Maximilian and ofSoundStream
    sampleRate = 44100;
    //INCREASE BUFFER SIZE IF STRUGGLING
    //(seems to affect framerate too)
    bufferSize = 512;
    maxiSettings::setup(sampleRate, 2, bufferSize);
    auto devices = soundStream.getMatchingDevices("default");
    //comment out the default device ^
    soundStream.printDeviceList();
    //prints the devices in the console window -
   // auto devices = soundStream.getDeviceList();
    //gets all the devices available in an array
    //soundSettings.setOutDevice(devices[1]); //sets the output device
    soundSettings.setOutListener(this);
    soundSettings.sampleRate = sampleRate;
    soundSettings.numOutputChannels = 2;
    soundSettings.numInputChannels = 0;
    soundSettings.bufferSize = bufferSize;
    soundStream.setup(soundSettings);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    //fps in window title
    //ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    gui->update();
    
    //update topographic information for
    //sounds and lenses
    //(positions, what sounds are in view, etc)
    for (int i = 0; i < sounds.size(); i++) {
        sounds[i]->update();
    }
    
    for (int i = 0; i < NUM_LENSES; i++) {
        lenses[i].update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0,0,21);

    //draw samples and lenses
    for (int i=0; i<sounds.size(); i++) {
        sounds[i]->draw();
    }

    for (int i=0; i<NUM_LENSES; i++) {
        lenses[i].draw();
    }
    
    //draw rotating playhead line
    ofPushStyle();
        ofPushMatrix();
            ofTranslate(70, 110);
            ofRotateDeg(phase * 360 + 180);
            ofSetLineWidth(2);
            ofSetColor(255, 255, 0, 100);
            ofDrawLine(0, 0, 0, 40);
        ofPopMatrix();
    
        //draw bpm and Ableton Link information text
        stringstream msg;
        msg << "bpm: " << ofToString(bpm) << endl;
        if(bLinkEnabled){
            msg << "L&R arrows changes bpm." << endl<< "link enabled"<<endl<<"press l to disable"<< endl;
        }else{
            msg << "L&R arrows changes bpm." << endl<< "link disabled"<<endl<<"press l to enable"<< endl;
        }
        ofDrawBitmapString(msg.str(), 20, 20);
    ofPopStyle();
    
    //draw gui
    gui->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'f'){
        ofToggleFullscreen();
    } else if(key == OF_KEY_LEFT) {
        if(20 < link.getBPM()){
            link.setBPM(link.getBPM() - 1.0);
            bpm = link.getBPM();
            bps = ((double)bpm / 60) /  link.getQuantum();
        }
    } else if(key == OF_KEY_RIGHT) {
        link.setBPM(link.getBPM() + 1.0);
        bpm = link.getBPM();
        bps = ((double)bpm / 60) /  link.getQuantum();
    } else if(key == 'b') {
        link.setBeat(0.0);
    } else if(key =='l'){
        bLinkEnabled = !bLinkEnabled;
        if(bLinkEnabled){
            link.enableLink();
        }else{
            link.disableLink();
        }
    }
}


//Mouse logic--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    //for hover-over feedback (sound names, hightlight hovered lens)
    for(int i = 0; i < sounds.size(); i++){
        sounds[i]->mouseMoved(x, y);
    }
    
    for(int i = 0; i < NUM_LENSES; i++){
        lenses[i].mouseMoved(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if((button == 0) && (x < ofGetWidth() - guiWidth)){
        if(bSizingLens){
            if(lenses[NUM_LENSES-1].resizing){
                lenses[NUM_LENSES-1].setRadiusFromXY(x, y);
            }
        }else{
            if(bMovingLens){
                lenses[selectedLens].move(x, y);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    if((button == 0) && (x < ofGetWidth() - guiWidth)) {
        if(!bSizingLens){
            for(int i = 0; i < NUM_LENSES; i++){
                if(!bMovingLens && ofDist(x, y, lenses[i].getPos().x, lenses[i].getPos().y) < 20){
                    bMovingLens = true;
                    selectedLens = i;
                }
            }
        }

        if(!bMovingLens){
            if(NUM_LENSES < MAX_LENSES){
                bSizingLens = true;
                int name = NUM_LENSES+1;
                lenses[NUM_LENSES].setup(x, y, ofToString(name));
                lenses[NUM_LENSES].setMap(sounds);
                NUM_LENSES++;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if(button == 0 && bSizingLens) {
        bSizingLens = false;
        lenses[NUM_LENSES-1].resizing = false;
        string currFolderName = ofToString(NUM_LENSES);
        //set visible the controls for the newly created lens
        gui->getFolder(currFolderName)->setVisible(true);
        //initialise parameters with regular values
        gui->getSlider(currFolderName+"freq")->setValue(1);
        gui->getSlider(currFolderName+"pitch")->setValue(1);
    }else if(button == 0){
        for(int i = 0; i < NUM_LENSES; i++){
            bMovingLens= false;
        }
        
    }
}
//End of mouse logic--------------------------------------------------------------

void ofApp::exit()
{
    //program was crashing on exit, this has fixed it
    delete gui;
}

//GUI stuff-----------------------------------------------------------------------
void ofApp::setupGui() {
    guiWidth = 270;
    gui = new ofxDatGui(ofGetWidth()-guiWidth, 0);
    gui->setTheme(new ofxDatGuiThemeCharcoal());
    gui->addHeader(":: controls ::");
    gui->getHeader()->setDraggable(false);
    
    for(int i = 0; i < MAX_LENSES; i++){
        ofxDatGuiFolder *folder;
        
        //name lenses starting at 1
        string name = ofToString(i+1);
        
        //create folder
        folder = gui->addFolder(name, ofColor::fromHex(0xFFD00B));
        
        //add sliders
        folder->addSlider("Radius", 20, 500)->setName(name + "rad");
        folder->addSlider("Freq", 0.5, 128, 1)->setName(name + "freq");
        gui->getSlider(name+"freq")->setPrecision(0);
        folder->addSlider("Pitch", 0.01, 8, 1)->setName(name + "pitch");
        folder->addSlider("Volume", 0, 1, 0.5)->setName(name + "volume");
        folder->addSlider("Phase offset", 0, 1, 0)->setName(name + "offset");
        folder->addSlider("Inner radius", 0, 1, 0)->setName(name + "inner");
        folder->addSlider("Wandering radius", 0, 300, 0)->setName(name + "wander");
        folder->addSlider("Wandering speed", 0, 1, 0.01)->setName(name + "speed");
        folder->addToggle("Ordered selection?")->setName(name + "ordered");
        folder->addToggle("Mute lens")->setName(name + "mute");

        //hide until lens has been created
        folder->setVisible(false);
    }
    
    //event listeners
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onToggleEvent(this, &ofApp::onToggleEvent);
    gui->setAutoDraw(false);
}

//--------------------------------------------------------------
void ofApp::onSliderEvent(ofxDatGuiSliderEvent e){
    //get which lens (which folder group has the slider come from)
    //first character of the component's name
    int id = (int)e.target->getName()[0] - 49;
    //get first letter of paramater name
    char param = e.target->getName()[1];

    switch(param){
        case 'r':
            lenses[id].setOuterRadius(e.value);
            break;
        case 'f':
            lenses[id].setFrequency(e.value);
            break;
        case 'p':
            lenses[id].setSpeed(e.value);
            break;
        case 'v':
            lenses[id].setVolume(e.value);
            break;
        case 'o':
            lenses[id].setPhaseOffset(e.value);
            break;
        case 'i':
            lenses[id].setInnerRadius(e.value);
            break;
        case 'w':
            lenses[id].setWanderRadius(e.value);
            break;
        case 's':
            lenses[id].setWanderSpeed(e.value * 0.1);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::onToggleEvent(ofxDatGuiToggleEvent e){
    int id = (int)e.target->getName()[0] - 49;
    char param = e.target->getName()[1];
    
    switch(param){
        case 'o':
            lenses[id].setRandom(!e.checked);
            break;
        case 'm':
            lenses[id].isActive=!e.checked;
            break;
    }
}
//end of GUI stuff-----------------------------------------------------------------------

//audio callback------------
void ofApp::audioOut(ofSoundBuffer & buffer){
    for(int i = 0; i < buffer.getNumFrames(); i++) {
        mix = 0;
        
        if(bLinkEnabled){
            //get phase from link
            phase = link.getPhase() / link.getQuantum();
        } else {
            //generate own phase clock from phasor
            phase = clockOsc.phasor(bps);
        }
        
        for (int i=0; i<NUM_LENSES; i++) {
            //update lens clocks, return a sample to trigger if necessary
            int samp = lenses[i].updateAudio(phase);
            
            //if there is a sound to trigger, trigger it and add it to the list of
            //currently playing sounds for audio updating purposes
            if(samp!=-1){
                sounds[samp]->trigger();
                currentSounds.insert(samp);
            }
        }
        
        //originally I was looping through all 500+ sounds and updating the audio,
        //but this caused audio buffer issues so I devised a way that only updates
        //the sounds that need updating
        
        //loop through currently playing sounds and update the audio for them
        for(set<int>::iterator it = currentSounds.begin(); it != currentSounds.end();){
            if(sounds[*it]->isPlaying()){
                mix += sounds[*it]->play();
                it++;
            }else{
                //if sound has finished, erase it from list of current sounds
                it = currentSounds.erase(it);
            }
        }
    
        buffer[i*buffer.getNumChannels()] = mix;  // left channel
        buffer[i*buffer.getNumChannels()+1] = mix; // right channel
    }

}
