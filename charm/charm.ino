#include <SPI.h>
#include <AIR430BoostETSI.h>

//define addresses. 
#define ADDRESS_LOCAL    0x02
#define ADDRESS_REMOTE   0x03

//FSM states. And commands
enum {
  IDLE=0x01, ACTIVE, ALERT};

unsigned char txData[60] = { 
  "\0" };

// constants won't change. They're used here to 
// set pin numbers:
const int setButton = PUSH1; //set to active mode
const int releaseButton = PUSH2;//set to idle mode
const int ledPin =  GREEN_LED; 
const int redPin = RED_LED;

void setup() {
  //define pinouts and add serial outputr
  pinMode(ledPin, OUTPUT);      
  pinMode(redPin, OUTPUT);
  Serial.begin(9600);

  pinMode(setButton, INPUT_PULLUP);     
  pinMode(releaseButton, INPUT_PULLUP);  
  Radio.begin(ADDRESS_LOCAL, CHANNEL_1, POWER_MAX);
  Serial.println("RF start");
  txData[0] = ADDRESS_LOCAL;//set zer0 byte as our address
}

void loop(){
  //check set button
  if (digitalRead(setButton) == LOW){
    //if button is pressed, send command
    Serial.println("Active mode");
    digitalWrite(redPin, HIGH);
    txData[1] = ACTIVE;//Set command
    txData[2] = 0x00;
    Radio.transmit(ADDRESS_REMOTE, (unsigned char*)&txData, 2);
  }
  else{
    digitalWrite(redPin, LOW);
  }
  //check release button
  if (digitalRead(releaseButton) == LOW){
    Serial.println("IDLE mode");
    digitalWrite(ledPin, HIGH);
    txData[1] = IDLE;//Set command
    txData[2] = 0x00;
    //send packet
    Radio.transmit(ADDRESS_REMOTE, (unsigned char*)&txData, 2);
  }
  else{
    digitalWrite(ledPin, LOW);
  }
  sleepSeconds(1);//Low-power mode delay
  delay(50);//to Clock stabilization
}

