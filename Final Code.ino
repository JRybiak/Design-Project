/*
  MSE 2202 Project 
  Language: Arduino
  Authors: Jordan Ingram, Jade Codinera, Julia Rybiak 
  Date: 05/04/18
  Purpose: Final code for the MSE 2202 design project - each part of the code is broke up into different cases 
*/


/*** Included Files ***/
#include <Servo.h>
#include <EEPROM.h>
#include <I2CEncoder.h>
#include <SoftwareSerial.h>


/*** Driving Motors ***/
//Setup the motors for motion, declare their pins and define associated variables  
Servo servo_RightMotor;
const int ci_Right_Motor = 8;
Servo servo_LeftMotor;
const int ci_Left_Motor = 9;
//Motors speeds
const int ui_Motors_Stop = 1500; //Stop
const int ui_Motors_Top_Speed = 2200; //Top Speed
const int ui_Motors_Reverse = 1200; //Reverse
const int ui_Motors_Slow_Speed; //Slow forward motion


/*** Servo Motors ***/
//Setup all four servo motors and define variables to keep track of certain positons 
//Left and Right servo motor
Servo servo_LeftRightClaw;
const int ci_LeftRightClaw_Motor = 12;
int OverSlide = 0;
int OverWall = 110;
//Up and Down servo motor
Servo servo_UpDownClaw;
const int ci_UpDownClaw_Motor = 10;
int up = 80;
int down = 110;
//Opened and Closed servo motor 
Servo servo_OpenCloseClaw;
const int ci_OpenCloseClaw_Motor = 11;
int Opened2 = 20; //Wider opening 
int Opened1 = 40; //Smaller opening 
int Closed = 70;
//Pyramid Tipper servo motor
Servo servo_PyramidTipper;
const int ci_PyramidTipper_Motor = 13;
const int pyramidDown = 20;
const int pyramidHalf = 110;
const int pyramidUp = 180;


/*** Sensors ***/
//Ultrasonic Sensor 1 - placed at the fron of robot
const int ci_Ultrasonic_PingOne = 2;   //input
const int ci_Ultrasonic_DataOne = 3;   //output
unsigned long ul_Echo_Time;
//Ultrasonic Sensor 2 - senses distance to the wall, placed on right side of the robot
const int ci_Ultrasonic_PingTwo = 4;   //input
const int ci_Ultrasonic_DataTwo = 5;   //output
unsigned long ul_Echo_TimeTwo;
//Ultrasonic Sensor 3 - attached by an arm to the robot, rests over top of wall to sense cube 
const int ci_Ultrasonic_PingThree = 6;   //input
const int ci_Ultrasonic_DataThree = 7;   //output
unsigned long ul_Echo_TimeThree;
//Infrared sensor - to hold data 
bool RightSensor; 


/*** Variables ***/
//Variables to keep track of progression through the program
int foundCube = 0;
int foundWall = 0;
//Counters used in parts 2 and 3 to keep track of how many times in a row a case is called - helps identify when robot is stuck somewhere
int twocounter = 0;
int threecounter = 0;
int fourcounter = 0;
int fivecounter = 0;
int sixcounter = 0;
int twoCounter = 0;
int fourCounter = 0;
//Used in part 3 to keep track of which direction the robot must turn - left or right
int TurnCounter = 1;
//Counter used in various while loops throughout the code 
int counter = 0;
//Variables for readings from the other microcontroller - holds just the second infrared sensor
byte sensorVal;
int LeftSensor;
//Variable needed to keep track of an infrared sensors' readings
int RightChecker = 0;
int LeftTracker = 0;


/*** Functions ***/
void TurnCorner(); //90 Degree turn left when a corner is reached during part 2
void GrabTheCube(); //Operates the claw to grab the cube 
void TipThePyramid(); //Tips the pyramid and drops off the cube 
void FullTurn(int TurnCounter); //Function to turn 180 degrees, based on turn counter function will cause a left or a right turn
int PyramidOrWall(); //Determines if a surface is a pyramid or a wall - returns 0 for a pyramid and 1 for a wall 
//Three ping functions, one for each ultrasonic sensor
void Ping1(); 
void Ping2(); 
void Ping3();


