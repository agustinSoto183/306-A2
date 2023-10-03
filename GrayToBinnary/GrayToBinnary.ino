
void setup() {
  Serial.begin(9600);
  int gray = 0b01110;
  int binary = grayToBinary(gray);
  Serial.println(binary, BIN);
  //	01011
  int msb3 = 1;
  int second = 0;
  int one = 0;
  int lsbzero = 1;

  int concat = 0b1000*msb3 + 0b100*second + 0b100*one + lsbzero;
  Serial.println(concat, BIN);
}

void loop() {
  // Nothing to do here
}


int grayToBinary(int gray){
  int binary=0;
  for(;gray;gray=gray>>1){
    binary^=gray; //binary=binary^gray;
  }
  return binary;
}