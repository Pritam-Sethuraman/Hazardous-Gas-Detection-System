#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
Servo servoMotor;
int Buzzer = 2;
int airVent = 3;
int exhaustFan = 5;
int waterPump = 6;
int sensorMQ6 = A0;
int sensorMQ7 = A1;
int sensorMQ135 = A2;
int tempSensor = A3;

float sensorValueMQ6 = 0;
float sensorValueMQ7 = 0;
float sensorValueMQ135 = 0;
float sensorValueLM35 = 0;


// Setting up the devices/components
void setup() 
{
  Serial.begin(9600);
  lcd.begin(16, 2); // set up the LCD's number of columns and rows
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hazardous Gas");
  lcd.setCursor(0, 1);
  lcd.print("Detection System");
  
  pinMode(sensorValueMQ6, INPUT);
  pinMode(sensorValueMQ7, INPUT);
  pinMode(sensorValueMQ135, INPUT);
  
  pinMode(exhaustFan, OUTPUT);
  pinMode(waterPump, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  
  servoMotor.attach(airVent);
  servoMotor.write(0);
}


// Function to read sensor value for MQ6
float readSensorMQ6()
{
  float sensorVolt;
  float gasRS; // Get value of RS in a GAS
  float R0 = 10000; //example value of R0. Replace with your own
  float ratio; // Get ratio gasRS/RS_air
  float LPG_PPM;  
  
  int sensorValue = analogRead(sensorMQ6);
  sensorVolt = (float)sensorValue/1024*5.0;
  gasRS = (5.0-sensorVolt)/sensorVolt;
  ratio = gasRS/R0;
  float x = 1000 * ratio;
  LPG_PPM = pow(x,-1.431);//LPG PPM
  return LPG_PPM;
}


// Function to read sensor value for MQ7
float readSensorMQ7()
{
  float sensorVolt;
  float gasRS; // Get value of RS in a GAS
  float R0 = 10000; //example value of R0. Replace with your own
  float ratio; // Get ratio gasRS/RS_air
  float valueCO;  
  
  int sensorValue = analogRead(sensorMQ7);
  sensorVolt = (float)sensorValue/1024*5.0;
  gasRS = (5.0 - sensorVolt)/sensorVolt;
  ratio = gasRS / R0;
  float x = 1538 * ratio;
  valueCO = pow(x, -1.709);//CO value in ppm
  return valueCO;
}


// Function to read temperature
float readSensorLM35()
{
  float temp_adc_val = 0;
  float tempC = 0;
  
  temp_adc_val = analogRead(tempSensor);
  tempC = temp_adc_val * 4.88/10;
  tempC = tempC - 50;

  return tempC;
}

// Driver Function
void loop() 
{
  lcd.clear();
  sensorValueMQ6 = readSensorMQ6();	// LPG
  sensorValueMQ7 = readSensorMQ7();	// CO
  sensorValueMQ135 = analogRead(sensorMQ135);	// CO2
  sensorValueLM35 = readSensorLM35();	// Temperature Sensor

  int emergencyFlag = 0; 
  // LPG Gas Leak Case
  if(sensorValueMQ6 >= 30)
  {
    digitalWrite(exhaustFan, LOW); // Turning off the exhaust fan
    digitalWrite(waterPump, LOW); // Turning of the sprinkler system
    servoMotor.write(180);  // Opening the vents
    digitalWrite(Buzzer, HIGH); // Buzzer on
    
    Serial.print("Buzzer: ");
    Serial.println("ON");
    
    lcd.clear();
    lcd.print("Caution:"); // Print a message to the LCD.
    lcd.setCursor(0,1); 
    lcd.print("LPG Gas leak!!!");
    delay(1000);  // Delay of 1 second
  }
  // Fire Case
  else if(sensorValueMQ135 >= 600 && sensorValueLM35 >= 50)
  {
    digitalWrite(exhaustFan, LOW); // Turning off the exhaust fan
    digitalWrite(waterPump, HIGH); // Turning on the sprinkler system
    servoMotor.write(0);  // Closing the vents
    digitalWrite(Buzzer, HIGH); // Buzzer on
    
    Serial.print("Buzzer: ");
    Serial.println("ON");

    lcd.clear();
    lcd.print("Caution:"); // Print a message to the LCD.
    lcd.setCursor(0,1); 
    lcd.print("Fire!!!");
    delay(1000);  // Delay of 1 second
  }
  // Harmful Gas Case
  else if(sensorValueMQ7 >= 25 || sensorValueMQ135 >= 600)
  {
    digitalWrite(exhaustFan, HIGH); // Turning on the exhaust fan
    digitalWrite(waterPump, LOW); // Turning of the sprinkler system
    servoMotor.write(180);  // Opening the vents
    digitalWrite(Buzzer, HIGH); // Buzzer on
    
    Serial.print("Buzzer: ");
    Serial.println("ON");

    lcd.clear();
    lcd.print("Caution:"); // Print a message to the LCD.
    lcd.setCursor(0,1); 
    lcd.print("Hazardous Gases!");
    delay(1000);  // Delay of 1 second
  }
  else
  {
    servoMotor.write(0); // Closing the vents
    digitalWrite(exhaustFan, LOW);  // Turning off the exhaust fan
    digitalWrite(waterPump, LOW); // Turning of the sprinkler system
    digitalWrite(Buzzer, LOW);  // Buzzer off
    
    Serial.print("Buzzer: ");
    Serial.println("OFF");

    lcd.clear();
    lcd.print("The Environment"); // Print a message to the LCD.
    lcd.setCursor(0,1);
    lcd.print("is Safe");
    delay(1000); // Delay of 1 second
  }

  Serial.print("LPG: ");
  Serial.println(sensorValueMQ6);
  Serial.print("CO: ");
  Serial.println(sensorValueMQ7);
  Serial.print("CO2: ");
  Serial.println(sensorValueMQ135);
  Serial.print("Temperature: ");
  Serial.println(sensorValueLM35);
  Serial.println("-----------------");
}
