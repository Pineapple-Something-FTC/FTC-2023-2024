#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// Define the pins for the PN532 module based on your wiring
#define SDA_PIN 2   // Define the SDA pin
#define SCL_PIN 3   // Define the SCL pin

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1);
  }

  nfc.SAMConfig();
  Serial.println("Waiting for an NFC card...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an NFC card!");

    // Define the block number where you want to write data
    uint8_t blockNumber = 4; // Change this to the desired block number

    // Define the data you want to write to the card
    //
    //
    uint8_t dataToWrite[16] = {
      /*
      NUMS
      0x32, 0x00 = 1
      0x32+x,0x00 = 1+x
        UNTIL 9
        9+x = 0x31,0x(31+x)
      SUITS: Spades(1) Hearts(2) Clubs(3) Diamonds(4)
      0x31 = 1
      0x31+x = 1+x
      */
  0x32,0x00,0x31
};





    uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	  // Start with block 4 (the first block of sector 1) since sector 0
	  // contains the manufacturer data and it's probably better just
	  // to leave it alone unless you know what you're doing
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

    if (success) {
      // Write the data to the specified block
      success = nfc.mifareclassic_WriteDataBlock(blockNumber, dataToWrite);
      if (success) {
        Serial.println("Data written successfully!");
      } else {
        Serial.println("Error writing data to the card.");
      }
    } else {
      Serial.println("Authentication failed.");
    }

    // Halt the card to stop communication
    nfc.mifareclassic_FormatNDEF();
    delay(1000); // Wait a moment before scanning for another card
  }
}
