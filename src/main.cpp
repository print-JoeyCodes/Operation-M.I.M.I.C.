#include <Arduino.h>
#include <IRremote.hpp>


const int IR_RECEIVE_PIN = 2;
const int IR_SEND_PIN = 3;
const int STATUS_PIN = 7;
const int BUTTON_PIN1 = 8;
const int BUTTON_PIN2 = 9;

struct ButtonState {
    bool state = LOW;
    bool lastReading = LOW;
    unsigned long lastTime = 0;
    bool stableState = LOW;
    bool pressed = false;
};

ButtonState button1;
ButtonState button2;

int debounceDelay = 50;

int menuState = 0;
int mode = 0;
bool printed = false;

uint16_t capturedSignal[100] = {0};
int capturedLength = 0;


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


void fireSignal() {
    IrSender.sendRaw(capturedSignal, capturedLength, 38);
}




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



void loop() {

    buttonPress(BUTTON_PIN1);
    buttonPress(BUTTON_PIN2);

    if(button1.state) {
        if(!button1.pressed){
            button1.pressed = true;
            if(mode == 0) {
                if (menuState > 2) {
                    menuState = 0;
                }
                else {
                    menuState++;
                }
            }
            if(mode == 2) {
                Serial.print("Signal Fired");
                fireSignal();
            }
            printed = false;
        }
    }
    else {
        button1.pressed = false;
    }

    if(button2.state){
        if(!button2.pressed){
            button2.pressed = true;
            if (mode == 1){
                Serial.println("checks");
                mode = 0;
                menuState = 0;
            }
            if(menuState == 1) {
                mode = 1;
            }
            else if (menuState == 2) {
                mode = 2;
            }
            else if (menuState == 3) {
                mode = 3;
            }
            printed = false;
        }
    }
    else {
        button2.pressed = false;
    }

    if(mode == 0){
        if(menuState == 0 && !printed) {
            Serial.println("MAIN MENU");
            Serial.println("============");
            Serial.println("Capture Signal");
            Serial.println("Fire Signal");
            Serial.println("Enter Storage");
            Serial.println("");
            printed = true;
        }
        else if(menuState == 1 && !printed) {
            Serial.println("MAIN MENU");
            Serial.println("============");
            Serial.println("-> Capture Signal");
            Serial.println("Fire Signal");
            Serial.println("Enter Storage");
            Serial.println("");
            printed = true;
        }
        else if (menuState == 2 && !printed) {
            Serial.println("MAIN MENU");
            Serial.println("============");
            Serial.println("Capture Signal");
            Serial.println("-> Fire Signal");
            Serial.println("Enter Storage");
            Serial.println("");
            printed = true;
        }
        else if (menuState == 3 && !printed) {
            Serial.println("MAIN MENU");
            Serial.println("============");
            Serial.println("Capture Signal");
            Serial.println("Fire Signal");
            Serial.println("-> Enter Storage");
            Serial.println("");
            printed = true;
        }
    }
    else if(mode == 1 && !printed) {
        Serial.println("Capture Mode Activated");
        //IrReceiver.resume();
        printed = true;
    }
    else if(mode == 2 && !printed){
        Serial.println("Fire Mode Activated");
        printed = true;
    }

    

    

    if(mode == 1){
        // IR Receiver Gets Signal
        if (IrReceiver.decode()) {
            int len = IrReceiver.decodedIRData.rawlen;
            if (len > 100) len = 100;
            int start = (IrReceiver.irparams.rawbuf[0] == 0) ? 1 : 0;
            capturedLength = 0;
            for (int i = start; i < IrReceiver.decodedIRData.rawlen && capturedLength < 100; i++) {
                capturedSignal[capturedLength++] = IrReceiver.irparams.rawbuf[i] * 50;
            }
            Serial.println("Signal Captured - ");
            Serial.print(capturedLength);
            Serial.print(" entries");
            IrReceiver.resume();
        }
        
    }

}