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
byte okButton = 5;

int motorTimeVariable = 5;
uint32_t previousTime = millis();
int topMenuPosition = 0;
int state = 0;
bool makeChange = false;
char status = 'n';
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
    pinMode(okButton, INPUT_PULLUP);
}
void loop()
{
    if (status == 'n')
    {
        if (!digitalRead(menuButton))
        {
            delay(300);
            state++;
            if (state == 2)
                state = 0;
            makeChange = true;
        }
        else if (!digitalRead(selectButton))
        {
            delay(300);
            topMenuPosition++;
            if (topMenuPosition == 2)
                topMenuPosition = 0;
            makeChange = true;
        }
        if (!digitalRead(okButton) && state == 1)
        {
            delay(300);
            if (state == 1 && topMenuPosition == 0)
                status = 'r';
            else if (state == 1 && topMenuPosition == 1)
                status = 'm';
            Serial.println(status);
            switch (status)
            {
            case 'm':
                motorTime();
                while (digitalRead(okButton))
                {
                    if (!digitalRead(selectButton))
                    {
                        delay(300);
                        motorTimeVariable = motorTimeVariable + 5;
                        motorTime();
                    }
                }
                save();
                break;
            case 'r':
                manageRack1();
                while (digitalRead(okButton))
                {
                    if (!digitalRead(selectButton))
                    {
                        delay(300);
                        rack1.incQuantity();
                        manageRack1();
                    }
                }
                save();
                manageRack2();
                while (digitalRead(okButton))
                {
                    if (!digitalRead(selectButton))
                    {
                        delay(300);
                        rack2.incQuantity();
                        manageRack2();
                    }
                }
                save();
                manageRack3();
                while (digitalRead(okButton))
                {
                    if (!digitalRead(selectButton))
                    {
                        delay(300);
                        rack3.incQuantity();
                        manageRack3();
                    }
                }
                save();
                manageRack4();
                while (digitalRead(okButton))
                {
                    if (!digitalRead(selectButton))
                    {
                        delay(300);
                        rack4.incQuantity();
                        manageRack4();
                    }
                }
                save();
                manageRack5();
                while (digitalRead(okButton))
                {
                    if (!digitalRead(selectButton))
                    {
                        delay(300);
                        rack5.incQuantity();
                        manageRack5();
                    }
                }
                save();
                break;
            }
        }
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
        topMenuPosition = 0;
        break;
    case 1:
        if (makeChange)
            topMenu();
        isUpdate = false;
        break;
    }
}
void manageRack1()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rack1  Quantity");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(rack1.getQuantity());
}
void manageRack2()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rack2 Quantity");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(rack2.getQuantity());
}
void manageRack3()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rack3 Quantity");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(rack3.getQuantity());
}
void manageRack4()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rack4 Quantity");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(rack4.getQuantity());
}
void manageRack5()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rack5 Quantity");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(rack5.getQuantity());
}
void motorTime()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motor Time");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(motorTimeVariable);
}
void save()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Saving...");
    delay(2000);
    status = 'n';
    state = 0;
}