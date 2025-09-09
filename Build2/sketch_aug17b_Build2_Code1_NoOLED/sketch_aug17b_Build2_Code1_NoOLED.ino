#define trigPin 9
#define echoPin 10

const int healthButtonPin = 4;
const int entryButtonPin = 5;
const int exitButtonPin = 6;

const int redLED = 2;
const int greenLED = 3;

long duration;
int distance;
int peopleCount = 0;

bool healthCheckPassed = false;
bool personDetected = false;
bool canEnter = false;

unsigned long lastDetectionTime = 0;
unsigned long detectionCooldown = 6000; // 3 sec

const int MAX_CAPACITY = 10;

void setup() {
  pinMode(healthButtonPin, INPUT_PULLUP);
  pinMode(entryButtonPin, INPUT_PULLUP);
  pinMode(exitButtonPin, INPUT_PULLUP);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Serial.println("System Initialized");
  Serial.print("Current People Count: ");
  Serial.println(peopleCount);
}

bool isPersonApproaching() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  return (distance > 0 && distance < 50);
}


void printOccupancyStatus() {
  Serial.print("Current People Count: ");
  Serial.println(peopleCount);

  float occupancyRate = (float)peopleCount / MAX_CAPACITY;

  if (occupancyRate <= 0.4) {
    Serial.println("Occupancy: LOW");
  } else if (occupancyRate <= 0.8) {
    Serial.println("Occupancy: MEDIUM");
  } else {
    Serial.println("Occupancy: HIGH");
  }
}


void loop() {
  unsigned long currentTime = millis();

  // Check for person approach only every 3 sec
if (currentTime - lastDetectionTime > detectionCooldown) {
  if (isPersonApproaching()) {
    lastDetectionTime = currentTime;

    if (peopleCount >= MAX_CAPACITY) {
      Serial.println("Individual approaching");
      Serial.println("Maximum Capacity Reached. Access Denied.");
      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
      // Block entry completely
      personDetected = false;
      canEnter = false;
      healthCheckPassed = false;
    } else {
      personDetected = true;
      canEnter = true;
      Serial.println("Individual approaching");
    }
  }
}


  // Health check button
  if (digitalRead(healthButtonPin) == LOW) {
    healthCheckPassed = true;
    Serial.println("Health Check Passed.");
    delay(300); // debounce
  }

  // Entry logic
  if (digitalRead(entryButtonPin) == LOW) {
    if (canEnter) {
      if (peopleCount >= MAX_CAPACITY) {
        Serial.println("Maximum Capacity Reached. Access Denied.");
        digitalWrite(redLED, HIGH);
        digitalWrite(greenLED, LOW);
      } else if (healthCheckPassed) {
        peopleCount++;
        Serial.println("Access Granted.");
        printOccupancyStatus();
        digitalWrite(greenLED, HIGH);
        digitalWrite(redLED, LOW);
        healthCheckPassed = false;
      } else {
        Serial.println("Health Check Failed. Access Denied.");
        digitalWrite(redLED, HIGH);
        digitalWrite(greenLED, LOW);
      }
      delay(300);  // debounce
      canEnter = false;
      personDetected = false;
    } else {
      Serial.println("No recent person detected. Ignoring entry.");
    }
  }

  // Exit logic
  if (digitalRead(exitButtonPin) == LOW) {
    if (peopleCount > 0) {
      peopleCount--;
      Serial.println("Exit Button Pressed.");
      printOccupancyStatus();
    } else {
      Serial.println("Exit Pressed but no one inside.");
    }
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, LOW);
    delay(600);  // debounce
  }
}
