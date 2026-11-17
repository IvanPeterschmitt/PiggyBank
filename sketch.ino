#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define ENTRYSERVO_PIN 2
#define EXITSERVO_PIN 3
#define TOUCH_PIN 4

Servo enteryServo;
Servo exitServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
int sensorValue;
float total;

const byte ROWS = 4; // Number of rows
const byte COLS = 4; // Number of columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {5, 6, 7, 8};   // Connect to R1, R2, R3, R4
byte colPins[COLS] = {9, 10, 11, 12};   // Connect to C1, C2, C3, C4
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const float coinValues[9] = {0.01, 0.02, 0.05, 0.10, 0.20, 0, 0.50, 1.00, 2.00};

void setup() {
    Serial.begin(9600);

    lcd.init();                    // au lieu de lcd.begin(16, 2)
    lcd.backlight();

    enteryServo.attach(ENTRYSERVO_PIN);
    enteryServo.write(90);
    exitServo.attach(EXITSERVO_PIN);
    exitServo.write(90);

    pinMode(TOUCH_PIN, INPUT);

    printWelcome();

    total = 0.0;
}

void loop() {
    char key = keypad.getKey();
    sensorValue = digitalRead(TOUCH_PIN);

    if (sensorValue == 1) coinOn();

    if (key == 'A') printTotal(0);

    if (key == 'D') retrieveCoins();
}

void coinOn() {

    float coinValue;

    lcd.setCursor(0, 0);
    lcd.print("Coin detected   ");
    lcd.setCursor(0, 1);
    lcd.print("enter value     ");

    while (sensorValue == 1) {
        sensorValue = digitalRead(TOUCH_PIN);

        coinValue = enterValue();

        if (coinValue) {
            total += coinValue;

            lcd.setCursor(0, 0);
            lcd.print("Coin registered ");
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            lcd.print(coinValue);

            enteryServo.write(15);
            while (sensorValue == 1) {
                sensorValue = digitalRead(TOUCH_PIN);
            }
            enteryServo.write(90);

            delay(1000);

            printTotal(2000);
        } else printWelcome();
    }
}

float enterValue() {
    delay(1000);
    int page = 0;

    lcd.setCursor(0, 0);
    lcd.print("1:1c  2:2c  3:5c");
    lcd.setCursor(0, 1);
    lcd.print("4:10c 5:20c 6:->");

    char key = keypad.getKey();
    while (key != 'A' && key != '1' && key != '2' && key != '3' && key != '4' && key != '5' && key != '7' && key != '8' && key != '9') {
        while (!key) {
            key = keypad.getKey();

            sensorValue = digitalRead(TOUCH_PIN);
            if (sensorValue != 1) return 0.0;
        }

        if (key == '6') {
            if (page == 0) {
                lcd.setCursor(0, 0);
                lcd.print("  6:<-   7:50c  ");
                lcd.setCursor(0, 1);
                lcd.print(" 8:1eu    9:2eu ");
            } else {
                lcd.setCursor(0, 0);
                lcd.print("1:1c  2:2c  3:5c");
                lcd.setCursor(0, 1);
                lcd.print("4:10c 5:20c 6:->");
            }
            page = (page + 1) % 2;
            key = '\0';
        }
    }

    if (key == 'A') return 0.0;
    
    return coinValues[key-'0'-1];
}

void retrieveCoins() {
    exitServo.write(0);

    lcd.setCursor(0, 0);
    lcd.print("Piggy Bank open ");
    lcd.setCursor(0, 1);
    lcd.print("Please take cash");

    char key;
    while (key != 'A') {
        key = keypad.getKey();
    }

    total = 0.0;
    exitServo.write(90);
    printWelcome();
}

void printTotal(int timer) {
    Serial.println(total);

    if (timer) {
        lcd.setCursor(0, 0);
        lcd.print("Total:          ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(total);
        lcd.setCursor(6,1);
        lcd.print("eur");
        
        delay(timer);
    }

    else {
        char key; 

        lcd.setCursor(0, 0);
        lcd.print("Total:          ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(total);
        lcd.setCursor(6,1);
        lcd.print("eur");
        lcd.setCursor(11,0);
        lcd.print("A:ret");

        while (key != 'A') {
            key = keypad.getKey();
        }
    }

    printWelcome();
}

void printWelcome () {
    lcd.setCursor(0, 0);
    lcd.print("Welcome to your ");
    lcd.setCursor(0, 1);
    lcd.print("Piggy Bank      ");
}
