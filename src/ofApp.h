#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxDatGui.h"
#include "ofxAbletonLink.h"
#include "ofxMaxim.h"

#include "lens.h"
#include "sample.h"


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void audioOut(ofSoundBuffer & buffer);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void setupGui();
    void exit();
    void keyPressed(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    const static int MAX_LENSES = 9;
    int NUM_LENSES;
    
    //gui stuff
    ofxDatGui* gui;
    int selectedLens;
    int guiWidth;
    
    //toggles
    bool bLinkEnabled;
    bool bSizingLens;
    bool bMovingLens;
    
    //json object for parsing the filenames and xy positions
    //of the sounds from the json file
    ofxJSONElement result;
    
    //smart pointer of sample objects
    vector<shared_ptr<Sample>> sounds;
    
    //set of currently playing sounds (set so unique values) -
    //can't play sound more than once at a time
    set<int> currentSounds;
    
    //the lens object as a static array
    //I had issues using pointers, this just worked
    Lens lenses[MAX_LENSES];
    
    //sound settings
    ofxAbletonLink link;
    ofSoundStreamSettings soundSettings;
    ofSoundStream soundStream;
    double mix;
    double phase;
    double bps;
    int bpm;
    maxiOsc clockOsc;
    int sampleRate = 44100;
    int bufferSize = 512;
};
