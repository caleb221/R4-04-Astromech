//==============================================
//            ASTROMECH DROID HEART
//==============================================
//
// Arduino Mega2560-pro 
//
// movement 
//  -2 DC motors
//  -3 Servo Motors
//   
// sensors
// Voice Recognition Module (maybe)
// BMP280
// C02 Sensor
//  
//
// Buzzer
// i2c Screen 1.3''
// i2c interrupt slaved
//  ESP32-CAM is MASTER (webCtrl)


//sensor libraries
#include<BH1750.h>
#include "MHZ19.h"
#include "GY521.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include<Wire.h>
#include <Unistep2.h>

#include <U8g2lib.h>


//L298N DC Motor Pins

#define en1 44 //D49 
#define en2 46//D48

#define in1 50//D50
#define in2 51//D51
#define in3 49//D52
#define in4 48//D53


#define HEART_ADDRESS 0x5F//I2C Adress for HEART

#define BUZZER_PIN 6
//screen settings
//.96'' I2C
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//1.3'' I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define SEALEVELPRESSURE_HPA (1013.25)



const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
//Stepper shoulderRight(stepsPerRevolution, 43, 42, 41, 40);
//Stepper shoulderLeft(stepsPerRevolution, 39, 38, 37, 36);
//Stepper neckStepper(stepsPerRevolution, 35, 34, 33, 32);

Unistep2 shoulderRight(43, 42, 41, 40, 2048, 1000); // pins for IN1, IN2, IN3, IN4, steps per rev, step delay(in micros)
Unistep2 shoulderLeft(39, 38, 37, 36, 2048, 1000);
Unistep2 neckStepper(35, 34, 33, 32,2048,1000);


GY521 gyro(0x68);
Adafruit_BME280 bme280;
//Control Globals (switched on I2C event)
bool hasCmd = false;
static int  usrCmd = 0;

// 0 - NOTHING
// 1 - FORWARD
// 2 - REVERSE
// 3 - RIGHT
// 4 - LEFT
// 5 - HEAD_LEFT
// 6 - HEAD_RIGHT

// Data globals
float humid;
float temp;
float c02;
float pressure;
float imuX;
int gyrox,gyroy,gyroz;

//Voice timers
const long toneInterval;
boolean outputTone = false;
unsigned long previousToneTime;
  int toneOut[8];
  int toneTimes[8];
  int thisNote=0;

// STATE MACHINE CONTROLS
unsigned long currentTime;
unsigned long previousTime;
double elapsedTime;
double lastErr;
double error, totError,rateError;


//PID CONSTANTS
double kp=2;
double ki=5;
double kd=1;
float setPoint;



void setup() {
Serial.begin(9600);
  //i2c setup
  Serial.println("hi!");

/*=======
 * MOTOR TEST
 */

  
  Wire.begin(HEART_ADDRESS);
  Wire.onReceive(getFromHead); // register event
    
  if(!  u8g2.begin()){
//display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
   // for(;;); // Don't proceed, loop forever
  }

//GY521 GYROSCOPE/ACCELEROMETER (only used for gyro)
if(gyro.wakeup()==false)
{
    Serial.print(millis());
    Serial.println("\tCould not connect to GY521");
    delay(1000);
 }
 
//BME 280 TEMP/HUMIDITY
if(!bme280.begin(0x76))
    {
     Serial.print("BME NOT FOUND");
     //while(1);   
    }   
  //

  
  setPoint = 0;
//*/


  //L298N setup
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //buzzer pin
  //pinMode(BUZZER_PIN,OUTPUT);

//init and calibrate


  //noTone(BUZZER_PIN);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

//gyroscope
  gyro.setAccelSensitivity(0);  // 2g
  gyro.setGyroSensitivity(0);   // 250 degrees/s
  gyro.setThrottle();

  // set callibration values from calibration sketch.
  gyro.gxe = 6.208;
  gyro.gye = -0.863;
  gyro.gze = -0.004;

Serial.println("All setup!");

}

