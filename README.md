# keyboard-macro

### 32u4 keyboard macro (Based on the Arduino Pro Micro Module)

Keyboard macro - It maps 16 buttons `4x4 button matrix` to strings `254 characters long each`.  
The strings are sent to the computer from an USB Keyboard HID. A serial console 9600,8,1,n is created to manage the strings.
The dual layer PCB was created using the easyeda cad software and the files for manufacruing the board are [availabe here](https://oshwlab.com/ebarranco/keybaordmacrov3).
  
To compile you will needs [visual studio code](https://code.visualstudio.com/) with the [PlatformIO extension](https://platformio.org)

On visual studio code:
*  Click on the PlatformIO extension
*  On the home select "Open project"
*  Navigate to where you clonned this project
*  On the com port select the newly added com port
*  Push the right arrow to build and upload the code to the board
*  Use your favorit com port terminal and connect

Libraries:
*  [Keypad](https://github.com/Chris--A/Keypad)
*  [EEPROM](https://www.arduino.cc/en/Reference/EEPROM)
*  [I2C_EEPROM](https://github.com/RobTillaart/I2C_EEPROM)
  
Pins are configured as follows:
*  Row pins: A0, A1, A2, A3
*  Column pins: 10, 16, 14, 15

## How to use:
When connected, it will be detected as two devices: Serial Port and HID Keyboard.
Using a terminal software connect to the serial port using 9600,8,1,n.
Once connected there are 3 options:
*  P: Prints the current macros.
*  W: Will allow you to program a string to a button – Follow on-screen instructions.
*  E: Erases all the stored data. (Confirmation is required).

Once the data has been saved, disconnect the terminal software.

 
73's  
