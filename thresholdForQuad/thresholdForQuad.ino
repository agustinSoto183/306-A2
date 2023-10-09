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
  double top = 0;
  double bottom = 1024;
  double sum = 0; 
  int numReadings = 10000;


  for (int i = 1; i <= numReadings; i++) {
    // The two if statements finds the maximum and minimum voltage values (0-1024) over the number of readings.
    if (top < analogRead(pinNum)) {
        top = analogRead(pinNum);
      }
    if (bottom > analogRead(pinNum)) {
        bottom = analogRead(pinNum);
      }
    // Cumulative sum of all the readings.
    sum += analogRead(pinNum);
  }

  // Threshold value based on averaging the minimum and maximum values.
  double threshold = (top + bottom) / 2;
  //Serial.print("Threshold from averaging the min and max values is :");
  //Serial.println(threshold);

  // Averaged cumulative sum of all the readings.
  double threshold2 = sum / numReadings;

  Serial.print("Threshold from average is: ");
  Serial.print(threshold2);
  Serial.print(" for pin ");
  Serial.println(pinNum);

  return threshold2; // threshold is the avg of max and min; threshold2 is the average of the 1000 readings
}