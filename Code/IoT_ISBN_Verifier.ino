
         ////////////////////////////////////////////////////  
        //      IoT ISBN Verifier with Nokia 5110 Screen   //
       //                                                //
      //           -------------------------            //
     //              Arduino Nano 33 IoT               //           
    //               by Kutluhan Aktar                // 
   //                                                //
  ////////////////////////////////////////////////////

// Via Arduino Nano 33 IoT and a PHP web application, check whether you have a particular book in your library or not before purchasing by using ISBN.
//
// Register ISBN entries for each book in your library on the web application named ISBN Registration System to gather information about them using Arduino Nano 33 IoT.
//
// For more information:
// https://www.theamplituhedron.com/projects/IoT-ISBN-Verifier-with-Nokia-5110-Screen/
// 
// You can use the mentioned web application in free version on TheAmplituhedron as the host server if you are a subscriber:
// https://www.theamplituhedron.com/dashboard/ISBN-Registration-System/
//
// Connections
// Arduino Nano 33 IoT:           
//                                Nokia 5110 Screen
// D8  --------------------------- SCK (Clk)
// D9  --------------------------- MOSI (Din) 
// D10 --------------------------- DC 
// D11 --------------------------- RST
// D12 --------------------------- CS (CE)
//                                4X4 Membrane KeyPad
// A0  --------------------------- (1)
// A1  --------------------------- (2)
// A2  --------------------------- (3)
// A3  --------------------------- (4)
// A4  --------------------------- (5)
// A5  --------------------------- (6)
// A6  --------------------------- (7)
// A7  --------------------------- (8)


// Include required libraries:
#include <SPI.h>
#include <WiFiNINA.h>
#include <LCD5110_Basic.h>
#include <Keypad.h>

char ssid[] = "[_SSID_]";        // your network SSID (name)
char pass[] = "[_PASSWORD_]";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

char server[] = "www.theamplituhedron.com";    // name address for TheAmplituhedron. Change it with your server if you are using a different host server than TheAmplituhedron.
// e.g., char server[] = "192.168.1.22"; // for the localhost

// Define the pathway of the web application. If you are using TheAmplituhedron as the host server for this project as I did, just enter your hedron. Otherwise, enter the pathway on your server.
String application = "/dashboard/ISBN-Registration-System/";

// Define your hedron if you are using TheAmplituhedron as the host server for this project:
String HEDRON = "[_HEDRON_]";

// Initialize the Ethernet client library
WiFiSSLClient client;

// Define screen settings.
LCD5110 myGLCD(8,9,10,11,12);

extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
// Define images for the gpaphics screen.
extern uint8_t Batman[];
extern uint8_t Spiderman[];
extern uint8_t starwars[];
extern uint8_t trek[];

// Define keypad settings.
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A4, A5, A6, A7}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

// Define the custom key and ISBN variables.
char customKey;
String ISBN;
String Registered = "Not Detected!";

// Define menu options and screens using volatile booleans.
volatile boolean option_A = false;
volatile boolean option_B = false;
volatile boolean option_C = false;
volatile boolean option_D = false;

void setup() {
  // Initiate screen.
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);

  Serial.begin(9600);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) { Serial.println("Communication with WiFi module failed!"); myGLCD.print("Connection Failed!", 0, 8); while (true); }
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    myGLCD.print("Waiting...", 0, 8);
    myGLCD.print("Attempting to", 0, 16);
    myGLCD.print("connect to", 0, 24);
    myGLCD.print("WiFi !!!", 0, 32);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  // Verify connection on both the serial monitor and Nokia 5110 Screen.
  Serial.println("Connected to wifi");
  myGLCD.clrScr();
  myGLCD.print("Connected to", 0, 8);
  myGLCD.print("WiFi!!!", 0, 16);
  delay(2000);
  myGLCD.clrScr();
}

