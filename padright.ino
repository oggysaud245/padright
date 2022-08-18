#include "padrack.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "OneButton.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
padrack rack1, rack2, rack3, rack4, rack5;

bool change = false;
bool isUpdate = false;
byte menuButton = 3;
byte selectButton = 4;

int settingVariable = 0;
uint32_t previousTime = millis();
int topMenuPosition = 0;
int state = 0;
bool makeChange = false;
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
    lcd.init(); // initialize the lcd
    lcd.backlight();
    lcd.createChar(0, arrow);
    pinMode(menuButton, INPUT_PULLUP);
    pinMode(selectButton, INPUT_PULLUP);
}
void loop()
{
    if (!digitalRead(menuButton))
    {
        delay(300);
        state++;
        if (state == 2)
            state = 0;
        makeChange = true;
    }
    if (!digitalRead(selectButton))
    {
        delay(300);
        topMenuPosition++;
        if (topMenuPosition == 2)
            topMenuPosition = 0;
        makeChange = true;
    }
    menu();
    makeChange = false;
}

void homePage()
{
    if (!isUpdate)
    {
        isUpdate = true;
        if (!change)
        {
            lcd.clear();
            lcd.setCursor(2, 0);
            lcd.print("Kaicho");
            lcd.setCursor(2, 1);
            lcd.print("Pad Vending");
        }
        else
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("R 1  2  3  4  5");
            lcd.setCursor(0, 1);
            lcd.print("Q");
            lcd.setCursor(2, 1);
            lcd.print(rack1.getQuantity());
            lcd.setCursor(5, 1);
            lcd.print(rack2.getQuantity());
            lcd.setCursor(8, 1);
            lcd.print(rack3.getQuantity());
            lcd.setCursor(11, 1);
            lcd.print(rack4.getQuantity());
            lcd.setCursor(14, 1);
            lcd.print(rack5.getQuantity());
        }
    }
    if (millis() - previousTime >= 5000)
    {
        change = !change;
        previousTime = millis();
        isUpdate = false;
    }
}
void topMenu()
{
    switch (topMenuPosition)
    {
    case 0:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write((byte)0);
        lcd.setCursor(3, 0);
        lcd.print("Rack Quantity");
        lcd.setCursor(3, 1);
        lcd.print("Motor Time");
        break;
    case 1:
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Rack Quantity");
        lcd.setCursor(0, 1);
        lcd.write((byte)0);
        lcd.setCursor(3, 1);
        lcd.print("Motor Time");
        break;
    }
}
void menu()
{
    switch (state)
    {
    case 0:
        homePage();
        break;
    case 1:
        if (makeChange)
            topMenu();
        break;
    }
}
void manageRack1()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rack 1  Quantity");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(settingVariable);
    lcd.blink();
}
void manageRack2()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rack 2 Quantity");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(settingVariable);
    lcd.blink();
}
void manageRack3()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rack 3 Quantity");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(settingVariable);
    lcd.blink();
}
void manageRack4()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rack 4 Quantity");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(settingVariable);
    lcd.blink();
}
void manageRack5()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rack 5 Quantity");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(settingVariable);
    lcd.blink();
}
void motorTime()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motor Time");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(settingVariable);
    lcd.blink();
}
void incVariable()
{
    settingVariable++;
}