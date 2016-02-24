#include <SoftwareSerial.h>

SoftwareSerial ESP8266(10, 11);

String NomduReseauWifi = "{{id_reseau}}"; // Garder les guillements
String MotDePasse      = "{{pass_reseau}}"; // Garder les guillements

String request = "";

const int capteurHygro = 0;
int valeurHygro = 0;
int capteurHygro_D = 7;
int sendAlert = 1;

String pushToken = "{{push_token}}";
String pushUserKey = "{{push_user_key}}";
String pushDevice = "{{push_device}}";
 


/****************************************************************/
/*                             INIT                             */
/****************************************************************/
void setup()
{
  Serial.begin(115200);
  ESP8266.begin(115200);  
  initESP8266();
  pinMode(capteurHygro_D,INPUT);
}
/****************************************************************/
/*                        BOUCLE INFINIE                        */
/****************************************************************/
void loop()
{
   while(ESP8266.available())
   {
     request = ESP8266.readString();
   }
   if(ESP8266.find("+IPD,"))
   {
     
//         char gauge[] PROGMEM  = "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">"
//                                 "<g name=\"gauge\" width=\"122px\" height=\"127px\">"
//                "<g inkscape:label=\"Calque 1\" inkscape:groupmode=\"layer\" id=\"layer1\" transform=\"translate(0,-925.36216)\">"
//                "<path"
//                "       style=\"opacity:0.98999999;fill:#ffffff;fill-opacity:1;stroke:#3eff11;stroke-width:5;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\""
//                "       id=\"path4136\""
//                "       sodipodi:type=\"arc\""
//                "       sodipodi:cx=\"657.70258\""
//                "       sodipodi:cy=\"-743.77502\""
//                "       sodipodi:rx=\"49.750011\""
//                "       sodipodi:ry=\"49.750011\""
//                "       sodipodi:start=\"0\""
//                "       sodipodi:end=\"4.7220057\""
//                "       sodipodi:open=\"true\""
//                "       d=\"m 707.45259,-743.77502 a 49.750011,49.750011 0 0 1 -30.82207,46.00865 49.750011,49.750011 0 0 1 -54.27526,-10.99964 49.750011,49.750011 0 0 1 -10.47719,-54.37853 49.750011,49.750011 0 0 1 46.30293,-30.3782\""
//                "       transform=\"matrix(-0.70710678,0.70710678,-0.70710678,-0.70710678,0,0)\" />"
//                "  </g>"
//                "       <circle id=\"led\" cx=\"39\" cy=\"76\" r=\"5\" style=\"fill: #999; stroke: none\">"
//                "            <animateColor id=\"ledAnimation\" attributeName=\"fill\" attributeType=\"css\" begin=\"0s\" dur=\"1s\""
//                "            values=\"none;#f88;#f00;#f88;none;\" repeatCount=\"0\"/>"
//                "        </circle>"
//                "        <g id=\"needle\" transform=\"rotate(0,62,62)\">"
//                "            <circle cx=\"62\" cy=\"62\" r=\"4\" style=\"fill: #c00; stroke: none\"/>"
//                "            <rect transform=\"rotate(-130,62,62)\" name=\"arrow\"  x=\"58\" y=\"38\" width=\"8\" height=\"24\" style=\"fill: #c00; stroke: none\"/>"
//                "            <polygon transform=\"rotate(-130,62,62)\" points=\"58,39,66,39,62,30,58,39\" style=\"fill: #c00; stroke: none\"/>"
//                "        </g>"
//                "        <text id=\"value\" x=\"51\" y=\"98\" focusable=\"false\" editable=\"no\" style=\"stroke:none; fill:#fff; font-family: monospace; font-size: 12px\"></text>"
//                "    </g>"
//
//                "    <script type=\"text/javascript\">"
//                "        var scale=100;"
//                "        var value;"
//                "        var value1 = 69;"
//
//                "        var needle_el = document.getElementById('needle');"
//                "        value = parseInt(value1);"
//                "        scale = parseInt(scale);"
//                "        if (value > scale) value = scale;"
//                "        var angle = value / scale * 260;"
//                "        needle_el.setAttribute('transform','rotate('+angle+',62,62)');"
//
//                "    </script>"
//                "</svg>";
//                
//         unsigned int gauge_length = sizeof(gauge);
              
       String Header;
       valeurHygro = analogRead(capteurHygro);
       Serial.println(valeurHygro);
       
       Header =  "HTTP/1.1 200 OK\r\n";
       Header += "Content-Type: text/html\r\n";
       Header += "Connection: close\r\n";  
       Header += "Refresh: 5\r\n"; 
       
       String HeaderPage = "<html><body>Valeur du capteur : "+String(valeurHygro);
              HeaderPage += "<br>Seuil : "+String(digitalRead(capteurHygro_D));
       String FooterPage = "</body></html>";
       Header += "Content-Length: ";
//         Header += (int)(HeaderPage.length()+gauge_length+FooterPage.length());
       Header += (int)(HeaderPage.length()+FooterPage.length());
       Header += "\r\n\r\n";
       delay(1000);
       Serial.println(request);
       int connectionId = ESP8266.read()-48;   
       Serial.println(connectionId);
       String cipSend = "AT+CIPSEND=";
              cipSend += connectionId;
              cipSend += ",";
              //cipSend +=(Header.length()+HeaderPage.length()+gauge_length+FooterPage.length());
              cipSend +=(Header.length()+HeaderPage.length()+FooterPage.length());
              cipSend +="\r\n";
       envoieAuESP8266(cipSend);
       delay(10);
       envoieAuESP8266(Header);
       envoieAuESP8266(HeaderPage);
//         sendProgStr( &gauge);
       envoieAuESP8266(FooterPage);
       delay(10);
 
       String closeCommand = "AT+CIPCLOSE=";
              closeCommand+=connectionId; // append connection id
              closeCommand+="\r\n";
   
       envoieAuESP8266(closeCommand);

//    if(sendAlert != digitalRead(capteurHygro_D))
//    {
//      Serial.println('Send Alerte');
//      String Msg = "token="+pushToken+"&user="+pushUserKey+"&device="+pushDevice+"&title=Allerte+arrosage&message=Il+serait+sympa+de+me+donner+a+boire.";
//      String PostMsg = "POST /1/messages.json";
//             PostMsg += "Host: api.pushover.net";
//             PostMsg += "Content-Type: application/x-www-form-urlencoded";
//             PostMsg += "Content-Length: "+Msg.length();
//      String cipPost = "AT+CIPSEND=";
////                cipPost += connectionId;
////                cipPost += ",";
//                cipPost +=(PostMsg.length()+Msg.length());
//                cipPost +="\r\n";
//       envoieAuESP8266(cipPost);
//       envoieAuESP8266(PostMsg);
//       envoieAuESP8266(Msg);
//
//    }
  }
       
   
   //Serial.println(valeurHygro);
}


