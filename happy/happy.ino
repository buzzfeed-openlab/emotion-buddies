
#include "InternetButton.h"
#include "math.h"

#define touchPin A0
#define vibPin D1
#define avgTotal 1000

InternetButton b = InternetButton();

int pubFrequency = 1000;
int lastPub = 0;

int period = 1500;
int vibMin = 50; // minimum to feel vibration
int vibMax = 500; // maximum vibration
int vibBase = 40;
int lightMin = 0;
int lightMax = 255;
int lightBase = 20;
float pi = 3.14;
int intensity=0;
int toAvg[avgTotal];

int touchMax=4000;
int touchMin=3000;
int lastTouchRead=0;
int n=0;
int t;

void setup() {
  pinMode(touchPin, INPUT);
  pinMode(vibPin, OUTPUT);

  Serial.begin(9600);
  b.begin();
}

void loop() {
  // check for squeeze
  // brighter red light on squeeze
  // stronger vibration on squeeze
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

  intensity=map(touchAvg,touchMin,touchMax,0,100);
  t = millis()%period;
  purr(intensity);
  beam(intensity);

}

void purr(int i) {
  float y = vibMin+i*(vibMax/2)/100+i*(vibMax/2)*sin(t*2*pi/period)/100;
  /*float y = i+(vibBase/2)*sin(t*2*pi/period);*/
  if (y>255) {y=255;}
  else if (y<0) {y=0;}
  analogWrite(vibPin,(int)y);
}

void beam(int i) {
  float y = lightMin+i*(lightMax/2)/100+i*(lightMax/2)*sin(t*2*pi/period)/100;
  /*float y = i+(lightBase/2)*sin(2*pi*t/period);*/
  if (y>255) {y=255;}
  else if (y<0) {y=0;}
  b.allLedsOn((int)y,(int)y,0);
}
