// ---------------------------------------
// Smart Dog Water Bowl - Solenoid Control
// ---------------------------------------

/*
   This program subscribes to the events published by the Particle Argon and controls a relay 
   connected to a solenoid valve.
   Uses a 12V 1/2" solenoid valve connected to a DFRobot 2-port relay board connected to a Particle Photon.
   
   Author: Jamie Illman
*/

// Constants
// Define relay name and pin
#define RELAY_PIN          D6 // Relay will be wired to D6 (aswell as GND and 3V#)
#define BOARD_LED          D7 // Little blue LED om the Photon next to D7

// The setup function is a standard part of any microcontroller program.
// It runs only once when the device boots up or is reset.
void setup() {
    
    // The setup function tells our device that D6 and D7 (which we named RELAY_PIN and BOARD_LED respectively) are going to both be output
    // (That means that we will be sending voltage to them, rather than monitoring voltage that comes from them)
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BOARD_LED, OUTPUT); 
    
    // The setup function also tells our device to subscribe to the event being sent by the Particle Argon called "moisture"
    Particle.subscribe("moisture", subscriptionHandler);
}

// Next we have the loop function, the other essential part of a microcontroller program.
// This is blank as its dealt with by subscription handler function.
void loop() {
    // Intentionally left blank
}

// Subscription Handler Function
void subscriptionHandler(const char *event, const char *data) {
    if (strcmp(data, "dry")==0) { // If event "dry" is published by Particle Argon
        digitalWrite(RELAY_PIN, HIGH); // Relay turns on, causing solenoid to open, allowing water through
        digitalWrite(BOARD_LED, HIGH); // Blue LED next to D7 on Photon turns on to inform us water is flowing
        Particle.publish("solenoidControl", "on"); // Publishes event, "on", to be read by Particle Console, which is subscribed to by Particle Argon
        delay (4000); // Used to stop water flow after 4 seconds in case of error with capacitive soil water sensor, which would lead to flooding.
        digitalWrite(RELAY_PIN, LOW); // Relay turns off, causing solenoid to shut, thereby stopping water flow
        digitalWrite(BOARD_LED, LOW); // Blue LED turns off to inform us water has stopped flowing
    }
    else if (strcmp(data, "full")==0) { // If event "full" is published by Particle Argon
        digitalWrite(RELAY_PIN, LOW); // If water flow stops before 4 s delay, relay turns off, causing solenoid to shut, thereby stopping water flow
        digitalWrite(BOARD_LED, LOW); // Blue LED turns off to inform us water has stopped flowing
        Particle.publish("solenoidControl", "off"); // Publishes event, "off", to be read by Particle Console, which is subscribed to by Particle Argon
    }
}
