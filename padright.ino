#include "padrack.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "OneButton.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
padrack rack1, rack2, rack3, rack4, rack5;
OneButton menuButton(3, true, true);
OneButton selectButton(4, true, true);
OneButton incButton(5, true, true);

bool change = false;
bool isUpdate = false;

int settingVariable = 0;
uint32_t previousTime = millis();
int currentMenuPosition = 0;
int currentMenu = 0;

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
    incButton.attachClick(incVariable);
    // selectButton.attachClick(select);
    menuButton.attachClick(menu);
    menuButton.attachLongPressStart(toggleMenu);
}
void loop()
{
    menuButton.tick();
    selectButton.tick();
    homePage();
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
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Rack Quantity");
    lcd.setCursor(3, 1);
    lcd.print("Motor Time");
    switch (currentMenu)
    {
    case 0:
        lcd.setCursor(0, 0);
        lcd.write((byte)0);
        break;
    case 1:
        lcd.setCursor(0, 1);
        lcd.write((byte)0);
        break;
    case 3:
        currentMenu = 0;
        break;
    }
}
void menu()
{
    switch (currentMenuPosition)
    {
    case 0:
        homePage();
        currentMenu = 0;
        break;

    case 1:
        topMenu();
        currentMenu = 1;
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

void toggleMenu()
{
    currentMenuPosition++;
    if (currentMenuPosition == 3)
        currentMenuPosition = 0;
    menu();
}

void gotoHome()
{
    currentMenuPosition = 0;
}
void select()
{
    // selection++;
}