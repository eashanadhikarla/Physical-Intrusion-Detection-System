#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information

byte ethernetMACAddress[] = ETHERNET_SHIELD_MAC;
EthernetClient client;

// The number of times to trigger the action if the condition is met
// We limit this so you won't use all of your Temboo calls while testing
int maxCalls = 10;

// The number of times this Choreo has been run so far in this sketch
int calls = 0;

int inputPin = 2;
int outputPin = 8;

void setup() {
  Serial.begin(9600);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);

  Serial.print("DHCP:");
  if (Ethernet.begin(ethernetMACAddress) == 0) {
    Serial.println("FAIL");
    while(true);
  }
  Serial.println("OK");
  delay(5000);

  // Initialize pins
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);

  Serial.println("Setup complete.\n");
}

void loop() {
  int sensorValue = digitalRead(inputPin);
  Serial.println("Sensor: " + String(sensorValue));

  if (sensorValue == HIGH) {
    if (calls < maxCalls) {
      Serial.println("\nTriggered! Calling TextToSpeech Choreo...");
      runTextToSpeech(sensorValue);
      calls++;
    } else {
      Serial.println("\nTriggered! Skipping to save Temboo calls. Adjust maxCalls as required.");
    }
  }
  delay(250);
}

void runTextToSpeech(int sensorValue) {
  TembooChoreo TextToSpeechChoreo(client);

  // Set Temboo account credentials
  TextToSpeechChoreo.setAccountName(TEMBOO_ACCOUNT);
  TextToSpeechChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  TextToSpeechChoreo.setAppKey(TEMBOO_APP_KEY);

  // Set Choreo inputs
  String APIKeyValue = "13e6ab28";
  TextToSpeechChoreo.addInput("APIKey", APIKeyValue);
  String TextValue = "someone entered your house";
  TextToSpeechChoreo.addInput("Text", TextValue);
  String ToValue = "918602165884";
  TextToSpeechChoreo.addInput("To", ToValue);
  String APISecretValue = "628aad7fddff8fb2";
  TextToSpeechChoreo.addInput("APISecret", APISecretValue);

  // Identify the Choreo to run
  TextToSpeechChoreo.setChoreo("/Library/Nexmo/Voice/TextToSpeech");

  // Run the Choreo
  unsigned int returnCode = TextToSpeechChoreo.run();

  // A return code of zero means everything worked
  if (returnCode == 0) {
    digitalWrite(outputPin, HIGH);
  }

  TextToSpeechChoreo.close();
}