/*** Setup Section ***/
void setup() {


  /*** Sensors ***/
  //Ultrasonic 1
  pinMode(ci_Ultrasonic_PingOne, OUTPUT);
  pinMode(ci_Ultrasonic_DataOne, INPUT);
  //Ultrasonic 2
  pinMode(ci_Ultrasonic_PingTwo, OUTPUT);
  pinMode(ci_Ultrasonic_DataTwo, INPUT);
  //Ultrasonic 3
  pinMode(ci_Ultrasonic_PingThree, OUTPUT);
  pinMode(ci_Ultrasonic_DataThree, INPUT);
  //Infrared sensor
  pinMode (A0, INPUT);


  /*** Driving Motors ***/
  //Right Motor
  pinMode(ci_Right_Motor, OUTPUT);
  servo_RightMotor.attach(ci_Right_Motor);
  //Left Motor
  pinMode(ci_Left_Motor, OUTPUT);
  servo_LeftMotor.attach(ci_Left_Motor);


  /*** Servo Motors ***/ 
  //Left and Right Servo
  pinMode(ci_LeftRightClaw_Motor, OUTPUT);
  servo_LeftRightClaw.attach(ci_LeftRightClaw_Motor);  
  //Up and Down servo
  pinMode(ci_UpDownClaw_Motor, OUTPUT);
  servo_UpDownClaw.attach(ci_UpDownClaw_Motor);
  //Open and Closed servo
  pinMode(ci_OpenCloseClaw_Motor, OUTPUT);
  servo_OpenCloseClaw.attach(ci_OpenCloseClaw_Motor);
  //Pyramid Tipper
  pinMode(ci_PyramidTipper_Motor, OUTPUT);
  servo_PyramidTipper.attach(ci_PyramidTipper_Motor);
}


