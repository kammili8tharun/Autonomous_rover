//Made for project in Autonomoues vehicle design under Prof. R.Calhoun at ASU

#include <AFMotor.h>  
#include <NewPing.h>
#include <Servo.h> 

#define SONAR_NUM 4 
#define TRIG_PIN A0 
#define ECHO_PIN A1 
#define MAX_DISTANCE 200 // MAX distance to be measured by a Sonar module
#define MAX_SPEED 135 // set speed of DC  motors
#define MAX_SPEED_OFFSET 20

//Drive mode parameters
int buttonPin = 9;
int buttonRead;
int dt=100;
int buttonNew;
int buttonOld = 1;
int s = -1;
int d1 = 0;
int d2 = 0;
int d3 = 0;
int d21 = 0;

NewPing sonar[SONAR_NUM] = {   // Sensor object array.
  NewPing(TRIG_PIN, A1, MAX_DISTANCE), 
  NewPing(TRIG_PIN, A2, MAX_DISTANCE),
  NewPing(TRIG_PIN, A3, MAX_DISTANCE),
  NewPing(TRIG_PIN, A4, MAX_DISTANCE)
};

//L293D Extension Module Motor pin connection definition
AF_DCMotor motor1(1, MOTOR12_1KHZ); 
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);
Servo myservo;
boolean goesForward=false;
int distance[4] = {100,100,100,100};
int speedSet = 0;

void setup() { // Void Setup function

  Serial.begin(115200);
  myservo.attach(10);  
  myservo.write(115); 
  for (uint8_t i = 0; i < SONAR_NUM; i++) { 
    delay(50); 
    distance[i] = sonar[i].ping_cm();
  }
  pinMode(buttonPin,INPUT);
}

void loop() { // Void loop function
  buttonNew = digitalRead(buttonPin);
  if(buttonOld==0 && buttonNew==1){ //Button variable to switch between drive modes
  s = (s+1)%6;
  }
  Serial.println(buttonNew);
  Serial.println(buttonOld);
  Serial.println(s);
  if(s==0 || s==2 || s==4){ //Neutral Mode for s=0, 2 and 4
    Serial.println("Neutral");
     moveStop();
  }
  if(s==1){ // Parallel parking mode
    Serial.println("Parallel park");
    parallel_park();
    Serial.println(s);
  }

  if(s==3){ // Crab parking mode
    Serial.println("Crab walk park");
    reverse_park();
     Serial.println(s);
  }

  if(s==5){ // Obstacle avoidance mode
    Serial.println("Obstacle avoidance");
    obstacle();
    Serial.println(s);
  }

buttonOld = buttonNew;
delay(dt);
  
 for (uint8_t i = 0; i < SONAR_NUM; i++) { 
    delay(50);
    distance[i] = sonar[i].ping_cm(); //Capturing all the sensor values in a loop
}
}

void obstacle() { // function for obstacle avoidance

 int distanceR = 0;
 int distanceL =  0;
 delay(40);
 Serial.println(distance[0]);
 if(distance[0]<=20)
 {    Serial.println(distance[0]);
      moveStop();
      delay(100);
      moveBackward();
      delay(300);
      moveStop();
      delay(200);
      distanceR = lookRight();
      delay(200);
      distanceL = lookLeft();
      delay(200);
      if(distanceL>=distanceR)
      {
        turnLeft();
        moveStop();
      }else
      {
        turnRight();
        moveStop();
      }
      delay(250);
 }

  if(sonar[0].ping_cm()>25)
  {
   moveForward();
  }

   for (uint8_t i = 0; i < SONAR_NUM; i++) { 
     delay(50); 
     distance[i] = sonar[i].ping_cm(); //Capturing all the sensor values in a loop
   }

}

