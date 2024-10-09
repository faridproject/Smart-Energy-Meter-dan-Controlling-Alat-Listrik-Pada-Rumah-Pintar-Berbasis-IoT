#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>

#if defined(ESP32)
    #error "Software Serial is not supported on the ESP32"
#endif

/* Use software serial for the PZEM
 * Pin 12 Rx (Connects to the Tx pin on the PZEM)
 * Pin 13 Tx (Connects to the Rx pin on the PZEM)
*/
#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 12
#define PZEM_TX_PIN 13
#endif

SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSWSerial);
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Replace with your network credentials
const char* ssid = "iotsaya";
const char* password = "farid123";

// Your server URL
const char* serverNamePost = "http://smartenergylistrik.my.id/store.php";
const char* serverNameGet = "http://smartenergylistrik.my.id/get_relay.php?ID_Listrik=PLN_69";

// set price per kwh
const float hargaPerKWh = 1.352;

// define pin relay
const int relayPin = 2;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to ");
  Serial.print(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void getRelay() {
  if (WiFi.status() == WL_CONNECTED) { // Check Wi-Fi connection status
    HTTPClient http;  // Declare an object of class HTTPClient

    http.begin(client, serverNameGet);  // Specify request destination using the new API
    int httpCode = http.GET(); // Send the request

    if (httpCode > 0) { // Check the returning code
      String payload = http.getString(); // Get the request response payload
      Serial.println(payload);

      if (payload == "0") {
        digitalWrite(relayPin, HIGH); // Turn LED on
      } else if (payload == "1") {
        digitalWrite(relayPin, LOW); // Turn LED off
      } else {
        Serial.println("Unexpected response");
      }
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end(); // Close connection
  }
}


void sendData(String url) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Use the new API method with WiFiClient
    http.begin(client, url);

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      if (httpResponseCode == HTTP_CODE_MOVED_PERMANENTLY || httpResponseCode == HTTP_CODE_FOUND) {
        // Follow the redirect
        String newUrl = http.getLocation();
        Serial.print("Redirected to: ");
        Serial.println(newUrl);
        http.end();
        http.begin(client, newUrl);
        httpResponseCode = http.GET();
      }

      String response = http.getString();
      Serial.println(httpResponseCode); // Print return code
      Serial.println(response);         // Print request answer
    } else {
      Serial.print("Error on sending GET: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

void loop() {

    // Read the data from the sensor
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();
    float harga = (energy,3* hargaPerKWh);
     
    
    // Form the complete URL with parameters
    String url = String(serverNamePost) + "?ID_Listrik=PLN_69&Voltage=" + String(voltage) + "&Current=" + String(current) + "&Power=" + String(power) + "&Energy=" + String(energy) + "&Frequency=" + String(frequency) + "&PF=" + String(pf);
    


    // Print the URL to debug
    Serial.print("Requesting URL: ");
    Serial.println(url);
    
    // Send data to the server
    sendData(url);

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("MONITORING LISTRIK");
    lcd.setCursor(0, 1);
    lcd.print("Ratman Mebel");
    lcd.setCursor(1, 2);
    lcd.print("Listrik 900Va");
    lcd.setCursor(1, 3);
    lcd.print("Rp.");
    lcd.setCursor(4, 3);
    lcd.print(harga);

    delay(3000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Voltase :");
    lcd.setCursor(9, 0);
    lcd.print(voltage);
    lcd.setCursor(0,1);
    lcd.print("Ampere  :");
    lcd.setCursor(9, 1);
    lcd.print(current);
    lcd.setCursor(0, 2);
    lcd.print("Watt    :");
    lcd.setCursor(9, 2);
    lcd.print(power);
    lcd.setCursor(0, 3);
    lcd.print("kWh     :");
    lcd.setCursor(9, 3);
    lcd.print(energy);

    delay(3000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Frequency :");
    lcd.setCursor(11, 0);
    lcd.print(frequency);
    lcd.setCursor(0,1);
    lcd.print("PF        :");
    lcd.setCursor(11, 1);
    lcd.print(pf);

    delay(3000);

    getRelay();

    delay(3000);
}
