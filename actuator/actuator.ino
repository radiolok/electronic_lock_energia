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

static unsigned char sensor = PUSH1;
static unsigned char releopen = BLUE_LED;
static unsigned char releclose = GREEN_LED;


// -----------------------------------------------------------------------------
// Main example

void setup()
{
  pinMode(sensor, INPUT_PULLUP);
  pinMode(releopen, OUTPUT);
  pinMode(releclose, OUTPUT);
 
  Radio.begin(ADDRESS_LOCAL, CHANNEL_1, POWER_MAX);

   // Setup serial for debug printing.
  Serial.begin(9600);
  
  txData[0] = ADDRESS_LOCAL;//set zer byte as our address
  pinMode(RED_LED, OUTPUT);       // Use red LED to display message reception
  digitalWrite(RED_LED, LOW);
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
          digitalWrite(releclose, HIGH);
          delay(500);
          digitalWrite(releclose, LOW);
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
          digitalWrite(releopen, HIGH);
          delay(500);
          digitalWrite(releopen, LOW);
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
          digitalWrite(releopen, HIGH);
          delay(500);
          digitalWrite(releopen, LOW);
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
