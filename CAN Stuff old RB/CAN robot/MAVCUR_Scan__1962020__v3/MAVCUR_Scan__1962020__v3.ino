
// M.A.V.C.U.R
// Motor And Vision Comunication Unit Robot

// 19 . Jun . 2020

// Auther - Ryan,A,Brown @ CMOSS Electronics

// COMMENTS
//-----------
// Range buffer added


// -----------------------PINOUT-------------------------
// Sonar pins
int trig = 4;
int echop = 5;
int turret = 3;


// -----------------------Libraries-------------------------
#include <Servo.h>


// -----------------------Variables-------------------------
float cm = 0;
float duration =0;
int pos = 0; 
Servo head;
int scanBuf[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int rangeBuf[18]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


void setup() {
   Serial.begin(9600); // setting up usb connection
   
   head.attach(turret); 
   pinMode(trig, OUTPUT);
   pinMode(echop, INPUT);
}

void snap(){
   digitalWrite(trig, LOW);
   delayMicroseconds(2);
   digitalWrite(trig, HIGH);
   delayMicroseconds(10);
   digitalWrite(trig, LOW);
   
   duration = pulseIn(echop, HIGH);
   cm = duration / 29 / 2;
   delay(100);
   
   //Serial.print("SNAP :");
   //Serial.print(cm);
   //Serial.println();
}

void pan()
{
  if (pos == 180)
  {
    pos = 0;
  }
  
  head.write(pos);  
  pos += 10;
  delay(50);     
}

void scan() {
  
  for (int i = 0; i <17; i++)
  {
    snap();
    pan();
    scanBuf[16-i] = cm;
  }

}

void dispScan()
{
  
  Serial.print(" SCAN : [-");
  for (int i = 0; i <17; i++)
  {
    int val = scanBuf[i];
    Serial.print(val);
    Serial.print("-");
  }
  Serial.print("]");
  Serial.println();
}


void range()
{
  Serial.print(" RANGE : [-");
  for (int i = 0; i <17; i++)
  {
    int val = scanBuf[i];
    
    if (val < 5)
    {
      val = 1;
    } else if (val <  10)
    {
      val = 2;
    } else if (val < 15)
    {
      val = 3;
    } else
    {
      val = 4;
    }
    
    rangeBuf[i] = val;
    Serial.print(val);
    Serial.print("-");
  }
  Serial.print("]");
  Serial.println();
}


void loop()
{
  scan(); 
  dispScan();
  range();
  
}



// cmoss-electronics.com

