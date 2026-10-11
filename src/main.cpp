#include <Arduino.h>
#include <IRremote.hpp>


const int IR_RECEIVE_PIN = 2;
const int IR_SEND_PIN = 3;
const int STATUS_PIN = 7;
const int BUTTON_PIN1 = 8;
const int BUTTON_PIN2 = 9;






void setup() {
    Serial.begin(115200);
    delay(500);

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    IrSender.begin(IR_SEND_PIN);
    pinMode(STATUS_PIN, OUTPUT);
    pinMode(BUTTON_PIN1, INPUT);
    pinMode(BUTTON_PIN2, INPUT);

    Serial.print("Operation M.I.M.I.C. INITIALIZED");
    digitalWrite(STATUS_PIN, HIGH);
    delay(1000);
    digitalWrite(STATUS_PIN, LOW);

}                           






struct ButtonState {
    bool state = LOW;
    bool lastReading = LOW;
    unsigned long lastTime = 0;
    bool stableState = LOW;
};

ButtonState button1;
ButtonState button2;

int debounceDelay = 50;


void buttonPress(int pin) {
    ButtonState* button;
    if(pin == BUTTON_PIN1) {
        button = &button1;
    }
    else {
        button = &button2;
    }
    bool reading = digitalRead(pin);
    if(reading != button->lastReading) {
        button->lastTime = millis();
    }

    if ((millis() - button->lastTime) > debounceDelay) {
        if(reading != button->stableState){
            button->stableState = reading;
            if(button->stableState == HIGH) {
                button->state = HIGH;
            }
            else {
                button->state = LOW;
            }
        }
    }
    button->lastReading = reading;
}


int menuState = 0;
bool printed = false;
bool pressed = false;

void loop() {

    buttonPress(BUTTON_PIN1);
    buttonPress(BUTTON_PIN2);

    if(button1.state) {
        if(!pressed){
            pressed = true;
            if (menuState > 1) {
                menuState = 0;
            }
            else {
                menuState++;
            }
            printed = false;
        }
    }
    else {
        pressed = false;
    }

    if(menuState == 0 && !printed) {
        Serial.println("MAIN MENU");
        Serial.println("============");
        Serial.println("-> Capture Signal");
        Serial.println("Fire Signal");
        Serial.println("Enter Storage");
        Serial.println("");
        printed = true;
    }
    else if (menuState == 1 && !printed) {
        Serial.println("MAIN MENU");
        Serial.println("============");
        Serial.println("Capture Signal");
        Serial.println("-> Fire Signal");
        Serial.println("Enter Storage");
        Serial.println("");
        printed = true;
    }
    else if (menuState == 2 && !printed) {
        Serial.println("MAIN MENU");
        Serial.println("============");
        Serial.println("Capture Signal");
        Serial.println("Fire Signal");
        Serial.println("-> Enter Storage");
        Serial.println("");
        printed = true;
    }

    // IR Receiver Gets Signal
    if (IrReceiver.decode()) {
        IrReceiver.resume();
    }

    // Send IR Signal
    if (Serial.available()) {

    }
}