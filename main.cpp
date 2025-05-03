#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define OLED display parameters
#define SCREEN_WIDTH   128    // OLED display width
#define SCREEN_HEIGHT  64     // OLED display height
#define OLED_RESET     -1     // Reset pin for the OLED display
#define BUTTON_PIN     5      // GPIO pin connected to the button (active-low)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Declare variables for button press count and button status tracking
unsigned long pressCount = 0;        // Counter for button presses
bool lastPressed = false;            // Variable to track the previous button state
char buttonStatus[16] = "Released";  // Button press status text (initially "Released")

/**
 * @brief Checks if the button is pressed.
 * 
 * The button is wired with an internal pull-up resistor, so a LOW signal indicates a press.
 * 
 * @return true if the button is pressed, false otherwise.
 */
bool isButtonPressed() {
  return digitalRead(BUTTON_PIN) == LOW;  // Button is pressed when pin reads LOW (active-low)
}

/**
 * @brief Initializes the system and sets up the OLED display and button pin.
 */
void setup() {
  // Start serial communication at 115200 baud
  Serial.begin(115200);

  // Initialize button pin as input with internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed!"); // If display initialization fails, print an error
    for (;;); // Halt the program if the display cannot be initialized
  }

  // Clear the display and prepare it for drawing
  display.clearDisplay();
  display.display();
}

/**
 * @brief Main loop of the program that continuously checks button state and updates the display.
 */
void loop() {
  // Compute elapsed time in seconds since the board was powered on
  unsigned long totalSeconds = millis() / 1000;
  unsigned int hours = (totalSeconds / 3600) % 24;   // Extract hours from the total seconds
  unsigned int minutes = (totalSeconds / 60) % 60;   // Extract minutes from the total seconds
  unsigned int seconds = totalSeconds % 60;          // Extract seconds from the total seconds

  // Check if the button state has changed (to avoid counting multiple presses for a single press)
  bool pressed = isButtonPressed();
  if (pressed != lastPressed) {
    if (pressed) {
      pressCount++;                   // Increment press count when button is pressed
      strcpy(buttonStatus, "Pressed"); // Update button status text
    } else {
      strcpy(buttonStatus, "Released"); // Update button status text
    }
    lastPressed = pressed;  // Update lastPressed to the current state
  }

  // Update the OLED display
  display.clearDisplay();   // Clear the previous display contents

  // Display the time at the top of the screen
  display.setTextSize(1);                // Set the text size for time display
  display.setTextColor(SSD1306_WHITE);   // Set the text color to white
  display.setCursor(0, 0);               // Set the cursor to the top-left corner
  char timeBuf[9];                       // Buffer to store time as a string (HH:MM:SS format)
  sprintf(timeBuf, "%02u:%02u:%02u", hours, minutes, seconds); // Format time as HH:MM:SS
  display.println(timeBuf);              // Display the formatted time

  // Display the current button status
  display.setCursor(0, 16);              // Move the cursor to the second line
  display.print("Status: ");             // Print the label
  display.println(buttonStatus);         // Print the current button status

  // Display the press count
  display.setCursor(0, 32);              // Move the cursor to the third line
  display.print("Count: ");              // Print the label
  display.println(pressCount);           // Print the current press count

  // Update the OLED display to show the changes
  display.display();

  // Small delay to debounce the button and reduce CPU usage
  delay(100);
}
