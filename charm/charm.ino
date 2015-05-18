#include <SPI.h>
#include <AIR430BoostETSI.h>

#define ADDRESS_LOCAL    0x02
#define ADDRESS_REMOTE   0x03

enum {IDLE=0x01, ACTIVE, ALERT};

unsigned char txData[60] = { 
  "\0" };

// constants won't change. They're used here to 
// set pin numbers:
const int setButton = PUSH1;     // the number of the pushbutton pin
const int releaseButton = PUSH2;
const int ledPin =  GREEN_LED;      // the number of the LED pin
const int redPin = RED_LED;

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);      
  pinMode(redPin, OUTPUT);
  Serial.begin(9600);
  // initialize the pushbutton pin as an input:
  pinMode(setButton, INPUT_PULLUP);     
  pinMode(releaseButton, INPUT_PULLUP);  
  Radio.begin(ADDRESS_LOCAL, CHANNEL_1, POWER_MAX);
  Serial.println("RF start");
  txData[0] = ADDRESS_LOCAL;//set zer byte as our address
}

void loop(){
  //check set button
  if (digitalRead(setButton) == LOW){
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
  delay(200);
}
