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

unsigned long time1 = 0;
unsigned long time2 = 0;


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
int counter3 = 0;
int Foundit = 0;
bool RightSensor = analogRead(A0); 


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

Servo servo_PyramidTipper;

//Setup the pyramid Tipper
const int pyramidLoc = 13;
const int pyramidDown = 20;
const int pyramidHalf = 140;
const int pyramidUp = 180;

//Additional Variables
int foundCube = 1;
int counter = 1;
int foundWall = 1;
int TurnCounter = 1;

int sixcounter = 0;
int twocounter = 0;
int threecounter = 0;
int fourcounter = 0;
int fivecounter = 0;

//Some additional functions
void TurnCorner();
void GrabTheCube();
void TipThePyramid();
void FullTurn(int TurnCounter);
void turnLeft180();
void turnLeft180();


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

//Setup the pyramid tipper motor
pinMode(pyramidLoc, OUTPUT);
servo_PyramidTipper.attach(pyramidLoc);
 


}


//Main program 
void loop(){

//Serial.print("*** STAGE 1 - FIND WALL ***\n\n");
//Stage 1 - Find the wall so that the robot can begin looking for the cube 
while (foundWall == 0){
  delay(100);
  //Using only the front ultrasonic 
  Ping1(); Ping2();
  Serial.print("First Tracker: "); Serial.print(ul_Echo_Time); Serial.print("\n");
  Serial.print("Second Tracker: "); Serial.print(ul_Echo_TimeTwo); Serial.print("\n");
  
  //CASE 1.1 - If somehow close enough to a wall to go on to the next stage
  if ((ul_Echo_TimeTwo < 400) && (ul_Echo_TimeTwo!=0)){
    Serial.print("CASE 1.1 - Close to the wall, moving on to stage 2\n\n");
    foundWall = 1;
    servo_RightMotor.writeMicroseconds(1500);
    servo_LeftMotor.writeMicroseconds(1500);
    delay(100);
  }
  //CASE 1.2 - Keep driving straight until a wall is found 
  else if ((ul_Echo_Time > 600) || (ul_Echo_Time == 0))  {
    Serial.print("CASE 1.2 - Looking for the wall\n\n");
    servo_RightMotor.writeMicroseconds(ui_Motors_Top_Speed);
    servo_LeftMotor.writeMicroseconds(ui_Motors_Top_Speed);
  } 
  //CASE 1.3 - If a wall is found 
  else {
      Serial.print("CASE 1.3 - Think we found the wall, double checking\n");
      servo_RightMotor.writeMicroseconds(1500);
      servo_LeftMotor.writeMicroseconds(1500);
      delay(2000);
          servo_RightMotor.writeMicroseconds(1300);
      servo_LeftMotor.writeMicroseconds(1300);
      delay(800);
      //Check side 1
      Serial.print("Check Side 1\n");
      servo_RightMotor.writeMicroseconds(1800);
      servo_LeftMotor.writeMicroseconds(1500);
      delay(1000);
      Ping1();
        servo_RightMotor.writeMicroseconds(1200);
      servo_LeftMotor.writeMicroseconds(1500);
      delay(1000);
             servo_RightMotor.writeMicroseconds(1500);
      servo_LeftMotor.writeMicroseconds(1500);
      delay(1000);
      RightSensor = analogRead(A0); 
      if ((ul_Echo_Time < 2000) && (RightSensor == 1)){
        //Side 1 pass, checking side 2
         Serial.print("Check Side 2\n");
      servo_RightMotor.writeMicroseconds(1500);
      servo_LeftMotor.writeMicroseconds(1800);
      delay(1000);
      Ping1();
      servo_RightMotor.writeMicroseconds(1500);
      servo_LeftMotor.writeMicroseconds(1200);
      delay(1000);
      
        if ((ul_Echo_Time < 2000) && (RightSensor == 1)){
          //CASE 1.3a - Found the wall, move on to next stage
          Serial.print("CASE 1.3a - Found a wall, moving on to stage 2 after turning a corner\n\n");
             foundWall = 1;
             servo_RightMotor.writeMicroseconds(1700);
    servo_LeftMotor.writeMicroseconds(1700);
    delay(1000);
              servo_RightMotor.writeMicroseconds(1700);
    servo_LeftMotor.writeMicroseconds(1300);
    delay(3300); //2300 is 90
    servo_RightMotor.writeMicroseconds(1500);
    servo_LeftMotor.writeMicroseconds(1500);
              }
               else {
                 Serial.print("CASE 1.3c - Not a wall, failed on side two. Moving around pyramid and restarting stage 1.\n\n");
             MoveAroundPyramid();
               }}          
       else {
              Serial.print("CASE 1.3b - Not a wall, failed on side one. Moving around pyramid and restarting stage 1.\n\n");
       MoveAroundPyramid();
        }}}

 
//Serial.print("*** STAGE 2 - FIND CUBE ***\n\n");
//Stage 2 - Looking for the cube
 while ((foundCube == 0) && (foundWall == 1)){ 

 if (fivecounter > 10 || twocounter > 50 || threecounter > 50 || fourcounter >50 || sixcounter > 50){

 Serial.print("SPECIAL CASE - He's stuck \n\n");
  servo_RightMotor.write(2200);
  servo_LeftMotor.write(2200);
  delay (2000);
  servo_RightMotor.write(2300);
  servo_LeftMotor.write(1000);
  delay (2000);
  servo_RightMotor.write(1000);
  servo_LeftMotor.write(2300);
  delay (2000);
  servo_RightMotor.write(1500);
  servo_LeftMotor.write(1500);
  delay(1000);
  
 }



  
   //Activate all three ultrasonic sensors and determine their readings 
   Ping3(); Ping2(); Ping1();
 //  delay(100);
   Serial.print("TWO First Tracker: "); Serial.print(ul_Echo_Time); Serial.print("\n");
   Serial.print("Second Tracker: "); Serial.print(ul_Echo_TimeTwo); Serial.print("\n");
   Serial.print("Third Tracker: "); Serial.print(ul_Echo_TimeThree); Serial.print("\n");


//Case 2.1 - Found the cube
if ((ul_Echo_TimeThree>100) &&(ul_Echo_TimeThree <980)){
  Serial.print ("Case 2.1 - Found The Cube \n\n");  
servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);

Ping3();
while (ul_Echo_TimeThree <400){
  servo_RightMotor.writeMicroseconds(1800);
servo_LeftMotor.writeMicroseconds(1800);
delay(10);
  Ping3();
}
while (ul_Echo_TimeThree > 1200){
 servo_RightMotor.writeMicroseconds(1300);
servo_LeftMotor.writeMicroseconds(1300);
delay(10);
  Ping3();
}


servo_RightMotor.writeMicroseconds(ui_Motors_Stop);
servo_LeftMotor.writeMicroseconds(ui_Motors_Stop);
 //GET CUBE::::
 servo_PyramidTipper.write(0);
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
 servo_PyramidTipper.write(180);
delay(2000);
foundCube = 1;
//Double Checking that the cube was found
delay(300); Ping3();
if((ul_Echo_TimeThree>850) &&(ul_Echo_TimeThree <980)){
  foundCube = 0;
}
}

//Case 2.5 - At a corner, turn around the corner 
else if (ul_Echo_Time <=600 && (ul_Echo_Time != 0)){
  Serial.print("Case 2.5 - Turn Corner\n\n");
  servo_RightMotor.writeMicroseconds(2050);
    servo_LeftMotor.writeMicroseconds(1300);
    delay(1300);
   servo_RightMotor.writeMicroseconds(1500);
   servo_LeftMotor.writeMicroseconds(1500);
   delay(100);

 fivecounter ++;  
 twocounter = 0;  
 threecounter =0;
 fourcounter = 0;
 sixcounter = 0;

}
//Case 2.2 - Straight forward - right distance from the wall and not turning 
else if (((ul_Echo_Time > 600) || (ul_Echo_Time== 0) ) && (ul_Echo_TimeTwo > 220) && (ul_Echo_TimeTwo < 310)){
  Serial.print ("Case 2.2 - Drive Straight\n\n");
  servo_RightMotor.writeMicroseconds(2050);
  servo_LeftMotor.writeMicroseconds(2050);
 twocounter++;
 threecounter =0;
 fourcounter = 0;
 fivecounter = 0;
 sixcounter = 0;
}

//Case 2.3 - Too far from wall, turn in towards wall
else if ((ul_Echo_TimeTwo > 310)){
  Serial.print("Case 2.3 - Too far from wall, move in\n\n");
  servo_RightMotor.writeMicroseconds(1600);
  servo_LeftMotor.writeMicroseconds(2050);

   threecounter ++;
 twocounter = 0;  
 fourcounter = 0;
 fivecounter = 0;
 sixcounter = 0;
}

//Case 2.4 - Too close to wall, turn away from wall 
else if ((ul_Echo_TimeTwo < 220)){
  Serial.print("Case 2.4 - Too Close, move away from wall\n\n");
   servo_RightMotor.writeMicroseconds(2050);
   servo_LeftMotor.writeMicroseconds(1600);
    fourcounter ++;
 twocounter = 0;  
 threecounter =0;
 fivecounter = 0;
 sixcounter = 0;
}

//Case 2.6 - Nowhere to be found 
else if (ul_Echo_TimeTwo > 1500){
 servo_RightMotor.writeMicroseconds(1500);
   servo_LeftMotor.writeMicroseconds(1500);
   delay(200);
  Ping2();
  if(ul_Echo_TimeTwo >1500) {
    Serial.print(" CASE 2.6 - Robot is lost, going back to stage 1 to find the wall \n\n");
    foundWall = 0;    }

 sixcounter ++;
 twocounter = 0;  
 threecounter =0;
 fourcounter = 0;
 fivecounter = 0;

}

}


  
    
