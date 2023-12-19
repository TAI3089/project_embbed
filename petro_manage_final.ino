#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>
#include <DHT.h>

// Wi-Fi credentials
const char* ssid = "iPhone";
const char* password = "tttt6666";

// Firebase configuration
#define FIREBASE_HOST "petropump2-d3633-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "m8toUWmdNRKYK4tiY4yDrwhUxqWRlP09MsrUgISZ"
const String firebasePath = "/id/";
// Firebase database path
const String TemperaturePath = "/temperature";
const String Fuel_levelPath = "/fuel_level";

// LCD configuration
const int lcdColumns = 16;
const int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

// HC-SR04 pins
const int trigPin = 32;
const int echoPin = 13;

// Temperature sensor configuration
#define DHT11PIN 16
DHT dht(DHT11PIN, DHT11);

// Define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

FirebaseData firebaseData;

void readTemperatureAndDistance(void* parameter) {
  while (true) {
    // Temperature measurement
    //sensors.requestTemperatures();
    float temperatureC = dht.readTemperature();
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.print("°C ");

    if (Firebase.setFloat(firebaseData, TemperaturePath, temperatureC)) {
      Serial.println("Temperature sent to Firebase successfully");
    } else {
      Serial.println("Failed to send Temperature to Firebase");
    }

    // Distance measurement
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    float distanceCm = duration * SOUND_SPEED / 2;
    String fuel_level = "";

    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);
    if(distanceCm>10) {
      fuel_level = "Da het nhien lieu trong thung chua";
    } else if(distanceCm>5 && distanceCm<=10) {
      fuel_level = "Thung chua sap het nhien lieu";
    } else {
      fuel_level = "Thung chua van con day nhien lieu";
    }

    if (Firebase.setString(firebaseData, Fuel_levelPath, fuel_level)) {
      Serial.println("Temperature sent to Firebase successfully");
    } else {
      Serial.println("Failed to send Temperature to Firebase");
    }
     // Display temperature and fuel level on LCD
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatureC);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Fuel: ");
    lcd.print(distanceCm);

    delay(1500);

  }
}

void setup() {
  Serial.begin(9600);
   // Initialize LCD
  lcd.begin(lcdColumns, lcdRows);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello Tai no pro");
  lcd.setCursor(0,1);
  lcd.print("PETRO MANAGE");
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  lcd.clear();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Wire.setClock(400000); // Giảm tốc độ xung clock I2C xuống 400kHz

  // Initialize temperature sensor
  dht.begin();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

}

void loop() {
 readTemperatureAndDistance(NULL);
}