/*
  MSE 2202 Project 
  Language: Arduino
  Authors: Jordan Ingram 
  Date: 1/04/18
  Purpose: Program for arduino to find the cube - Code for robot to ensure it is the right distance from the wall and to turn corners when necessary
           Also, when the cube is found, the claw will turn and retrieve it. 
*/

//Files to include so that program will run 
#include <Servo.h>
#include <EEPROM.h>
#include <uSTimer2.h>
#include <CharliePlexM.h>
#include <Wire.h>
#include <I2CEncoder.h>
#include <SoftwareSerial.h>


/*** MOTORS ***/
//Setup the motors for motion and their encoders, also declare the pins they are connected to 
Servo servo_RightMotor;
I2CEncoder encoder_RightMotor;
const int ci_Right_Motor = 8;
Servo servo_LeftMotor;
I2CEncoder encoder_LeftMotor;
const int ci_Left_Motor = 9;
//Motors speeds 
const int ui_Motors_Stop = 1500; //Stop
unsigned int ui_Motors_Top_Speed = 2200; //Full Speed

//Claw servo motors
Servo servo_LeftRightClaw;
const int ci_LeftRightClaw_Motor = 12;
int OverSlide = 0;
int OverWall = 110;
Servo servo_UpDownClaw;
const int ci_UpDownClaw_Motor = 10;
int up = 80;
int down = 110;
Servo servo_OpenCloseClaw;
const int ci_OpenCloseClaw_Motor = 11;
int Opened2 = 20;
int Opened1 = 40;
int Closed = 70;


/*** SENSORS ***/
//Ultrasonic Sensor 1 - placed at the fron of robot, sense corners
const int ci_Ultrasonic_PingOne = 2;   //input plug
const int ci_Ultrasonic_DataOne = 3;   //output plug
unsigned long ul_Echo_Time;
void Ping1();

//Ultrasonic Sensor 2 - senses distance to the wall, placed on right side of robot chasis
const int ci_Ultrasonic_PingTwo = 4;   //input plug
const int ci_Ultrasonic_DataTwo = 5;   //output plug
unsigned long ul_Echo_TimeTwo;
void Ping2();

//Ultrasonic Sensor 3 - attached by an arm to the robot, rests on top of wall to sense cube 
const int ci_Ultrasonic_PingThree = 6;   //input plug
const int ci_Ultrasonic_DataThree = 7;   //output plug
unsigned long ul_Echo_TimeThree;
void Ping3();

//Additional Variables
int foundCube = 0;
int counter = 0;
int foundWall = 0;
int TurnCounter = 1;

//Some additional functions
void TurnCorner();
void GrabTheCube();
void TipThePyramid();
void FullTurn(int TurnCounter);


//To setup components of the program 
void setup() {
 
  // set up the serial monitor - useful for debugging
  Serial.begin(9600);

  // set up ultrasonic one and ultrasonic two 
  pinMode(ci_Ultrasonic_PingOne, OUTPUT);
  pinMode(ci_Ultrasonic_DataOne, INPUT);
  pinMode(ci_Ultrasonic_PingTwo, OUTPUT);
  pinMode(ci_Ultrasonic_DataTwo, INPUT);
  pinMode(ci_Ultrasonic_PingThree, OUTPUT);
  pinMode(ci_Ultrasonic_DataThree, INPUT);

  // Setup drive motors
  pinMode(ci_Right_Motor, OUTPUT);
  servo_RightMotor.attach(ci_Right_Motor);
  pinMode(ci_Left_Motor, OUTPUT);
  servo_LeftMotor.attach(ci_Left_Motor);

  //Setup claw motors  
  pinMode(ci_LeftRightClaw_Motor, OUTPUT);
  servo_LeftRightClaw.attach(ci_LeftRightClaw_Motor);  
  pinMode(ci_UpDownClaw_Motor, OUTPUT);
  servo_UpDownClaw.attach(ci_UpDownClaw_Motor);
  pinMode(ci_OpenCloseClaw_Motor, OUTPUT);
  servo_OpenCloseClaw.attach(ci_OpenCloseClaw_Motor);


//Reset the claw so it is at rest over the slide during the robot motion 
servo_OpenCloseClaw.write(Closed);
servo_UpDownClaw.write(up);
servo_LeftRightClaw.write(OverSlide);


//Setup the Infrared sensor
pinMode (A0, INPUT);

}


