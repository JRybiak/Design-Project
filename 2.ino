/*
  MSE 2202 Project 
  Language: Arduino
  Authors: Jordan Ingram 
  Date: 16/03/18
  Purpose: Program turns arduino to run parallel to a wall on the right side based on readings from an ultrasonic sensor 
*/

//Files to include so that program will run 
#include <Servo.h>
#include <EEPROM.h>
#include <uSTimer2.h>
#include <CharliePlexM.h>
#include <Wire.h>
#include <I2CEncoder.h>
#include <SoftwareSerial.h>

/* *** Wheel Motors *** */
//Declare the two motors for motion, a left and a right motor
Servo servo_RightMotor;
Servo servo_LeftMotor;
//Declare the encoders for each of the motors 
I2CEncoder encoder_RightMotor;
I2CEncoder encoder_LeftMotor;
//Pins for the motors 
const int ci_Right_Motor = 8;
const int ci_Left_Motor = 9;
//Motors speeds 
const int ui_Motors_Stop = 1500;        // 200 for brake mode; 1500 for stop
unsigned int ui_Motors_Top_Speed = 2200;        // Default run speed

/* *** ULTRASONIC SENSORS *** */
//Pins for the ultrasonic sensors - 1 input and 1 output for each of the 2 sensors
const int ci_Ultrasonic_Ping = 2;   //input plug
const int ci_Ultrasonic_Data = 3;   //output plug
const int ci_Ultrasonic_PingTwo = 4;   //input plug
const int ci_Ultrasonic_DataTwo = 5;   //output plug
unsigned long ul_Echo_Time;
unsigned long ul_Echo_TimeTwo;
//Functions for each of the sensors 
void Ping();
void Ping2();


//To setup components of the program 
void setup() {
 
  // set up the serial monitor - useful for debugging
  Serial.begin(9600);

  // set up ultrasonic one and ultrasonic two 
  pinMode(ci_Ultrasonic_Ping, OUTPUT);
  pinMode(ci_Ultrasonic_Data, INPUT);
  pinMode(ci_Ultrasonic_PingTwo, OUTPUT);
  pinMode(ci_Ultrasonic_DataTwo, INPUT);

  // set up drive motors
  pinMode(ci_Right_Motor, OUTPUT);
  servo_RightMotor.attach(ci_Right_Motor);
  pinMode(ci_Left_Motor, OUTPUT);
  servo_LeftMotor.attach(ci_Left_Motor);
}


//Main program 
void loop(){

//Drive straight when the right distance form the wall 
while ((ul_Echo_Time > 300) && (ul_Echo_Time < 400)){
  Serial.print("Straight: " + ul_Echo_Time);
  Serial.print("\n");
  servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
    delay(100);
    Ping();
 }

//Turn away from the wall when too close 
  while (ul_Echo_Time < 300){
    Serial.print("Turn away: " + ul_Echo_Time);
  Serial.print("\n");
    servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
      delay(10);
    Ping();
 }

//Turn towards the wall when too far away 
  while (ul_Echo_Time > 400){
    Serial.print("Turn towards: " + ul_Echo_Time);
  Serial.print("\n");
    servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
      delay(10);
    Ping();
 }
}  


// Function to get readings from first ultrasonic sensor 
void Ping()
{
  //Ping Ultrasonic and send the Ultrasonic Range Finder a 10 microsecond pulse per tech spec
  digitalWrite(ci_Ultrasonic_Ping, HIGH);
  delayMicroseconds(10);  //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ci_Ultrasonic_Ping, LOW);
  //use command pulseIn to listen to Ultrasonic_Data pin to record the
  //time that it takes from when the Pin goes HIGH until it goes LOW
  ul_Echo_Time = pulseIn(ci_Ultrasonic_Data, HIGH, 10000);
}


// Function to get readings from second ultrasonic sensor 
void Ping2()
{
  //Ping Ultrasonic and send the Ultrasonic Range Finder a 10 microsecond pulse per tech spec
  digitalWrite(ci_Ultrasonic_PingTwo, HIGH);
  delayMicroseconds(10);  //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ci_Ultrasonic_PingTwo, LOW);
  //use command pulseIn to listen to Ultrasonic_Data pin to record the
  //time that it takes from when the Pin goes HIGH until it goes LOW
  ul_Echo_TimeTwo = pulseIn(ci_Ultrasonic_DataTwo, HIGH, 10000);
} 