/*** Main Program ***/
void loop(){


/*** Reset Servo Motors ***/
//Reset the claw and pyramid tipper in case they aren't in the correct position
servo_OpenCloseClaw.write(Closed);
servo_UpDownClaw.write(up);
servo_LeftRightClaw.write(OverSlide);
servo_PyramidTipper.write(pyramidUp);
delay(1000); //Give the motors time to get into the correct positons before the program starts


/*** Stage 1 - Find the wall ***/
//Find the wall so that the robot can begin looking for the cube, once the wall is found the robot can orient itself accordingly
//This mode is to account for the fact that the robot can be placed to start anywhere in the arena 


//Repeat until the wall is found, in a while loop so that it will repeat whever a wall needs to be found 
while (foundWall == 0){
  //Using only ultrasonic 1 and ultrasonic 2 for this section  
  Ping1(); 
  Ping2();

  
  /*** Case 1.1 - Found the wall ***/
  //Ultrasonic two (on right side of robot) senses a wall
  //No code to check if this is actually a pyramid because if it is, there is code in section 2 
  //that will force the robot to return to this stage and resume searching 
  if ((ul_Echo_TimeTwo < 600) && (ul_Echo_TimeTwo!=0)){
    //Ends while loop to move on to stage 2
    foundWall = 1;
    //Stops motion of the robot
    servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
    delay(100); //Gives the robot time to stop motion and move on 
  }


  /*** Case 1.2 - No wall is found ***/
  //Neither ultrasonic 1 or 2 have found a wall, so the robot will just keep driving straight
  else if ((ul_Echo_Time > 600) || (ul_Echo_Time == 0))  {
    Serial.print("CASE 1.2 - Looking for the wall\n\n");
    servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
  } 

  
  /*** Case 1.3 - Found a surface ***/
  //When the front sensor finds a surface, it must double check to make sure it's not mistaking a pyramid for a wall
  else {
      //Motion is stopped
      servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
      servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
     //The 'PyramidOrWall' function will return 1 if a wall is found, or 0 if it was a pyramid
     if (PyramidOrWall() == 1){
      //If a wall was found, then this variable is changed to tell the program to move on to the next section 
      foundWall = 1; 
     } else {
      //If a pyramid was found, the robot will move around the pyramid and continue looking for a wall
      MoveAroundPyramid();
     } }}
        

/*** Stage 2 - Find the cube ***/
//This while loop will repeat after the wall was found in stage 1 and until the cube is obtained
 while ((foundCube == 0) && (foundWall == 1)){ 
   //Activate all three ultrasonic sensors and determine their readings
   Ping1();
   Ping2(); 
   Ping3(); 


/*** Case 2.0 - Robot is stuck ***/
//Each counter in the following if statement keeps track of how many times in a row most of the cases in part 2 are called 
//If one case is called too many times in a row, this if statement contains code to run the motors in a way that will make the 
//robot become unstuck 
 if (fivecounter > 20 || twocounter > 50 || threecounter > 50 || fourcounter >50 || sixcounter > 50){
  //Code that will get the robot out of wherever it is stuck
  servo_RightMotor.write(ui_Motors_Top_Speed);
  servo_LeftMotor.write(ui_Motors_Top_Speed);
  delay (2000);
  servo_RightMotor.write(ui_Motors_Top_Speed);
  servo_LeftMotor.write(ui_Motors_Reverse);
  delay (2000);
  servo_RightMotor.write(ui_Motors_Reverse);
  servo_LeftMotor.write(ui_Motors_Top_Speed);
  delay (2000);
  servo_RightMotor.write(ui_Motors_Stop);
  servo_LeftMotor.write(ui_Motors_Stop);
  delay(1000);
 }


/*** Case 2.1 - Cube is found ***/
//When the cube is within a range, as sensed by the third ultrasonic sensor, it is considered found
if ((ul_Echo_TimeThree>100) &&(ul_Echo_TimeThree <980)) {  
  //Motion is stopped
  servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
  //Third ultrasonic is consulted again to ensure that the cube is within the right range 
  Ping3(); 
  //if too close, the robot will reverse
  while (ul_Echo_TimeThree <400){
  servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Slow_Speed);
  //Third ultrasonic is consulted again
  Ping3();}
  //if too far away, the robot will move forward
  while (ul_Echo_TimeThree > 1200){
  servo_RightMotor.writeMicroseconds(ui_Motors_Reverse);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Reverse);
  Ping3();
}
//Stop Motion
servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
//Function to grab the cube is called 
GrabTheCube();
//Variable to exit while loop and move onto part 3 of the code
foundCube = 1;
}


/*** Case 2.2 - Turn Corner ***/
//If front sensor senses a corner, first check if it's actually a pyramid then act accordingly
else if (ul_Echo_Time <=1000 && (ul_Echo_Time != 0)){
  //If a wall was found, then turn the corner and continue
  if (PyramidOrWall()){
    //Code to turn the corner
    servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Reverse);
    delay(300);}
  //If it was actually a pyramid, then move around the pyramid  
  else {
    MoveAroundPyramid();
  }
 //Reset case counters and increase this case's counter by one to ensure robot is not stuck 
 twocounter = 0;  
 threecounter = 0;
 fourcounter = 0;
 fivecounter++;  
 sixcounter = 0;
}


/*** Case 2.3 - Drive Straight ***/
//If front sensor doesn't sense anything and right right sensor varifies that robot is correct distance from the wall 
//for the third sensor can detect the cube, then keep driving straight
else if (((ul_Echo_Time > 600) || (ul_Echo_Time== 0) ) && (ul_Echo_TimeTwo > 220) && (ul_Echo_TimeTwo < 310)){
//Keep driving straight
  servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
 //Reset case counters and increase this case's counter by one to ensure robot is not stuck  
 twocounter++;
 threecounter =0;
 fourcounter = 0;
 fivecounter = 0;
 sixcounter = 0;
}


/*** Case 2.4 - Move in ***/
//If the second ultrasonic sensor detects that the robot is too far from the wall, it will rotate towards the wall to line up 
else if ((ul_Echo_TimeTwo > 310)){
  //Code to rotate slightly
  servo_RightMotor.writeMicroseconds(ui_Motors_Reverse);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Slow_Speed);
 //Reset case counters and increase this case's counter by one to ensure robot is not stuck 
 threecounter ++;
 twocounter = 0;  
 fourcounter = 0;
 fivecounter = 0;
 sixcounter = 0;
}