void loop() {
  // put your main code here, to run repeatedly:

currentTime = millis();



//Serial.println(thisNote);

//voiceControl(0);

if(hasCmd)
{
  /*
  switch(cmd)
  {
  case 0:
    
  break;  
  default:
  break;  
  }
*/
  

  //int balancer = computePID(gyrox);
  //moveStepper(balancer)

  
}


  
  //readBME();
  //readGyro();
  //screenControl(0);


  float myPID = computePID(gyrox);
  Serial.println(myPID);
//stepperTurn(1,0.1);
//  delay(200);
  
 // Serial.println("SHOULDER FORWARD");
  //stepperTurn(0,30);



  //Serial.println("SHOULDER BACKWARD");

 // stepperTurn(1,10);
  
 //float myPID = computePID(gyrox);
 //Serial.print("\nPID:: ");
 //Serial.print(myPID);
  //delay(200);
  
  
 
  stepperTurn(2,20);



  //stepperTurn(0,2);
//Serial.print("\n\nGYRO X:");
//Serial.print(gyrox);
//Serial.print("\n");
/*
if(thisNote > 8)
{
  voiceControl(0);
}
else
{
  voiceControl(1);
} 
*/

/*
Serial.print("\n\nGYRO X:");
Serial.print(gyrox);
Serial.print("\nGYRO Y:");
Serial.print(gyroy);
Serial.print("\nGYRO Z:");
Serial.print(gyroz);
Serial.print("\nTEMPERATURE: ");
Serial.print(temp);
 Serial.print("\nHUMIDITY:");
Serial.print(humid);
Serial.print("\npressure:");
Serial.print(pressure);
delay(1000);
*/

//wheelTurn(100,0);
//Serial.println("turning? 0");
//delay(1000);

//wheelTurn(0,0);
//delay(1000);
//Serial.println("TURNING 1");
//wheelTurn(150,0);
//delay(1000);




//wheelTurn(200,1);
//delay(1000);



/*
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(en1,150);
  analogWrite(en2,150);

  
  delay(1000);

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);


  digitalWrite(in3, LOW);
  digitalWrite(in4,HIGH);
 analogWrite(en1,200);
  analogWrite(en2,200);
delay(1000);
*/


//delay(1000);

//Serial.println("looped");
}

//========================================================================
//                         COMMAND PROCESSING
//========================================================================


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void getFromHead(int len)
{

  char message [len];
  int i=0;
  while(0 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
    message[i]=c;
    i++;
  }

if(message[0] =='f') //FORWARD
{
  hasCmd = true;
  usrCmd = 1; 
}
else if (message[0] =='l') //LEFT 
{
    hasCmd = true;
    usrCmd = 4;
  
}

else if (message[0] =='r' && len ==0) //RIGHT
{
    hasCmd = true;
    usrCmd = 3;
}
else if (message[0] =='r' && message[1]=='r') //REVERSE
{
    hasCmd = true;
    usrCmd = 2;
}
else if (message[0] =='h' && message[1]=='l') //HEAD LEFT
{
    hasCmd = true;
    usrCmd = 5;
}
else if (message[0] =='h' && message[1]=='r') //HEAD RIGHT
{
    hasCmd = true;
    usrCmd = 6;
}
else //DUNNO, SOMETHING MESSED UP?
{
    message[0]='\0';
    hasCmd = false;
    usrCmd = 0;
  
}
/*  switch statement for 
 *  commands from esp32
 *  figure out a good sending scheme
 *  switch on scheme
 *      case write to functions with proper params
 *       or return data if need be
 */

//send data with:
//Wire.write("data");
  
  //char x = Wire.read();    // receive byte as an integer
  // Serial.println(x);         // print the integer


}

//========================================================================
//                         SENSOR READINGS
//========================================================================
void readGyro()
{
   if( gyro.read() == GY521_OK)
   {
   gyrox = gyro.getGyroX();
   gyroy = gyro.getGyroY();
   gyroz = gyro.getGyroZ(); 
   }
   else
   {
    //TELL THE GYRO TO WAKEUP!
    Serial.println("GYRO FAILED TO READ");
    gyro.wakeup();
    gyro.setAccelSensitivity(0);  // 2g
    gyro.setGyroSensitivity(0);   // 250 degrees/s
    gyro.setThrottle();
    gyro.gxe = 6.208;
    gyro.gye = -0.863;
    gyro.gze = -0.004;
    return;
   }
   
}

void readBME()
{
  temp=bme280.readTemperature();
  humid= bme280.readHumidity();
  pressure=bme280.readPressure();
}



//========================================================================
//                         PID CONTROL
//========================================================================

float computePID(float angleIn)
{
  elapsedTime = (double)(currentTime-previousTime);


  if(elapsedTime < 2000)
  {
    Serial.println("RESETTING PID ERRORS!");
    elapsedTime=0;
    error=0;
    lastErr=0;
    
  } 
  error = setPoint -angleIn;
  totError += error*elapsedTime;
  rateError = (error-lastErr)/elapsedTime;
  float out = kp*error+ki*totError+kd*rateError;
  
 return out; 
 }



//========================================================================
//                         OUTPUT CONTROL
//========================================================================



