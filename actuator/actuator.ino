#include <SPI.h>
#include <AIR430BoostETSI.h>

// -----------------------------------------------------------------------------
/**
 *  Defines, enumerations, and structure definitions
 */

#define ADDRESS_LOCAL    0x03
#define ADDRESS_REMOTE   0x01

unsigned char rxData[60] = {
  "\0" };
  
unsigned char txData[60] = {  
  "\0" };
/*
Finite state machine:
1. IDLE mode
2. Action Mode
3. Alert mode
1 -> 2 Received command 0x01
2 -> 1 received command 0x02
2 -> 3 sensor triggered. Send alert
*/
enum {IDLE=0x01, ACTIVE, ALERT};
unsigned int fsm = IDLE;

static unsigned char sensor = 11;
static unsigned char releopen = 9;
static unsigned char releclose = 10;
static unsigned char releopenled = BLUE_LED;
static unsigned char relecloseled = GREEN_LED;

// -----------------------------------------------------------------------------
// Main example

void setup()
{
  pinMode(sensor, INPUT_PULLUP);
  pinMode(releopen, OUTPUT);
  digitalWrite(releopen, HIGH);
  pinMode(releopenled, OUTPUT);
  pinMode(relecloseled, OUTPUT);
  pinMode(releclose, OUTPUT);
  digitalWrite(releclose, HIGH);
  pinMode(12, OUTPUT);
  pinMode(12, LOW); //return pin for sensor
 
  Radio.begin(ADDRESS_LOCAL, CHANNEL_1, POWER_MAX);

   // Setup serial for debug printing.
  Serial.begin(9600);
  
  txData[0] = ADDRESS_LOCAL;//set zer byte as our address
}
uint8_t length = 0;

void loop()
{
  
  switch (fsm){
    case IDLE:
      length = Radio.receiverOn(rxData, sizeof(rxData), 1000);
      if (length > 0){
        if (rxData[1] == ACTIVE){
          //change state stage
          fsm = ACTIVE;
          txData[1] = ACTIVE;//Set command
          txData[2] = 0x00;
          Radio.transmit(ADDRESS_REMOTE, (unsigned char*)&txData, 2); 
          //send 500ms inpuls to actuator
          digitalWrite(releclose, LOW);
          digitalWrite(relecloseled, HIGH);
          delay(500);
          digitalWrite(releclose, HIGH);
          digitalWrite(relecloseled, LOW);
        }
        else{
          fsm = IDLE;
        }
      }
      else{
      }
    break;
    
    case ACTIVE:
      length = Radio.receiverOn(rxData, sizeof(rxData), 100);
      if (length > 0){
        if (rxData[1] == IDLE){
          //change state stage
          fsm = IDLE;
          txData[1] = IDLE;//Set command
          txData[2] = 0x00;
          Radio.transmit(ADDRESS_REMOTE, (unsigned char*)&txData, 2); 
          //send 500ms inpuls to actuator
          digitalWrite(releopen, LOW);
          digitalWrite(releopenled, HIGH);
          delay(500);
          digitalWrite(releopen, HIGH);
          digitalWrite(releopenled, LOW);
        }
        else{
          fsm = ACTIVE;
        }
      }
      else{
      }
         checkperimeter();   
    break;
    
    case ALERT:
      length = Radio.receiverOn(rxData, sizeof(rxData), 1000);
      if (length > 0){
        if (rxData[1] == IDLE){
          //change state stage
          fsm = IDLE;
          txData[1] = IDLE;//Set command
          txData[2] = 0x00;
          Radio.transmit(ADDRESS_REMOTE, (unsigned char*)&txData, 2); 
          //send 500ms inpuls to actuator
          digitalWrite(releopen, LOW);
          digitalWrite(releopenled, HIGH);
          delay(500);
          digitalWrite(releopen, HIGH);
          digitalWrite(releopenled, LOW);
        }
        else{
          fsm = ALERT;
        }
      }
      else{
      }   
    break;
    
    default:
    break;
  }

}

void checkperimeter(){
    uint8_t sensorstate = digitalRead(sensor);
    if (sensorstate == LOW){
      Serial.println("Alert!");
          //alert!!
        fsm = ALERT;
        txData[1] = ALERT;//Set command
        txData[2] = 0x00;
        Radio.transmit(ADDRESS_REMOTE, (unsigned char*)&txData, 2);     
    }
}
