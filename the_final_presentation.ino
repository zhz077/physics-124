#include <AFMotor.h>
#include <Servo.h>
#include <Wire.h>
#include <Math.h>

// 9 is trig and 7 is echo
const int trigPin = 9;
const int echoPin = 7;
Servo hitec;   // instantiate a servo
int deg;   // where is servo (in degrees)
int minDistance;// count how many objects are found
int minDeg;
int curDistance;
int objCounter;
int error =0;
// 80 degree is the front
AF_DCMotor motorL(3); //left 4
AF_DCMotor motorR(4); //right 1

int angle; //right 0; left 180
String inputStr = "";
// COLOR
#define SensorAddressWrite 0x29 //
#define SensorAddressRead 0x29 // 
#define EnableAddress 0xa0 // register address + command bits
#define ATimeAddress 0xa1 // register address + command bits
#define WTimeAddress 0xa3 // register address + command bits
#define ConfigAddress 0xad // register address + command bits
#define ControlAddress 0xaf // register address + command bits
#define IDAddress 0xb2 // register address + command bits
#define ColorAddress 0xb4 // register address + command bits

byte i2cWriteBuffer[10];
byte i2cReadBuffer[10];

void setup(){
// set up interaction
// set up the servo
 hitec.attach(11,650,2281);   

 // set up the laser
 pinMode (2, OUTPUT);


  
 // set up  the ultrasonic sensors
  pinMode(echoPin,INPUT);
  pinMode(trigPin,OUTPUT);


motorL.setSpeed(170);
motorR.setSpeed(175);

// color stuff




Wire.begin();
Serial.begin(9600);  // start serial for output
init_TCS34725();
get_TCS34725ID();     // get the device ID, this is just a test to see if we're connected
objCounter = 0;
}


void loop(){  
//Serial.println("FOUND IT");
objCounter += 1;
// find the object
 locate() ;
/*if(objCounter == 1){
Serial.println("First Object:");
}
else if(objCounter == 2){
Serial.println("Second Object:");
}
else if(objCounter == 3){
Serial.println("Third Object:");
}
// wont go beyond three objs
Serial.print("@");
Serial.print(minDeg);
Serial.print(" degree ");
Serial.print("; ");
Serial.print(minDistance);
Serial.println(" cm");
*/
// go to the object

int curDeg = minDeg;
  if (minDeg >90){
  
       curDeg += 2;
   hitec.write(curDeg);


 motorR.setSpeed(250); //left
 delay(40);
 while (curDeg != 80) {
  motorR.run(BACKWARD);
    delay(47);
    curDeg -= 2;
    hitec.write(curDeg);
    motorR.run(RELEASE);
     delay(100);    
 }

  }
 
 else if (minDeg < 70){ // the object is right,  right stop, left movesfor(int j = 0; j< abs(90-minDeg)/2; j++ ){
while (curDeg != 80) {
  
   motorL.setSpeed(250); 
  motorL.run(BACKWARD);
    delay(45);
    curDeg +=  2;
    hitec.write(curDeg);
    motorL.run(RELEASE);
     delay(100);
    
 }
 }
   delay(1000);



// go forward


 motorL.setSpeed(190); //left
 motorR.setSpeed(225); //right
 delay(50); 
 motorR.run(BACKWARD);
 delay(25);
 motorL.run(BACKWARD); 
 curDistance = sense();

while (curDistance > 20) {

  delay(100);
  curDistance = sense();
}
//delay(abs(minDistance)*6.5/2/3.14/14/14*90/32*700);

motorL.setSpeed(0);
motorR.setSpeed(0);
motorL.run(RELEASE);
motorR.run(RELEASE);

delay(1000);

digitalWrite(2,LOW);

//Serial.print("The color is ");
get_Colors();
//Serial.println("Continue searching?");
/*if (Serial.available()){
  delay (100);
  while(Serial.available()){
    char userA = (char)Serial.read();
    inputStr += userA;
  }
  if(inputStr== "Y"){
  }
   else if(inputStr == "N"){
    Serial.println("FOUND IT!"); 
    while(1); // let's pause here
  }
  inputStr = "";
}
*/
delay(1000);

}


