/*
  2.4l General Motors engine simulator
  Simulator produces 0-5v square wave pulses coinciding with bse engine timing for crankshaft and camshafts
  Also produces simulated signals for MAP and MAF based on inputs from an APP potentiometer

  All outputs route through 74HCT125 to bring digital high to 5V
  Analog intputs to ECM are then routed to RC low pass filters

*/

#include <Arduino.h>

int RPMpin = 5;
int MAFHzPin = 16;
int MAPPin = 17;

int APPpin = 4;
float APPvalue = 0;
float APPone = 0;
float APPtwo = 0;
float APPoneOutput = 0;
float APPtwoOutput = 0;
int APPOneOutputPin = 6;
int APPTwoOutputPin = 7;

float RPMinput = 0;

int Crank = 15;
int IntakeCam = 2;
int ExhaustCam = 0;
int IntakeOffset = 0;
int ExhaustOffset = 30;
float EngineSpeed = 200;
int IntakeArrayPosition = 0;
int ExhaustArrayPosition = 0;
//int EngineSpeedDecreasing = 1;
int MAFHz = 0;
int MAPOutput = 500;


//                                                                                                                                                                                                                                 1                   1    TDC |         |1                   1                   1                   1                   1                   1                   1                   1                   2                   2                   2                   2   TDC |         |      
//                         0                   1                   2                   3                   4                   5                   6                   7                   8                   9                   0                   1                   2                   3                   4                   5                   6                   7                   8                   9                   0                   1                   2                   3                         
//                         0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 
int CrankProfile[240]   = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0};
int IntakeProfile[240]  = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0};


void setup(){
  pinMode(APPpin, INPUT);
//  pinMode(APPone, OUTPUT);
//  pinMode(APPtwo, OUTPUT);
//  Serial.begin(115200);

  pinMode (Crank, OUTPUT);
  pinMode (IntakeCam, OUTPUT);
  pinMode (ExhaustCam, OUTPUT);
}

void loop(){



RPMinput = 20;

if (RPMinput < 1020){
    if (EngineSpeed > 450){
      delayMicroseconds(EngineSpeed - 450);
    }
    EngineSpeed = RPMinput + 50;
      for (int i = 0; i < 239; i++){
        digitalWrite(Crank, CrankProfile [i]);
        IntakeArrayPosition = i + IntakeOffset;
        if (IntakeArrayPosition > 239){
          IntakeArrayPosition = IntakeArrayPosition - 240;
        }
        digitalWrite(IntakeCam, IntakeProfile[IntakeArrayPosition]);
        ExhaustArrayPosition = i + ExhaustOffset;
        if (ExhaustArrayPosition > 239) {
          ExhaustArrayPosition = ExhaustArrayPosition - 240;
        }
        digitalWrite(ExhaustCam, IntakeProfile[ExhaustArrayPosition]);
        delayMicroseconds(EngineSpeed); 
      }
}


  APPvalue = analogRead(APPpin);
  APPone = ((APPvalue/1024)*3) + .84;
  APPtwo = ((APPvalue/1024)*1.5) + .47;

  APPoneOutput = (APPone / 5) * 255;
  APPtwoOutput = (APPtwo / 5) * 255;

  ledcWrite(APPOneOutputPin, APPoneOutput);
  ledcWrite(APPTwoOutputPin, APPtwoOutput);

  MAPOutput = (APPvalue / 4) + 75;
  if (MAPOutput > 255){
    MAPOutput = 255;
  }
  ledcWrite(MAPPin, MAPOutput);

  
  MAFHz = (APPvalue * 5) + 200;
  if (MAFHz > 10000){
    MAFHz = 9999;
  }

  tone(MAFHzPin,MAFHz);

}