//Serial.print("*** STAGE 3 - DELIVER CUBE ***\n\n");
byte sensorVal = Serial.read();
int LeftSensor = sensorVal; //Converts byte to integer

while (foundCube == 1){
    Ping1();
    sensorVal = Serial.read();
    LeftSensor = sensorVal; //Converts byte to integer
    Serial.print("Stage Three - First Tracker: "); Serial.print(ul_Echo_Time); Serial.print("\n");
    Serial.print("Infrared Reading - "); Serial.print(LeftSensor); Serial.print("\n\n");
    delay(300);


    
//CASE 3.3 - Left InfraRed sees the right pyramid
if (sensorVal == 1){
 Serial.print("Case 3.3 - Left infrared right pyramid\n\n"); 
 TipThePyramid();

}


//CASE 3.4 - Left InfraRed sees the right pyramid
else if (sensorVal == 2){
 Serial.print("Case 3.4 - Left infrared wrong pyramid\n\n");
 MoveAroundPyramid(); 

}
 

//Case 3.1 - Drive straight
else if ( (ul_Echo_Time > 600)){
  Serial.print ("Case 3.1 DRIVE STRAIGHT\n\n");
  servo_RightMotor.writeMicroseconds(2050);
  servo_LeftMotor.writeMicroseconds(2050);
} 

//Case 3.2 - Found a surface from ultrasonic
else if ( (ul_Echo_Time < 600)  && (ul_Echo_Time !=0)) {
//Determine if there is a pyramid, and if it's the right one
      servo_RightMotor.writeMicroseconds(1500);
      servo_LeftMotor.writeMicroseconds(1500);
      delay(100);
      
int RightChecker = 0;
int counter7 = 0;

while (counter7<20){
  RightSensor = analogRead(A0); 
  counter7++;
  delay(100);
  Serial.print("CHECK RIGHT FOR PYRAMID\n");
  if (RightSensor == 0){
    RightChecker = 1;
  }
}


if (RightChecker == 0){
 Serial.print("Case 3.2a - it's a wall\n\n");
 FullTurn(TurnCounter);
  TurnCounter++;
  
} else {
  
 Serial.print("Case 3.2b - it's a pyramid, checking which one\n\n");

while (counter3<100){
  sensorVal = Serial.read();
  delay(10);
  servo_RightMotor.writeMicroseconds(1300);
  servo_LeftMotor.writeMicroseconds(1700);
  if (sensorVal == 2){
     Serial.print("Wrong Pyramid\n");
  MoveAroundPyramid();
  counter3 = 101;
  }
  if (sensorVal == 1){
     Serial.print("Right Pyramid\n");
 TipThePyramid();
 counter3 = 101;
  }
counter3++;
}
  servo_RightMotor.writeMicroseconds(1500);
  servo_LeftMotor.writeMicroseconds(1500);

}}

}}


