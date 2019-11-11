#include <Arduino.h>
#include <Keyboard.h>
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 2
#define RST_PIN 3
#define KEY_RETURN 0xB0 //The hex value for the return key is 0xB0.
#define Sensor_key_pin 4

#define print_to_Serial false

MFRC522 mfrc522(SS_PIN, RST_PIN);
char Enter = KEY_RETURN; //Return key is declared as Enter.
String readid;
String card1 = "a9988776"; //Change this value to the UID of your card. Specify the UID RFID card using capital letters.
String card2 = "1122334b"; //Change this value to the UID of your card. Specify the UID RFID card using capital letters.
int timer = 0;

void lock_pc()
{
  Keyboard.press(KEY_LEFT_GUI); //Press the left windows key.
  Keyboard.press('l');          //Press the "l" key.
  Keyboard.releaseAll();        //Release all keys.
  delay(100);
}

void unlock_pc()
{
  if (readid == card1 || readid == card2)
  {
    Keyboard.press(KEY_LEFT_GUI); //Press the left windows key.
    Keyboard.press('l');          //Press the "l" key.
    Keyboard.releaseAll();        //Release all keys.
    delay(100);
    Keyboard.press(0xD7);   //Press the Enter key.
    Keyboard.release(0xD7); //Release the Enter key.
    delay(100);
    Keyboard.releaseAll();
    Keyboard.print("password"); // Change this value to your Windows PIN/Password.
    Keyboard.releaseAll();
    delay(100);
    Keyboard.press(Enter);
    Keyboard.releaseAll();
  }
}

void setup()
{
#if print_to_Serial
  Serial.begin(9600);
#endif
  Keyboard.begin();
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(Sensor_key_pin, INPUT);
  timer = millis();
}

void read_uid_rfid_card(byte *buffer, byte bufferSize) //function to store card uid as a string datatype.
{
  readid = "";
  for (byte i = 0; i < bufferSize; i++)
  {
    readid = readid + String(buffer[i], HEX);
  }
}
void loop()
{
  if (digitalRead(Sensor_key_pin) == HIGH)
  {
    lock_pc();
    return;
  }

  if (millis() > timer + 300)
  {
    if (!mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    if (!mfrc522.PICC_ReadCardSerial())
    {
      return;
    }
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid)); // Display card details in serial Monitor.
    #if print_to_Serial
    Serial.print(" ---> READ = ");
    Serial.println(readid);
    #endif
    read_uid_rfid_card(mfrc522.uid.uidByte, mfrc522.uid.size);

    if (readid == card1 || readid == card2)
    {
      lock_pc() ;
      delay (100);
      unlock_pc();
      delay (100);
      return ;
    }
    else
      return;
  }
}