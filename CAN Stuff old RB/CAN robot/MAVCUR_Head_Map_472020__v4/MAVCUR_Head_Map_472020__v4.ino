

// M.A.V.C.U.R
// Motor And Vision Comunication Unit Robot

// 19 . Jun . 2020

// Auther - Ryan,A,Brown @ CMOSS Electronics

// COMMENTS
//-----------
// Sending left, right, mid through CAN


// -----------------------PINOUT-------------------------
// Sonar pins
int trig = 5;
int echop = 6;
int turret = 3;
int scantop = 4;


// -----------------------Libraries-------------------------
#include <Servo.h>


// -----------------------Variables-------------------------
float cm = 0;
float duration =0;
int pos = 0; 
int posB = 0;
Servo head;
Servo top;
int scanBuf[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char rangeBuf[18]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char mapA[4][18];
unsigned char left[6] = {0,0,0,0,0,0};
unsigned char mid[6] = {0,0,0,0,0,0};
unsigned char right[6] = {0,0,0,0,0,0};
unsigned char ender[6] = {0,0,0,0,0,0};

#include <mcp_can.h>
#include <SPI.h>

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif

const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);  



void setup() {
   Serial.begin(9600); // setting up usb connection
   
   head.attach(turret); 
   top.attach(scantop); 
   
   pinMode(trig, OUTPUT);
   pinMode(echop, INPUT);
   
   //CAN.begin(CAN_500KBPS);

   if (CAN_OK != CAN.begin(CAN_500KBPS)) { 
        SERIAL.println("CAN - FAIL");
        SERIAL.println("CAN - Retrying ...");
        delay(100);
    }
    else
    {
      SERIAL.println("CAN - OK!");
    }
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


void rangeSplit()
{

  Serial.println();
  Serial.println();
  Serial.print("  Left :  ");
  for (int i=0; i<6; i++)
  {
    left[i] = rangeBuf[i];
    Serial.print(left[i]);
  }

  Serial.println();
  Serial.println();
  Serial.print("  Mid :  ");
  for (int i=0; i<6; i++)
  {
    mid[i] = rangeBuf[i+6];
    Serial.print(mid[i]);
  }

  Serial.println();
  Serial.println();
  Serial.print("  Right :  ");
  for (int i=0; i<6; i++)
  {
    right[i] = rangeBuf[i+12];
    Serial.print(right[i]);
  }

  delay(100);
}



void pan()
{
  if (pos == 180)
  {
    pos = 0;
  }
  if (posB == 180)
  {
    posB = 0;
  }
  
  //pos = 90;
  head.write(pos);  
  top.write(posB);
  
  pos += 10;
  posB += 20;
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
  for (int i = 0; i <18; i++)
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

void mapSetup()
{
  for (int i=0; i<4; i++)
  {
    for (int j=0; j<18; j++)
    {
      mapA[i][j] = ' ';
    }
  }
}


void mapUpdate()
{
  for (int i=0; i<4; i++)
  {
    for (int j=0; j<17; j++)
    {
      if (rangeBuf[j] == 4-i)
      {
        mapA[i][j] = '#';
      }
    }
  }
}


void mapShow()
{
  Serial.println(" --------MAP-------- ");
  for (int i=0; i<4; i++)
  {
    Serial.print("[");
    for (int j=0; j<18; j++)
    {
      Serial.print(mapA[i][j]);
    }
    Serial.print("]");
    Serial.println();
  }
}


void loop()
{
  mapSetup();
  
  scan(); 
  dispScan();
  range();
  
  mapUpdate();
  mapShow();

  rangeSplit();

  CAN.sendMsgBuf(10, 0, 6, left);
  CAN.sendMsgBuf(11, 0, 6, mid);
  CAN.sendMsgBuf(12, 0, 6, right);
  
  CAN.sendMsgBuf(100, 0, 6, ender);
  
  Serial.println();
  Serial.println("-----------------------------");
  Serial.println(" VISION DATA DUMP : CAN SENT ");
  Serial.println("-----------------------------");
  Serial.println();
}



// cmoss-electronics.com
