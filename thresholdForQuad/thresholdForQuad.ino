void setup() {
  pinMode(0, INPUT);
  pinMode(1, INPUT); 

  // Comment out pins 2,3,4 if trying to test quadrature thresholds.
  // pinMode(2, INPUT);
  // pinMode(3, INPUT);
  // pinMode(4, INPUT);

  Serial.begin(250000);
  analogWrite(6,100); // Provides 100% power to the motor.

  int pinNum = 0; // Or make it equal 0, 1, 2, 3, or 4 depending on which pin you want to test.
  threshold(pinNum);

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