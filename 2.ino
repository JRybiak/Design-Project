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

}


//Main program 
void loop(){

 while (foundCube == 0){ 
   //Activate all three ultrasonic sensors and determine their readings 
   Ping3(); Ping2(); Ping1();
   Serial.print("First Tracker: "); Serial.print(ul_Echo_Time); Serial.print("\n");
   Serial.print("Second Tracker: "); Serial.print(ul_Echo_TimeTwo); Serial.print("\n");
   Serial.print("Third Tracker: "); Serial.print(ul_Echo_TimeThree); Serial.print("\n");

//Case 1 - Found the cube
if ((ul_Echo_TimeThree>850) &&(ul_Echo_TimeThree <980)){
servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
foundCube = 1;
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

Ping3();
if((ul_Echo_TimeThree>720) &&(ul_Echo_TimeThree <850)){
  foundCube = 0;
}}

//Case 2 - Straight forward - right distance from the wall and not turning 
else if ((ul_Echo_Time > 600) && (ul_Echo_TimeTwo >= 220) && (ul_Echo_TimeTwo <= 310)){
  Serial.print ("Case 1 DRIVE STRAIGHT\n\n");
  servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
}

//Case 3 - Too far from wall, turn in towards wall
else if ((ul_Echo_TimeTwo > 310) && (ul_Echo_Time > 600)){
  Serial.print("Case 2 MOVE IN\n\n");
  servo_RightMotor.writeMicroseconds(1600);
  servo_LeftMotor.writeMicroseconds(1800);
  //delay(10);
}

//Case 4 - Too close to wall, turn away from wall 
else if ((ul_Echo_TimeTwo < 220) && (ul_Echo_Time > 600) && (ul_Echo_TimeTwo !=0)){
  Serial.print("Case 3 MOVE AWAY\n\n");
   servo_RightMotor.writeMicroseconds(1800);
   servo_LeftMotor.writeMicroseconds(1600);
   //delay(10);
}

//Case 5 - At a corner, turn around the corner 
else if (ul_Echo_Time<=320 && (ul_Echo_Time != 0)){
  Serial.print("Case 4 CORNER\n\n");
  while (ul_Echo_Time<=400){
    Ping1();
  servo_RightMotor.writeMicroseconds(1400);
  servo_LeftMotor.writeMicroseconds(1400);
  delay(100); }
 while (ul_Echo_Time<=600){
  
  servo_RightMotor.writeMicroseconds(1700);
  servo_LeftMotor.writeMicroseconds(1400);
  delay(400); 
  servo_RightMotor.writeMicroseconds(1700);
  servo_LeftMotor.writeMicroseconds(1700);
  delay(400); 
    Ping1();
  
  }

  
}
  }
    Serial.print("END OF FINDING CUBE\n\n");
  
  while (foundCube == 1){
  
 
  
  
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
