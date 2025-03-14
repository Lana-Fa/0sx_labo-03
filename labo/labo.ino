#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
#include <math.h>

#define THERMISTOR_PIN A0  
#define LED_PIN 8          
#define JOY_X A1           
#define JOY_Y A2           
#define BUTTON_PIN 2       
#define R_FIXED 10000.0    
#define BETA 3950 

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastMillis = 0;
bool affichePage1 = true;
unsigned long lastButtonPress = 0;

byte customChar[8] = {
    0b11111,
    0b10001,
    0b11111,
    0b10001,
    0b11111,
    0b0010,
    0b0100,
    0b01111
};

float lireTemperature(int valeurAnalogique) {
    if (valeurAnalogique == 0) return -273.15;
    float resistance = R_FIXED * (1023.0 / valeurAnalogique - 1.0);
    if (resistance <= 0) return -273.15;
    float temperature = 1.0 / (log(resistance / R_FIXED) / BETA + 1.0 / 298.15) - 273.15;
    return temperature;
}

void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();
    
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    lcd.createChar(0, customChar);
    
    lcd.setCursor(0, 0);
    lcd.print("Nom: Soro");
    lcd.setCursor(0, 1);
    lcd.write(byte(0));  
    lcd.setCursor(14, 1);
    lcd.print("84");
    delay(3000);
    lcd.clear();
}

void loop() {
    unsigned long currentMillis = millis();
    
    int thermistorValue = analogRead(THERMISTOR_PIN);
    Serial.print("Valeur brute thermistance : ");
    Serial.println(thermistorValue);
    
    float temperature = lireTemperature(thermistorValue);

    int joyX = analogRead(JOY_X);
    int joyY = analogRead(JOY_Y);

    int vitesse = map(joyY, 0, 1023, -25, 120);
    char direction = (joyX < 512) ? 'G' : 'D';

    if (temperature > 30) {
        digitalWrite(LED_PIN, HIGH);
    } else if (temperature < 29) {
        digitalWrite(LED_PIN, LOW);
    }

    if (digitalRead(BUTTON_PIN) == LOW && millis() - lastButtonPress > 300) {
        affichePage1 = !affichePage1;
        lastButtonPress = millis();
    }

    lcd.clear();
    if (affichePage1) {
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(temperature);
        lcd.print(" C");
        
        lcd.setCursor(0, 1);
        lcd.print("AC: ");
        lcd.print((temperature > 30) ? "ON " : "OFF");
    } else {
        lcd.setCursor(0, 0);
        lcd.print((vitesse < 0) ? "Recule: " : "Avance: ");
        lcd.print(abs(vitesse));
        lcd.print(" km/h");
        
        lcd.setCursor(0, 1);
        lcd.print("Direction: ");
        lcd.print(direction);
    }

    if (currentMillis - lastMillis >= 100) {
        lastMillis = currentMillis;
        Serial.print("etd:2412384,x:");
        Serial.print(joyX);
        Serial.print(",y:");
        Serial.print(joyY);
        Serial.print(",sys:");
        Serial.println(digitalRead(LED_PIN));
    }

    delay(100);
}
