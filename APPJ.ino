#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//IMPORTANT
// OLED display size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

//OPTIONAL
// you can ignore this if you plan on using a default ring
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

//IMPORTANT
// OLED reset pin
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//IMPORTANT
// Pin definitions
const int startButtonPin = A0;    // Start button
const int incButtonPin = A1;      // Increase value button
const int decButtonPin = A2;      // Decrease value button
const int selectButtonPin = A3;   // Select between seconds, minutes, and hours button
const int buzzerPin = 6;          // Buzzer
const int powerSupplyPin = 5;     // Power Supply

// Variables for time and selection
int seconds = 0;
int minutes = 0;
int hours = 0;
int selectedValue = 0; // 0 = seconds, 1 = minutes, 2 = hours
bool timerRunning = false;
int tempo = 180;
int melody[] = {
  NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4, 
  NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4, 
  NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
  NOTE_A4, 2, 
};
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;

//IMPORTANT
// Function to initialize the OLED display
void setup() {
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(incButtonPin, INPUT_PULLUP);
  pinMode(decButtonPin, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(powerSupplyPin, OUTPUT);

  digitalWrite(buzzerPin, LOW); // Ensure buzzer is off initially
  digitalWrite(powerSupplyPin, LOW);

  Serial.begin(9600);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

//IMPORTANT
// Function to draw the time on the OLED display
void displayTime() {
  display.clearDisplay();
  display.setTextSize(1);  // Set text size to 1 to fit everything on one line
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Highlight the selected value
  if (selectedValue == 0) display.print(">"); else display.print(" ");
  display.print(seconds < 10 ? "0" : "");
  display.print(seconds);
  display.print(":");

  if (selectedValue == 1) display.print(">"); else display.print(" ");
  display.print(minutes < 10 ? "0" : "");
  display.print(minutes);
  display.print(":");

  if (selectedValue == 2) display.print(">"); else display.print(" ");
  display.print(hours < 10 ? "0" : "");
  display.print(hours);
  
  display.display();
}

//IMPORTANT
// Function to handle the button inputs and the timer logic
void loop() {
  if (digitalRead(startButtonPin) == LOW) {
    delay(200); // Debounce delay
    timerRunning = !timerRunning;
    Serial.println("Start button pressed, timerRunning: " + String(timerRunning));
    digitalWrite(powerSupplyPin, HIGH);
    Serial.println("Timer started. Power supply turned on.");
  }

  if (digitalRead(incButtonPin) == LOW) {
    delay(200); // Debounce delay
    if (selectedValue == 0) seconds++;
    if (selectedValue == 1) minutes++;
    if (selectedValue == 2) hours++;
    Serial.println("Increase button pressed");
  }

  if (digitalRead(decButtonPin) == LOW) {
    delay(200); // Debounce delay
    if (selectedValue == 0 && seconds > 0) seconds--;
    if (selectedValue == 1 && minutes > 0) minutes--;
    if (selectedValue == 2 && hours > 0) hours--;
    Serial.println("Decrease button pressed");
  }

  if (digitalRead(selectButtonPin) == LOW) {
    delay(200); // Debounce delay
    selectedValue = (selectedValue + 1) % 3;
    Serial.println("Select button pressed, selectedValue: " + String(selectedValue));
  }

  if (timerRunning) {
    delay(1000);
    if (seconds > 0 || minutes > 0 || hours > 0) {
      if (seconds > 0) seconds--;
      else if (minutes > 0) {
        seconds = 59;
        minutes--;
      } else if (hours > 0) {
        seconds = 59;
        minutes = 59;
        hours--;
      }
    } else {
      timerRunning = false;
      digitalWrite(powerSupplyPin, LOW);
      Serial.println("Timer finished. Power supply turned off.");
      //digitalWrite(buzzerPin, HIGH);
      //delay(2500) //in miliseconds
      song(); //remove if you plan on using the default ring above, just remove comment sign
      digitalWrite(buzzerPin, LOW);
      Serial.println("Time's up, buzzer activated");
    }
  } else {
    digitalWrite(buzzerPin, LOW); // Ensure buzzer is off when timer is not running
  }

  displayTime();
}

//OPTIONAL
void song(){ // This is customizable, you can look for more custom songs on https://github.com/robsoncouto/arduino-songs
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzerPin, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzerPin);
    digitalWrite(buzzerPin, LOW);
  }
}
