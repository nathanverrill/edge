// output all sensor data
// loop output is numeric for easy consumption into Edge Impulse and other tools that use serial data forwarders
#include <Arduino_APDS9960.h>
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>
#include <Arduino_LSM9DS1.h>
// TODO include sensor serial number in output
// https://github.com/ricaun/ArduinoUniqueID
// #include <ArduinoUniqueID.h>

#define CONVERT_G_TO_MS2 9.80665f
#define FREQUENCY_HZ 50
#define INTERVAL_MS (1000 / (FREQUENCY_HZ + 1))

// Device name
const char *nameOfPeripheral = "Arduino Nano 33 BLE Sense";
const char *productDataSheet = "ABX00031-datasheet.pdf";

// link to open source or secure source control
const char *softwareSource = "https://github.com/nathanverrill/edge";

// data column names
// tab separatebed values for data; pipe separated for column names
// while including outputing this string can degrade device performance it makes the device easier for downstream engineers to use
// intended for prototyping, there are better ways
// json output could also be used, but some streaming data processes what time series of numbers and then faster without parsing json
// hosting script or application can transform into json and other representations as needed
const char *metadata = "millis|temp|humidity|pressure|accelX|accelY|accelZ|gyroX|gyroY|gyroZ|magX|magY|magZ|proximity|gesture|lightColorR|lightColorG|lightColorB|lightColorA|inferenceResults|mlModelInfo|softwareVersion|softwareSource|device|productId|serialNumber|vendorId|datasheet|additionalMetaData";
const char *additionalMetadata = "none"; // included as raw text

// get these values with the Arduino Create Agent
const char *serialNumber = "6514DA94E0BF0E08";
const char *vendorId = "0x2341";
const char *productId = "0x805a";

void setup()
{

  Serial.begin(115200);
  Serial.println("Started");

  if (!APDS.begin())
  {
    digitalWrite(LEDR, LOW);
    while (true)
      ;
  }

  if (!HTS.begin())
  {
    digitalWrite(LEDR, LOW);
    while (true)
      ;
  }

  if (!BARO.begin())
  {
    digitalWrite(LEDR, LOW);
    while (true)
      ;
  }

  if (!IMU.begin())
  {
    digitalWrite(LEDR, LOW);
    while (true)
      ;
  }
}

int proximity = 0;
uint8_t gesture = 0x0;
int r = 0, g = 0, b = 0, a = 0;
float temperature;
float humidity;
float atmosphericPressure;

float accelerometerX;
float accelerometerY;
float accelerometerZ;

float gyroscopeX;
float gyroscopeY;
float gyroscopeZ;

float magnetometerX;
float magnetometerY;
float magnetometerZ;

unsigned long lastLedReset = 0;
unsigned long lastHtsUpdate = 0;
unsigned long lastBaroUpdate = 0;

float softwareVersion = 1.1;

void loop()
{

  static unsigned long last_interval_ms = 0;

  if (millis() > last_interval_ms + INTERVAL_MS)
  {

    last_interval_ms = millis();

    sensors();
  }
}

void sensors()
{

  // Check if a proximity reading is available.
  if (APDS.proximityAvailable())
  {
    proximity = APDS.readProximity();
  }

  // Check if a gesture reading is available
  if (APDS.gestureAvailable())
  {
    gesture = APDS.readGesture();
  }

  // Check if a color reading is available
  if (APDS.colorAvailable())
  {
    APDS.readColor(r, g, b, a);
  }

  // Check if a accelerometer, gyroscope or magnetometer reading is available
  // Do we need this? x * CONVERT_G_TO_MS2
  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(accelerometerX, accelerometerY, accelerometerZ);
  }
  if (IMU.gyroscopeAvailable())
  {
    IMU.readGyroscope(gyroscopeX, gyroscopeY, gyroscopeZ);
  }
  if (IMU.magneticFieldAvailable())
  {
    IMU.readMagneticField(magnetometerX, magnetometerY, magnetometerZ);
  }

  // Read temperature and humidity from sensor
  if (millis() - lastHtsUpdate > 1000)
  {
    lastHtsUpdate = millis();
    temperature = HTS.readTemperature();
    humidity = HTS.readHumidity();
  }

  // Read atmospheric pressure from sensor
  if (millis() - lastBaroUpdate > 1000)
  {
    lastBaroUpdate = millis();
    atmosphericPressure = BARO.readPressure();
  }

  Serial.print(millis());
  Serial.print('\t');
  Serial.print(temperature);
  Serial.print('\t');
  Serial.print(humidity);
  Serial.print('\t');
  Serial.print(atmosphericPressure);
  Serial.print('\t');
  Serial.print(accelerometerX);
  Serial.print('\t');
  Serial.print(accelerometerY);
  Serial.print('\t');
  Serial.print(accelerometerZ);
  Serial.print('\t');
  Serial.print(gyroscopeX);
  Serial.print('\t');
  Serial.print(gyroscopeY);
  Serial.print('\t');
  Serial.print(gyroscopeZ);
  Serial.print('\t');
  Serial.print(magnetometerX);
  Serial.print('\t');
  Serial.print(magnetometerY);
  Serial.print('\t');
  Serial.print(magnetometerZ);
  Serial.print('\t');
  Serial.print(proximity);
  Serial.print('}\t');
  Serial.print(gesture);
  Serial.print('\t');
  Serial.print(r);
  Serial.print('\t');
  Serial.print(g);
  Serial.print('\t');
  Serial.print(b);
  Serial.print('\t');
  Serial.print(a);
  Serial.print('\t');
  Serial.print(softwareVersion);
  Serial.print('\t');
  Serial.print(softwareSource);
  Serial.print('\t');
  Serial.print(nameOfPeripheral);
  Serial.print('\t');
  Serial.print(productId);
  Serial.print('\t');
  Serial.print(serialNumber);
  Serial.print('\t');
  Serial.print(vendorId);
  Serial.print('\t');
  Serial.print(productDataSheet);
  Serial.print('\t');
  Serial.print(additionalMetadata);
  Serial.print('\t');
  Serial.println(metadata);    
}
