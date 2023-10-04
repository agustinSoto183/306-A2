//Henissa was here!
//#define NOPRINT // comment me out if you want to have no prints
#define NOPRINTANALOGUE // comment me out if you want to have no analogue prints
#define TRESHOLDA0 100
#define TRESHOLDA1 100


int numStripes = 40; // number of stripes in each trace. ALTER THIS VALUE AS NEEDED

int b=0; //reading the time for main loop to be run for 15s
int c=0; //memory for the time in mainloop

// encoder counts
float s = 0;   //built-in encoder counts
float s_2 = 0;   //built-in encoder counts for RPM calculation for PI controler
float Qcounter5s = 0;
 
//rpm holder
float Qrpmm; //our quadrature encoder RPM
float rpmm;  //rpm obtained each 5s from built-in encoder

//Encoder input
int s1=0;    //built-in encoder chanel one outpot
int s2=0;    //built-in encoder chanel two outpot
int QuadratureChannel1 = 0; //Quadrature Channel 1
int QuadratureChannel2 = 0; //Quadrature Channel 2

//repetition indicator
int r=0;     //repetition indicator for reading counts of bult-in encoder
int repetition = 0; //Quad repetition indicator

//previous channel 2 state
int s2m=0;   //memory of built-in encoder chanel two outpot
int QuadratureChannel2m = 0;

//Direction variables
int directionm=0;  //indicator for direction read by built-in encoder
int Qdirectionm = 0;
int dirm = 0;          //indicator for direction read by built-in encode
int Qdirm = 0;
 
int RPM;           //Commanded RPM
int exitt=0;       //mainloop exit condition

//PI controller stuff
float ctrl;      //PI controller outpot
float kp=.4;     //proportional gain of PI controller
float ki=.01;    //integral gain of PI controller
float eri;       //integral of error of PI controller
 
int repc=1;      //repetition condition of PI controller
int t0;          //memory of time for the Purpose of displaying the results
int repeat=0;    //repeat indicator to only let the memory of time for the Purpose of displaying the results be updated once\



void setup() {
  // put your setup code here, to run on
  pinMode(0, INPUT); //Set quadriture encoders signal as inputs
  pinMode(1, INPUT);

  Serial.begin(250000);                  //Baud rate of communication

  Serial.println("Enter the desired RPM.");  
  //while (Serial.available() == 0) {
    //Wait for user input
//}  

  //RPM = Serial.readString().toFloat(); //Reading the Input string from Serial port.
  RPM = 60;

  if (RPM<0){
    analogWrite(3,255);                 //changing the direction of motor's rotation
  }

  RPM=abs(RPM);
}


