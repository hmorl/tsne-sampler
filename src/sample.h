//
//  sample.h
//  audioTSNE
//

#include "ofMain.h"
#include "ofxMaxim.h"

#ifndef sample_h
#define sample_h

class Sample {
private:
    maxiSample snd;
    glm::vec2 point;
    float radius, sSq, a;
    bool bMouseOn;
    double speed;
    int w, h;
    double volume;
    string filename;
    float rand;
    
public:
    Sample(string path, float x, float y, float _radius);
    void setup(string path, float x, float y, float _radius);
    void update();
    void draw();
    void trigger();
    void mouseMoved(int x, int y);
    void setSpeed(double _speed);
    void setVolume(double _volume);
    void stop();
    bool isPlaying();
    double play();
    glm::vec2 getPos();
    float getRad();

};


#endif /* sample_h */
