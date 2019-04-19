int En0 = 7; 
int En1 = 6; 
int S0  = 5;
int S1  = 4;
int S2  = 3;
int S3  = 2; 
int SP  = A3; 

void setup() {
  pinMode(En0, OUTPUT);
  pinMode(En1, OUTPUT);
  pinMode(S0,  OUTPUT);
  pinMode(S1,  OUTPUT);
  pinMode(S2,  OUTPUT);
  pinMode(S3,  OUTPUT);
}

void loop() {
  getdata();
  delay(2000);
}

#define MAXCH 31

char* getdata() {
  int controlPin[] = {S0,S1,S2,S3,En0,En1};
  int muxChannel[MAXCH][6]={
    {0,0,0,0,0,1}, //channel 0
    {0,0,0,1,0,1}, //channel 1
    {0,0,1,0,0,1}, //channel 2
    {0,0,1,1,0,1}, //channel 3
    {0,1,0,0,0,1}, //channel 4
    {0,1,0,1,0,1}, //channel 5
    {0,1,1,0,0,1}, //channel 6
    {0,1,1,1,0,1}, //channel 7
    {1,0,0,0,0,1}, //channel 8
    {1,0,0,1,0,1}, //channel 9
    {1,0,1,0,0,1}, //channel 10
    {1,0,1,1,0,1}, //channel 11
    {1,1,0,0,0,1}, //channel 12
    {1,1,0,1,0,1}, //channel 13
    {1,1,1,0,0,1}, //channel 14
    {1,1,1,1,0,1}, //channel 15
    {0,0,0,0,1,0}, //channel 16
    {0,0,0,1,1,0}, //channel 17
    {0,0,1,0,1,0}, //channel 18
    {0,0,1,1,1,0}, //channel 19
    {0,1,0,0,1,0}, //channel 20
    {0,1,0,1,1,0}, //channel 21
    {0,1,1,0,1,0}, //channel 22
    {0,1,1,1,1,0}, //channel 23
    {1,0,0,0,1,0}, //channel 24
    {1,0,0,1,1,0}, //channel 25
    {1,0,1,0,1,0}, //channel 26
    {1,0,1,1,1,0}, //channel 27
    {1,1,0,0,1,0}, //channel 28
    {1,1,0,1,1,0}, //channel 29
    {1,1,1,0,1,0}  //channel 30
  };
  for(int channel=0; channel< MAXCH; channel++) {
    for(int i=0; i<6; i++) {
      digitalWrite(controlPin[i], muxChannel[channel][i]);
    }
    Serial.print(analogRead(SP));
  }
  Serial.println();
}


