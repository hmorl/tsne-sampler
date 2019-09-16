//
//  sample.cpp
//  audioTSNE
//

#include "sample.h"

Sample::Sample(string path, float x, float y, float _radius){
    //load maxiSample
    snd.load(path);
    snd.setPosition(1.);
    snd.mySampleRate = 44100;
    speed = 1.0;
    volume = 1.0;
    filename = path;
    point = glm::vec2(x, y);
    radius = _radius;
    sSq = pow(radius,2);
    rand = ofRandom(20,50);
}

void Sample::update(){
    //decrease alpha value if it has been triggered
    a = ofClamp(a-5, 0, 155);
}
void Sample::stop(){
    snd.setPosition(snd.getLength());
}

bool Sample::isPlaying(){
    //.atEnd() is a custom function I built into Maximilian
    return !snd.atEnd();
}

void Sample::draw(){
    ofSetCircleResolution(12);
    ofFill();
    ofSetColor(255,0,255, a+100);
    ofDrawCircle(point.x, point.y, radius-3);
    if(bMouseOn){
        ofSetColor(255, 255, 0);
        //rand is bad hack so the name for 2 sounds on top of each other
        //can still be seen (baddddd hack)
        ofDrawBitmapString(filename, point.x-120, point.y - rand);
    }

}

double Sample::play() {
    return snd.playOnce(speed) * volume;
}

void Sample::setSpeed(double _speed) {
    speed = _speed;
}

void Sample::setVolume(double _volume) {
    volume = _volume;
}

void Sample::trigger(){
    snd.trigger();
    //set alpha value
    a = 165;
}

void Sample::mouseMoved(int x, int y){
    //so the sample lights up when hovered over
    float distanceToMouse = ofDistSquared(x, y, point.x, point.y);
    if (distanceToMouse < sSq) {
        bMouseOn = true;
    } else {
        bMouseOn = false;
    }
}

glm::vec2 Sample::getPos() {
    return glm::vec2(point.x, point.y);
}

float Sample::getRad() {
    return radius;
}
