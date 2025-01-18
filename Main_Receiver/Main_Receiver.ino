#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// Define motor driver pins for Motor Set 1 (IN1 and IN2)
#define IN1 12   // IN1 pin connected to GPIO 12
#define IN2 13   // IN2 pin connected to GPIO 13


// Define motor driver pins for Motor Set 2 (IN3 and IN4)
#define IN3 4    // IN3 pin connected to GPIO 4
#define IN4 25   // IN4 pin connected to GPIO 25

// OLED display object
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Variable to store the incoming data
String camData;

void moveMotorSet1Forward() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 80);
}

void moveMotorSet2Forward() {
analogWrite(IN3, 0);
  analogWrite(IN4, 80);
}

void stopMotors() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}

// Callback function executed when data is received
void onDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
    // Convert the incoming data to a String
    camData = String((char*)incomingData).substring(0, len);
    
    delay(200);

    Serial.print("Message received: ");
    Serial.println(camData);

    // Optionally print the MAC address of the sender
    Serial.print("From MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", info->src_addr[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();

    // Display the incoming message on the OLED
    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SH110X_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.println(camData); // Print the message
    display.display();  // Display the buffer

    if(camData == "Nothing" || camData == "GREEN LIGHT")
    {
      moveMotorSet1Forward();
      moveMotorSet2Forward();
    }

    if(camData == "RED LIGHT" || camData == "YELLOW LIGHT")
    {
      stopMotors();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-NOW Receiver");

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // Initialize the display
    if (!display.begin(0x3C, true)) { // Default I2C address for SH1106
        Serial.println("SH1106 allocation failed");
        while (1); // Don't proceed, loop forever
    }
    display.display();
    delay(1000); // Pause for 1 second

    // Clear the buffer
    display.clearDisplay();

    // Initialize Wi-Fi in Station Mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register receive callback
    esp_now_register_recv_cb(onDataRecv);

    Serial.println("ESP-NOW Receiver Ready");
}

void loop() {
    // Nothing to do here, everything is handled in the callback
}
