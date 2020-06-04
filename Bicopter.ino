#include<Servo.h>

Servo SL,SR;
const int PS[2] = {6,7};
const int PJx = A0;
const int PJy = A1;
const int Pc = 1; 
const int M[2][3] = {{11,10,3},{0,0,0}};       //col - {ip1, ip2, en}, row - {Left Motor,Right Motor}
//const int BaseSpeed =  175;
const int BaseAngle =  90;
const int Button[2] = {4,3};
const int Pot = ;

int Mspeed[2] = {0,0};
int yangle = 0;
int Jx = 0;
int Jy = 0;
int count = 0;
int DEAD_ZONE = 5; //Joystick dead zone, gives a tiny null area in the middle of the stick to stop ghost values.
int nullX = 500; //average output of X axis at center
int nullY = 528; //average output of Y axis at center
int moveAmt[2] = {0,0}; //Movement of joystick along X,Y axis
int Speed_Base = 0;
bool b;

void setup() {
  // put your setup code here, to run once:
pinMode(PJx,INPUT);
pinMode(PJy,INPUT);
pinMode(Pc,INPUT);
pinMode(Pot,INPUT);
//pinMode(Button[0],INPUT);
//pinMode(Button[1],INPUT);
for(int i=0;i<2;i++){
  for(int j=0;j<3;j++){
    pinMode(M[i][j],OUTPUT);
  }
}
SL.attach(PS[0]);
SR.attach(PS[1]);


  //Propeller motors are made to spin in opposite direction.
  // Left - clk, Right - anti-clk
  
  digitalWrite(M[0][0],HIGH);
  digitalWrite(M[0][1],LOW);
  digitalWrite(M[1][0],LOW);
  digitalWrite(M[1][1],HIGH);
  SL.write(0);
  SR.write(0);
  SL.write(BaseAngle);
  SR.write(BaseAngle);
  
Serial.begin(9600);
}

void Direction(){
  if (Jx > nullX && (Jx - nullX) >= DEAD_ZONE) {
//Up
moveAmt[0] = constrain((Jx - nullX), 0, 495);
moveAmt[0] = map(moveAmt[0],0,495,Speed_Base,255);
}
else if (Jx < nullX && (nullX - Jx) >= DEAD_ZONE) {
//Down
moveAmt[0] = constrain((nullX - Jx), 0, 495);
moveAmt[0] = -1 * map(moveAmt[0],0,495,0,Speed_Base);
}
else
  moveAmt[0] = 0;;

if (Jy > nullY && (Jy - nullY) >= DEAD_ZONE) {
//Right
moveAmt[1] = constrain((Jy - nullY), 0, 495);
}
else if (Jy < nullY && (nullY - Jy) >= DEAD_ZONE) {
//Left
moveAmt[1] = -1 * constrain((nullY - Jy), 0, 495);
}
else
  moveAmt[1] = 0;

}

void loop() {

Jx = analogRead(PJx);
Jy = analogRead(PJy);
Speed_Base = analogRead(Pot);

Speed_Base = map(Speed_Base,0,1023,0,255);

Direction();
/*
if(digitalRead(Button[0]))
  Serial.println("Rise()");
else if (digitalRead(Button[1]))
  Serial.println("Fall()");
*/
// control left right movement or x - axis movement

Mspeed[1] = constrain((Speed_Base[0] + moveAmt[0]),0,255);
Mspeed[0] = 255 - Mspeed[1];

// control forward, backward movement or y - axis movement
yangle = BaseAngle + map(moveAmt[1], -495, 495, -90, 90);

Serial.print(moveAmt[0]);
Serial.print("  ");
Serial.print(moveAmt[1]);
Serial.print("  ");
Serial.println(yangle);
Serial.print(Mspeed[0]);
Serial.print("  ");
Serial.println(Mspeed[1]);


SL.write(yangle);
SR.write(yangle);


analogWrite(M[0][2],Mspeed);
analogWrite(M[1][2],Mspeed);


Serial.print(Jx);
Serial.print("  ");
Serial.println(Jy);
//Serial.print("  ");
//Serial.println(Mspeed);
delay(100);
}