//  void sendProgStr(const char* str)
//  {
//    char c;
//    if(!str) return;
//    while((c = pgm_read_byte(str++)))
//         ESP8266.print(c);
//  }

/****************************************************************/
/*                Fonction qui initialise l'ESP8266             */
/****************************************************************/
void initESP8266()
{  
  Serial.println("**********************************************************");  
  Serial.println("**************** DEBUT DE L'INITIALISATION ***************");
  Serial.println("-------------------- Restart Module ----------------------");  
  envoieAuESP8266("AT+RST");
  recoitDuESP8266(2000);
  Serial.println("------------------ Wifi Mode STA + AP --------------------");
  envoieAuESP8266("AT+CWMODE=3");
  recoitDuESP8266(5000);
  Serial.println("-------------- Connexion au réseau Wifi ------------------");
  envoieAuESP8266("AT+CWJAP=\""+ NomduReseauWifi + "\",\"" + MotDePasse +"\"");
  recoitDuESP8266(10000);
  Serial.println("------------------ Check Adresse IP ----------------------");
  envoieAuESP8266("AT+CIFSR");
  recoitDuESP8266(1000);
  Serial.println("----------------- Connexions multiples -------------------");
  envoieAuESP8266("AT+CIPMUX=1");   
  recoitDuESP8266(1000);
  Serial.println("------------ Ouverture du server sur le port 80 ----------");
  envoieAuESP8266("AT+CIPSERVER=1,80");
  recoitDuESP8266(1000);
  Serial.println("***************** INITIALISATION TERMINEE ****************");
  Serial.println("**********************************************************");
  Serial.println("");  
}

/****************************************************************/
/*        Fonction qui envoie une commande à l'ESP8266          */
/****************************************************************/
void envoieAuESP8266(String commande)
{  
  ESP8266.println(commande);
}
/****************************************************************/
/*Fonction qui lit et affiche les messages envoyés par l'ESP8266*/
/****************************************************************/
void recoitDuESP8266(const int timeout)
{
  String reponse = "";
  long int time = millis();
  while( (time+timeout) > millis())
  {
    while(ESP8266.available())
    {
      char c = ESP8266.read();
      reponse+=c;
    }
  }
  Serial.print(reponse);   
}

