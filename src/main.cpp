#include <Arduino.h>
#include <SPI.h>
#include <FT81x.h>

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
  FT81x::sendCommand(CMD_RST_PULSE);
  delay(500);

  // ACTIVE
  FT81x::sendCommand(CMD_ACTIVE);
  delay(500);

  dumpChipID();

  delay(100);

  Serial.println("REG_ID");
  Serial.printf("%x\n", FT81x::read8(REG_ID));

  delay(100);

  Serial.printf("REG_HCYCLE %i\n", FT81x::read16(REG_HCYCLE));

  delay(100);

  FT81x::write16(REG_HCYCLE, 548);
  FT81x::write16(REG_HOFFSET, 43);
  FT81x::write16(REG_HSYNC0, 0);
  FT81x::write16(REG_HSYNC1, 41);
  FT81x::write16(REG_VCYCLE, 292);
  FT81x::write16(REG_VOFFSET, 12);
  FT81x::write16(REG_VSYNC0, 0);
  FT81x::write16(REG_VSYNC1, 10);
  FT81x::write8(REG_SWIZZLE, 0);
  FT81x::write8(REG_PCLK_POL, 1);
  FT81x::write8(REG_CSPREAD, 0);
  FT81x::write8(REG_CSPREAD, 0);
  FT81x::write16(REG_HSIZE, 480);
  FT81x::write16(REG_VSIZE, 272);

  delay(100);

  Serial.printf("REG_HCYCLE %i\n", FT81x::read16(REG_HCYCLE));
  Serial.printf("REG_HSIZE %i\n", FT81x::read16(REG_HSIZE));

  Serial.println("Enable display");
  FT81x::write8(REG_PCLK, 5);
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