#include "padrack.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10 // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

MFRC522::MIFARE_Key key;
MFRC522::StatusCode rfidstatus;
// regarding rfid card
int blockAddr = 2;
byte readByte[18];
byte writeByte[16];
int authAddr = 3;
byte byteSize = sizeof(readByte);

// --------- eeprom address for menu variables ---------
static byte rack1Address = 1;
static byte rack2Address = 2;
static byte rack3Address = 3;
static byte rack4Address = 4;
static byte rack5Address = 5;
static byte motorTimeAddress = 10;
//----------- class objects ---------------------

LiquidCrystal_I2C lcd(0x27, 16, 2);
padrack rack1, rack2, rack3, rack4, rack5;
//-------------------- input and output varaibles --------------
byte motor1 = 6;
byte motor2 = 7;
byte motor3 = 8;
byte motor4 = 14;
byte motor5 = 15;
byte menuButton = 2;
byte selectButton = 3;
byte okButton = 4;
byte buzzer = 5;
////----------- logic variables -------------
bool change = false;
bool isUpdate = false;
bool changeDone = true;
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
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card
    pinMode(menuButton, INPUT_PULLUP);
    pinMode(selectButton, INPUT_PULLUP);
    pinMode(okButton, INPUT_PULLUP);
    pinMode(buzzer, OUTPUT);
    digitalWrite(buzzer, LOW);
    readFromEEPROM();

    // key for auth rfid
    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }
    startMessage();
}
void startMessage()
{
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Powered By");
    lcd.setCursor(2, 1);
    lcd.print("Kaicho Group");
    delay(3000);
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("RED");
    lcd.setCursor(5, 1);
    lcd.print("INT'L");
    delay(5000);
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Kaicho");
    lcd.setCursor(2, 1);
    lcd.print("Pad Vending");
    delay(2000);
    homePage2();
}
void loop()
{
    menuManagement();
    manageRFID();
}
void manageRFID()
{
    if (mfrc522.PICC_IsNewCardPresent())
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Card Detected!");
        lcd.setCursor(0, 1);
        lcd.print("Please Wait.....");
        if (mfrc522.PICC_ReadCardSerial())
        {
            if (readCard())
            {
                if (readByte[15] != 0 && readByte[0] == 107) // verify the card and available quantity on card
                {
                    dumpToWriteVar(readByte, 16);
                    if (!writeCard())
                    {
                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcd.print("Failed!");
                        lcd.setCursor(0, 1);
                        lcd.print("Try Again");
                        delay(3000);
                    }
                    else
                    {
                        if (getStock() != 0) // check the stock before proceeding
                        {
                            delay(500);
                            lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("Left for Card:");
                            lcd.setCursor(0, 1);
                            lcd.print(writeByte[15]);
                            delay(3000);
                            lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("Receive Pad");
                            lcd.setCursor(0, 1);
                            lcd.print("Thank you!");
                            delay(2000);
                            runMotor();
                        }
                        else // print if no stock remaining
                        {
                            lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("Sorry");
                            lcd.setCursor(0, 1);
                            lcd.print("No Stocks");
                            delay(2000);
                        }
                    }
                }
                else
                {
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Opps!");
                    lcd.setCursor(0, 1);
                    lcd.print("Zero Balance");
                    delay(3000);
                }
            }
            else
            {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Failed!");
                lcd.setCursor(0, 1);
                lcd.print("Try Again");
                delay(3000);
            }
        }
        halt();
        status = 'n';
        changeDone = true;
    }
}
void menuManagement()
{
    if (status == 'n' || status == 'a')
    {
        if (!digitalRead(menuButton))
        {
            delay(300);
            state++;
            if (state == 2)
                state = 0;
            makeChange = true;
            changeDone = true;
        }
        else if (!digitalRead(selectButton))
        {
            delay(300);
            topMenuPosition++;
            if (topMenuPosition == 2)
                topMenuPosition = 0;
            makeChange = true;
            changeDone = true;
        }
        if (!digitalRead(okButton) && state == 1)
        {
            delay(300);
            if (state == 1 && topMenuPosition == 0)
                status = 'r';
            else if (state == 1 && topMenuPosition == 1)
                status = 'm';
            // Serial.println(status);
            switch (status)
            {
            case 'm':
                motorTime();
                while (digitalRead(okButton))
                {
                    delay(300);
                    if (!digitalRead(selectButton))
                    {
                        delay(300);
                        motorTimeVariable = motorTimeVariable + 100;
                        if (motorTimeVariable > 5000)
                            motorTimeVariable = 100;
                        motorTime();
                    }
                }
                break;
            case 'r':
                manageRack1();
                while (digitalRead(okButton))
                {
                    // delay(300);
                    if (!digitalRead(selectButton))
                    {
                        delay(300);
                        rack1.incQuantity();
                        manageRack1();
                    }
                }
                while (!digitalRead(okButton))
                    ;
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
                while (!digitalRead(okButton))
                    ;
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
                while (!digitalRead(okButton))
                    ;
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
                while (!digitalRead(okButton))
                    ;
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
                break;
            }
            writeToEPPROM(status);
            save();
            changeDone = true;
        }
    }

    menu();
    makeChange = false;
}

