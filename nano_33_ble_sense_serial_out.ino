// Outputs all Arduino Nano BLE Sense data to terminal for the CLI forwarder
// See https://docs.edgeimpulse.com/docs/cli-data-forwarder
#include <Arduino_APDS9960.h>
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>
#include <Arduino_LSM9DS1.h>

#define CONVERT_G_TO_MS2 9.80665f
#define FREQUENCY_HZ 50
#define INTERVAL_MS (1000 / (FREQUENCY_HZ + 1))

// Device name
const char *nameOfPeripheral = "Nano BLE Sense";

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

  Serial.print("{'proximity':")
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
  Serial.println(magnetometerZ);
}