void TipThePyramid(){
  servo_PyramidTipper.write(180);
  delay(2000);
  servo_PyramidTipper.write(100);
  delay(1000);
  servo_PyramidTipper.write(10);
  servo_RightMotor.write(1200);
  servo_LeftMotor.write(1200);
  delay(1000);
  servo_RightMotor.write(1500);
  servo_LeftMotor.write(1500);
  delay (500);
  servo_PyramidTipper.write(60);
  servo_OpenCloseClaw.write(Opened1);
  delay (1000);
  servo_PyramidTipper.write(10);
  delay(1000);
}


void  MoveAroundPyramid(){
    servo_RightMotor.writeMicroseconds(1800);
    servo_LeftMotor.writeMicroseconds(1300);
    delay(1800);
    servo_RightMotor.writeMicroseconds(2200);
    servo_LeftMotor.writeMicroseconds(2200);
    delay(1000);
    servo_RightMotor.writeMicroseconds(1300);
    servo_LeftMotor.writeMicroseconds(1800);
    delay(1800);
    servo_RightMotor.writeMicroseconds(2200);
    servo_LeftMotor.writeMicroseconds(2200);
    delay(1000);
    servo_RightMotor.writeMicroseconds(1300);
    servo_LeftMotor.writeMicroseconds(1800);
    delay(1800);
    servo_RightMotor.writeMicroseconds(2200);
    servo_LeftMotor.writeMicroseconds(2200);
    delay(1000);
    servo_RightMotor.writeMicroseconds(1800);
    servo_LeftMotor.writeMicroseconds(1300);
    delay(1800);
    servo_RightMotor.writeMicroseconds(1500);
    servo_LeftMotor.writeMicroseconds(1500);
    delay(1000);
}
  
  
void GrabTheCube(){
  
  servo_PyramidTipper.write(100);
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
 servo_PyramidTipper.write(180);
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
     Serial.print("180 DEGREE TURN WOOHOO \n\n");
  if ((TurnCounter%2) == 0){
 //ODD so turn LEFT
turnRight180();
  } else {
    //EVEN so turn RIGHT
       
 turnLeft180();
    
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


void turnRight180(){
 servo_RightMotor.writeMicroseconds(1300);
    servo_LeftMotor.writeMicroseconds(2050);
    delay(1400);
    servo_RightMotor.writeMicroseconds(1700);
    servo_LeftMotor.writeMicroseconds(1700);
    delay (700);
 servo_RightMotor.writeMicroseconds(1300);
    servo_LeftMotor.writeMicroseconds(2050);
    delay(1400);
    servo_RightMotor.writeMicroseconds(1500);
     servo_LeftMotor.writeMicroseconds(1500);
}

void turnLeft180(){
 servo_RightMotor.writeMicroseconds(2050);
    servo_LeftMotor.writeMicroseconds(1300);
    delay(1400);
    servo_RightMotor.writeMicroseconds(1700);
    servo_LeftMotor.writeMicroseconds(1700);
    delay (400);
 servo_RightMotor.writeMicroseconds(2050);
    servo_LeftMotor.writeMicroseconds(1300);
    delay(1400);
    servo_RightMotor.writeMicroseconds(1500);
     servo_LeftMotor.writeMicroseconds(1500);
 }