// void homePage()
// {
//     if (!isUpdate)
//     {
//         isUpdate = true;
//         if (!change)
//         {
//             lcd.clear();
//             lcd.setCursor(6, 0);
//             lcd.print("Scan");
//             lcd.setCursor(4, 1);
//             lcd.print("Card Here");
//         }
//         else
//         {
//             lcd.clear();
//             lcd.setCursor(0, 0);
//             lcd.print("R 1  2  3  4  5");
//             lcd.setCursor(0, 1);
//             lcd.print("Q");
//             lcd.setCursor(2, 1);
//             lcd.print(rack1.getQuantity());
//             lcd.setCursor(5, 1);
//             lcd.print(rack2.getQuantity());
//             lcd.setCursor(8, 1);
//             lcd.print(rack3.getQuantity());
//             lcd.setCursor(11, 1);
//             lcd.print(rack4.getQuantity());
//             lcd.setCursor(14, 1);
//             lcd.print(rack5.getQuantity());
//         }
//     }
//     if (millis() - previousTime >= 5000)
//     {
//         change = !change;
//         previousTime = millis();
//         isUpdate = false;
//     }
// }
void homePage2()
{
    if (changeDone == true)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SCAN CARD HERE");
        lcd.setCursor(0, 1);
        lcd.print("N0 OF STOCKS:");
        lcd.print(getStock());
        changeDone = false;
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
        homePage2();
        topMenuPosition = 0;
        status = 'a';
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
void writeToEPPROM(char status)
{

    if (status == 'r')
    {
        EEPROM.write(rack1Address, rack1.getQuantity());
        EEPROM.write(rack2Address, rack2.getQuantity());
        EEPROM.write(rack3Address, rack3.getQuantity());
        EEPROM.write(rack4Address, rack4.getQuantity());
        EEPROM.write(rack5Address, rack5.getQuantity());
    }
    else if (status == 'm')
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

bool readCard()
{
    byte buffersize = 18;
    if (auth_A())
    {
        rfidstatus = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, readByte, &buffersize);
        if (rfidstatus != MFRC522::STATUS_OK)
        {
            return false;
        }
    }
    return true;
}
bool writeCard()
{
    if (auth_B())
    {
        rfidstatus = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, writeByte, 16);
        if (rfidstatus != MFRC522::STATUS_OK)
        {
            return false;
        }
    }
    return true;
}

void dumpToWriteVar(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i]);
        writeByte[i] = buffer[i];
    }
    writeByte[15]--;
}
void halt()
{
    mfrc522.PICC_HaltA();      // Halt PICC
    mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD
}
bool auth_A()
{
    rfidstatus = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, authAddr, &key, &(mfrc522.uid));
    if (rfidstatus != MFRC522::STATUS_OK)
    {
        return false;
    }
    return true;
}
bool auth_B()
{
    rfidstatus = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, authAddr, &key, &(mfrc522.uid));
    if (rfidstatus != MFRC522::STATUS_OK)
    {
        return false;
    }
    return true;
}
void runMotor()
{
    if (getStock() > 0)
    {
        if (rack1.getQuantity() != 0)
        {
            digitalWrite(motor1, HIGH);
            delay(motorTimeVariable);
            digitalWrite(motor1, LOW);
            rack1.decQuantity();
        }
        else if (rack2.getQuantity() != 0)
        {
            digitalWrite(motor2, HIGH);
            delay(motorTimeVariable);
            digitalWrite(motor2, LOW);
            rack2.decQuantity();
        }
        else if (rack3.getQuantity() != 0)
        {
            digitalWrite(motor3, HIGH);
            delay(motorTimeVariable);
            digitalWrite(motor3, LOW);
            rack3.decQuantity();
        }
        else if (rack4.getQuantity() != 0)
        {
            digitalWrite(motor4, HIGH);
            delay(motorTimeVariable);
            digitalWrite(motor4, LOW);
            rack4.decQuantity();
        }
        else if (rack5.getQuantity() != 0)
        {
            digitalWrite(motor5, HIGH);
            delay(motorTimeVariable);
            digitalWrite(motor5, LOW);
            rack5.decQuantity();
        }
        writeToEPPROM('r');
    }
}
int getStock()
{
    return rack1.getQuantity() + rack2.getQuantity() + rack3.getQuantity() + rack4.getQuantity() + rack5.getQuantity();
}