
#include<Wire.h>

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;


int minVal=265;
int maxVal=402;
int timer = 1;

void setup(){
  Wire.begin();
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
}
void loop(){
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
    
    int xAng = map(AcX,minVal,maxVal,-90,90);
    int yAng = map(AcY,minVal,maxVal,-90,90);
    int zAng = map(AcZ,minVal,maxVal,-90,90);

       double calc_X = RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
       double calc_Y = RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
       double calc_Z = RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
       
    if(calc_X <= 195 && calc_X >= 150 )
    {
    // als de gyroscope ondersteboven(onderzijde van chip ligt rechtop) is print die shutdown.
      Serial.print("shut down functions");
      Serial.print("DegX= ");  Serial.println(calc_X);
    }else
    {
        //Serial.print("DegX= ");  Serial.println(calc_X);
    }
    if(calc_X >= 357 && calc_X <= 360)
    {
     
      if(timer == 5)
      {
        Serial.print("shut down MLG");
        timer = 1;
      }else
      {
        delay(1000);
        timer++;
        Serial.print(timer);
      }
     
    }
    if(calc_X <= 355)
      timer = 1;
     Serial.println("-----------------------------------------");
     delay(450);
}