//Main program 
void loop(){

//Stage 1 - Find the wall so that the robot can begin looking for the cube 
while (foundWall == 0){
  delay(400);
  Ping1();
  Serial.print("First Tracker: "); Serial.print(ul_Echo_Time); Serial.print("\n");
  if (((ul_Echo_Time > 600) || (ul_Echo_Time == 0)) &&(ul_Echo_TimeTwo > 400))  {
    Serial.print("Looking for the wall\n");
  servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
  } else {
      Serial.print("Think we found it, double checking\n");
      servo_RightMotor.writeMicroseconds(1500);
      servo_LeftMotor.writeMicroseconds(1500);
      delay(100);
      Serial.print("Check Side 1");
      servo_RightMotor.writeMicroseconds(1800);
      servo_LeftMotor.writeMicroseconds(1500);
      delay(1000);
      Ping1();
      if ((ul_Echo_Time < 900)){
         Serial.print("Check Side 2");
      servo_RightMotor.writeMicroseconds(1200);
      servo_LeftMotor.writeMicroseconds(1500);
      delay(300);
      servo_RightMotor.writeMicroseconds(1500);
      servo_LeftMotor.writeMicroseconds(1800);
      delay(1000);
      Ping1();
        if (ul_Echo_Time < 900){
          Serial.print("Success it's a wall");
             foundWall = 1;
              TurnCorner();
               }
               else {
              TurnCorner();
                 Serial.print("Side 2 failed, not a wall");
               }
                  
               }
           
       else {
         Serial.print("Side 1 failed, not a wall");
        TurnCorner();
        }}
}
  

 while ((foundCube == 0) && (foundWall == 1)){ 
   //Activate all three ultrasonic sensors and determine their readings 
   Ping3();
   Ping2(); 
   Ping1();
   delay(500);
   Serial.print("First Tracker: "); Serial.print(ul_Echo_Time); Serial.print("\n");
   Serial.print("Second Tracker: "); Serial.print(ul_Echo_TimeTwo); Serial.print("\n");
   Serial.print("Third Tracker: "); Serial.print(ul_Echo_TimeThree); Serial.print("\n");

//Case 1 - Found the cube
if ((ul_Echo_TimeThree>850) &&(ul_Echo_TimeThree <980)){
  Serial.print ("Case 1 FOUND THE CUBE\n\n");  
servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
void GrabTheCube();
foundCube = 1;

Ping3();
if((ul_Echo_TimeThree>720) &&(ul_Echo_TimeThree <850)){
  foundCube = 0;
}}

//Case 2 - Straight forward - right distance from the wall and not turning 
else if ((ul_Echo_Time > 450) && (ul_Echo_TimeTwo >= 220) && (ul_Echo_TimeTwo <= 310)){
  Serial.print ("Case 2 DRIVE STRAIGHT\n\n");
  servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
}

//Case 3 - Too far from wall, turn in towards wall
else if ((ul_Echo_TimeTwo > 310) && (ul_Echo_Time > 450)  && (ul_Echo_Time < 600)){
  Serial.print("Case 3 MOVE IN\n\n");
  servo_RightMotor.writeMicroseconds(1600);
  servo_LeftMotor.writeMicroseconds(1800);
  //delay(10);
}

//Case 4 - Too close to wall, turn away from wall 
else if ((ul_Echo_TimeTwo < 220) && (ul_Echo_Time > 450) && (ul_Echo_Time < 600) && (ul_Echo_TimeTwo !=0)){
  Serial.print("Case 4 MOVE AWAY\n\n");
   servo_RightMotor.writeMicroseconds(1800);
   servo_LeftMotor.writeMicroseconds(1600);
   //delay(10);
}

//Case 5 - At a corner, turn around the corner 
else if (ul_Echo_Time <=450 && (ul_Echo_Time != 0)){
  Serial.print("Case 5 CORNER\n\n");
  TurnCorner();

  
}

//Case 6 - Nowhere to be found 
else if (( (ul_Echo_Time > 600) || (ul_Echo_Time == 0)) &&(ul_Echo_TimeTwo > 450)){
    Serial.print(" CASE 6 LOST \n\n");
    foundWall = 0;    
}



  }
    Serial.print("END OF FINDING CUBE\n\n");
  
  while (foundCube == 1){
//Case 1 - DRIVE STRAIGHT
if ( (ul_Echo_Time > 450) ){
  Serial.print ("Case 2 DRIVE STRAIGHT\n\n");
  servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
  
} 

//Case 2 - FOUND A SURFACE
else if (ul_Echo_Time < 450) {
//Determine if there is a pyramid, and if it's the right one
Serial.print("Think we found a pyramid, double checking\n");
      servo_RightMotor.writeMicroseconds(1500);
      servo_LeftMotor.writeMicroseconds(1500);
      delay(100);

//Infrared on left side - Communicates with other microcontroller 
//2 - Wrong Pyramid 
//1 - Right Pyramid
//0 - No Pyramid :( 
byte sensorVal = Serial.read();
int LeftSensor = sensorVal; //Converts byte to integer

//LOW means pyramid found
//HIGH means no pyramid
bool RightSensor = analogRead(A0); 


if (RightSensor == 0) {
Serial.print("It's definitly a pyramid, which one tho\n\n");

int counter2 = 0;
//Rotate Slightly for other ultrasonic to check 
while (counter2 < 100 && (sensorVal!=1)){
//Rotate a little bit at a time
sensorVal = Serial.read();
counter++;
  
}
if (sensorVal == 1){
Serial.print("Right Pyramid\n\n");
TipThePyramid();
  
} else {
Serial.print("Wrong Pyramid\n\n");
  
}
} else if (sensorVal == 1){
 Serial.print("RIGHT PYRAMID but on the left side of robot\n\n"); 

}else if (sensorVal == 2){
 Serial.print("WRONG PYRAMID but on the left side of robot\n\n"); 

}  else if (sensorVal == 0){
  Serial.print("NO PYRAMID 100% IS A WALL\n\n"); 
TurnCounter ++;
TurnCorner(TurnCounter);
}    
}
    
  
 
  
  }
}


