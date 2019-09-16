//
//  lens.h
//  audioTSNE
//

#include "ofMain.h"

#include "sample.h"

#ifndef lens_h
#define lens_h


class Lens {
private:
    struct sampleDistance {
        int idx;
        float distance;
    };
    
    glm::vec2 pos;
    glm::vec2 origin;
    
    float outerRad;
    float innerRad;
    
    float wanderRad;
    float wanderSpeed;
    
    double volume;
    double speed;
    
    bool chooseRandom;
    bool bMouseOn;
    double currPhase;
    double prevPhase;
    double phaseOffset;
    
    float freq;
    int timerInt;

    int current;
    float nOffset;
    
    int count;
    
    vector<shared_ptr<Sample>>* samples;
    
    string name;
    
public:
    
    void setup(int initX, int initY, string _name);
    void update();
    void draw();
    void setRadiusFromXY(int x, int y);
    void setPhaseOffset(double offset);
    void setFrequency(double freqHz);
    void setSpeed(double _speed);
    void setVolume(double _volume);
    void setRandom(bool _isRandom);
    void setOuterRadius(float _radius);
    void setInnerRadius(float _radius);
    
    void setWanderRadius(float _radius);
    void setWanderSpeed(float _speed);


    void move(int x, int y);
    void mouseReleased();
    void mouseMoved(int x, int y);

    float getRadius();
    void setMap(vector<shared_ptr<Sample>> &_samples);
    int updateAudio(double phase);
    int trigger();
    bool resizing;

    glm::vec2 getPos();

    vector<sampleDistance> inView;
    
//    map<int, float> inView;
    bool isActive;

};


#endif /* lens_h */


