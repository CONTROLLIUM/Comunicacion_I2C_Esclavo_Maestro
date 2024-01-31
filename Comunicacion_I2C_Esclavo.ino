#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Wire.h>
// SDA  21
// SCl  22
float Hume = 0, Temp = 0;
bool ButtonState;
String hora, ip;
const void *dir_ip;
const void *horaUTC;
int periodo = 10000;
unsigned long Tiempo = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
const char *ssid = "Micro_Controllium";
const char *password = "Stk412l$";
WiFiServer server(80);
TaskHandle_t loops;
void ReceiveEvent(int CantBytes)
{
    if (CantBytes == 10)
    {
        byte Load[10];
        Wire.readBytes(Load, CantBytes);

        // leer los datos de temperatura y humead de el arreglo de bytes recibido
        memcpy(&Temp, Load, sizeof(float));
        memcpy(&Hume, Load + sizeof(float), sizeof(float));
        memcpy(&ButtonState, Load + 2 * sizeof(float), sizeof(float));
    }
}
void setup()
{
    Serial.begin(115200);
    Wire.begin(8);// poner un valor en el begin, implica slave, el # tiene que ser el mismo que el de trasmision
    WiFi.begin(ssid, password);
    Wire.onReceive(ReceiveEvent); // funcion para recibir datos
    pinMode(2, OUTPUT);
    pinMode(27, OUTPUT);
    xTaskCreatePinnedToCore(loop2, "loop_2", 1000, NULL, 1, &loops, 0);
    
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Conectado con éxito a la red WiFi.");
    Serial.println("La dirección IP es: ");
    Serial.println(WiFi.localIP());
    server.begin();
    Serial.println("Servidor iniciado");
    timeClient.begin(); // Inicializa servidor NTP
    delay(500);
}

void loop()//aqui estaran los datos en el servidor web
{
    timeClient.update(); // Obtenemos la hora actulizada
    ip = String() + WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
    hora = timeClient.getFormattedTime();
    dir_ip = ip.c_str();
    horaUTC = hora.c_str();

    WiFiClient client = server.available();

    if (client) // Si se cumple la condición, mostramos dotos en el webserver
    {
        Serial.println("Cliente web conectado ");
        String request = client.readStringUntil('\r');
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();
        client.println("<!DOCTYPE html><html>");
        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
        client.println("<link rel=\"icon\" href=\"data:,\">");
        client.println("</style></head><body><h1>Servidor Web con esp32 MICRO_L </h1>");
        client.println("<h2>Tipo de sensor DHT11</h2>");
        client.println("<h2>Luis Miguel Echevarría R</h2>");
        client.println("<table><tr><th>Valores Leidos</th><th>VALUE</th></tr>");
        client.println("<tr><td>Temp. Celsius</td><td><span class=\"sensor\">");
        client.println(Temp);
        client.println(" *C</span></td></tr>");
        client.println("<tr><td>Humedad</td><td><span class=\"sensor\">");
        client.println(Hume);
        client.println(" %</span></td></tr>");
        client.println("<tr><td>Ultima actualizacion Hora UTC</td><td><span class=\"sensor\">");
        client.println(hora);
        client.println("</span></td></tr>");
        client.println("</body></html>");
        client.stop();
        client.println();
        Serial.println("Client disconnected.");
        Serial.println("");
    }

    delay(1000);
}
void loop2(void *parameter)//aqui esta la ejecucion de el pulser
{
    for (;;)
    {   
        if (ButtonState)
        {
            digitalWrite(27, HIGH);
           
        }
        else
        {
            digitalWrite(27, LOW);
            
        }
        vTaskDelay(10);
    }
}