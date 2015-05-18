#include <SPI.h>
#include <AIR430BoostETSI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information

WiFiClient client;

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 10;   // Maximum number of times the Choreo should be executed
// -----------------------------------------------------------------------------
/**
 *  Defines, enumerations, and structure definitions
 */

#define ADDRESS_LOCAL    0x01

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
enum {
  IDLE=0x01, ACTIVE, ALERT};
unsigned int fsm = IDLE;

// -----------------------------------------------------------------------------
// Main example

void setup()
{
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  Radio.begin(ADDRESS_LOCAL, CHANNEL_1, POWER_MAX);

  // Setup serial for debug printing.
  txData[0] = ADDRESS_LOCAL;//set zer byte as our address

  Serial.begin(9600);

  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID, WPA_PASSWORD);

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } 
    else {
      Serial.println("FAIL");
    }
    delay(5000);
  }
  digitalWrite(YELLOW_LED, HIGH);
  Serial.println("Setup complete.\n");
}
uint8_t length = 0;

void loop()
{
  length = Radio.receiverOn(rxData, sizeof(rxData), 1000);
  if (length > 0){
    Serial.print("Get state: ");
    Serial.println(rxData[1], DEC);

    switch (fsm){
    case IDLE:
      if (rxData[1] == ACTIVE){
        fsm = ACTIVE;
        digitalWrite(GREEN_LED, HIGH);
      }
      break;

    case ACTIVE:
      if (rxData[1] == IDLE){
        fsm = IDLE;
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, LOW);
      }
      if (rxData[1] == ALERT){
        fsm = ALERT;
        digitalWrite(RED_LED, HIGH);
        sendmessage();
      }    
      break;

    case ALERT:
      if (rxData[1] == IDLE){
        fsm = IDLE;
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, LOW);
      }     
      break;
    }
  }
}

void sendmessage(){
  Serial.println("Running SendMessage - Run #" + String(numRuns++));

    TembooChoreo SendMessageChoreo(client);

    // Invoke the Temboo client
    SendMessageChoreo.begin();

    // Set Temboo account credentials
    SendMessageChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendMessageChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendMessageChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String TextValue = "Testing! Testing! 1, 2, 3";
    SendMessageChoreo.addInput("Text", TextValue);
    String APIKeyValue = "000";
    SendMessageChoreo.addInput("APIKey", APIKeyValue);
    String ToValue = "000";
    SendMessageChoreo.addInput("To", ToValue);
    String APISecretValue = "000";
    SendMessageChoreo.addInput("APISecret", APISecretValue);
    String FromValue = "000";
    SendMessageChoreo.addInput("From", FromValue);

    // Identify the Choreo to run
    SendMessageChoreo.setChoreo("/Library/Nexmo/SMS/SendMessage");

    // Run the Choreo; when results are available, print them to serial
    SendMessageChoreo.run();

    while(SendMessageChoreo.available()) {
      char c = SendMessageChoreo.read();
      Serial.print(c);
    }
    SendMessageChoreo.close();
}



