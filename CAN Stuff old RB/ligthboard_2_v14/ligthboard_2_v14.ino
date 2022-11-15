
// Libraries 
//-----------
#include <SPI.h>
#include "mcp_can.h"

// CAN Definitions
//-------------------
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif

const int SPI_CS_PIN = 9; // CAN selectpin
MCP_CAN CAN(SPI_CS_PIN);   

// CAN Variables
//-------------------
const int messages = 11; // Ten Messages Readable
unsigned char len[messages+1];
unsigned long canId[messages+1];
unsigned char buf[messages+1][10];
int discCounter = 0; // Disconnect counter

// lights
int mainB = 7;
int brake = 6;
int reverse = 5;
int left = 3;
int right = 4;

// Engine Pins
int cut = 10;
int fan = 8;

//timing
unsigned long milliNow = 0;
unsigned long miliOldL = 0;
unsigned long miliOldR = 0;
bool leftState = false;
bool rightState = false;

void setup() {
  SERIAL.begin(115200); // USB speed

  // Declaire outputs
  pinMode(mainB, OUTPUT);
  pinMode(brake, OUTPUT);
  pinMode(reverse, OUTPUT);
  pinMode(left, OUTPUT);
  pinMode(right, OUTPUT);
  pinMode(cut, OUTPUT);
  pinMode(fan, OUTPUT);

  // Verify connection to can unit
  while (CAN_OK != CAN.begin(CAN_500KBPS)) { 
    SERIAL.println("CAN - FAIL");
    SERIAL.println("CAN - Retrying ...");
    // Turn fan on to show error
    digitalWrite(fan, false);
    
    delay(100);
  }
  // Turn Fan off
  digitalWrite(fan, true);
  SERIAL.println("CAN - OK!");
  SERIAL.println("");
  SERIAL.println("");

  // Disable cut / alarm sound
  digitalWrite(cut, true);
}


void readCan()
{
  int readCount = 0;
  for(readCount = 0; readCount < messages;)
  {

    if (CAN_MSGAVAIL == CAN.checkReceive()) {  
      readCount += 1; // update counter to show read
      int i = readCount;
      
      // read current message
      CAN.readMsgBuf(&len[i], buf[i]); 
      canId[i] = CAN.getCanId();
      
    }
  }
}


void dispCan(){

  for(int i = 0; i < messages; i++)
  {
    
    // Print Descriptopn
    if (canId[i] == 0)
    {
      SERIAL.print(" ENGINE ");
    }
    else if (canId[i] == 1)
    {
      SERIAL.print(" LIGHTS ");
    }
    else
    {
      SERIAL.print(" OTHER  ");
    }

    // Print CAN ID
    SERIAL.print("|  ");
    SERIAL.print(canId[i]);
    
    // Print Data
    SERIAL.print("  |  ");
    for (int j = 0; j < len[i]; j++) {
        SERIAL.print(buf[i][j], HEX);
    }
    SERIAL.print("  |  ");
    SERIAL.print(len[i]);
    
    SERIAL.print("  |  @");
    SERIAL.println(millis());
    SERIAL.println();
  }
  
  SERIAL.println("----------------------------------------");
  //delay(200);
}


// Lighting Functions
//-----------------------------------------

void leftBlink() {

  if ((milliNow - miliOldL) > 600)
  {
    miliOldL = millis();
    leftState = !leftState;
  }
  
  digitalWrite(left, leftState);
}

void rightBlink() {

  if ((milliNow - miliOldR) > 600)
  {
    miliOldR = millis();
    rightState = !rightState;
  }
  
  digitalWrite(right, rightState);
}

void hazzards() {

  leftBlink();
  rightBlink();
}





// Analysis Functions
//-----------------------------------------

void analysis()
{
  for(int i = 0; i < messages; i++)
  {
    if (canId[i] == 0)
    {
      if(len[i] > 0)
      {
        if (buf[i][5] == 1){
          digitalWrite(fan, false);
        } else {
          digitalWrite(fan, true);
        }
        if (buf[i][7] == 1){
          digitalWrite(cut, false);
          SERIAL.print(" CUT ! ");
        } else {
          digitalWrite(cut, true);
        }
      }
    }
    if (canId[i] == 1)
    {
      if (buf[i][0] == 0){
        digitalWrite(mainB, true);
      } else {
        digitalWrite(mainB, false);
      }
      if (buf[i][2] == 1){
        digitalWrite(mainB, true);
      } else {
        digitalWrite(mainB, false);
      }
      if (buf[i][4] == 1){
        digitalWrite(brake, true);
      } else {
        digitalWrite(brake, false);
      }
      if (buf[i][5] == 1){
        digitalWrite(reverse, true);
      } else {
        digitalWrite(reverse, false);
      }
      if (buf[i][6] == 0){
        digitalWrite(left, true);
      } else {
        //digitalWrite(left, false);
        leftBlink();
      }
      if (buf[i][7] == 0){
        digitalWrite(right, true);
      } else {
        //digitalWrite(right, false);
        rightBlink();
      }
    }
  }
}






// Main Loop
//-----------------------------------------

void loop() {

  milliNow = millis();

  readCan();
  dispCan();

  analysis();
  
}






// Ryan Brown @ www.cmoss-electronics.com
