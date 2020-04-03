#include <Arduino.h>
#include <SPI.h>

#define SPI_SETTINGS SPISettings(1, MSBFIRST, SPI_MODE0)

void waitForKeyPress();
void dumpChipID();

void setup() {
  Serial.begin(9600);

  SPI.begin();
  //SPI.setCS(SS);

  pinMode(SS, OUTPUT); 
  digitalWrite(SS, HIGH);
  delay(500);

  waitForKeyPress();

  // RESET
  digitalWrite(SS, LOW);
  SPI.beginTransaction(SPI_SETTINGS);
  SPI.transfer(0x68);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.endTransaction();
  digitalWrite(SS, HIGH);
  delay(500);

  // ACTIVE
  digitalWrite(SS, LOW);
  SPI.beginTransaction(SPI_SETTINGS);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.endTransaction();
  digitalWrite(SS, HIGH);
  delay(500);

  dumpChipID();
}

void loop() {
}

void waitForKeyPress()
{
  Serial.println("\nPress a key to continue\n");
  while(!Serial.available());
  while(Serial.available())
  {
    Serial.read();
  }
}

void dumpChipID() {
  digitalWrite(SS, LOW);
  SPI.beginTransaction(SPI_SETTINGS);
  SPI.transfer(0x0C);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00); // dummy byte
  Serial.println("Chip ID:");
  Serial.printf("0x0C0000: %x (supposed to be 0x8)\n", SPI.transfer(0x00));
  Serial.printf("0x0C0001: %x (supposed to be 0x12)\n", SPI.transfer(0x00));
  Serial.printf("0x0C0002: %x (supposed to be 0x1)\n", SPI.transfer(0x00));
  Serial.printf("0x0C0003: %x (supposed to be 0x0)\n", SPI.transfer(0x00));
  Serial.println("");
  SPI.endTransaction();
  digitalWrite(SS, HIGH);
}