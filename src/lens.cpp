//
//  lens.cpp
//  audioTSNE
//

#include "lens.h"

void Lens::setup(int initX, int initY, string _name){
    origin = glm::vec2(initX, initY);
    pos = origin;
    isActive = true;
    nOffset = ofRandom(10000);
    freq = 1.;
    phaseOffset = 0;
    innerRad = 0;
    outerRad = 20;
    resizing = true;
    name = _name;
    chooseRandom = true;
    wanderSpeed = 0.01;
    speed = 1;
    volume = 0.5;
}

void Lens::update(){
    if(isActive && !resizing){
        float nR = (ofGetFrameNum()+nOffset) * wanderSpeed;
        float nTh = (ofGetFrameNum()+nOffset) * wanderSpeed + nOffset;
        float x = ofSignedNoise(nR);
        float y = ofSignedNoise(nTh);
        pos = origin + glm::vec2(x, y) * wanderRad;
        
        inView.clear();
                
        for(int i = 0; i < samples->size(); i++){
            float dist = glm::fastDistance(samples->at(i)->getPos(), pos) + samples->at(i)->getRad()/2;
            //if sound lies outside inner circle and inside outer circle,
            //then sound is in view
            if(dist < outerRad && dist > innerRad){
                inView.push_back({i, dist});
            }
        }
        
        sort(inView.begin(), inView.end(), []( const sampleDistance& lhs, const sampleDistance& rhs )
        {
            return lhs.distance < rhs.distance;
        });

    }

}

void Lens::draw(){
    if(isActive){
        ofSetCircleResolution(50);
        ofFill();
        
        if(bMouseOn){
            ofSetColor(255,50);

        }else{
            ofSetColor(255,25);

        }
        
        ofDrawCircle(pos, innerRad);
        ofDrawCircle(pos, outerRad);
        ofSetColor(255, 255,0);
        ofDrawBitmapString(name, pos);
        
    }
}

void Lens::setRadiusFromXY(int x, int y){
    outerRad = glm::distance(glm::vec2(x,y), pos);
}



void Lens::setRandom(bool _isRandom){
    chooseRandom = _isRandom;
}

void Lens::setMap(vector<shared_ptr<Sample>> &_samples){
    samples = &_samples;
}

int Lens::updateAudio(double phase) {
    int sampID = -1;
    
    if(isActive){
        currPhase = fmod(phase+(phaseOffset/freq), 1./freq);
        
        if(currPhase < prevPhase){
            sampID = trigger();
        }
        
        prevPhase = currPhase;
    }

    
    return sampID;
    
}

int Lens::trigger(){
    int id = -1;
    if(inView.size()>0){
        if(chooseRandom){
            //choose a random sample index from the list of in view samples
            int rand = ofRandom(inView.size());
            id = inView[rand].idx;
        } else{
            //cycle through the sorted (by distance from center) list of in view samples
            count++;
            if(count > inView.size()-1){
                count = 0;
            }
            id = inView[count].idx;
        }
        
        samples->at(id)->setVolume(volume);
        samples->at(id)->setSpeed(speed);
    }
    
    return id;
}

void Lens::setFrequency(double freqHz) {
    freq = freqHz;
    prevPhase = 0;
}

void Lens::setPhaseOffset(double offset) {
    phaseOffset = offset;
    prevPhase = 0;
}

void Lens::setSpeed(double _speed) {
    speed = _speed;

}

void Lens::setVolume(double _volume) {
    
    volume = _volume;

}

void Lens::move(int x, int y) {
    origin.x = x;
    origin.y = y;
}

void Lens::mouseMoved(int x, int y) {
    float distanceToMouse = ofDist(x, y, pos.x, pos.y);
    if (distanceToMouse < outerRad) {
        bMouseOn = true;
    } else {
        bMouseOn = false;
    }
}



glm::vec2 Lens::getPos(){
    return pos;
}

void Lens::setWanderSpeed(float _speed){
    wanderSpeed = _speed;
}

void Lens::setWanderRadius(float _radius){
    wanderRad = _radius;
}

void Lens::setOuterRadius(float _radius) {
    outerRad = _radius;
}

//scale 0 to 1 => 0 to outer radius
void Lens::setInnerRadius(float _radius) {
    innerRad = ofMap(_radius, 0, 1, 0, outerRad-24);
}

float Lens::getRadius() {
    return outerRad;
}
