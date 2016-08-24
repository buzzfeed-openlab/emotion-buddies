
#include "InternetButton.h"
#include "math.h"

#define touchPin A0
#define vibPin D1
#define avgTotal 200

InternetButton b = InternetButton();

int pubFrequency = 1000;
int lastPub = 0;

int period = 3000;
int vibMin = 50; // minimum to feel vibration
int vibMax = 500; // maximum vibration
int vibBase = 40;
int lightMin = 0;
int lightMax = 255;
int lightBase = 20;
float pi = 3.14;
int intensity=0;
int toAvg[avgTotal];

int touchMax=3500;
int touchMin=2500;
int lastTouchRead=0;
int n=0;
int t;

int randomVar = 5;  // out of 10
int extinguishAt = 20;
int extinguishTime; // calculate as randomVar*500;
int extinguishTimeMax=12000;

void setup() {
  pinMode(touchPin, INPUT);
  pinMode(vibPin, OUTPUT);

  Serial.begin(9600);
  b.begin();
}

void loop() {
  // random factors
  randomVar += 2-random(5);
  if (randomVar<1) {randomVar=1;}
  else if (randomVar>10) {randomVar=10;}

  // check for squeeze
  int touchReadRaw=analogRead(touchPin);

  int touchRead;

  if (touchReadRaw>touchMax) {touchRead=touchMax;}
  else if (touchReadRaw<touchMin) {touchRead=touchMin;}
  else {touchRead=touchReadRaw;}

  if (n>=avgTotal) { n=0; }
  // put it in the array
  toAvg[n]=touchRead;
  n++;

  int touchAvg=toAvg[0];
  for (int x=1; x<avgTotal; x++) {
    touchAvg=(toAvg[x]+touchAvg)/2;
  }

  Serial.println("avg: "+String(touchAvg)+"   now: "+String(touchRead)+"   from: "+String(touchReadRaw));

  if (millis()-lastPub>pubFrequency) {
    char touchValue[64];
    sprintf(touchValue, "%d", touchRead);
    Serial.println("Publishing...");
    Particle.publish("librato_happybot",touchValue,60,PRIVATE);
    lastPub=millis();
  }

  /*Serial.println("avg: "+String(touchAvg)+"   now: "+String(touchRead));*/

  intensity=map(touchAvg,touchMin,touchMax,0,100);

  if (intensity>extinguishAt) {
    extinguishTime+=intensity/10;
    if (extinguishTime>extinguishTimeMax) {extinguishTime=extinguishTimeMax;}
  }

  if (extinguishTime>0) {
    /*Serial.println(extinguishTime);*/
    extinguishTime--;
    analogWrite(vibPin,0);
    b.allLedsOff();
    if (extinguishTime<2000) {
      int rampUp = (2000-extinguishTime)/20;
      randomVar += random(2);
      shudder(rampUp);
      flicker(rampUp);
    }
  }
  else {
    shudder(100);
    flicker(100);
  }

}

void shudder(int i) {
  float vibLevel = (i/100)*vibMax*randomVar/10;
  /*float y = vibLevel-i*vibLevel/50;*/
  float y;
  if (i>extinguishAt) {y=0;}
  else {y = vibLevel;}
  if (y>255) {y=255;}
  else if (y<0) {y=0;}
  analogWrite(vibPin,(int)y);
}

void flicker(int i) {
  float lightLevel = (i/100)*lightMax*randomVar/10;
  Serial.println(lightLevel);
  /*float y = lightLevel-i*lightLevel/50;*/
  float y = lightLevel;
  /*if (i>extinguishAt) {y=0;}
  else {y = lightLevel;}*/
  if (y>255) {y=255;}
  else if (y<0) {y=0;}
  b.allLedsOn(0,0,(int)y);
}
