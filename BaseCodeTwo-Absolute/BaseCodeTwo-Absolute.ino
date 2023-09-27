#define treshold 50

float deg=45; // Rotation degree
float s = 0;  //Encoder counts
int sm1 = 0;  //Built-in chanel 1
int sm2 = 0;  //Built-in chanel 2
int r = 0;    //indicator for reading builtin encoder to avoid the reading redundancy
float er;     //Proportional error for PI controller
float eri;    //Integral error for PI controller

int fourmsb;
int three;
int two;
int one;
int zerolsb;




int t=0;    //time in ms
int t0=0;   //memory for time in ms




int finish=0;  //finish indicator
int rep=1;     //Repetition indicator




void setup() 
{
  
  Serial.begin(250000);                                                 //Baud rate of communication 

  Serial.println("Enter the desired rotation in degree.");  
  
  while (Serial.available() == 0)                                       //Obtaining data from user
  { 
    //Wait for user input
  }  
  
  deg = Serial.readString().toFloat(); //Reading the Input string from Serial port.
  if (deg<0)
  {
    analogWrite(3,255);                                                   //change the direction of rotation by applying voltage to pin 3 of arduino
  }
  deg=abs(deg);                                       
}


float kp = .6*90/deg;                         //proportional gain of PI
float ki = .02;                               //integral gain of PI 






void loop() 
{
  // put your main code here, to run repeatedly:
  pinMode(0, INPUT); //Set absolu encoders signal as inputs
  pinMode(1, INPUT); 
  pinMode(2, INPUT); 
  pinMode(3, INPUT); 
  pinMode(4, INPUT); //MSB

      
  t=millis();                 //reading time
  t0=t;                       //saving the current time in memory
  while (t<t0+4000 && rep<=10)         //let the code to ran for 4 seconds each with repetitions of 10
  {
      
    if (t%10==0){                                      //PI controller that runs every 10ms
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



    rep=rep+1;                               // increasing the repetition indicator
    Serial.print("shaft possition from optical absolute sensor from home position: ");
    Serial.println(0);
    
    Serial.print("shaft displacement from optical absolute sensor: ");
    Serial.println(0);
    
    Serial.print("Shaft displacement from motor's builtin encoder: ");
    
    //every full Revolution of the shaft is associated with 228 counts of builtin  
    //encoder so to turn it to degre we can use this formula (s * 360 / 228), "s" is the number of  built-in encoder counts
    Serial.println(s * 360 / 228);
  
    float Error=0-s*360/228;
    Serial.print("Error :");
    Serial.println(Error);                                    //displaying error
    Serial.println();
    s = 0;
    finish=0; 
  }
  analogWrite(6,0);               //turning off the motor
}

int greycodeToAngle(int Afourmsb, int Athree, int Atwo, int Aone, int Azerolsb){
  fourmsb = Afourmsb > treshold ? 1 : 0;
  three = Athree > treshold ? 1 : 0;
  two = Atwo > treshold ? 1 : 0;
  one = Aone > treshold ? 1 : 0;
  zerolsb = Azerolsb > treshold ? 1 : 0;

}

uint8_t Gray2Binary(uint8_t data)
 {
  byte b[8]={0,0,0,0,0,0,0,0};
  int8_t i=0;
  for(i=0;i<8;i++)
  {
    b[i]=(data & (1<<i))>>i;
  }
  uint8_t n_data=0;
  n_data= (b[7]<<7);
  for(i=6;i>=0;i--)  
  {
    if( (b[i+1]+b[i])==0 || (b[i+1]+b[i])==2 )
    {
      n_data|=(0<<i);
    }else if( (b[i+1]+b[i])==1 || (b[i+1]+b[i])==3)
    {
      n_data|=(1<<i);
    }
  }
 return n_data;
}
