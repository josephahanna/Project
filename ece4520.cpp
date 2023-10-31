#define speedPinL 6
#define speedPinR 9
#define LeftMotorDirPin1  8    
#define LeftMotorDirPin2  7  
#define RightMotorDirPin1  3  
#define RightMotorDirPin2  4    
#define sensorPin 2  
const float wheelCircumference = 0.25; // in meters
unsigned long programStartTime;

bool tachometerActive = false;
long lastTachometerPulseTime;
int rpm;
long timeOld;
volatile byte pulses;

enum Color {
  RED = 1,
  YELLOW = 2,
  GREEN = 3
};

void setup() {
  Serial.begin(9600);
  pixy.init();
  programStartTime = millis();
 
  pinMode(speedPinL, OUTPUT);  
  pinMode(LeftMotorDirPin1, OUTPUT);
  pinMode(LeftMotorDirPin2, OUTPUT);
  pinMode(RightMotorDirPin1, OUTPUT);
  pinMode(RightMotorDirPin2, OUTPUT);
  pinMode(speedPinR, OUTPUT);

  // Tachometer setup
  pinMode(sensorPin, INPUT);  
  attachInterrupt(digitalPinToInterrupt(sensorPin), countPulses, FALLING);
 
  pulses = 0;
  rpm = 0;
  timeOld = 0;
  lastTachometerPulseTime = millis();
  stopMotors();
}

void loop() {
  uint16_t blocks = pixy.ccc.getBlocks();
 
  if (blocks) {
    for (int j = 0; j < blocks; j++) {
      Color detectedColor = static_cast<Color>(pixy.ccc.blocks[j].m_signature);
     
      switch (detectedColor) {
        case RED:
          stopMotors();
          tachometerActive = false;
          rpm = 0;
          Serial.println("Detected red");
          break;
       
        case YELLOW:
          slowSpeed();
          if (dist <= 10) {
            stopMotors();
          }
          tachometerActive = true;
          Serial.println("Detected yellow");
          break;
         
        case GREEN:
          maxSpeed();
          tachometerActive = true;
          Serial.println("Detected green");
          break;
      }
     
      float speed = rpm * wheelCircumference / 60.0;
      unsigned long elapsedTime = millis() - programStartTime;

      Serial.print("RPM: ");
      Serial.print(rpm);
      Serial.print(" Speed: ");
      Serial.print(speed);
      Serial.print(" m/s Time Elapsed: ");
      Serial.print(elapsedTime / 1000.0);
      Serial.println(" seconds");
     
      int dist = -0.3143 * (pixy.ccc.blocks[j].m_height) + 56.23;
      Serial.print("Distance: ");
      Serial.println(dist);
    }
  }
 
  if (tachometerActive && millis() - lastTachometerPulseTime >= 2000) {
    rpm = 0;
    tachometerActive = false;
  }

  delay(50);
}

void countPulses() {
  pulses++;
  lastTachometerPulseTime = millis();
  if (millis() - timeOld >= 1000) {
    rpm = (pulses * 60) / 2;
    pulses = 0;
    timeOld = millis();
  }
}

void maxSpeed() {
  digitalWrite(RightMotorDirPin1, HIGH);
  digitalWrite(RightMotorDirPin2, LOW);
  digitalWrite(LeftMotorDirPin1, HIGH);
  digitalWrite(LeftMotorDirPin2, LOW);
  analogWrite(speedPinL, 100);
  analogWrite(speedPinR, 80);
}

void slowSpeed() {
  digitalWrite(RightMotorDirPin1, HIGH);
  digitalWrite(RightMotorDirPin2, LOW);
  digitalWrite(LeftMotorDirPin1, HIGH);
  digitalWrite(LeftMotorDirPin2, LOW);
  analogWrite(speedPinL, 90);  
  analogWrite(speedPinR, 70);  
}

void stopMotors() {
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2, LOW);
  digitalWrite(LeftMotorDirPin1, LOW);
  digitalWrite(LeftMotorDirPin2, LOW);
}