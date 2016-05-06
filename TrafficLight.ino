#include <SPI.h>
#include <Ethernet.h>

int ledNorth = 2;
int ledSouth = 3;
int ledEast = 8;
int ledWest = 9;

int leds[] = {ledNorth, ledSouth, ledEast, ledWest};

int redDuration = 2000;
int yellowDuration = 3000;
int greenDuration = 10000;
int flashingDuration = 5000;

long lastTime = 0;
long currentTime = 0;
long blinkTime = 0;
byte light = 0;
bool toggle = true;

String inputString = "";
bool readData = false;

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x12, 0x10 };   //physical mac address
EthernetServer server(80);                             //server port     
String readString;

void setup()
{
  for (int i = 0; i < 4; i++){
     pinMode(leds[i], OUTPUT);
  }
  
    Serial.begin(9600);
    
  // start the Ethernet connection and the server:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    //do nothing forever:
    //for(;;)
    //  ;
  }
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
    
  digitalWrite(ledEast, HIGH);
  digitalWrite(ledWest, HIGH);
  digitalWrite(ledNorth, HIGH);
  digitalWrite(ledSouth, LOW);
  inputString.reserve(200);
}

void loop()
{
  /*if (readData)
  {
    char whichDuration = inputString.charAt(0);
    int newDuration = inputString.substring(2).toInt();
    
    switch (whichDuration)
    {
      case 'r':
        redDuration = newDuration;
        break;
      case 'y':
        yellowDuration = newDuration;
        break;
      case 'g':
        greenDuration = newDuration;
        break;
      case 'f':
        flashingDuration = newDuration;
        break;
    }
    
    inputString = "";
    readData = false;
  }*/
  
  handleClient();//make the web server check for clients and potentially update timings
  
  currentTime = millis();
  switch (light)
  {
    case 0:
      if (currentTime - lastTime < flashingDuration)
      {
        if ((currentTime - blinkTime) >= 125)
        {
          if (toggle)
          {
            digitalWrite(ledNorth, LOW);
          }
          else
          {
            digitalWrite(ledNorth, HIGH);
          }

          toggle = !toggle;
          blinkTime = currentTime;
        }
      }
      else
      {
        digitalWrite(ledNorth, LOW);
        lastTime = currentTime;
        light++;
      }
      break;

    case 1:
      if (currentTime - lastTime >= greenDuration)
      {
        digitalWrite(ledSouth, HIGH);
        lastTime = currentTime;
        light++;
      }
      break;

    case 2:
      if (currentTime - lastTime >= yellowDuration)
      {
        digitalWrite(ledNorth, HIGH);
        lastTime = currentTime;
        light++;
      }
      break;

    case 3:
      if (currentTime - lastTime >= redDuration)
      {
        digitalWrite(ledWest, LOW);
        lastTime = currentTime;
        light++;
        toggle = true;
      }
      break;

    case 4:
      if (currentTime - lastTime < flashingDuration)
      {
        if ((currentTime - blinkTime) >= 125)
        {
          if (toggle)
          {
            digitalWrite(ledEast, LOW);
          }
          else
          {
            digitalWrite(ledEast, HIGH);
          }

          toggle = !toggle;
          blinkTime = currentTime;
        }
      }
      else
      {
        digitalWrite(ledEast, LOW);
        lastTime = currentTime;
        light++;
      }
      break;

    case 5:
      if (currentTime - lastTime >= greenDuration)
      {
        digitalWrite(ledWest, HIGH);
        lastTime = currentTime;
        light++;
      }
      break;

    case 6:
      if (currentTime - lastTime >= yellowDuration)
      {
        digitalWrite(ledEast, HIGH);
        lastTime = currentTime;
        light++;
      }
      break;

    case 7:
      if (currentTime - lastTime >= redDuration)
      {
        digitalWrite(ledSouth, LOW);
        lastTime = currentTime;
        light = 0;
        toggle = true;
      }
      break;
  }
}

/*void serialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    inputString += inChar;

    if (inChar == '\n')
    {
      readData = true;
    }
  }
}*/