void loop() {

    interface();
  
    change_menu_options();

    // Pressing '*' on the keypad, select the marked option.
    // To exit from the selected option in order to return back to the interface, press '#'.
    if(option_A == true){
      do{
        myGLCD.invertText(true);
        myGLCD.print("A. Search ISBN", 0, 16);
        myGLCD.invertText(false);
        delay(100);
        if(customKey == '*'){
          myGLCD.clrScr();
          while(customKey == '*'){
             myGLCD.print("Enter ISBN:", 0, 0);
             myGLCD.print(ISBN, CENTER, 16);
             myGLCD.print("(*) => SEND", 0, 40);
             // Collect the recently entered key as c tentatively.
             char c = customKeypad.getKey();
             // Define tasks.
             switch(c){
              case '#': // Exit and clear.
                customKey = c; ISBN = ""; myGLCD.clrScr();
              break;
              case '*': // Press '*' to send the entered ISBN to the web application to be interpreted.
                myGLCD.clrScr();
                // Connect to the web application named ISBN Registration System.
                if (client.connect(server, 443)) {
                  Serial.println("connected to server"); // if you get a connection, report back via serial:
                  myGLCD.print("Connected to", 0, 8);
                  myGLCD.print("the server!!!", 0, 16);
                  // Make a HTTP request:
                  client.println("GET " + application + HEDRON + "/?ISBN=" + ISBN + " HTTP/1.1");
                  client.println("Host: www.theamplituhedron.com");
                  client.println("Connection: close");
                  client.println();
                }
                delay(2000); // Wait 2 seconds after searching...
                myGLCD.clrScr();
              break;
             }
             // Update ISBN with the recently entered key.
             if(c && c != '*' && c != '#') ISBN += c;
             // If there are incoming bytes available, get the response from the web application.
             String response = "";
             while (client.available()) { char r = client.read(); response += r; }
             if(response != ""){
               // Split the response string by a pre-defined delimiter in a simple way. '%'(percentage) is defined as the delimiter in this project.
               int delimiter, delimiter_1, delimiter_2, delimiter_3, delimiter_4;
               delimiter = response.indexOf("%");
               delimiter_1 = response.indexOf("%", delimiter + 1);
               delimiter_2 = response.indexOf("%", delimiter_1 +1);
               delimiter_3 = response.indexOf("%", delimiter_2 +1);
               delimiter_4 = response.indexOf("%", delimiter_3 +1);
               // Glean information as substrings.
               String Name = response.substring(delimiter + 1, delimiter_1);
               String Page = response.substring(delimiter_1 + 1, delimiter_2);
               String Author = response.substring(delimiter_2 + 1, delimiter_3);
               Registered = response.substring(delimiter_3 + 1, delimiter_4);
               // Print information.
               Serial.print(Page);
               myGLCD.clrScr();
               myGLCD.print("Information:", 0, 0);
               myGLCD.print(Name, 0, 16);
               myGLCD.print(Page, 0, 32);
               myGLCD.print(Author, 0, 40);
              // Wait 5 seconds to display the book information from the database of the web application.
              delay(5000);
              myGLCD.clrScr();
             }
          }
        }
      }while(option_A == false);
    }

    if(option_B == true){
      do{
        myGLCD.invertText(true);
        myGLCD.print("B. Registered", 0, 24);
        myGLCD.invertText(false);
        delay(100);
        if(customKey == '*'){
          myGLCD.clrScr();
          while(customKey == '*'){
             myGLCD.print("Registered", 0, 0);
             myGLCD.print("Books:", 0, 8);
             myGLCD.print(Registered, CENTER, 24);
             myGLCD.print("(#) => BACK", 0, 40);
             // Collect the recently entered key as c tentatively.
             char c = customKeypad.getKey();
             // Define tasks.
             if(c == '#'){ customKey = c; Registered = "Expired!"; myGLCD.clrScr(); }
          }
        }
      }while(option_B == false);
    }

    if(option_C == true){
      do{
        myGLCD.invertText(true);
        myGLCD.print("C. Graphics", 0, 32);
        myGLCD.invertText(false);
        delay(100);
        if(customKey == '*'){
          myGLCD.clrScr();
          while(customKey == '*'){
             // Draw graphics.
             myGLCD.clrScr();
             myGLCD.drawBitmap(22, 0, Batman, 40, 20);
             myGLCD.print("BATMAN", CENTER, 40);
             delay(1000);
             for(int i=0;i<2;i++){ myGLCD.invert(true); delay(500); myGLCD.invert(false); delay(500); }
             myGLCD.clrScr();
             myGLCD.drawBitmap(22, 0, Spiderman, 40, 40);
             myGLCD.print("Spider-Man", CENTER, 40);
             delay(1000);
             for(int i=0;i<2;i++){ myGLCD.invert(true); delay(500); myGLCD.invert(false); delay(500); }
             myGLCD.clrScr();
             myGLCD.drawBitmap(22, 0, starwars, 40, 24);
             myGLCD.print("StarWars", CENTER, 40);
             delay(1000);
             for(int i=0;i<2;i++){ myGLCD.invert(true); delay(500); myGLCD.invert(false); delay(500); }
             myGLCD.clrScr();
             myGLCD.drawBitmap(12, 0, trek, 64, 60);
             myGLCD.print("Star Trek", CENTER, 40);
             delay(1000);
             for(int i=0;i<2;i++){ myGLCD.invert(true); delay(500); myGLCD.invert(false); delay(500); }
             // Exit and clear.
             myGLCD.clrScr();
             for(int i=0;i<10;i++){
                myGLCD.print("Graphics", CENTER, 8);
                myGLCD.print("Screen", CENTER, 16);
                myGLCD.print("(#) => BACK", CENTER, 40);
                // Collect the recently entered key as c tentatively.
                char c = customKeypad.getKey();
                // Define tasks.
                if(c == '#'){ customKey = c; }
                delay(500);
             }
             // Clear.
             myGLCD.clrScr();
          }
        }
      }while(option_C == false);
    }

    if(option_D == true){
      do{
        myGLCD.invertText(true);
        myGLCD.print("D. Sleep", 0, 40);
        myGLCD.invertText(false);
        delay(100);
        if(customKey == '*'){
          // Activate the sleep mode in 10 seconds.
          myGLCD.clrScr();
          myGLCD.print("Entering", CENTER, 0);
          myGLCD.print("Sleep Mode", CENTER, 8);
          myGLCD.print("in", CENTER, 16);
          myGLCD.print("Seconds", CENTER, 40);
          // Print remaining seconds.
          myGLCD.setFont(MediumNumbers);
          for (int s=10; s>=0; s--){ myGLCD.printNumI(s, CENTER, 24, 2, '0'); delay(1000); }
          myGLCD.enableSleep();
          while(customKey == '*'){
             // Collect the recently entered key as c tentatively.
             char c = customKeypad.getKey();
             // Define tasks.
             if(c == '#'){ customKey = c; myGLCD.disableSleep(); myGLCD.setFont(SmallFont); }
          }
        }
      }while(option_D == false);
    }
}

void interface(){
    // Define options.
    myGLCD.print("Menu Options :", 0, 0);
    myGLCD.print("A. Search ISBN", 0, 16);
    myGLCD.print("B. Registered", 0, 24);
    myGLCD.print("C. Graphics", 0, 32);
    myGLCD.print("D. Sleep", 0, 40);

}

void change_menu_options(){
  // Get the recently entered custom key.
  customKey = customKeypad.getKey();
  // Define conditions for each option.
  if(customKey){
    switch(customKey){
      case 'A':
      option_A = true;
      option_B = false;
      option_C = false;
      option_D = false;
      break;
      case 'B':
      option_A = false;
      option_B = true;
      option_C = false;
      option_D = false;
      break;
      case 'C':
      option_A = false;
      option_B = false;
      option_C = true;
      option_D = false;
      break;
      case 'D':
      option_A = false;
      option_B = false;
      option_C = false;
      option_D = true;
      break;

    }
  }
}