void parallel_park() { //Function for parallel park
    Serial.println("Parallel Park Mode");
  
    while(sonar[1].ping_cm()<=13 || sonar[2].ping_cm()<=13){

    moveForward();
    }
    d1= sonar[1].ping_cm();
    d2= sonar[2].ping_cm();
    if(d1==0 || d2==0){
      if(d1==0){d1=200;}
      if(d2==0){d2=200;}
    }
    if(d1>13 && d2>13){
      // d11 = sonar[1].ping_cm();
      // d21 = sonar[2].ping_cm();
      // if(d11==0 || d21==0){
      //   if(d11==0){d11=200;}
      //   if(d21==0){d21=200;}
      // }
      // if(d11<=13 && d21<=13){
        delay(700);
        //Serial.println("loop enter");
        moveStop();
        delay(2000);
        turnRight();
        delay(500);
        // d3 = sonar[3].ping_cm();
        while(sonar[3].ping_cm()>22){
          moveBackward();
          delay(250);
        }
        moveStop();
        delay(1000);
        // while(sonar[1].ping_cm()>5 || sonar[2].ping_cm()>5){
        //   delay(250);
           turnLeft();
        // }
        delay(250);
        s=2;
      // }
    }
  }
  

void reverse_park() { //Function for crab walk park
      Serial.println("Crab Park Mode");
    while(sonar[1].ping_cm()<=13 || sonar[2].ping_cm()<=13){
    //Serial.println("Forward");

    moveForward();
    }
    d1= sonar[1].ping_cm();
    d2= sonar[2].ping_cm();
    if(d1==0 || d2==0){
      if(d1==0){d1=200;}
      if(d2==0){d2=200;}
    }
    if(d1>13 && d2>13){
      delay(250);
      moveStop();
      delay(2000);
      while(sonar[1].ping_cm()>=10 && sonar[2].ping_cm()>=10){
      moveLeft();
      Serial.println("Left");
      }
      moveStop();
      delay(1000);
      //while(sonar[3].ping_cm()>5){moveBackward();}
      s=4;
    }
  }

int lookRight()
{
    myservo.write(50); 
    delay(500);
    int dist = sonar[0].ping_cm();
    delay(100);
    myservo.write(115); 
    return dist;
}

int lookLeft()
{
    myservo.write(170); 
    delay(500);
    int dist = sonar[0].ping_cm();
    delay(100);
    myservo.write(115); 
    return dist;
    delay(100);
}

void moveStop() {
  motor1.run(RELEASE); 
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
  } 

void moveLeft(){
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  delay(300);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=5) 
  {
    motor1.setSpeed(speedSet);
    motor2.setSpeed(speedSet);
    motor3.setSpeed(speedSet);
    motor4.setSpeed(speedSet);
    delay(5);
  }
}

void moveRight(){
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  delay(500);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=5) 
  {
    motor1.setSpeed(speedSet);
    motor2.setSpeed(speedSet);
    motor3.setSpeed(speedSet);
    motor4.setSpeed(speedSet);
    delay(5);
  }
}

void moveForward() {
    goesForward=true;
    motor1.run(BACKWARD);      
    motor2.run(BACKWARD);
    motor3.run(BACKWARD); 
    motor4.run(FORWARD);     
    speedSet=MAX_SPEED;
 //  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) 
   // {
    motor1.setSpeed(speedSet);
    motor2.setSpeed(speedSet);
    motor3.setSpeed(speedSet);
    motor4.setSpeed(speedSet);
   // delay(5);
   //}
}

void moveBackward() {
    goesForward=false;
    motor1.run(FORWARD);      
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(BACKWARD);  
    speedSet=MAX_SPEED;   
   // for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2)
    //{
      motor1.setSpeed(speedSet);
      motor2.setSpeed(speedSet);
      motor3.setSpeed(speedSet);
      motor4.setSpeed(speedSet);
   //   delay(5);
  // }
}  

void turnRight() {
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  delay(500);
  motor1.run(BACKWARD);      
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(FORWARD);
} 

void turnLeft() {
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  delay(500);
  motor1.run(BACKWARD);     
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(FORWARD);
}

