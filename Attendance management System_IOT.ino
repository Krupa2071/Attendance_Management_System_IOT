#include <SPI.h>  // Serial Peripheral Interface 
#include <MFRC522.h>  // for MFRC522 and other RFID RC522 based modules.
#include <ESP8266WiFi.h>
//  #include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

// MFRC522 Read and write different types of Radio-Frequency IDentification (RFID) cards on your Arduino using a RC522 based reader connected via the Serial Peripheral Interface (SPI) 
// interface.

constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above


char ssid[] = "AnishG";   // your network SSID (name)
char pass[] = "anish123";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = 1917865;
const char * myWriteAPIKey = "MZY15GMVPPPC3G1K";


MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

String tag,tag1;
int flag=0;

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);    
    }
    Serial.println("\nConnected.");
  }
  
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);
    if(tag=="4127255202" && flag==0)
    {
      flag=1;
      Serial.println("Faculty has granted attendance access to students");
      Serial.println("All present students can mark present by tapping on RFID reader");
    }
    else if(tag=="4127255202" && flag==1)
    {
      Serial.println("Attendance are closed now");
      flag=0;
    }
    else if(flag==1)
    {
      Serial.print(tag);
      Serial.println(" - Mark Present");
      // ThingSpeak.setField(1, tag);
      
      //delay(3000);
      //int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      int x = ThingSpeak.writeField(myChannelNumber, 1, tag, myWriteAPIKey);
      if(x == 200){
        Serial.println("Channel update successful.");
      }
      else{
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
    }
    else
    {
      Serial.println("No Class are started yet, Faculty must Start class by tapping it's card.");
    }
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}