/*** Case 2.5 - Move away ***/ 
//If the second ultrasonic sensor detects that the robot is too close to the wall, it will rotate away from the wall to line up 
else if ((ul_Echo_TimeTwo < 220) && (ul_Echo_TimeTwo != 0)){
  //Code to rotate slightly
 servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
 servo_LeftMotor.writeMicroseconds(ui_Motors_Reverse);
 //Reset case counters and increase this case's counter by one to ensure robot is not stuck 
 fourcounter ++;
 twocounter = 0;  
 threecounter =0;
 fivecounter = 0;
 sixcounter = 0;
}


/*** Case 2.6 - Lost the wall ***/
//If the robot is too far from the wall to recover with the normal code above, then it will go back to section one and relocated the wall  
else if (ul_Echo_TimeTwo > 1500){
 //Motion is stopped 
 servo_RightMotor.writeMicroseconds(1500);
 servo_LeftMotor.writeMicroseconds(1500);
 delay(100); //Wait a short time for robot to actually stop
 //Double check the sensor incase of a false reading 
 Ping2();
 //If sensor still detects robot is too close to the wall, then go back to section one
 if(ul_Echo_TimeTwo > 1500) {
    //Variable tells program to go back to section 1
    foundWall = 0;    }
 //Reset case counters and increase this case's counter by one to ensure robot is not stuck 
 sixcounter ++;
 twocounter = 0;  
 threecounter =0;
 fourcounter = 0;
 fivecounter = 0;}}


  
    
/*** Stage 3 - Find the pyramid and deliver the cube ***/
//Activate this while loop once the cube has been found 
while (foundCube == 1){
//Front ultrasonic sensor is the only one needed for motion in this section
   Ping1();
   //Left infrared sensor (other microcontroller) is read each time the loop repeats 
   sensorVal = Serial.read();
   LeftSensor = sensorVal; //Converts byte to integer


/*** Case 3.0 - Robot is stuck ***/
//Each counter in the following if statement keeps track of how many times in a row most of the cases in part 3 are called 
//If one case is called too many times in a row, this if statement contains code to run the motors in a way that will make the 
//robot become unstuck 
 if ((twoCounter == 30) || (fourCounter == 30)){
   //Code that will get the robot out of wherever it is stuck
  servo_RightMotor.write(ui_Motors_Top_Speed);
  servo_LeftMotor.write(ui_Motors_Top_Speed);
  delay (2000);
  servo_RightMotor.write(ui_Motors_Top_Speed);
  servo_LeftMotor.write(ui_Motors_Reverse);
  delay (2000);
  servo_RightMotor.write(ui_Motors_Reverse);
  servo_LeftMotor.write(ui_Motors_Top_Speed);
  delay (2000);
  servo_RightMotor.write(ui_Motors_Stop);
  servo_LeftMotor.write(ui_Motors_Stop);
  delay(1000);
 }

    
/*** Case 3.1 - Found the right pyramid ***/
//When the infrared located on the left side of the robot has found the correct pyramid
if (sensorVal == 1){
 //Since this infrared sensor is on the left side of the robot, this code is needed to ensure the robot is properly lined up to tip the pyramid
 //Code to line the robot up:
  servo_RightMotor.write(ui_Motors_Slow_Speed);
  servo_LeftMotor.write(ui_Motors_Stop);
  delay (250);
  servo_RightMotor.write(ui_Motors_Reverse);
  servo_LeftMotor.write(ui_Motors_Reverse);
  delay (667);
  servo_RightMotor.write(ui_Motors_Stop);
  servo_LeftMotor.write(ui_Motors_Stop);
  delay (100);
  //Function to tip the pyramid and deliver the cube
 TipThePyramid();
 //Reset case counters
 twoCounter = 0;
 fourCounter = 0;
}


/*** Case 3.2 - Found the wrong pyramid ***/
//When the infrared located on the left side of the robot has found the incorrect pyramid
else if (sensorVal == 2){
  //Function called to move around the pyramid
  MoveAroundPyramid(); 
  //Reset case counters 
  twoCounter = 0;
  fourCounter = 0;
}
 

/*** Case 3.3 - Drive straight ***/
//To continue along the regular path if no pyramid has been found 
else if ((ul_Echo_Time > 600)){
  //Motors drive straight
  servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Slow_Speed);
  //Reset case counters 
  twoCounter = 0;
  fourCounter = 0;
} 