void screenControl(int screenDisp)
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_tinytim_tf);    
    char buff[15];
      
  u8g2.setCursor(0, 0);
  dtostrf(temp,5,2,buff); 
  u8g2.drawStr(10,20,"Temp: ");
  u8g2.drawStr(40,20,buff);
  u8g2.drawStr(65,20," *C");
  dtostrf(humid,5,2,buff);
  u8g2.drawStr(0,30,"Humidity:");
  u8g2.drawStr(60,30,buff);
  u8g2.drawStr(90,30,"%");
  dtostrf(pressure,5,2,buff);
  u8g2.drawStr(0,40,"Pressure:");
  u8g2.drawStr(60,40,buff);
  u8g2.drawStr(90,40,"hPa");  
 u8g2.drawStr(0,50,"X:");
 dtostrf(gyrox,5,2,buff);
 u8g2.drawStr(10,50,buff);
 u8g2.drawStr(35,50,"Y:");
 dtostrf(gyroy,5,2,buff);
 u8g2.drawStr(42,50,buff);
 u8g2.drawStr(68,50,"Z:");
 dtostrf(gyroz,5,2,buff);
 u8g2.drawStr(75,50,buff);

 
u8g2.sendBuffer();  
   
}

void voiceControl(int voiceOUT)
{
//each 'word' must be be a global number n long
// probably 20, fill the void with 0 s
if(thisNote == 0)
{
  int pSize=0;
  switch(voiceOUT)
  {
   case 0:

int p1[]={ 440, 196, 196, 220, 196, 0, 247, 262};
int pt1[] ={8,8,8,8,8,8,8,8};
    pSize = sizeof(p1)/sizeof(p1[0]);
 for (int i=0;i<pSize;i++)
   {
   toneOut[i]  =p1[i];
    toneTimes[i]=pt1[i];
   } 
  
   break;
   case 1:


int p2[]={ 440, 10, 440, 10, 220, 10,220, 10};
int pt2[] ={8,8,8,8,8,8,8,8};
    pSize = sizeof(p2)/sizeof(p2[0]);
 for (int i=0;i<pSize;i++)
   {
   toneOut[i]  =p2[i];
    toneTimes[i]=pt2[i];
   } 
   
   break;

   default:
int pd[]={ 0, 10, 440, 10, 220, 10,220, 10};
int ptd[] ={8,8,8,8,8,8,8,8};
    pSize = sizeof(pd)/sizeof(pd[0]);
 for (int i=0;i<pSize;i++)
   {
   toneOut[i]  =pd[i];
    toneTimes[i]=ptd[i];
   } 

  
// phraseSelect={0,0,0,0,0,0,0,0};
// phraseTime[]=  {0,0,0,0,0,0,0,0,0,0,0}; 
   break;
  }
}
int noteDuration = 1000 / toneTimes[thisNote];
int pauseBetweenNotes = noteDuration * 1.30;

if(outputTone)
{
    if(currentTime - previousToneTime >= noteDuration)
    {
      previousToneTime = currentTime;
      noTone(BUZZER_PIN);
      outputTone=false;
    }
}
else
{

  if(currentTime - previousToneTime >=pauseBetweenNotes)
  {
    previousToneTime=currentTime;
    tone(BUZZER_PIN,toneOut[thisNote]);
    outputTone = true;
    thisNote +=1;
      if(thisNote >=(sizeof(toneOut)/sizeof(toneOut[0])))
      {
        thisNote=0;
      }
  }
}
  

}



//========================================================================
//                         MOTOR CONTROL
//========================================================================



//STEPPER MOTORS
void stepperTurn(int mSelect,float steps)
{
/*  
 *     // step one revolution  in one direction:
  //Serial.println("clockwise");
  //myStepper.step(stepsPerRevolution);
  delay(500);

  // step one revolution in the other direction:
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
 */
shoulderLeft.run();
shoulderRight.run();
neckStepper.run();

//neckStepper.move(15);

if( shoulderLeft.stepsToGo() ==0 && shoulderRight.stepsToGo() == 0)
{
  switch(mSelect)
  {
    //BACKWARDS
    case 0:
      shoulderLeft.move( steps);
      shoulderRight.move(-steps);
    break;
    case 1://FORWARDS
        shoulderRight.move(steps);
        shoulderLeft.move(-steps);
    break;

    case 2:
    neckStepper.move(steps);
    break;
    case 3:
    neckStepper.move(-steps);
    default:
    break;
  }
}
 
  
}

// DC MOTORS
void wheelTurn(int pwr,int dir)
{
  
// Turn on motor A & B

if(pwr >255)
  pwr=255;
//Serial.println(pwr);  



switch(dir)
{
case 0: //FORWARD
  
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  break;
// REVERSE
 case 1:


  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4,LOW);

  break;
case 2:
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4,HIGH);
  break;
 case 3:
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4,LOW);
  break; 

default:
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  pwr=0;
  
//analogWrite(en1,pwr);
//analogWrite(en2,pwr);
break;

}   


analogWrite(en1,pwr);
analogWrite(en2,pwr);
//turn ON motor control TIMER
}
