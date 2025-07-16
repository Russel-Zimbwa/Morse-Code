#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Timings
const unsigned int dotThreshold = 300;
const unsigned int dashThreshold = 1000;
const unsigned int letterPause = 1200;
const unsigned int wordPause = 2500;  // New: Pause time to consider space

// States
bool space= false;
bool buttonPressed = false;
unsigned long pressStartTime = 0;
unsigned long pressDuration = 0;
unsigned long lastInputTime = 0;

// Morse buffers
String sMorse = "";
String Word = "";

const int buttonPin = 6;
const int buttonclr =7;

struct MorseSymbol {
  char letter;
  const char* code;
};



// Morse table
MorseSymbol morseTable[] = {
  {'A', ".-"}, {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},
  {'E', "."}, {'F', "..-."}, {'G', "--."}, {'H', "...."},
  {'I', ".."}, {'J', ".---"}, {'K', "-.-"}, {'L', ".-.."},
  {'M', "--"}, {'N', "-."}, {'O', "---"}, {'P', ".--."},
  {'Q', "--.-"}, {'R', ".-."}, {'S', "..."}, {'T', "-"},
  {'U', "..-"}, {'V', "...-"}, {'W', ".--"}, {'X', "-..-"},
  {'Y', "-.--"}, {'Z', "--.."},
  {'1', ".----"}, {'2', "..---"}, {'3', "...--"}, {'4', "....-"},
  {'5', "....."}, {'6', "-...."}, {'7', "--..."}, {'8', "---.."},
  {'9', "----."}, {'0', "-----"},
};

char morseToChar(String morse) {
  for (int i = 0; i < sizeof(morseTable) / sizeof(morseTable[0]); i++) {
    if (morse.equals(morseTable[i].code)) {
      return morseTable[i].letter;
    }
  }
  return '?';
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(13, OUTPUT);
pinMode(buttonclr, INPUT_PULLUP);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse Decoder");
  delay(3000);
  lcd.clear();
   space= false;
}

void loop() {
  int sensorVal = digitalRead(buttonPin);
	int sensorclr= digitalRead(buttonclr);
  
  //Button clear
if (sensorclr == LOW) {
  lcd.clear();
  Word = "";
  sMorse = "";
  space = false;
  delay(500);  // debounce
}
  
  
  // Button pressed
  if (sensorVal == LOW && !buttonPressed) {
    buttonPressed = true;
    pressStartTime = millis();
    digitalWrite(13, HIGH);
  }

  // Button released
  if (sensorVal == HIGH && buttonPressed) {
    buttonPressed = false;
    pressDuration = millis() - pressStartTime;
    digitalWrite(13, LOW);

    if (pressDuration <= dotThreshold) {
      sMorse += ".";
    } else if (pressDuration <= dashThreshold) {
      sMorse += "-";
    }

    lastInputTime = millis();
  }

  unsigned long timeSinceLastInput = millis() - lastInputTime;

  // Letter detection
  if (sMorse.length() > 0 && timeSinceLastInput > letterPause) {
    char result = morseToChar(sMorse);

    Serial.print("Morse: ");
    Serial.print(sMorse);
    Serial.print(" -> ");
    Serial.println(result);

    Word += result;  // Add to word

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Code: " + sMorse);
    lcd.setCursor(0, 1);
    lcd.print("Word: " + Word);

    sMorse = "";
     space= false;
    delay(200);  // Slight delay so user can see transition
  }

  
  
  // Word gap detection
  if (Word.length() > 0 && sMorse.length() == 0 && timeSinceLastInput > wordPause&&!space) {
    Word += ' ';
    Serial.println("Space detected");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Code: ");
    lcd.setCursor(0, 1);
    lcd.print("Word: " + Word);
    delay(500);
    lastInputTime = millis(); // Prevent multiple spaces
    space= true;
    
  }

  delay(10);
}