/*** Case 3.4 - Found a surface ***/
//When the first ultrasonic sensor finds a surface, program must determine 
else if ((ul_Echo_Time < 600)  && (ul_Echo_Time !=0)) {
//Determine if there is a pyramid, and if it's the right one
      servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
      servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
      delay(100); //Give the wheels time to sop 
  //Ensure variables are reset incase were used earlier in the program 
  RightChecker = 0;
  counter = 0;
//Sometimes the infrared sensor will say there is no pyramid present, when it actually is.
//This loop will give the sensor 20 attempts to identify a pyramid
//From testing the pyramid never says there is a pyramid when there isn't one present
while (counter<20){
  //Read the sensor - 0 means a pyramid is found, 1 means no pyramid 
  RightSensor = analogRead(A0); 
  //Increase counter to keep track of how many readings have been done 
  counter++;
  //if a pyramid is found, change RightChecker to 1 to record this
  if (RightSensor == 0){
    RightChecker = 1;
  }}
//If surface was a wall, turn 180 degrees and continue path 
 if (RightChecker == 0){
  //Function takes turn coutner to keep track of whether robot should turn right or left 
 FullTurn(TurnCounter);
 //Increase turn counter by 1
  TurnCounter++;
  } 
//If right infrared found a pyramid, robot must rotate so that left sensor can identify which pyramid is present 
 else {
//Reset variables incase they were used earlier in the program 
LeftTracker = 0;
counter = 0;
//This loop will slightly turn 20 times to ensure that the second infrared has the chance to see the pyramid
while (counter<20){
  servo_RightMotor.write(ui_Motors_Reverse);
  servo_LeftMotor.write(ui_Motors_Slow_Speed);
  delay(10);
  servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
  servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
  //Read the left sensor - 0 means no pyramid, 1 means correct pyramid, 2 means incorrect pyramid
  sensorVal = Serial.read();
  //Keep track of the results in the left tracker variable
  if (sensorVal == 2){
    LeftTracker = 2;
  }
   else if (sensorVal == 1){
    LeftTracker = 1;
  }
  //Increase the counter to keep track of how many times the program has run through the loop
  counter++;}
//If left sensor found the incorrect pyramid
  if (LeftTracker == 1){
    //Move around the pyramid
      MoveAroundPyramid();
//If left sensor found the correct pyramid
  }else {
  //Code to line the robot up with the pyramid 
  servo_RightMotor.write(ui_Motors_Reverse);
  servo_LeftMotor.write(ui_Motors_Reverse);
  delay (667);
  servo_RightMotor.write(ui_Motors_Stop);
  servo_LeftMotor.write(ui_Motors_Stop);
  delay (100);
  //Tip the pyramid function is called to deliver the cube 
 TipThePyramid();
  }}
  //Reset case counters and increase this case's counter by one to ensure robot is not stuck
  twoCounter++;
  fourCounter = 0;
}}}


/*** Tip The Pyramid Function ***/
//This function is called to tip the pyramid 
void TipThePyramid(){
  //The robot is reversed slightly to line up with the pyramid
  servo_RightMotor.write(ui_Motors_Reverse);
  servo_LeftMotor.write(ui_Motors_Reverse);
  delay (500);
  //Wheels are stopped 
  servo_RightMotor.write(ui_Motors_Stop);
  servo_LeftMotor.write(ui_Motors_Stop);
  delay(100);
  //Pyramid tipper is lowered 
  servo_PyramidTipper.write(pyramidUp);
  delay(2000);
  servo_PyramidTipper.write(pyramidHalf);
  delay(1000);
  servo_PyramidTipper.write(pyramidDown);
  delay(1000);
  //Pyramid is tipped
  servo_PyramidTipper.write(20);
  delay (500);
  servo_PyramidTipper.write(40);
  delay (500);
  servo_PyramidTipper.write(55);
  delay(500);
  servo_PyramidTipper.write(65);
  delay(2000);
  //Cube is dropped 
  servo_OpenCloseClaw.write(Opened1);
  delay (2000);
  //Pyramid is lowered again, on top of the cube
  servo_PyramidTipper.write(pyramidDown);
  //Long delay to end the program 
  delay(30000000000);
}


