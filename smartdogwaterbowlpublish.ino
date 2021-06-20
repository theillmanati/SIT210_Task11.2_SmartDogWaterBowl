// --------------------
// Smart Dog Water Bowl
// --------------------

/*
   This program detects the moisture reading of the water bowl and depending 
   if the value is above or below a threshold publishes an event to be subscribed
   to by a Particle Photon.
   Uses a DFRobot Capacitive Soil Moisture Sensor connected to a Particle Argon.
   
   Author: Jamie Illman
*/

// Constants
#define DRY_THRESHOLD       10      // The percentage the waterbowl will be considered to not be dry
#define SENSOR_PIN          A0      // Capacitive soil moisture sensor wired to A0 (aswell as GND and 3v3)
#define GREEN_LED_PIN       D6      // Green LED wired to D6 (aswell as GND)
#define RED_LED_PIN         D5      // Red LED wired to D5 (aswell as GND)
#define READING_INTERVAL    1000    // Device will read moisture level every 1 second
#define PUBLISH_INTERVAL    60000   // Device will publish moisture level every minute
#define BOARD_LED           D7      // Little blue LED om the Argon next to USB jack

// Variables
bool wasDry = false;    // Boolean of wasDry starts as being false
int moisture;           // Integer for moisture reading
unsigned long lastPublishTime = 0;  // Determines length of time between publishing of data

// The setup function is a standard part of any microcontroller program.
// It runs only once when the device boots up or is reset.
void setup() {
    // The setup function tells our device that A0 (which we named SENSOR_PIN) is going to be input
    // (That means that we will be monitoring voltage that comes from them, rather than sending voltage to them)
    pinMode(SENSOR_PIN, INPUT);
    // The setup function tells our device that D5, D6 and D7 (which we named GREEN_LED_PIN, RED_LED-PIN and BOARD_LED respectively) are going to all be output
    // (That means that we will be sending voltage to them, rather than monitoring voltage that comes from them)
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BOARD_LED, OUTPUT);
    
    Serial.begin(9600);
    Particle.variable("moisture", moisture);
    // The setup function also tells our device to subscribe to the event being sent by the Particle Photon called "solenoidControl"
    Particle.subscribe("solenoidControl", subscriptionHandler);
    
    
    // Takes an intial reading. This was added as neither LED was on until a change in isMoist boolean occured previously.
    int moistureValue = analogRead(SENSOR_PIN);
    // Map these values to a percentage of how full. In this case when water bowl was full moisture reading was 2190 and when dry it was 3220.
    int moisturePercentage = map(moistureValue, 2190, 3220, 100, 0);
    
    moisture = moisturePercentage;
    
    if (isMoist(moisturePercentage)) {
        wasDry = false;
        Particle.publish("moisture", "full");
    } else {
        wasDry = true;
        Particle.publish("moisture", "dry");
    }
}
    
void loop() {
    // Take the reading
    int moistureValue = analogRead(SENSOR_PIN);
    // Print to serial so you can keep track, particularly when determining moisture values from sensor when dry and when water bowl is full.
    Serial.print("Moisture Level: ");
    Serial.println(moistureValue);
    Serial.print(Time.hourFormat12());
    Serial.print(":");
    Serial.print(Time.minute());
    Serial.print(":");
    Serial.print(Time.second());
    // Map these values to a percentage of how full. In this case when water bowl was full moisture reading was 2190 and when dry it was 3220,
    int moisturePercentage = map(moistureValue, 2190, 3220, 100, 0);
    
    moisture = moisturePercentage;
    
    unsigned long now = millis();
    
    if (isMoist(moisturePercentage) && wasDry) {    // If wasDry and !wasDry are not also added, device will publish events every 5 seconds, instead this only piublishes events when wasDry boolean changes between true and false
        wasDry = false;
        digitalWrite(GREEN_LED_PIN, HIGH);  // Green LED turns on when bowl is not dry
        digitalWrite(RED_LED_PIN, LOW);     // Red LED turns off when bowl is not dry
        // Send notification that waterbowl is full to Particle Console, which can be subscribed to by Particle Photon
            Particle.publish("moisture", "full");
            lastPublishTime = lastPublishTime + PUBLISH_INTERVAL;
    } else if (!isMoist(moisturePercentage) && !wasDry) {
        wasDry = true;
        digitalWrite(GREEN_LED_PIN, LOW);   // Green LED turns off when bowl is dry
        digitalWrite(RED_LED_PIN, HIGH);    // Red LED turns on when bowl is dry
        // Send notification that waterbowl is dry to Particle Console, which can be subscribed to by Particle Photon
            Particle.publish("moisture", "dry");
            lastPublishTime = lastPublishTime + PUBLISH_INTERVAL;
    }
    // ThingSpeak
    const char * eventName = "thingSpeakMoisturePercentageReading_";
    // ThingSpeak Channel information
    unsigned long myChannelNumber =XXXXXXX;
    const char * myWriteAPIKey = "XXXXXXXXXXXXXXXX";
    Particle.publish(eventName, "{\"Moisture Percentage\": \"" + String(moisture) + "\", \"key\": \"" + myWriteAPIKey + "\" }", PRIVATE, NO_ACK);
    delay(PUBLISH_INTERVAL);    // Moisture level read every minute
}

// Function to return true if the sensor is sensing moisture below 10 % full (as defined at start of code)
bool isMoist(int moisturePercentage) {
    return moisturePercentage >= DRY_THRESHOLD;
}

// Subscription Handler Function
void subscriptionHandler(const char *event, const char *data) {
    if (strcmp(data, "off")==0) {   // If event "off" is published by Particle Photon
        digitalWrite(BOARD_LED, LOW); // Blue LED next to USB jack turns off to inform us water has stopped flowing
    }
    else if (strcmp(data, "on")==0) {   // If event "on" is published by Particle Photon
        digitalWrite(BOARD_LED, HIGH);  // Blue LED next to USB jack turns on to inform us water is flowing
    }
}