void locate() {
  minDistance = 9000;
  digitalWrite(9,LOW);
  delayMicroseconds(10);
for ( deg =0; deg<= 180; deg = deg+2)
{ 
   hitec.write(deg);
  delay(50);  
  curDistance = sense();
 Serial.print(deg);
  Serial.print(",");
  Serial.print(curDistance);
  Serial.print(".");
 
  if (minDistance >= curDistance){
  minDistance = curDistance;
  minDeg = deg;
  delay(170);
  }
  

}
 Serial.print("*");
  Serial.print(",");
  Serial.print("*");
  Serial.print(".");
  delay(3000); 
/*Serial.print(minDeg);
Serial.print(",");
Serial.print(minDistance);
Serial.print(".");
delay(3000);
 Serial.print("+");
  Serial.print(",");
  Serial.print("+");
  Serial.print(".");*/ 
hitec.write(minDeg);
digitalWrite(2,HIGH); 
delay(100); 

}


float sense(){
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin,LOW);
  float distance = pulseIn(echoPin,HIGH)*0.034/2;
  return distance;
}


/********************************************************************************************/
// Everything about colors
/*  
Send register address and the byte value you want to write the magnetometer and 
loads the destination register with the value you send
*/
void Writei2cRegisters(byte numberbytes, byte command)
{
    byte i = 0;

    Wire.beginTransmission(SensorAddressWrite);   // Send address with Write bit set
    Wire.write(command);                          // Send command, normally the register address 
    for (i=0;i<numberbytes;i++)                       // Send data 
    Wire.write(i2cWriteBuffer[i]);
    Wire.endTransmission();

    delayMicroseconds(100);      // allow some time for bus to settle      
}
/*  
Send register address to this function and it returns byte value
for the magnetometer register's contents 
*/
byte Readi2cRegisters(int numberbytes, byte command)
{
   byte i = 0;

    Wire.beginTransmission(SensorAddressWrite);   // Write address of read to sensor
    Wire.write(command);
    Wire.endTransmission();

    delayMicroseconds(100);      // allow some time for bus to settle      

    Wire.requestFrom(SensorAddressRead,numberbytes);   // read data
    for(i=0;i<numberbytes;i++)
      i2cReadBuffer[i] = Wire.read();
    Wire.endTransmission();   

    delayMicroseconds(100);      // allow some time for bus to settle      
}  
void init_TCS34725(void)
{
  i2cWriteBuffer[0] = 0x10;
  Writei2cRegisters(1,ATimeAddress);    // RGBC timing is 256 - contents x 2.4mS =  
  i2cWriteBuffer[0] = 0x00;
  Writei2cRegisters(1,ConfigAddress);   // Can be used to change the wait time
  i2cWriteBuffer[0] = 0x00;
  Writei2cRegisters(1,ControlAddress);  // RGBC gain control
  i2cWriteBuffer[0] = 0x03;
  Writei2cRegisters(1,EnableAddress);    // enable ADs and oscillator for sensor  
}
 
void get_TCS34725ID(void)
{
  Readi2cRegisters(1,IDAddress);
  if (i2cReadBuffer[0] = 0x44)
  error =0;
    //Serial.println("TCS34725 is present");    
  else
  error=1; 
   // Serial.println("TCS34725 not responding");    
}
/*
Reads the register values for clear, red, green, and blue.
*/
char get_Colors(void)
{
  unsigned int clear_color = 0;
  unsigned int red_color = 0;
  unsigned int green_color = 0;
  unsigned int blue_color = 0;

  Readi2cRegisters(8,ColorAddress);
  clear_color = (unsigned int)(i2cReadBuffer[1]<<8) + (unsigned int)i2cReadBuffer[0];
  red_color = (unsigned int)(i2cReadBuffer[3]<<8) + (unsigned int)i2cReadBuffer[2];
  green_color = (unsigned int)(i2cReadBuffer[5]<<8) + (unsigned int)i2cReadBuffer[4];
  blue_color = (unsigned int)(i2cReadBuffer[7]<<8) + (unsigned int)i2cReadBuffer[6];

  // send register values to the serial monitor 

  Serial.print(clear_color,DEC);
  Serial.print("?");
  Serial.print(red_color, DEC);    
  Serial.print("/");
  Serial.print(green_color, DEC);    
  Serial.print(":");
  Serial.println(blue_color, DEC);
  Serial.print(";");

 // Basic RGB color differentiation can be accomplished by comparing the values and the largest reading will be 
 // the prominent color

 /* if((red_color>blue_color) && (red_color>green_color))
    Serial.println("red");
  else if((green_color>blue_color) && (green_color>red_color))
    Serial.println("green");
  else if((blue_color>red_color) && (blue_color>green_color))
    Serial.println("blue");
  else
    Serial.println("not detectable");
    */

}  
