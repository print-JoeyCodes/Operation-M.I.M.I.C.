#include <Arduino.h>
#include <IRremote.hpp>


const int IR_RECEIVE_PIN = 2;   // VS1838B OUT pin
const int IR_SEND_PIN    = 3;   // IR LED driver (MUST be 3 on Uno)



uint16_t replayData[] = {1600, 750, 700, 800, 350, 450, 750, 400, 450, 750, 400, 800, 350, 800, 350, 850, 350, 400, 450, 350, 450, 350, 850, 350, 450, 350, 450, 350, 400, 400, 400, 800, 750, 400, 350, 450, 400, 800, 750, 750, 350};int replayLength = 0;   // Will be auto-calculated in setup()


void setup() {
    Serial.begin(115200);
    delay(500);

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    IrSender.begin(IR_SEND_PIN);

    // Auto-calculate array length from whatever you pasted above
    replayLength = sizeof(replayData) / sizeof(replayData[0]);

    Serial.println();
    Serial.println("==========================================");
    Serial.println("   OPERATION M.I.M.I.C. — IR TOOL");
    Serial.println("==========================================");
    Serial.println("Commands:");
    Serial.println("  Any key  -> Fire the replay array");
    Serial.println("  'c'      -> Clear/re-arm receiver");
    Serial.println();
    Serial.print("Replay array loaded: ");
    Serial.print(replayLength);
    Serial.println(" entries");
    Serial.println("------------------------------------------");
    Serial.println("Point gun at receiver and pull trigger.");
    Serial.println("==========================================");
    Serial.println();
}


void loop() {

    // ---------- CAPTURE MODE ----------
    if (IrReceiver.decode()) {

        // Print protocol info for diagnostics
        Serial.print(">> Protocol: ");
        Serial.print(IrReceiver.decodedIRData.protocol);
        Serial.print(" | Bits: ");
        Serial.print(IrReceiver.decodedIRData.rawlen);
        Serial.println(" timings");

        // Print the array in copy-paste-ready format
        Serial.println();
        Serial.println("--- COPY BELOW THIS LINE ---");
        Serial.print("uint16_t replayData[] = {");
        for (int i = 0; i < IrReceiver.decodedIRData.rawlen; i++) {
            unsigned int duration = IrReceiver.irparams.rawbuf[i] * 50;
            Serial.print(duration);
            if (i < IrReceiver.decodedIRData.rawlen - 1) Serial.print(", ");
        }
        Serial.println("};");
        Serial.println("--- COPY ABOVE THIS LINE ---");
        Serial.println();

        // ALWAYS resume to clear buffer for next signal
        IrReceiver.resume();
    }

    // ---------- REPLAY MODE ----------
    if (Serial.available()) {
        char cmd = Serial.read();

        // Clear any extra newline/carriage return characters
        while (Serial.available()) Serial.read();

        if (cmd == 'c' || cmd == 'C') {
            Serial.println(">> Receiver re-armed.");
            return;
        }

        // Fire the replay array
        if (replayLength > 0 && replayData[0] != 0) {
            Serial.println(">> FIRING DEATH RAY...");
            IrSender.sendRaw(replayData, replayLength, 38);
            Serial.println(">> Sent.");
        } else {
            Serial.println(">> No replay data loaded. Capture first, then paste into the sketch.");
        }
    }
}