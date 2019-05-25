#include <ESP8266WiFi.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11


// Replace with your network credentials
const char* ssid     = "SSID";
const char* password = "PASSWORD";

#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 

// Set web server port number to 80
WiFiServer server(80);


String header;


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.begin();

   Serial.println("Humidity and temperature\n\n");
  delay(700);

}

void loop(){
    //lastinsert( dht.readHumidity()); Circular buffer
    //temp_buff[i] = dht.readTemperature();  filter application
    float h = dht.readHumidity();
    float t = dht.readTemperature();         
    Serial.print("Current humidity = ");
    Serial.print(h);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(t); 
    Serial.print("C  ");
    int val=analogRead(A0);
    Serial.print("moisture= ");
    Serial.println(val);
    /* For filter application
     if(i<=512)
    {
      for(int j=0;j<510;j++)
      {
        for(int k=0;k<3;k++)
        {
          temp_sum+=temp_buff[i+j];
        }
        temp_y[i] = temp_sum/3;
      }
      i=0;
    }
     */
    if(t>32)
      digitalWrite(LED_BUILTIN, LOW);
    else
      digitalWrite(LED_BUILTIN, HIGH);
    delay(2000);
  WiFiClient client = server.available();  
  if (client) {                            
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();          
        header += c;
        if (c == '\n') {                   
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
          
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            client.println("<body><h1>ESP8266 Web Server</h1>");

            client.println("<body><h3>FAN(On if greater than 35deg C)</h3>");
              
            if (t>35) {
              client.println("<p><a href=\"/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            client.println("<body><h3>Pump(On if less than 500(adc val))</h3>");
            if (val>500) {
              client.println("<p><a href=\"/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
    header = "";
    client.stop();
  }
}

/* For implmenting Moving average filter
float temp_buff[512];
float hum_buff[512];

float hum_y[512];
float temp_y[512];

float hum_sum = 0, temp_sum = 0;

int i=0;
int n=0;
*/
/* For implementing Circuilar buffer
struct node   
{  
    float data;  
    struct node *next;   
};  
struct node *head;  
int count=0; 
void beginsert (float);   
void lastinsert (float);  
void last_delete();  
void display();

void lastinsert(float item)  
{  

if(count>511)//in place of 3 add the size of buffer required-1
{

if(count%511==0 && count>0)
display();

int c=0;
struct node *temp2;
temp2=head;
while(c<count%511)
{

temp2=temp2->next;
c++;

}
temp2->data=item;
count++;
}

else{

    struct node *ptr,*temp;   
     
    ptr = (struct node *)malloc(sizeof(struct node));  
    if(ptr == NULL)  
    {  
        //printf("\nOVERFLOW\n");  
    }  
    else  
    {  
         
        ptr->data = item;  
        if(head == NULL)  
        {  
            head = ptr;  
            ptr -> next = head;
    count++; 
//printf("%d", count);   
        }  
        else  
        {  
            temp = head;  
            while(temp -> next != head)  
            {  
                temp = temp -> next;  
            }  
            temp -> next = ptr;   
            ptr -> next = head;
        count++; 
        }  
          
        
    } 
} 
}

void display()  
{  
    struct node *ptr;  
    ptr=head; float sum=0;  
    if(head == NULL)  
    {  
        //printf("\nnothing to print");  
    }     
    else  
    {  
        //printf("\n sending to server ... \n");  
         n=0; int c1=1;
        while(ptr -> next != head)  
        {  sum=sum+ptr->data; 

          
            printf("%d ", ptr->data);
           sendreading(temp_y[n],ptr->data);
            ptr = ptr -> next; 
            n++;
        }           
    }  
              
}
*/ 

/* For sending readings to the server
void sendreading(float temp, float hum)
{
    wifiStatus = WiFi.status();

      if(wifiStatus == WL_CONNECTED){

         HTTPClient http;
         Serial.println("Sensor value to be sent");
         Serial.println("Your are now connected!");  
         String ip = "<IP>";

         String hum = String(hum);
         String temp = String(temp);
         http.begin("http://" + ip + "/recieve.php?humidity=" + hum + "temperature" + temp);
         int httpCode = http.GET();
         if(httpCode > 0)
         {
          Serial.println("[HTTP] GET... code: \n");
         }
         else{ 
          Serial.println("[HTTP] GET... failed, error: \n");
         }
        http.end();
      }
      else{
        Serial.println("");
        Serial.println("WiFi not connected");
      }
      delay(1000);
} 
 */
