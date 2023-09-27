uint8_t Binary2Gray(uint8_t data)
 {
   int n_data=(data>>1);
   n_data=(data ^ n_data);
   
  return n_data;
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
void setup() {
  // put your setup code here, to run once:

Serial.begin(4800);
Serial.println(" Gray binary conversion test");
uint8_t i=0;
uint8_t gc=0;
uint8_t try1 = 0b00110;
  for (i=0;i<=20;i++)
  { 
    Serial.print("GRAY CODE of  BINARY  ");Serial.print(i,BIN);Serial.print(" is ");
   gc=Binary2Gray(i);
    Serial.println(gc,BIN); 
  }
  gc=0x00;

    for (i=0;i<=20;i++)
  { 
    Serial.print("BINARY of  GRAY CODE ");Serial.print(i,BIN);Serial.print(" is ");
   gc=Gray2Binary(i);
    Serial.println(gc,BIN);
  }

  
  uint8_t test = Gray2Binary(try1);
  Serial.println(test, BIN);
}

void loop() {
 
}
