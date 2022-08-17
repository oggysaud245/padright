#include "padrack.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "OneButton.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);
padrack rack1, rack2, rack3, rack4, rack5;
OneButton selectButton(A2, true,true);
OneButton okButton(A2, true,true);
OneButton backButton(A2, true,true);

byte arrow[8] = {
    0b00000,
    0b11100,
    0b10010,
    0b01001,
    0b01001,
    0b10010,
    0b11100,
    0b00000};
void setup()
{
    Serial.begin(9600);

}
void loop()
{
}