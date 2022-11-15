
#include <mcp_can.h>
#include <SPI.h>

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif


// -------- PINOUT --------
// Light pannel
int mdS = A3, sideS = 7, fogS = 6;
int hazS = A4, lrS = A5;
int brakeS = 3, reverseS = 4, oilS = 5;
// Engine pannel
int tempS = A0, tiltS = A1, fuelS = A2;
// CAN
const int SPI_CS_PIN = 9;

// -------- VARIABLES --------
//bool fog, side, haz, left, right, main, dip;
//bool brake, reverse, oil, tiltT, tempH, fuelL;
float temp, tilt, fuel;

boolean fog = false;
boolean side = false;
boolean haz = false;
boolean left = false;
boolean right = false;
boolean mainB = false;
boolean dip = false;
boolean brake = false;
boolean reverse = false;
boolean oil = false;
boolean tiltT = false;
boolean tempH = false;
boolean fuelL = false;

MCP_CAN CAN(SPI_CS_PIN); 
unsigned char eng[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char lights[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char endM[8] = {1, 1, 1, 1, 1, 1, 1, 1};
unsigned char testB[8] = {0, 0, 0, 0, 0, 0, 0, 0};


void setup() {
   Serial.begin(115200); // setting up usb connection
   
   // pin setups
   pinMode(mdS, INPUT);
   pinMode(sideS, INPUT);
   pinMode(fogS, INPUT);
   pinMode(hazS, INPUT);
   pinMode(lrS, INPUT);
   pinMode(brakeS, INPUT);
   pinMode(reverseS, INPUT);
   pinMode(oilS, INPUT);
   pinMode(tempS, INPUT);
   pinMode(tiltS, INPUT);
   pinMode(fuelS, INPUT);

   // test CAN
   if (CAN_OK != CAN.begin(CAN_500KBPS)) { 
        SERIAL.println("CAN - FAIL");
        SERIAL.println("CAN - Retrying ...");
        delay(100);
    }
    else
    {
      SERIAL.println("CAN - OK!");
    }
   
   // usb connection confirmation
   Serial.print("Switch Board Connected !");
   Serial.println();
}


void can()
{
  Serial.println();
  for (int i=0; i < 8; i++)
  {
    Serial.print(eng[i]);
  }
  Serial.println();

  for (int i=0; i < 8; i++)
  {
    Serial.print(lights[i]);
  }
  Serial.println();
}


void canClear()
{
  Serial.println();
  for (int i=0; i < 8; i++)
  {
    eng[i] = 0;
  }
  Serial.println();

  for (int i=0; i < 8; i++)
  {
    lights[i] = 0;
  }
  Serial.println();
}


void readDigital()
{
  

  // Read Digital Switches
  if (digitalRead(sideS))
  {
    side = true;
    lights[2] = 1;
  }
  if (digitalRead(fogS))
  {
    fog = true;
    lights[3] = 1; 
  }
  if (digitalRead(brakeS))
  {
    brake = true;
    lights[4] = 1; 
  }
  if (digitalRead(reverseS))
  {
    reverse = true;
    lights[5] = 1; 
  }
  if (digitalRead(oilS))
  {
    oil = true;
    eng[4] = 1;  
  }
  

}


void readAnalogue() {
  
  // Reading ananlogue pins
//  Serial.println("ind");
//  Serial.println(analogRead(lrS));
//  Serial.println();
//  delay(300);
  temp = analogRead(tempS);
  tilt = analogRead(tiltS);
  int beam = analogRead(mdS);
  int ind = analogRead(lrS);

  
  // Mapping analogue input data
  temp = map(temp, 0, 1023, 0, 200);
  tilt = map(tilt, 0, 1023, 0, 180);

  // updating CAN engine readings
  eng[2] = tilt;
  eng[3] = temp;
  
  // updating variables
  if (temp >= 30) // turning on fan
  {
    eng[5] = 1;    
  }
  
  if (temp >= 100) // over heat
  {
    tempH = true;
    eng[7] = 1;
  }
  
  if (tilt < 10 || tilt > 170)
  {
    tiltT = true;
    // cutting engine
    eng[7] = 1;
  }
  if (beam > 600)
  {
    mainB = true;
    lights[0] = 1;
  }
  if (ind > 200 && ind < 300)
  {
    left = false;
    right = false;
    lights[6] = 0;
    lights[7] = 0;
  }
  if (ind > 300 && ind < 500)
  {
    left = true;
    right = false;
    lights[6] = 1;
  }
  if (ind > 500)
  {
    right = true;
    lights[7] = 1;
  }
  

}


void dataOut() {
  
  // clear screen
  for (int i=0; i<20; i++)
  {
    Serial.println();
  }
  
 // analoge heading 
 Serial.println();
 Serial.println();
 Serial.println();
 Serial.println(" ---------------- ");
 Serial.println(" | ANALOGUE DATA |");
 Serial.println(" ---------------- ");
 Serial.println();
 
  
 // reporting variable data
 Serial.println("| TEMP | TILT |");
 Serial.print("  ");
 Serial.print(temp);
 Serial.print("  ");
 Serial.print(tilt);
 Serial.println();
 
 
 // allert header
 if (tiltT || tempH)
 {
   Serial.println();
   Serial.println();
   Serial.println();
   Serial.println(" --------------- ");
   Serial.println(" | ALLERT DATA |");
   Serial.println(" --------------- ");
   Serial.println();
 }
 
 // reporting variable limits
 // = true;
 if (tiltT)
 {
   Serial.println("BIKE TILT - Fallen Over");
 }
 if (tempH == true)
 {
   Serial.println("ENGINE TEMP - Water Boiling");
 }
 
 
  // lighing header
  Serial.println();
  Serial.println();
  Serial.println();
 Serial.println(" --------------- ");
 Serial.println(" | LIGHTS DATA |");
 Serial.println(" --------------- ");
 Serial.println();
 
 // reporting lights
 if (mainB)
  {
    Serial.println("Main Beam");
  }
  if (fog)
  {
    Serial.println("Fog Light");
  }
  if (brake)
  {
    Serial.println("Brake Light");
  }
  if (reverse)
  {
    Serial.println("Reverse Light");
  }
  if (oil)
  {
    Serial.println("Oil Light");
  }
 
  can();
}


void variableClear() {

// clear CAN buffers
//unsigned char eng[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//unsigned char lights[8] = {0, 0, 0, 0, 0, 0, 0, 0};

 // clear variables
 fog = false;
 side = false;
 haz = false;
 left = false;
 right = false;
 mainB = false;
 dip = false;
 brake = false;
 reverse = false;
 oil = false;
 tiltT = false;
 tempH = false;
 fuelL = false;
  
}


void loop()
{ 
  variableClear();

  canClear();
  
  readDigital();
  readAnalogue();
  
  dataOut();

  CAN.sendMsgBuf(0x09, 0, 8, endM);
  CAN.sendMsgBuf(0x16, 0, 8, endM);
  CAN.sendMsgBuf(0x11, 0, 8, testB);
  CAN.sendMsgBuf(0x16, 0, 8, testB);
  CAN.sendMsgBuf(0x01, 0, 8, lights);
  CAN.sendMsgBuf(0x09, 0, 8, endM);
  CAN.sendMsgBuf(0x16, 0, 8, endM);
  CAN.sendMsgBuf(0x11, 0, 8, testB);
  CAN.sendMsgBuf(0x16, 0, 8, testB);
  CAN.sendMsgBuf(0x00, 0, 8, eng);
  CAN.sendMsgBuf(0x02, 0, 8, endM);
  CAN.sendMsgBuf(0x05, 0, 8, endM);
  CAN.sendMsgBuf(0x09, 0, 8, endM);
  CAN.sendMsgBuf(0x16, 0, 8, endM);
  CAN.sendMsgBuf(0x11, 0, 8, testB);
  CAN.sendMsgBuf(0x16, 0, 8, testB);
  CAN.sendMsgBuf(0x6, 0, 8, testB);
  

}
