
// M.A.V.C.U.R
// Motor And Vision Comunication Unit Robot

// 19 . Jun . 2020

// Auther - Ryan,A,Brown @ CMOSS Electronics

// COMMENTS
//-----------
// Reading left, right, mid through CAN


#include <mcp_can.h>
#include <SPI.h>

// -----------------------PINOUT-------------------------
// Motor Pins
int la = 3;
int lb = 4;
int ra = 5;
int rb = 6;
// CAN Pins
const int SPI_CS_PIN = 9;

// -----------------------Variables-------------------------
unsigned char canBuf[6] = {0,0,0,0,0,0};

// Directions split
unsigned char left[6] = {0,0,0,0,0,0};
unsigned char mid[6] = {0,0,0,0,0,0};
unsigned char right[6] = {0,0,0,0,0,0};
// Directioned summed
int l=0;
int m=0;
int r=0;

unsigned char len = 0;


#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif

MCP_CAN CAN(SPI_CS_PIN); 


void setup() {
  
  // Set up motor pins
  pinMode(la, OUTPUT);
  pinMode(lb, OUTPUT);
  pinMode(ra, OUTPUT);
  pinMode(rb, OUTPUT);

  // Set up USB connection
  Serial.begin(9600);

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


void mBackwards(int amount)
{

  Serial.println();
  Serial.print("  ----  BACK  ----  ");
  Serial.println();
   
  for (int i=0; i<amount; i++)
  {
    digitalWrite(la, LOW);
    digitalWrite(lb, HIGH);
    digitalWrite(ra, LOW);
    digitalWrite(rb, HIGH);
    delay(50);
  }
}


void mForwards(float amount)
{

  Serial.println();
  Serial.print("  ----  FORW  ----  ");
  Serial.println();
   
  for (int i=0; i<amount; i++)
  {
    digitalWrite(la, HIGH);
    digitalWrite(lb, LOW);
    digitalWrite(ra, HIGH);
    digitalWrite(rb, LOW);
    delay(50);
  }
}


void mLeft(float amount)
{

  Serial.println();
  Serial.print("  ----  LEFT  ----  ");
  Serial.println();
   
  for (int i=0; i<amount; i++)
  {
    digitalWrite(la, LOW);
    digitalWrite(lb, HIGH);
    digitalWrite(ra, HIGH);
    digitalWrite(rb, LOW);
    delay(50);
  }
}


void mRight(int amount)
{

  Serial.println();
  Serial.print("  ----  RIGHT  ----  ");
  Serial.println();
   
  for (int i=0; i<amount; i++)
  {
    digitalWrite(la, HIGH);
    digitalWrite(lb, LOW);
    digitalWrite(ra, LOW);
    digitalWrite(rb, HIGH);
    delay(50);
  }
}


void nav()
{
  if((l+m+r) == 18)
  {
    mBackwards(4);
    mRight(18); // 5*18 = 90 degreas
  }

  if(m >= l)
  {
    if(m > r)
    {
      mForwards(4);
    }
    else 
    {
      mRight(4);
    }
  }
  else
  {
    mLeft(4);
  }
  
}


void canRead()
{
  if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
    CAN.readMsgBuf(&len, canBuf);
    unsigned long canId = 0; //CAN.getCanId();    
    canId = 1;//CAN.getCanId();
  
    
    Serial.println();
    Serial.print("  Left :  ");
    Serial.println(canId);
    if (canId == 0)
    {
      for (int i = 0; i < 6; i++) {
        left[i] = canBuf[i];
        SERIAL.print(left[i]);
      }
    }

    Serial.println();
    Serial.println();
    Serial.print("  Mid :  ");
    if (canId == 1)
    {
      for (int i = 0; i < 6; i++) {
        mid[i] = canBuf[i];
        SERIAL.print(mid[i]);
        SERIAL.print(mid[i]);
      }
    }

    Serial.println();
    Serial.println();
    Serial.print("  Right :  ");
    if (canId == 2)
    {
      for (int i = 0; i < 6; i++) {
        right[i] = canBuf[i];
        SERIAL.print(right[i]);
      }
    }

    // End of data read
  }

  delay(10);
}


void loop() {

  canRead();

  //rangeDisp();

  nav();

  delay(50);
}



// www.cmoss-electronics.com
