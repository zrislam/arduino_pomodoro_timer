#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

// Initialize the display object using the U8G2 library for a 128x64 SSD1306 
//OLED display with hardware I2C
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, \
/* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);  // High-speed I2C

// Pin assignments
const int btn_pin = 6;       // Button pin
const int potPin = A0;       // Potentiometer pin
const int buzzPin = 5;       // Buzzer pin

// Variables for countdown functionality
int countdownDuration = 0;   // Countdown duration in seconds
int countdownDuration_min = 0; // Countdown duration in minutes
boolean buttonState;         // State of the button
unsigned long startTime = 0; // Variable to store the start time

// Setup function, runs once at the beginning
void setup() {
  u8g2.begin();               // Initialize the display
  pinMode(buzzPin, OUTPUT);    // Set buzzer pin as output
  pinMode(btn_pin, INPUT);     // Set button pin as input
}

// Function to print text on the OLED display
void oled_print(const char *text){
  u8g2.clearBuffer();                    // Clear the internal memory of the display
  u8g2.setFont(u8g2_font_ncenB08_tr);    // Choose a suitable font
  u8g2.setFontDirection(2);              // Rotate text 180 degrees
  u8g2.drawStr(120,50,text);             // Write text to the internal memory
  u8g2.sendBuffer();                     // Transfer internal memory to the display
  delay(1000);                           // Delay for 1 second
}

// Function to set the countdown duration based on the potentiometer value
void setCountdownDuration() {
  String countdown;
  int potValue = analogRead(potPin);
  
  // Map potentiometer value to countdown range (1 minute to 60 minutes)
  countdownDuration = map(potValue, 1023, 0, 1, 60) * 60;  
  countdownDuration_min = countdownDuration / 60;
  countdown += "Pomodoro Timer: " + String(countdownDuration_min);
  
  //.c_str() method returns a pointer to the C-style null-terminated 
  //version of the String object it is called on.
  oled_print(countdown.c_str());
}

// Function to start the countdown timer
void startCountdown() {
  startTime = millis();  // Store the current time as the start time
}

// Function to get the remaining time in seconds
int getTimeRemaining() {
  unsigned long currentTime = millis();
  // Convert milliseconds to seconds
  int elapsedTime = (currentTime - startTime) / 1000;  
  // Ensure timeRemaining is not negative
  int timeRemaining = max(0, countdownDuration - elapsedTime);  
  return timeRemaining;
}

// Main loop function, runs repeatedly
void loop() {
  setCountdownDuration();  // Set the countdown duration based on potentiometer value

  buttonState = digitalRead(btn_pin);  // Read the state of the button
  
  if (buttonState == HIGH) {
    startCountdown();  // Start the countdown when the button is pressed

    while (getTimeRemaining() > 0) {
      // Display the remaining time on the OLED display
      
      //timeRemaining is divided by 60 to give minutes remaining.
      //The remainder of dividing timeRemaining by 60 is found to give seconds remaining.
      String displayString = "Time Left: " + String(getTimeRemaining() / 60) + "m " \
      + String(getTimeRemaining() % 60) + "s";
  
      oled_print(displayString.c_str());
    }

    // Countdown complete, activate buzzer and display message.
    while(1){
      buttonState = digitalRead(btn_pin);
      oled_print("Countdown Finished!");
      tone(buzzPin, 260, 500);
      
      if (buttonState == HIGH){
        break;
      }
    }
  }
}