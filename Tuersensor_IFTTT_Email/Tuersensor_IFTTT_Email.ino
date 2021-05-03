include <ESP8266WiFi.h> //Boardbibliotheken müssen installiert sein
	#include <ESP8266HTTPClient.h>
	
	char ssid[] = "WLAN12345";       // deine Wlan SSID (Name)
	char password[] = "Admin123";  // dein Wlan Passwort 
	bool knopfi = false;   //ein boolean wird erzeugt, der entweder true oder false sein kann 
	int Reading; 
	
	
	
	
	void setup() {
	  pinMode(D1, INPUT_PULLUP);
	  pinMode(LED_BUILTIN,OUTPUT); 
	  digitalWrite(LED_BUILTIN, HIGH);
	  Serial.begin(115200); //Start des seriellen Monitors, zur zeitnahen Überprüfung 
	  WiFi.mode(WIFI_STA);
	  WiFi.disconnect();
	  delay(100);
	
	  Serial.print("Connecting Wifi: ");
	  Serial.println(ssid);
	  WiFi.begin(ssid, password);
	  while (WiFi.status() != WL_CONNECTED) {
	    Serial.print(".");
	    delay(500);
	  }
	  Serial.println("");
	  Serial.println("WiFi connected");
	  Serial.println("IP address: ");
	  IPAddress ip = WiFi.localIP();
	  Serial.println(ip);
	
	
	
	  if (knopfi == false) // damit diese Fallunterscheidung nur einmal aktiv wird
	    {                                        
	    knopfi = true;
	  HTTPClient http;
	  http.begin("http://maker.ifttt.com/trigger/EVENTNAME/with/key/Vorgegebener Code von Webhooks<ifttt");   //!!! http und nicht httpS !!!
	  http.GET();
	  http.end();
	  Serial.print("EVENTNAME wurde versendet");
	}}
	
	void loop() //wiederholt sich fortlaufend
	{
	  Reading = digitalRead(D1); // liest den Pin D1 aus - dort ist der eine kontakt des Reed-Schalters anzubinden, genauso wie an einem GROUND Anschluss des NodeMCUS
	  if(Reading == HIGH)   //Wenn Kontakt zwischen Tür und Wand weg ist
	  { 
	        digitalWrite(LED_BUILTIN, LOW);
	        HTTPClient http;
	        http.begin("http://maker.ifttt.com/trigger/EVENTNAME2/with/key/Vorgegebener Code von Webhooks<ifttt");//!!! http und nicht httpS !!! + der selbe Code wie zuvor
	        http.GET();
	        http.end();
	        Serial.println("...EVENTNAME2...Email wurde versand");
	        Serial.println("15sek warten");
	        delay(15000); // 15sek verbleiben um die Tür wieder in normalzustand zu versetzen
	        }
	
	        if(Reading == LOW)
	       {
	          digitalWrite(LED_BUILTIN, HIGH);
	          Serial.println("Tür ist zu");
	          delay(500); //ein kurzer Delay ist nötig, um Performancetechnisch dabei zu bleiben
	          
	          }
	     }