/*** Move Around Pyramid Function ***/
void  MoveAroundPyramid(){
  //this code ensures the robot drives around the pyramid and lines up with its original path 
    servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Reverse);
    delay(1000);
    servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
    delay(800);
    servo_RightMotor.writeMicroseconds(ui_Motors_Reverse);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Slow_Speed);
    delay(1000);
    servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
    delay(600);
    servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Reverse);
    delay(1000);
    //Stop the wheels
    servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
    delay(100);
}
  

/*** Grab the Cube Function ***/
void GrabTheCube(){
//Counter is reset incase it was used previously
counter = 0;
//Inside a loop to repeat 3 times incase the claw tries to grab the cube but it slips out
while (counter < 3){
  //Only repeat if there is still a cube waiting to be picked up by the claw
  if((ul_Echo_TimeThree>850) &&(ul_Echo_TimeThree <980)){
//Tipper must be lowered halfway so that it is not in the way of the claw
servo_PyramidTipper.write(pyramidHalf);
delay(2000);
//Claw moves over wall and opens
servo_LeftRightClaw.write(OverWall);
delay(2000);
servo_OpenCloseClaw.write(Opened2);
delay(2000);
//Lowers to the wall and closes on the cube
servo_UpDownClaw.write(down);
delay(2000);
servo_OpenCloseClaw.write(Closed);
delay(2000);
//Lifts back up and moves over the slide, remaining closed and holding the cube
servo_UpDownClaw.write(up);
delay(2000);
servo_LeftRightClaw.write(OverSlide);
delay(2000);
//Tippe returns to its original upright position 
servo_PyramidTipper.write(180);
delay(2000);
//Double Checking that the cube was found by pinging the third ultrasonic
delay(300); 
Ping3();} 
//If there is no cube left, increase the counter to ensure that the code continues 
else {
counter++; 
delay(100);
Ping3;}
}}


/*** Function to turn 180 degrees ***/
void FullTurn(int TurnCounter){
  //If turn counter is even, turn right
  if ((TurnCounter%2) == 0){
turnRight180();
  }
//If turn counter is odd, turn left
else {       
 turnLeft180();
  }}


//*** Ping1, Ping2, and Ping3 Functions ***/
//To obtain readings from ultrasonic sensor 1 
void Ping1()
{
  //This ping the ultrasonic sensor to get data from it 
  digitalWrite(ci_Ultrasonic_PingOne, HIGH);
  //The 10 microsecond pause where the pulse in "high"
  //This is the reason why the minimum distance that can be sensed by the sensor is 3cm 
  delayMicroseconds(10);  
  digitalWrite(ci_Ultrasonic_PingOne, LOW);
  //Sets ul_Echo_Time equal to the amount of time in microsecnds that it took for the sound to return 
  ul_Echo_Time = pulseIn(ci_Ultrasonic_DataOne, HIGH, 10000);
}
//To obtain readings from ultrasonic sensor 2
void Ping2(){
  //Ping Ultrasonic and send the Ultrasonic Range Finder a 10 microsecond pulse per tech spec
  digitalWrite(ci_Ultrasonic_PingTwo, HIGH);
  delayMicroseconds(10);  //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ci_Ultrasonic_PingTwo, LOW);
  //use command pulseIn to listen to Ultrasonic_Data pin to record the
  //time that it takes from when the Pin goes HIGH until it goes LOW
  ul_Echo_TimeTwo = pulseIn(ci_Ultrasonic_DataTwo, HIGH, 10000);
}
//To obtain readings from ultrasonic sensor 3  
void Ping3(){
  //Ping Ultrasonic and send the Ultrasonic Range Finder a 10 microsecond pulse per tech spec
  digitalWrite(ci_Ultrasonic_PingThree, HIGH);
  delayMicroseconds(10);  //The 10 microsecond pause where the pulse in "high"
  digitalWrite(ci_Ultrasonic_PingThree, LOW);
  //use command pulseIn to listen to Ultrasonic_Data pin to record the
  //time that it takes from when the Pin goes HIGH until it goes LOW
  ul_Echo_TimeThree = pulseIn(ci_Ultrasonic_DataThree, HIGH, 10000);
}


