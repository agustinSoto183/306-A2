void setup() {
  // put your setup code here, to run once:
  pinMode(0, INPUT); 
  Serial.begin(250000);
}

void loop(){
}

double threshold() {
  double top = 0;
  double bottom = 1024;
  double sum = 0;
  int numReadings = 1000;

  for (int i = 1; i <= numReadings; i++) {
    if (top < analogRead(0)) {
        top = analogRead(0);
      }
    if (bottom > analogRead(0)) {
        bottom = analogRead(0);
      }

    sum += analogRead(0);
  }

  double threshold = top + bottom / 2;
  Serial.print("Threshold from averaging the min and max values is :");
  Serial.println(threshold);

  double threshold2 = sum / numReadings;

  Serial.print("Threshold from average is: ");
  Serial.println(threshold2);

  return threshold2; // threshold is the avg of max and min; threshold2 is the average of the 1000 readings
}