void TipThePyramid(){


  
}


void GrabTheCube(){
delay(2000);
servo_LeftRightClaw.write(OverWall);
delay(2000);
servo_OpenCloseClaw.write(Opened2);
delay(2000);
servo_UpDownClaw.write(down);
delay(2000);
servo_OpenCloseClaw.write(Closed);
delay(2000);
servo_UpDownClaw.write(up);
delay(2000);
servo_LeftRightClaw.write(OverSlide);
delay(2000);
}


void TurnCorner(){
   Serial.print("TURNING THE CORNER\n\n");
  while (ul_Echo_Time<=400){
    Ping1();
  servo_RightMotor.writeMicroseconds(1400);
  servo_LeftMotor.writeMicroseconds(1400);
  delay(100); }
 while (ul_Echo_Time<=1000){
  servo_RightMotor.writeMicroseconds(1700);
  servo_LeftMotor.writeMicroseconds(1300);
  delay(200); 
    Ping1();
  }
}

void FullTurn(int TurnCounter){
  if ((TurnCounter%2) == 0){
    //EVEN so turn LEFT
  } else {
    //ODD so turn RIGHT
    
  }
  
}


// Function to get readings from first ultrasonic sensor 
void Ping1()
{
  //Ping Ultrasonic and send the Ultrasonic Range Finder a 10 microsecond pulse per tech spec
  digitalWrite(ci_Ultrasonic_PingOne, HIGH);
  delayMicroseconds(10);  //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ci_Ultrasonic_PingOne, LOW);
  //use command pulseIn to listen to Ultrasonic_Data pin to record the
  //time that it takes from when the Pin goes HIGH until it goes LOW
  ul_Echo_Time = pulseIn(ci_Ultrasonic_DataOne, HIGH, 10000);
}


// Function to get readings from second ultrasonic sensor 
void Ping2(){
  //Ping Ultrasonic and send the Ultrasonic Range Finder a 10 microsecond pulse per tech spec
  digitalWrite(ci_Ultrasonic_PingTwo, HIGH);
  delayMicroseconds(10);  //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ci_Ultrasonic_PingTwo, LOW);
  //use command pulseIn to listen to Ultrasonic_Data pin to record the
  //time that it takes from when the Pin goes HIGH until it goes LOW
  ul_Echo_TimeTwo = pulseIn(ci_Ultrasonic_DataTwo, HIGH, 10000);
} 


// Function to get readings from third ultrasonic sensor 
void Ping3(){
  //Ping Ultrasonic and send the Ultrasonic Range Finder a 10 microsecond pulse per tech spec
  digitalWrite(ci_Ultrasonic_PingThree, HIGH);
  delayMicroseconds(10);  //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ci_Ultrasonic_PingThree, LOW);
  //use command pulseIn to listen to Ultrasonic_Data pin to record the
  //time that it takes from when the Pin goes HIGH until it goes LOW
  ul_Echo_TimeThree = pulseIn(ci_Ultrasonic_DataThree, HIGH, 10000);
}
