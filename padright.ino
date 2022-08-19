#include "padrack.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "OneButton.h"
#include <EEPROM.h>
// ADDRESS FOR VARIABLES
static byte rack1Address = 1;
static byte rack2Address = 2;
static byte rack3Address = 3;
static byte rack4Address = 4;
static byte rack5Address = 5;
static byte motorTimeAddress = 10;
//---------------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);
padrack rack1, rack2, rack3, rack4, rack5;

bool change = false;
bool isUpdate = false;
byte menuButton = 3;
byte selectButton = 4;
byte okButton = 5;
byte buzzer = 7;

int motorTimeVariable = 100;
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
    pinMode(buzzer, OUTPUT);
    digitalWrite(buzzer, LOW);
    readFromEEPROM();
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
                        motorTimeVariable = motorTimeVariable + 100;
                        if (motorTimeVariable > 5000)
                            motorTimeVariable = 100;
                        motorTime();
                    }
                }
                writeToEPPROM(state);
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
                writeToEPPROM(state);
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
    lcd.print("Motor Time (ms)");
    lcd.setCursor(0, 1);
    // enter number
    lcd.print(motorTimeVariable);
}
void save()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Saving...");
    digitalWrite(buzzer, HIGH);
    delay(2000);
    digitalWrite(buzzer, LOW);
    status = 'n';
    state = 0;
}
void writeToEPPROM(char state)
{

    if (state == 'r')
    {
        EEPROM.write(rack1Address, rack1.getQuantity());
        EEPROM.write(rack2Address, rack2.getQuantity());
        EEPROM.write(rack3Address, rack3.getQuantity());
        EEPROM.write(rack4Address, rack4.getQuantity());
        EEPROM.write(rack5Address, rack5.getQuantity());
    }
    else if (state = 'm')
    {
        writeIntIntoEEPROM(motorTimeAddress, motorTimeVariable);
    }
}
void readFromEEPROM()
{
    motorTimeVariable = readIntFromEEPROM(motorTimeAddress);
    if (motorTimeVariable == -1)
        motorTimeVariable = 0;
    rack1.setQuantity(EEPROM.read(rack1Address));
    rack2.setQuantity(EEPROM.read(rack2Address));
    rack3.setQuantity(EEPROM.read(rack3Address));
    rack4.setQuantity(EEPROM.read(rack4Address));
    rack5.setQuantity(EEPROM.read(rack5Address));
}

void writeIntIntoEEPROM(int address, int number)
{
    EEPROM.write(address, number >> 8);
    EEPROM.write(address + 1, number & 0xFF);
}
int readIntFromEEPROM(int address)
{
    return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}