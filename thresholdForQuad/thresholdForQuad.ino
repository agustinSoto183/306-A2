void setup() {
  pinMode(0, INPUT);
  pinMode(1, INPUT); 
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  Serial.begin(250000);
  analogWrite(6,100); // Provides 100% power to the motor.

  // Find the threshold value for pins 0-6. Comment out the lines that use pins 2-4 if testing the quadrature.
  threshold(0);
  threshold(1);
  threshold(2);
  threshold(3);
  threshold(4);
  
  analogWrite(6,0); // Turn off the motor.
}

void loop(){
}

double threshold(int pinNum) {
  double sum = 0; 
  int numReadings = 10000;


  for (int i = 1; i <= numReadings; i++) {
    // Cumulative sum of all the readings.
    sum += analogRead(pinNum);
  }

  // Averaged cumulative sum of all the readings.
  double threshold = sum / numReadings;

  Serial.print("Threshold from average is: ");
  Serial.print(threshold);
  Serial.print(" for pin ");
  Serial.println(pinNum);

  return threshold;
}