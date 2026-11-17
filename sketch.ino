#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define ENTRYSERVO_PIN 2
#define EXITSERVO_PIN 3
#define TOUCH_PIN 4

Servo entryServo;
Servo exitServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
int sensorValue;
int total;

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

const int coinValues[9] = {1, 2, 5, 10, 20, 0, 50, 100, 200};

void setup() {
    // Serial.begin(9600);

    lcd.init();
    lcd.backlight();

    entryServo.attach(ENTRYSERVO_PIN);
    entryServo.write(90);
    exitServo.attach(EXITSERVO_PIN);
    exitServo.write(90);

    pinMode(TOUCH_PIN, INPUT);

    printWelcome();

    total = 0;
}

void loop() {
    char key = keypad.getKey();
    sensorValue = digitalRead(TOUCH_PIN);

    if (sensorValue == HIGH) coinOn();

    if (key == 'A') printTotal(0);

    if (key == 'D') retrieveCoins();
}

void coinOn() {

    int coinValue;

    lcd.setCursor(0, 0);
    lcd.print("Coin detected   ");
    lcd.setCursor(0, 1);
    lcd.print("enter value     ");

    while (sensorValue == HIGH) {
        sensorValue = digitalRead(TOUCH_PIN);

        coinValue = enterValue();

        if (coinValue) {

            lcd.setCursor(0, 0);
            lcd.print("Coin registered ");
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            printEuros(coinValue);

            entryServo.write(15);
            while (sensorValue == HIGH) {
                sensorValue = digitalRead(TOUCH_PIN);
            }
            entryServo.write(90);

            total += coinValue;
            delay(1000);

            printTotal(2000);
        } else printWelcome();
    }
}

int enterValue() {
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
            if (sensorValue != 1) return 0;
        }

        if (key == 'B' || key == 'C' || key == 'D' || key == '#' || key == '*' || key == '0') key = '\0';

        if (key == '6') {
            if (page == 0) {
                lcd.setCursor(0, 0);
                lcd.print("  6:<-   7:50c  ");
                lcd.setCursor(0, 1);
                lcd.print(" 8:1eur   9:2eur");
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

    if (key == 'A') {
        lcd.setCursor(0, 0);
        lcd.print("Please remove   ");
        lcd.setCursor(0, 1);
        lcd.print("coin            ");
        while (sensorValue == HIGH) {
            sensorValue = digitalRead(TOUCH_PIN);
        }
        return 0;
    }
    
    return coinValues[key-'0'-1];
}

void retrieveCoins() {
    exitServo.write(0);

    lcd.setCursor(0, 0);
    lcd.print("Piggy Bank open ");
    lcd.setCursor(0, 1);
    lcd.print("Please take cash");

    char key = '\0';
    while (key != 'A') {
        key = keypad.getKey();
    }

    total = 0;
    exitServo.write(90);
    printWelcome();
}

void printEuros(int cents) {
    lcd.print(cents/100);
    lcd.print(".");
    int rest = cents%100;
    if (rest < 10) lcd.print("0");
    lcd.print(rest);
    lcd.print(" eur");
}

void printTotal(int timer) {
    
    if (timer) {
        lcd.setCursor(0, 0);
        lcd.print("Total:          ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        printEuros(total);
        
        delay(timer);
    }

    else {
        char key = '\0'; 

        lcd.setCursor(0, 0);
        lcd.print("Total:          ");
        lcd.setCursor(11,0);
        lcd.print("A:ret");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        printEuros(total);

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
