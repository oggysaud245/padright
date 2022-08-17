#include "padrack.h"
padrack rack1,rack2,rack3,rack4,rack5;
byte selectButton, okButton, backButton;
void setup()
{
    Serial.begin(9600);
    pinMode(selectButton, INPUT_PULLUP);
    pinMode(okButton, INPUT_PULLUP);
    pinMode(backButton, INPUT_PULLUP);
}
void loop()
{

}