void loop() {

  b=millis();    //reading time
  c=b;           //storing the current time

  while ((b>=c) && (b<=(c+15500)) && exitt==0){   //let the main loop to be run for 15s
  
    if (b%13==0 && repc==1){                   //PI controller
      eri=ki*(RPM-rpmm)+eri;
      ctrl=50+kp*(RPM-rpmm)+eri;
      analogWrite(6,ctrl);
      repc=0;
    }

    if(b%13==1){
      repc=1;
    }

    s1=digitalRead(7);           //reading Chanel 1 of builtin encoder
    s2=digitalRead(8);           //reading Chanel 2 of builtin encoder

    if (analogRead(0) > TRESHOLDA0){ // WAS 650, TRYING 200
      QuadratureChannel1 = 1;
    }
    else {
      QuadratureChannel1 = 0;
    }

    if (analogRead(1) > TRESHOLDA1){
      QuadratureChannel2 = 1;
    }
    else {
      QuadratureChannel2 = 0;
    }
 
    //Print analogue values
    #ifndef NOPRINTANALOGUE
      Serial.print(analogRead(0) > TRESHOLDA0);
      Serial.print(", ");
      Serial.println(analogRead(1) > TRESHOLDA1);
    #endif

    if (s1!=s2 && r==0){
      s=s+1;      //counters for rpm that displyed every 5s
      s_2=s_2+1;  //counters for rpm that used in PI contoller
      r=1;        // this indicator wont let this condition, (s1 != s2), to be counted until the next condition, (s1 == s2), happens

    }

    if (s1==s2 && r==1){
      s=s+1;                    //counters for rpm that displyed every 5s
      s_2=s_2+1;                //counters for rpm that used in PI contoller
      r=0;                      // this indicator wont let this condition, (sm1 == sm2), to be counted until the next condition, (sm1 != sm2), happens
    }

    if(QuadratureChannel1 != QuadratureChannel2 && repetition == 0){
      Qcounter5s = Qcounter5s + 1;
      repetition = 1;
    }

    if(QuadratureChannel1 == QuadratureChannel2 && repetition == 1){
      Qcounter5s = Qcounter5s + 1;
      repetition = 0;
    }

    b=millis();                                             //updating time

    if (b%100<=1 && repeat==0){
      t0=b;                                                 //storing the current time once
      repeat=1;
    }

    if (b%100==0)
    {
      #ifndef NOPRINT
        Serial.print("time in ms: ");
        Serial.print(b-t0);
        Serial.print("  spontaneous speed from builtin encoder:  ");
      #endif
      rpmm=(s_2/(2*114))*600;    
      #ifndef NOPRINT                 //formulation for rpm in each 100ms for PI controller
        Serial.println(rpmm);
      #endif
      s_2=0;                          //reseting the counters of PI controller rpm meter

      if ((b-t0)%5000==0) {

        #ifndef NOPRINT
          Serial.println();
          Serial.print("RPM from builtin encoder: ");
          Serial.println((s/(228))*12);                  //formula for rpm in each 5s
        
          Serial.print("RPM from optical quadrature encoder: ");
          Serial.println(((((360/(numStripes * 2)) * Qcounter5s)/5)/6)); // CODE ME

          Serial.print("Error: ");
          Serial.println(((((360/(numStripes * 2)) * Qcounter5s)/5)/6)-((s/(228))*12)); //CODE ME

          Serial.print("direction read by motor's sensor: ");
          if (dirm==0){Serial.print("CW");}
          else{Serial.print("CCW");}
          Serial.print("  ,   ");

          Serial.print("direction read by sensor:  ");
          if (Qdirm == 1){Serial.println("CW");}
          else{Serial.println("CCW");}
          Serial.println();

        #endif
        s=0;
        Qcounter5s = 0;
        directionm=0;
        Qdirectionm = 0;
      }

      delay(1);

    }

    if((s1==HIGH)&&(s2==HIGH)&&(s2m==LOW)){                  //reading the direction of motor by cheaking which chanel follows which
      directionm=directionm+1;
    }

    if((s1==LOW)&&(s2==LOW)&&(s2m==HIGH)){
      directionm=directionm+1;
    }

    if((QuadratureChannel1 == HIGH)&&(QuadratureChannel2 == HIGH)&&(QuadratureChannel2m == LOW)){
      Qdirectionm = Qdirectionm + 1;
    }

    if((QuadratureChannel1 == LOW)&&(QuadratureChannel2 == LOW)&&(QuadratureChannel2m == HIGH)){
      Qdirectionm = Qdirectionm + 1;
    }

    s2m=s2;                    //memory of the previous builtin encoder chanel 2

    QuadratureChannel2m = QuadratureChannel2;

    if (directionm>100){
      dirm=0;
    }

    if (directionm<20){
      dirm=1;
    }

    if (Qdirectionm>100)    {
      Qdirm=0;
    }

    if (Qdirectionm<20){
      Qdirm=1;
    }

    b=millis();                                             //updating time
  }

  //analogWrite(6,0);                                       //turning off the motor
  exitt=0;                                                //changing the exit condition to prevent the motor to run after 15s
}