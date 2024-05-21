#include <SoftwareSerial.h>
#include <Ultrasonic.h>

SoftwareSerial bluetooth(0, 1); // RX, TX pins for Bluetooth module
Ultrasonic ultrasonic(10, 11);

// Define pins for each lane's LEDs
const int lane1Green = 2;
const int lane1Red = 3;
const int lane2Green = 4;
const int lane2Red = 5;
const int lane3Green = 6;
const int lane3Red = 7;

const long minDistance = 7;                   // Minimum distance to trigger the change (in cm)
const unsigned long minDuration = 4000;       // Minimum duration of being within minimum distance (in milliseconds)
const unsigned long greenDuration = 2000;     // 2 seconds
const unsigned long redDuration = 1000;       // 1 second

bool isManualOverride = false;
bool isQueueActive = false;
char queueData = '\0';

void setup()
{
    // Initialize all LED pins as OUTPUT
    pinMode(lane1Green, OUTPUT);
    pinMode(lane1Red, OUTPUT);
    pinMode(lane2Green, OUTPUT);
    pinMode(lane2Red, OUTPUT);
    pinMode(lane3Green, OUTPUT);
    pinMode(lane3Red, OUTPUT);

    Serial.begin(9600);
    bluetooth.begin(9600);
}

void loop()
{
    unsigned long startTime = millis(); // Start time for checking duration
    bool withinRange = false;
    bool lane1GreenOn = false;

    // Initial state of LEDs
    digitalWrite(lane1Green, LOW);
    digitalWrite(lane1Red, HIGH);
    digitalWrite(lane2Green, LOW);
    digitalWrite(lane2Red, HIGH);
    digitalWrite(lane3Green, LOW);
    digitalWrite(lane3Red, HIGH);

    while (millis() - startTime < minDuration)
    {
        long distance = ultrasonic.distanceRead(CM);
        if (distance < minDistance)
        {
            Serial.println("Within range");
            withinRange = true;
            if (!isManualOverride && !lane1GreenOn)
            {
                digitalWrite(lane1Green, HIGH);
                digitalWrite(lane1Red, LOW); // Turn off lane 1 red
                lane1GreenOn = true;
            }
        }
        else
        {
            Serial.println("Out of range");
            withinRange = false;
            lane1GreenOn = false;
            digitalWrite(lane1Red, HIGH); // Turn on lane 1 red
        }

        // Check for Bluetooth data
        if (bluetooth.available())
        {
            char receivedData = bluetooth.read();
            handleBluetoothData(receivedData);
        }

        // Check for Serial Monitor data
        if (Serial.available())
        {
            char receivedData = Serial.read();
            handleBluetoothData(receivedData);
        }

        delay(100); // Check distance and Bluetooth/Serial data every 100 milliseconds
    }

    if (withinRange)
    {
        while (ultrasonic.distanceRead(CM) < minDistance)
        {
            // Wait until distance is greater than 7cm
            delay(100);
        }
        digitalWrite(lane1Green, LOW);
        digitalWrite(lane1Red, HIGH); // Turn on lane 1 red
    }

    // Cycle through traffic light phases unless manual override is active
    if (!isManualOverride)
    {
        cycleTrafficLights();
    }
}

void handleBluetoothData(char data)
{
    switch (data)
    {
    case 'A':
    case 'F':
        if (isQueueActive)
        {
            queueData = data;
        }
        else
        {
            isManualOverride = true;
            digitalWrite(lane1Green, HIGH);
            digitalWrite(lane1Red, LOW);
            delay(5000); // Keep the green light on for 10 seconds
            digitalWrite(lane1Green, LOW);
            digitalWrite(lane1Red, HIGH);
            isManualOverride = false;
            isQueueActive = false;
            if (queueData != '\0')
            {
                char temp = queueData;
                queueData = '\0';
                handleBluetoothData(temp);
            }
        }
        break;
    case 'B':
    case 'G':
        if (isQueueActive)
        {
            queueData = data;
        }
        else
        {
            isManualOverride = true;
            digitalWrite(lane2Green, HIGH);
            digitalWrite(lane2Red, LOW);
            delay(5000); // Keep the green light on for 10 seconds
            digitalWrite(lane2Green, LOW);
            digitalWrite(lane2Red, HIGH);
            isManualOverride = false;
            isQueueActive = false;
            if (queueData != '\0')
            {
                char temp = queueData;
                queueData = '\0';
                handleBluetoothData(temp);
            }
        }
        break;
    case 'C':
    case 'H':
        if (isQueueActive)
        {
            queueData = data;
        }
        else
        {
            isManualOverride = true;
            digitalWrite(lane3Green, HIGH);
            digitalWrite(lane3Red, LOW);
            delay(5000); // Keep the green light on for 10 seconds
            digitalWrite(lane3Green, LOW);
            digitalWrite(lane3Red, HIGH);
            isManualOverride = false;
            isQueueActive = false;
            if (queueData != '\0')
            {
                char temp = queueData;
                queueData = '\0';
                handleBluetoothData(temp);
            }
        }
        break;
    case 'E':
    case 'J':
        isManualOverride = false;
        digitalWrite(lane1Green, LOW);
        digitalWrite(lane2Green, LOW);
        digitalWrite(lane3Green, LOW);
        digitalWrite(lane1Red, HIGH);
        digitalWrite(lane2Red, HIGH);
        digitalWrite(lane3Red, HIGH);
        isQueueActive = false;
        if (queueData != '\0')
        {
            char temp = queueData;
            queueData = '\0';
            handleBluetoothData(temp);
        }
        break;
    default:
        break;
    }
}

void cycleTrafficLights()
{
    digitalWrite(lane1Green, HIGH);
    digitalWrite(lane1Red, LOW);
    delay(greenDuration);
    digitalWrite(lane1Green, LOW);
    digitalWrite(lane1Red, HIGH);
    delay(redDuration);

    digitalWrite(lane2Green, HIGH);
    digitalWrite(lane2Red, LOW);
    delay(greenDuration);
    digitalWrite(lane2Green, LOW);
    digitalWrite(lane2Red, HIGH);
    delay(redDuration);

    digitalWrite(lane3Green, HIGH);
    digitalWrite(lane3Red, LOW);
    delay(greenDuration);
    digitalWrite(lane3Green, LOW);
    digitalWrite(lane3Red, HIGH);
    delay(redDuration);
}
