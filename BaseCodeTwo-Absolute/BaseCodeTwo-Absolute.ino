#define treshold4 785
#define treshold3 651
#define treshold2 854
#define treshold1 829
#define treshold0 820

const byte decimalToGrayTable[32] = {
  0,   // Decimal: 0  => 00000 (Gray Code)
  1,   // Decimal: 1  => 00001 (Gray Code)
  31,   // Decimal: 2  => 00011 (Gray Code)
  30,   // Decimal: 3  => 00010 (Gray Code)
  3,   // Decimal: 4  => 00110 (Gray Code)
  2,   // Decimal: 5  => 00111 (Gray Code)
  4,   // Decimal: 6  => 00101 (Gray Code)
  5,   // Decimal: 7  => 00100 (Gray Code)
  27,  // Decimal: 8  => 01100 (Gray Code)
  26,  // Decimal: 9  => 01101 (Gray Code)
  28,  // Decimal: 10 => 01111 (Gray Code)
  29,  // Decimal: 11 => 01110 (Gray Code)
  24,  // Decimal: 12 => 01010 (Gray Code)
  25,  // Decimal: 13 => 01011 (Gray Code)
  23,   // Decimal: 14 => 01001 (Gray Code)
  22,   // Decimal: 15 => 01000 (Gray Code)
  11,  // Decimal: 16 => 11000 (Gray Code)
  10,  // Decimal: 17 => 11001 (Gray Code)
  12,  // Decimal: 18 => 11011 (Gray Code)
  13,  // Decimal: 19 => 11010 (Gray Code)
  8,  // Decimal: 20 => 11110 (Gray Code)
  9,  // Decimal: 21 => 11111 (Gray Code)
  7,  // Decimal: 22 => 11101 (Gray Code)
  6,  // Decimal: 23 => 11100 (Gray Code)
  16,  // Decimal: 24 => 10100 (Gray Code)
  17,  // Decimal: 25 => 10101 (Gray Code)
  15,  // Decimal: 26 => 10111 (Gray Code)
  14,  // Decimal: 27 => 10110 (Gray Code)
  19,  // Decimal: 28 => 10010 (Gray Code)
  18,  // Decimal: 29 => 10011 (Gray Code)
  20,  // Decimal: 30 => 10001 (Gray Code)
  21   // Decimal: 31 => 10000 (Gray Code)
};

float deg=45; // Rotation degree
float s = 0;  //Encoder counts
int sm1 = 0;  //Built-in chanel 1
int sm2 = 0;  //Built-in chanel 2
int r = 0;    //indicator for reading builtin encoder to avoid the reading redundancy
float er;     //Proportional error for PI controller
float eri;    //Integral error for PI controller

int fourLSB;
int three;
int two;
int one;
int zeroMSB;
int grayIn;
int binary;
int CurrentReading;
int prevReading = 0;
int home = 0;
int distanceFromHome = 0;
int displacement = 0;
int direction = 0;
int OverflowExtra = 0;
int crossedZero = 0;
int dif = 0;


int t=0;    //time in ms
int t0=0;   //memory for time in ms


int finish=0;  //finish indicator
int rep=1;     //Repetition indicator

int concat;
int gray;


void setup() 
{
  
  Serial.begin(250000);                                                 //Baud rate of communication 

  Serial.println("Enter the desired rotation in degree.");  
  pinMode(3,OUTPUT); //direction
  pinMode(6,OUTPUT); // power of motor
    // put your main code here, to run repeatedly:
  pinMode(A0, INPUT); //Set absolut encoders signal as inputs
  pinMode(A1, INPUT); 
  pinMode(A2, INPUT); 
  pinMode(A3, INPUT); 
  pinMode(A4, INPUT); //LSB
  
  analogWrite(6,0);

  while (Serial.available() == 0)                                       //Obtaining data from user
  { 
    //Wait for user input
  }  
  
  deg = Serial.readString().toFloat(); //Reading the Input string from Serial port.
  if (deg<0)
  {
    analogWrite(3,255);               //change the direction of rotation by applying voltage to pin 3 of arduino
  }
  deg=abs(deg);

  grayIn = greycodeToAngle(analogRead(0), analogRead(1), analogRead(2), analogRead(3), analogRead(4));
  binary = grayToBinary(grayIn);
  CurrentReading = (binary * 360)/32;

  home = CurrentReading; //315
  prevReading = CurrentReading; //315                                   
}