//*** Function to turn right 180 degrees ***/
void turnRight180(){
 servo_RightMotor.writeMicroseconds(ui_Motors_Reverse);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
    delay(950);
    servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Slow_Speed);
    delay (700);
    servo_RightMotor.writeMicroseconds(ui_Motors_Reverse);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
    delay(950);
    servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
}


//*** Function to turn left 180 degrees ***/
void turnLeft180(){
 servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
 servo_LeftMotor.writeMicroseconds(ui_Motors_Reverse);
 delay(1400);
 servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
 servo_LeftMotor.writeMicroseconds(ui_Motors_Slow_Speed);
 delay (400);
 servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
 servo_LeftMotor.writeMicroseconds(ui_Motors_Reverse);
 delay(1400);
 servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
 servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
 }


//*** Function to determine if a surface is a wall or pyramid ***/
//Returns 0 for a pyramid, and 1 for a wall
 int PyramidOrWall(){
   //Motion is stopped
      servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
      servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
      delay(100); //Time for wheels to stop
      //Robot backs up 
      servo_RightMotor.writeMicroseconds(ui_Motors_Reverse);
      servo_LeftMotor.writeMicroseconds(ui_Motors_Reverse);
      delay(800);
      //The robot turns slightly to see if the front sensor still gets a surface
      servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
      servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
      delay(300);
      //Consult front ultrasonic sensor and infrared sensor
      //Note - Ultrasonic sensor returns 0 for a pyramid, and 1 for no pyramid
      RightSensor = analogRead(A0); 
      Ping1(); 
      //Robot returns to original position
      servo_RightMotor.writeMicroseconds(ui_Motors_Reverse);
      servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
      delay(300);
      //Motion is stopped
      servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
      servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
      delay(100); //Time for wheels to stop 
      //If still sensing a surface, and infrared has not seen a pyramid, then continue to check the other side 
      if ((ul_Echo_Time < 2000) && (RightSensor == 1)){
      //The robot turns slightly to see if the front sensor still gets a surface
      servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
      servo_LeftMotor.writeMicroseconds(ui_Motors_Slow_Speed);
      delay(300);
      //Consult front ultrasonic sensor
      Ping1();
      //Robot returns to original position
      servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
      servo_LeftMotor.writeMicroseconds(ui_Motors_Reverse);
      delay(300);
      //If after the second check a surface is still found, then there was a wall
      if (ul_Echo_Time < 2000){
        //Turn 90 degrees to the left - to position next to wall, with third servo motor over the wall
        //Move forward slightly
        servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
        servo_LeftMotor.writeMicroseconds(ui_Motors_Slow_Speed);
        delay(800);
        //Turn right
        servo_RightMotor.writeMicroseconds(ui_Motors_Slow_Speed);
        servo_LeftMotor.writeMicroseconds(ui_Motors_Reverse);
        delay(1700);
        //Stop motion until section 2 decides what to do 
        servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
        servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
        //This variable will ensure that the first while loop will end because the wall is found
        //The code will now move to the second section - finding the cube
        return(1);
              }         
       //When the second check fails, the surface must have been a pyramid
       //Return 0 to tell the program it was a pyramid 
       else {
         return(0);
               }}                   
        //When the first check fails, the surface must have been a pyramid
        //Return 0 to tell the program it was a pyramid 
       else { 
         return(0);
        }}
