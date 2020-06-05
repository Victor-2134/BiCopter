#include<Servo.h>

Servo SL,SR;
const int PS[2] = {0,0};
const int PJx = A0;
const int PJy = A1;
const int M[2][3] = {{6,9,2},{13,12,10}};       // row - {Left Motor,Right Motor}, col - {ip1, ip2, en}
const int BaseAngle =  90;
const int Pot = A2;

int Mspeed[2] = {0,0};
int yangle = 0;
int Jx = 0;
int Jy = 0;
int DEAD_ZONE = 5; //Joystick dead zone, gives a tiny null area in the middle of the stick to stop ghost values.
int nullX = 500; //average output of X axis at center
int nullY = 528; //average output of Y axis at center
int moveAmt[2][2] = {{0,0},{0,0}};  //Movement of joystick : row - {Left ,Right }, col - {Motor speed, Servo Angle}
int ChangeAmt[2][2]= {{0,0},{0,0}}; // row - {Left Motor,Right Motor}, col - {Motor speed, Servo Angle}
int BaseSpeed = 0;
int l;  //to turn on/off the motor according to the altitude pot
bool b;
int AngleError = 6;
int SpeedError = 7;

void setup() {
  // put your setup code here, to run once:
  pinMode(PJx,INPUT);
  pinMode(PJy,INPUT);
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

  SL.write(0);
  delay(150);
  SR.write(0);
  delay(150);
  SL.write(BaseAngle);
  delay(150);
  SR.write(BaseAngle);
  delay(150);
  
  Serial.begin(9600);
}

void Direction(){
  if (Jx > nullX && (Jx - nullX) >= DEAD_ZONE) {
    //Right
    moveAmt[1][0] = constrain((Jx - nullX), 0, 495);
    ChangeAmt[1][0] = map(moveAmt[1][0],0,495,0,BaseSpeed);
    ChangeAmt[0][0] = 0;
  }
  else if (Jx < nullX && (nullX - Jx) >= DEAD_ZONE) {
    //Left
    moveAmt[0][0] = constrain((nullX - Jx), 0, 495);
    ChangeAmt[0][0] = -1* map(moveAmt[0][0],0,495,0,BaseSpeed);
    ChangeAmt[1][0]= 0;
  }
  else{
    moveAmt[0][0] = 0;
    moveAmt[1][0] = 0;
  }
    

  if (Jy > nullY && (Jy - nullY) >= DEAD_ZONE) {
    //Up
    moveAmt[1][1] = constrain((Jy - nullY), 0, 495); 
    ChangeAmt[1][1] = map(moveAmt[1][1],0,495,0,90); //ChangeAmt[1][1] = -1 * map(moveAmt[1][1],0,495,0,90); -> To change direction of rotation
    ChangeAmt[0][1] = 0;
  }
  else if (Jy < nullY && (nullY - Jy) >= DEAD_ZONE) {
    //Down
    moveAmt[0][1] = constrain((nullY - Jy), 0, 495);
    ChangeAmt[0][1] = -1 * map(moveAmt[0][1],0,495,0,90);  //ChangeAmt[0][1] = map(moveAmt[0][1],0,495,0,90); -> To change direction of rotation
    ChangeAmt[1][1] = 0;
  }
  else{
    moveAmt[0][1] = 0;
    moveAmt[1][1] = 0;
  }
    
  
  // control Roll / left right movement / x - axis movement
  Mspeed[0] = BaseSpeed + ChangeAmt[0][0] + ChangeAmt[1][0] + SpeedError;
  Mspeed[0] = constrain(Mspeed[0],0,255);
  Mspeed[1] = BaseSpeed - ChangeAmt[0][0] - ChangeAmt[1][0] - SpeedError; 
  Mspeed[1] = constrain(Mspeed[1],0,255);
  
  // control Pitch / forward backward movement / y - axis movement
  yangle = BaseAngle + ChangeAmt[1][1] + ChangeAmt[0][1] + AngleError; // map(moveAmt[1], (-1*BaseSpeed), BaseSpeed, -90, 90);
}

void loop() {

Jx = analogRead(PJx);
Jy = analogRead(PJy);
BaseSpeed = analogRead(Pot);
l = map(BaseSpeed,0,50,0,1);
/*
while(BaseSpeed == 0){
  Serial.print("Return");
  goto Skip;
}*/
//Propeller motors are made to spin in opposite direction.
// Left - clk, Right - anti-clk
  
digitalWrite(M[0][0],l);
digitalWrite(M[0][1],LOW);
digitalWrite(M[1][0],LOW);
digitalWrite(M[1][1],l);
  
  
BaseSpeed = map(BaseSpeed,0,1023,0,255);

Direction();

Serial.print("RAW             :   ");
Serial.print(Jx);
Serial.print("  ");
Serial.print(Jy);
Serial.print("  ");
Serial.println(BaseSpeed);

Serial.print("Speed Movement  :   ");
Serial.print(moveAmt[0][0]);
Serial.print("  ");
Serial.println(moveAmt[1][0]);

Serial.print("Angle Movement  :   ");
Serial.print(moveAmt[0][1]);
Serial.print("  ");
Serial.println(moveAmt[1][1]);

Serial.print("Speed Change    :   ");
Serial.print(ChangeAmt[0][0]);
Serial.print("  ");
Serial.println(ChangeAmt[1][0]);

Serial.print("Angle Change    :   ");
Serial.print(ChangeAmt[0][1]);
Serial.print("  ");
Serial.println(ChangeAmt[1][1]);

Serial.print("Final Values    :   ");
Serial.print(Mspeed[0]);
Serial.print("  ");
Serial.print(Mspeed[1]);
Serial.print("  ");
Serial.println(yangle);
Serial.println();


SL.write(yangle);
SR.write(yangle);


analogWrite(M[0][2],Mspeed[0]);
analogWrite(M[1][2],Mspeed[1]);


//Serial.print("  ");
//Serial.println(Mspeed);
Skip:
delay(1000);
}
