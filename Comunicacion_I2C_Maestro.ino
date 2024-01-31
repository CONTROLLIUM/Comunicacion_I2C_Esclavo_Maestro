#include <DHT_U.h>
#include <DHT.h>
#include <Wire.h>
//SDA   2
//SCL   3
#define DHTPIN 4
#define DHTTYPE DHT11
float Hume=0,Temp=0;
bool ButtonState = false;
DHT sensor(DHTPIN,DHTTYPE);
void setup() 
{
    Wire.begin();//iniciar la comunicacion i2c
    sensor.begin();
    pinMode(DHTPIN,INPUT);
    pinMode(7,INPUT);
    
}
void loop()
{   Hume = sensor.readHumidity();
    Temp = sensor.readTemperature();
    ButtonState = digitalRead(7);
    //formar un arreglo de bytescon los datos a enviar
    byte Load[10];
    memcpy(Load,&Temp,sizeof(float));
    memcpy(Load + sizeof(float),&Hume,sizeof(float));
    memcpy(Load + 2* sizeof(float),&ButtonState,sizeof(float));
    //enviar los datos al esclavo
    Wire.beginTransmission(8);//el 8 es la direccion
    Wire.write(Load,10);
    Wire.endTransmission();

    delay(1000);
}