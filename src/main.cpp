#include <Arduino.h>
#include <Keyboard.h>
#include <EEPROM.h>
#include <I2C_eeprom.h>
#include <Keypad.h>

// Global variables
byte btn,led=13;
char ans, singleKey[256];
String cmnd;
uint32_t addr, eAddr, stSize, extEepromSize, charsPerKey;

// Define the Keymap
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
byte keys [ROWS][COLS] = {
  {1,2,3,4},
  {5,6,7,8},
  {9,10,11,12},
  {13,14,15,16}
};
byte colPins[COLS] = {10, 16, 14, 15};  
byte rowPins[ROWS] = {A0, A1, A2, A3};
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// Define the external EEPROM
I2C_eeprom extEEPROM(0x50, I2C_DEVICESIZE_24LC32);

// Function prototypes
void printMenu();
void eraseEEPROM();
void printEEPROM(byte);
void writeEEPROM();
void getKey(byte);

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(100);
  Wire.begin();
  extEEPROM.begin();
  extEepromSize = extEEPROM.determineSize(false);
  charsPerKey = extEepromSize / 16;

// Local EEPROM
  if (EEPROM.read(0) == 255)
    for ( unsigned int i = 0 ; i < EEPROM.length() ; i++ )
      EEPROM.write(i, 0);

  // External EEPROM
  if (extEEPROM.readByte(0) == 255)
    for (uint32_t address = 0; address < extEepromSize; address += 128)
      extEEPROM.setBlock(address, 0x00, 128);

  printMenu();
  pinMode(led, OUTPUT);
}

void loop(){
  byte key = kpd.getKey();
  if (key){
    key--;
    digitalWrite(led, HIGH);
    Serial.print("Pressed key: ");
    Serial.println(key,HEX);

    getKey(key);
    for (size_t i = 0; i < strlen(singleKey); i++) {
      Keyboard.print(singleKey[i]);
      delay(10);
    }
    digitalWrite(led, LOW);
  }
  else if (Serial.available()){
    printMenu();
    ans = Serial.read();
    Serial.println(ans);
    if (ans == 'p' || ans == 'P') printEEPROM(16);
    else if (ans == 'w' || ans == 'W') writeEEPROM();
    else if (ans == 'e' || ans == 'E') eraseEEPROM();
  }
}

void getKey(byte btn) {
  addr = (btn * sizeof(uint32_t));
  // clear singleKey
  for (uint32_t i = 0; i < 256; i++) singleKey[i] = 0;
  EEPROM.get(addr, stSize);
  eAddr = (btn * charsPerKey);
    for (uint32_t i = 0; i <= stSize; i++) {
      if (extEEPROM.readByte(eAddr + i))
        singleKey[i] = (char)extEEPROM.readByte(eAddr + i);
  }
}

void printMenu(){
  Serial.println("\r\nKeyboard macros menu");
  Serial.println("p: Print EEPROM");
  Serial.println("w: Write EEPROM");
  Serial.println("e: Erase EEPROM");
  Serial.print("Input: ");
}

void eraseEEPROM(){
  Serial.print("\n\rErase the eeprom? (y/n): ");
  while (!Serial.available());
  ans = Serial.read();
  Serial.print(ans);
  if (ans == 'y' || ans == 'Y') 
  { 
    Serial.print("\r\nErasing external EEPROM size: ");
    Serial.print(extEepromSize); 
    digitalWrite(led, HIGH);

    for (uint32_t address = 0; address < extEepromSize; address += 128)
    {
      if (address % 1024 == 0) Serial.print('.');
      extEEPROM.setBlock(address, 0x00, 128);
    }

    Serial.print("\r\nErasing internal EEPROM size: ");
    Serial.print(EEPROM.length());
    for (unsigned int i = 0; i < EEPROM.length(); i++)
    {
      if (i % 256 == 0) Serial.print('.');
      EEPROM.write(i, 0);
    }
    Serial.println();

    digitalWrite(led, LOW);
    Serial.println("\r\nDone.");
  }
  else Serial.println("\r\nAborting.");
}

void printEEPROM(byte btn){
  if (btn == 16){ 
    char indx[4];
    for (int i = 0;i < btn ; i++){ 
      Serial.print("Button: ");
      sprintf(indx, "%1X ",i);
      Serial.print(indx);
      printEEPROM(i);
      Serial.println();
    }
    Serial.println();
  } else {
    digitalWrite(led, HIGH);
    addr = (btn * sizeof(uint32_t));
    eAddr = (btn * charsPerKey);
    stSize = EEPROM.read(addr);
    Serial.print("(");
    Serial.print(stSize);
    Serial.print(")");
    Serial.print("\t: ");
    for (uint32_t j = 0; j <= stSize; j++){
      if (extEEPROM.readByte(eAddr+j))
        Serial.print((char)extEEPROM.readByte(eAddr+j));
    digitalWrite(led, LOW);
    }
  } 
}

void writeEEPROM(){
  btn = 0;
  uint32_t i=0;
  digitalWrite(led, HIGH);
  Serial.print("Press the button you want to program. ");
  while (!btn){
    btn = kpd.getKey();
  }
  btn--;
  
  Serial.print(btn,HEX);Serial.print(": [");printEEPROM(btn); Serial.println("]");
  Serial.print("Enter macro (");
  Serial.print(charsPerKey - 2);
  Serial.print(" characters max): ");
  while (!Serial.available());
  while ((ans != '\n') && (ans != '\r')){
    while (!Serial.available());
    ans = Serial.read();
    if (ans == 127) {
      ans = 8;
    }
    if (ans == 8){
      if (i) {
        cmnd.remove(--i);
        Serial.print((char)ans);
        Serial.print((char)' ');
        Serial.print((char)ans);
      }
    }
    else if ((ans >= 32) && (ans <= 255)) {
        i++;
        Serial.print((char)ans);
        if (cmnd.length() >= charsPerKey - 2) {
            Serial.println("\r\nSorry you have reached the character limit for this macro.");
            ans = '\r';
            break;
        }
        if ((ans != '\n') && (ans != '\r')) cmnd += ans;
    }
  }
  
  Serial.print("\r\nReplacing:\r\n [");printEEPROM(btn);Serial.print("]\r\nWith:\r\n [");Serial.print(cmnd);Serial.println("]");
  Serial.print("Correct? (y/n): ");
  while (!Serial.available());
  ans = Serial.read();
  Serial.println(ans);  
  if (ans == 'y' || ans == 'Y') { 
    stSize = cmnd.length();
    Serial.print("\n\rLength: ");
    Serial.println(stSize);
    addr = (btn * sizeof(uint32_t));
    eAddr = (btn * charsPerKey);
    EEPROM.put(addr, stSize);
    for ( i = 0; i <= stSize; i++) {
      extEEPROM.writeByte((eAddr+i),cmnd[i]);
    }

    while (i < charsPerKey) {
      extEEPROM.writeByte(eAddr + i, 0);
      i++;
    }
  Serial.println("Done writing.");
  }
  else Serial.println("Aborting.");
  cmnd="";
  digitalWrite(led, LOW);
}
