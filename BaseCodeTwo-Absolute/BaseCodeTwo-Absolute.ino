#define treshold4 785
#define treshold3 651
#define treshold2 854
#define treshold1 829
#define treshold0 820

const byte GrayToDecimal[32] = {
  0,   // Gray: 0
  1,   // Gray: 1
  31,   // Gray: 2
  30,   // Gray: 3
  3,   // Gray: 4
  2,   // Gray: 5
  4,   // Gray: 6
  5,   // Gray: 7
  27,  // Gray: 8
  26,  // Gray: 9
  28,  // Gray: 10
  29,  // Gray: 11
  24,  // Gray: 12
  25,  // Gray: 13
  23,   // Gray: 14
  22,   // Gray: 15
  11,  // Gray: 16
  10,  // Gray: 17
  12,  // Gray: 18
  13,  // Gray: 19
  8,  // Gray: 20
  9,  // Gray: 21
  7,  // Gray: 22
  6,  // Gray: 23
  16,  // Gray: 24
  17,  // Gray: 25
  15,  // Gray: 26
  14,  // Gray: 27 
  19,  // Gray: 28 
  18,  // Gray: 29
  20,  // Gray: 30 
  21   // Gray: 31
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


int t=0;    //time in ms
int t0=0;   //memory for time in ms


int finish=0;  //finish indicator
int rep=1;     //Repetition indicator

int concat; //Concatenate variable
int gray;


void setup() 
{
  //Pin Setup
  pinMode(3,OUTPUT); //direction
  pinMode(6,OUTPUT); // power of motor
  pinMode(A0, INPUT); //Set absolut encoders signal as inputs
  pinMode(A1, INPUT); 
  pinMode(A2, INPUT); 
  pinMode(A3, INPUT); 
  pinMode(A4, INPUT); //LSB

  Serial.begin(250000);                                                 //Baud rate of communication 

  Serial.println("Enter the desired rotation in degree."); 
  while (Serial.available() == 0)                                       //Obtaining data from user
  { 
    //Wait for user input
  }  
  
  analogWrite(6,0);

  deg = Serial.readString().toFloat(); //Reading the Input string from Serial port.
  if (deg<0)
  {
    analogWrite(3,255);               //change the direction of rotation by applying voltage to pin 3 of arduino
  }
  deg=abs(deg);

  grayIn = AnalogueToGray(analogRead(0), analogRead(1), analogRead(2), analogRead(3), analogRead(4)); //Read current position and convert to gray code
  binary = grayToBinary(grayIn); //Convert gray code to Binary
  CurrentReading = (binary * 360)/32; //Convert binary value to angle value

  home = CurrentReading;
  prevReading = CurrentReading;                                
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
    finish=1;             //changing finish indicator

  }

  if (finish==1){                            //this part of the code is for displaying the result
    delay(500);                              //half second delay

    grayIn = AnalogueToGray(analogRead(0), analogRead(1), analogRead(2), analogRead(3), analogRead(4));
    binary = grayToBinary(grayIn);
    CurrentReading = (binary * 360)/32; //Read values and convert to an angular value

    float diff = prevReading - CurrentReading;
    bool zeroed = abs(diff)>180;
    float add = 0;
    
    if (zeroed && (diff>0)) {add = -360;}
    else if (zeroed && (diff < 0)) {add = 360;} //Check for a full rotation and adjust the displacement accordingly

    displacement = -(diff + add); //Angular displacement
    distanceFromHome = CurrentReading - home; 
    
    rep=rep+1;                               // increasing the repetition indicator

    Serial.print("shaft possition from optical absolute sensor from home position: ");
    Serial.println(abs(distanceFromHome));

    Serial.print("shaft displacement from optical absolute sensor: ");
    Serial.println(abs(displacement));
   
    prevReading = CurrentReading; //Update previous reading
    

    Serial.print("Shaft displacement from motor's builtin encoder: ");
    
    //every full Revolution of the shaft is associated with 228 counts of builtin  
    //encoder so to turn it to degre we can use this formula (s * 360 / 228), "s" is the number of  built-in encoder counts
    Serial.println(s * 360 / 228);
  
    float Error=(abs(displacement))-s*360/228;
    Serial.print("Error :");
    Serial.println(Error);                                    //displaying error
    Serial.println();

    Serial.print("direction read by sensor:  "); //If displacement is negative, direction is CW else direction is CCW
    if (displacement < 0){
      Serial.println("CW");
    }else{
      Serial.println("CCW");  //Print direction
    }
    Serial.println();


    s = 0;
    finish=0; 
  }
  analogWrite(6,0);               //turning off the motor
}

int AnalogueToGray(int Azeromsb, int Aone, int Atwo, int Athree, int Afourlsb){
  zeroMSB = Azeromsb > treshold0 ? 1 : 0;
  one = Aone > treshold1 ? 1 : 0;
  two = Atwo > treshold2 ? 1 : 0;
  three = Athree > treshold3 ? 1 : 0;
  fourLSB = Afourlsb > treshold4 ? 1 : 0;

  concat = 0b10000*(!zeroMSB) + 0b1000*one + 0b100*two + 0b10*three + fourLSB; //Concatenate values to gray code

  return concat;
}

int grayToBinary(int gray){
  return GrayToDecimal[gray]; //Uses index as gray and maps corresponding binary value
}