float kp = .6*90/deg;                         //proportional gain of PI
float ki = .02;                               //integral gain of PI 






void loop() 
{


      
  t=millis();                 //reading time
  t0=t;                       //saving the current time in memory
  while (t<t0+4000 && rep<=10)      //let the code to ran for 4 seconds each with repetitions of 10
  {
      
    if (t%10==0){                               //PI controller that runs every 10ms
      if (s < deg * 114 * 2 / 360)
      {
        er = deg - s *360/228;
        eri = eri + er;
        analogWrite(6, kp * er + ki * eri);
      }

      if (s >= deg * 228/360)
      {
        analogWrite(6, 0);
        eri = 0;
      }
      delay(1);
    }

    sm1 = digitalRead(7);         //reading chanel 1 
    sm2 = digitalRead(8);         //reading chanel 2

       
    if (sm1 != sm2 && r == 0){               //counting the number changes for both chanels
      s = s + 1;
      r = 1;                                 // this indicator wont let this condition, (sm1 != sm2), to be counted until the next condition, (sm1 == sm2), happens
    }
    if (sm1 == sm2 && r == 1) {
      s = s + 1;
      r = 0;                                 // this indicator wont let this condition, (sm1 == sm2), to be counted until the next condition, (sm1 != sm2), happens
    }

    t=millis();           //updating time
    finish=1;             //cghanging finish indicator

  }

  if (finish==1){                            //this part of the code is for displaying the result
    delay(500);                              //half second delay

    grayIn = greycodeToAngle(analogRead(0), analogRead(1), analogRead(2), analogRead(3), analogRead(4));
    binary = grayToBinary(grayIn);
    CurrentReading = (binary * 360)/32;

    Serial.println(CurrentReading);

    float diff = prevReading-CurrentReading;
    bool zeroed = abs(diff)>180;
    float add = 0;
    
    if (zeroed && (diff>0)) {add = -360;}
    else if (zeroed && (diff < 0)) {add = 360;}
    displacement = -(diff + add);
 

    distanceFromHome = CurrentReading - home;
    rep=rep+1;                               // increasing the repetition indicator
    Serial.print("shaft possition from optical absolute sensor from home position: ");
    Serial.println(abs(distanceFromHome));

    Serial.print("shaft displacement from optical absolute sensor: ");
    Serial.println(abs(displacement));
   
    prevReading = CurrentReading;
    

    Serial.print("Shaft displacement from motor's builtin encoder: ");
    
    //every full Revolution of the shaft is associated with 228 counts of builtin  
    //encoder so to turn it to degre we can use this formula (s * 360 / 228), "s" is the number of  built-in encoder counts
    Serial.println(s * 360 / 228);
  
    float Error=(abs(displacement))-s*360/228;
    Serial.print("Error :");
    Serial.println(Error);                                    //displaying error
    Serial.println();

    Serial.print("direction read by sensor:  ");
    if (displacement < 0){
      Serial.println("CW");
    }else{
      Serial.println("CCW");
    }
    Serial.println();


    s = 0;
    finish=0; 
  }
  analogWrite(6,0);               //turning off the motor
}

int greycodeToAngle(int Azeromsb, int Aone, int Atwo, int Athree, int Afourlsb){
  zeroMSB = Azeromsb > treshold0 ? 1 : 0;
  one = Aone > treshold1 ? 1 : 0;
  two = Atwo > treshold2 ? 1 : 0;
  three = Athree > treshold3 ? 1 : 0;
  fourLSB = Afourlsb > treshold4 ? 1 : 0;

  concat = 0b10000*(!zeroMSB) + 0b1000*one + 0b100*two + 0b10*three + fourLSB;

  return concat;
}

int grayToBinary(int gray){
  return decimalToGrayTable[gray];
}