void handleClient(){

  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
      while (client.connected()) {   
        if (client.available()) {
          char c = client.read();
       
          //read char by char HTTP request
          if (readString.length() < 100) {
            //store characters to string
            readString += c;
            //Serial.print(c);
           }
  
           //if HTTP request has ended
           if (c == '\n') {          
             Serial.println(readString); //print to serial monitor for debuging
       
             client.println("HTTP/1.1 200 OK"); //send new page
             client.println("Content-Type: text/html");
             client.println();     
             client.println("<HTML>");
             client.println("<BODY>");
             client.println("<H1>Team Scorpion: Canadian Traffic Lights</H1>");
             client.println("<hr />");
             client.println("<br />");  
             client.println("<H2>Traffic Light Adjuster</H2>");
             client.println("<br />");  
             client.println("<form action=\"submitFields\">");
             client.println("<fieldset>");
             client.println("<legend>Change light durations:</legend>");
             client.println("RED Duration: <output name=\"redOutputName\" id=\"redOutputId\">" + String(redDuration) + " </output> MS<br>");
             client.println("<input type=\"range\" name=\"red\" id=\"redInputId\" value=\"" + String(redDuration) + "\" min=\"0\" max=\"20000\" oninput=\"redOutputId.value = redInputId.value\">");
             client.println("<br>YELLOW Duration: <output name=\"yellowOutputName\" id=\"yellowOutputId\">"+ String(yellowDuration) +" </output> MS<br>");
             client.println("<input type=\"range\" name=\"yellow\" id=\"yellowInputId\" value=\""+ String(yellowDuration) +"\" min=\"0\" max=\"20000\" oninput=\"yellowOutputId.value = yellowInputId.value\">");
             client.println("<br>GREEN Duration: <output name=\"greenOutputName\" id=\"greenOutputId\">"+ String(greenDuration) +" </output> MS<br>");
             client.println("<input type=\"range\" name=\"green\" id=\"greenInputId\" value=\""+ String(greenDuration) +"\" min=\"0\" max=\"20000\" oninput=\"greenOutputId.value = greenInputId.value\">");
             client.println("<br>FLASHING Duration: <output name=\"flashingOutputName\" id=\"flashingOutputId\">"+ String(flashingDuration) +"</output> MS<br>");
             client.println("<input type=\"range\" name=\"flashing\" id=\"flashingInputId\" value=\""+ String(flashingDuration) +"\" min=\"0\" max=\"20000\" oninput=\"flashingOutputId.value = flashingInputId.value\">");
             client.println("<br><br>");     
             client.println("<input type=\"submit\" value=\"Submit\">");
             client.println("<br><p><strong>Created by: Therese Horey, Kory Dondzila, and Garret Richardson</strong></p>");
             client.println("<br><p>Please use a modern browser to view this webpage.</p>");
             client.println("</BODY>");
             client.println("</HTML>");
       
             delay(1);
             //stopping client
             client.stop();
             //controls the Arduino if you press the buttons
             if (readString.indexOf("submitFields") >0){
               int redIndex = readString.indexOf("red=");
               int yellowIndex = readString.indexOf("&yellow=");
               int greenIndex = readString.indexOf("&green=");
               int flashingIndex = readString.indexOf("&flashing=");
               /*Serial.print("Indexof redDuration: ");
               Serial.print(redIndex);
               Serial.print(" -- Value: ");
               Serial.println(readString.substring(redIndex + 4,yellowIndex));//on each of these, i'm adding the length of the indexing string so we start at the = sign on the left
               
               Serial.print("Indexof yellowDuration: ");
               Serial.print(yellowIndex);
               Serial.print(" -- Value: ");
               Serial.println(readString.substring(yellowIndex + 8,greenIndex));
               
               Serial.print("Indexof greenDuration: ");
               Serial.print(greenIndex);
               Serial.print(" -- Value: ");
               Serial.println(readString.substring(greenIndex + 7,flashingIndex));
               
               Serial.print("Indexof flashingDuration: ");
               Serial.print(flashingIndex);
               Serial.print(" -- Value: ");
               Serial.println(readString.substring(flashingIndex + 10,readString.indexOf(" HTTP/")));*/
               redDuration = readString.substring(redIndex + 4,yellowIndex).toInt();
               yellowDuration = readString.substring(yellowIndex + 8,greenIndex).toInt();
               greenDuration = readString.substring(greenIndex + 7,flashingIndex).toInt();
               flashingDuration = readString.substring(flashingIndex + 10,readString.indexOf(" HTTP/")).toInt();
               
             }
             //clearing string for next read
             readString="";  
             
           }
         }
      }
  }
}

