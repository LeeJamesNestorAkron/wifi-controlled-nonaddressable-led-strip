
// Wifi Libraries
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <WebSocketsServer.h>
#include <NTPClient.h>

//IR management
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>

//Number shit
#include <inttypes.h>


//Wifi vars
const char* ssid = "*******";
const char* password = "******";
const char* mdnsName = "UberLeds";
const long utcOffsetInSeconds = -18000;

//Ir Vars
const uint16_t IrLED = 14;
const uint16_t kRecvPin = 13;

//The IR Remote keycodes divided by 256
const int RemoteKey[] {
  65595, 65723, 65667, 65539,
  65563, 65691, 65699, 65571,
  65579, 65707, 65683, 65555,
  65547, 65675, 65715, 65587,
  65593, 65721, 65657, 65785,
  65561, 65689, 65625, 65753,
  65577, 65705, 65641, 65769,
  65545, 65673, 65609, 65737,
  65585, 65713, 65649, 65777,
  65553, 65681, 65617, 65745,
  65569, 65697, 65633, 65761
};

// Alarm Vars
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool ledAlarm = false;
char * alarmTime = "00:00";
char * currentTime = "00:00";
int anotherAlarmTime;


//Wifi objects
ESP8266WebServer server(80);
WebSocketsServer webSocket(81);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//Ir objects
IRrecv irrecv(kRecvPin);
IRsend irsend(IrLED);
decode_results results;

//Function
void wifiSetup();
void wifiMaintain();
void ifNotFound();

//The Webpage
char webpage[] PROGMEM = R"=====(
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1" /> 
  <script>
var Socket
//Initializing function, sets up the websocket object and retrieve the alarm time value
function initial()
    {
    console.log("initial");
    Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
    Socket.onmessage = function(event)
        {
        console.log("recieved");
        console.log(event.data);
        document.getElementById("clock").value = event.data;
        }
    Socket.onopen=() => Socket.send("retrieveAlarmTime");
    }
//Sends the set time alarm Value to the socket
function sendTime()
    {
    console.log("send Time");
    Socket.send(document.getElementById("Alarm").value);
    }

<!--Sends the input on the remote-->
function sendSignal(value)
    {
    console.log("send Signal");
    Socket.send(value);
    }
  </script>
</head>
<body onload="javascript:initial()">
<!--list of buttons and inputs-->
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "Bright" onclick="sendSignal(this.value)" value = "00" >Bright </button>
    <button style="width:20%" id = "Dim" onclick="sendSignal(this.value)" value = "01">Dim</button>
    <button style="width:20%" id = "Skip" onclick="sendSignal(this.value)" value = "02"> Skip</button>
    <button style="width:20%" id = "OnOff" onclick="sendSignal(this.value)" value = "03">On/Off </button>
</div>
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "Red" onclick="sendSignal(this.value)" value = "04" >RED </button>
    <button style="width:20%" id = "Green" onclick="sendSignal(this.value)" value = "05">GREEN</button>
    <button style="width:20%" id = "Blue" onclick="sendSignal(this.value)" value = "06"> BLUE</button>
    <button style="width:20%" id = "White" onclick="sendSignal(this.value)" value = "07">WHITE </button>
</div>
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "RedOrange" onclick="sendSignal(this.value)" value = "08" >RedOrange </button>
    <button style="width:20%" id = "GreenAqua" onclick="sendSignal(this.value)" value = "09">GreenAqua</button>
    <button style="width:20%" id = "BluePurple" onclick="sendSignal(this.value)" value = "10"> BluePurple</button>
    <button style="width:20%" id = "Pink" onclick="sendSignal(this.value)" value = "11">Pink </button>
</div>
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "Orange" onclick="sendSignal(this.value)" value = "12" >Orange </button>
    <button style="width:20%" id = "Aqua" onclick="sendSignal(this.value)" value = "13">Aqua</button>
    <button style="width:20%" id = "Purple" onclick="sendSignal(this.value)" value = "14"> Purple</button>
    <button style="width:20%" id = "LightPink" onclick="sendSignal(this.value)" value = "15">LightPink </button>
</div>
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "OrangeYellow" onclick="sendSignal(this.value)" value = "16" >OrangeYellow </button>
    <button style="width:20%" id = "CyanMoss" onclick="sendSignal(this.value)" value = "17">CyanMoss</button>
    <button style="width:20%" id = "PurpleViolet" onclick="sendSignal(this.value)" value = "18"> PurpleViolet</button>
    <button style="width:20%" id = "LightBlue" onclick="sendSignal(this.value)" value = "19">LightBlue </button>
</div>
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "Yellow" onclick="sendSignal(this.value)" value = "20" >Yellow </button>
    <button style="width:20%" id = "Moss" onclick="sendSignal(this.value)" value = "21">Moss</button>
    <button style="width:20%" id = "Violet" onclick="sendSignal(this.value)" value = "22"> Violet</button>
    <button style="width:20%" id = "SkyBlue" onclick="sendSignal(this.value)" value = "23">SkyBlue </button>
</div>
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "CustomRedUp" onclick="sendSignal(this.value)" value = "24" >CustomRedUp </button>
    <button style="width:20%" id = "CustomGreenUp" onclick="sendSignal(this.value)" value = "25">CustomGreenUp</button>
    <button style="width:20%" id = "CustomBlueUp" onclick="sendSignal(this.value)" value = "26"> CustomBlueUp</button>
    <button style="width:20%" id = "Quicken" onclick="sendSignal(this.value)" value = "27">Quicken </button>
</div>
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "CustomRedDown" onclick="sendSignal(this.value)" value = "28" >CustomRedDown </button>
    <button style="width:20%" id = "CustomGreenDown" onclick="sendSignal(this.value)" value = "29">CustomGreenDown</button>
    <button style="width:20%" id = "CustomBlueDown" onclick="sendSignal(this.value)" value = "30"> CustomBlueDown</button>
    <button style="width:20%" id = "Slowen" onclick="sendSignal(this.value)" value = "31">Slowen </button>
</div>
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "DIYColour1" onclick="sendSignal(this.value)" value = "32" >DIYColour1 </button>
    <button style="width:20%" id = "DIColour2" onclick="sendSignal(this.value)" value = "33">DIColour2</button>
    <button style="width:20%" id = "DIYColour3" onclick="sendSignal(this.value)" value = "34"> DIYColour3</button>
    <button style="width:20%" id = "Auto" onclick="sendSignal(this.value)" value = "35">Auto </button>
</div>
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "DIY4" onclick="sendSignal(this.value)" value = "36" >DIY4 </button>
    <button style="width:20%" id = "DIY5" onclick="sendSignal(this.value)" value = "37">DIY5</button>
    <button style="width:20%" id = "DIY6" onclick="sendSignal(this.value)" value = "38"> DIY6</button>
    <button style="width:20%" id = "FLASH" onclick="sendSignal(this.value)" value = "39">FLASH </button>
</div>
<div class="btn-group"  style="width:100%">
    <button style="width:20%" id = "JUMP3" onclick="sendSignal(this.value)" value = "40" >JUMP3</button>
    <button style="width:20%" id = "JUMP7" onclick="sendSignal(this.value)" value = "41">JUMP7</button>
    <button style="width:20%" id = "FADE3" onclick="sendSignal(this.value)" value = "42"> FADE3</button>
    <button style="width:20%" id = "FADE7" onclick="sendSignal(this.value)" value = "43">FADE7 </button>
</div>
<div class="slidecontainer">
<!-- //Send and Display Alarm time -->
  <input type="time" oninput="sendTime()" class ="clock" id="Alarm">
  <textarea id="clock"> </textarea> 
</div>
</form>
</body>
</html>
)=====";


//Interpretting Websocket Event data
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
    switch (type) 
    {
        case WStype_DISCONNECTED:             // if the websocket is disconnected
            Serial.printf("[%u] Disconnected!\n", num);
            break;
            
        case WStype_CONNECTED: 
            {              // if a new websocket connection is established
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            }
            break;
            
          case WStype_TEXT:  
            {
    //If the payload is an alarm time
                if(payload[2]==':')
                  {
    //Replace the global variable, alarmTime, with the payload
                for(int i =0;i<5;i++)
                    {
                    alarmTime[i] = payload[i];
                    }
                Serial.printf("[%u] get Text: %s\n", num, payload);
    //Send back the alarmTime
                webSocket.broadcastTXT(alarmTime, sizeof(alarmTime)+1);
    //Take the last two numbers of the payload into the third and fourth characters
                for(int i =0;i<2;i++)
                      {
                      alarmTime[i+2] = payload[i+3];
                      }
    //Terminate the alarmTime and move it into an integer representation
                alarmTime[4] = 0;
                anotherAlarmTime = atoi(alarmTime);
                Serial.println("Alarm Time Recieved");
                Serial.println(alarmTime);
    //If the payload is requesting the alarmTime already set
                  } else if(payload[0]=='r')
                      {
                        //Turn the alarm back into a normal alarm time with a colon
                        Serial.println("Alarm Time requested");
                        if(alarmTime[2] != ':')
                        {
                          alarmTime[4] = alarmTime[3];
                          alarmTime[3] = alarmTime[2];
                          alarmTime[2] = ':';
                        }
                      webSocket.broadcastTXT(alarmTime, sizeof(alarmTime)+1);
                      }else 
    //if it is not an alarmTime or an alarm Time request, it will be an IR mesage
                        {
                          Serial.println("Ir signal Recieved");
                        Serial.printf("[%u] get Text: %s\n", num, payload);
                        int code =  atoi((const char*)payload);
                        printf("%" PRIu16 "\n",code);
                        printf("%" PRIu32 "\n",RemoteKey[code]*255);
                        irsend.sendNEC(RemoteKey[code]*255);
                        } 
            break;
            }
    }
return;
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting");
  wifiSetup();
  irsend.begin();
  irrecv.enableIRIn();
}


void loop() 
{
wifiMaintain();
int globalHours = timeClient.getHours();
int globalMinutes = timeClient.getMinutes();
int globalTime = (globalHours*100)+globalMinutes;
//if the set alarm time equals the global alarm time, turn the controller on and off
if(globalTime == anotherAlarmTime && ledAlarm == false)
    {
    irsend.sendNEC(RemoteKey[3]*255);
    ledAlarm =true;
    } else 
    if(globalTime != anotherAlarmTime && ledAlarm == true )
        {
        ledAlarm = false;
        } 


//If you recieve results then print them, used for debugging
if (irrecv.decode(&results)) 
  {
  Serial.println('\n');
  serialPrintUint64(results.value, HEX);
  Serial.println('\n');
  irrecv.resume();  // Receive the next value
  }
}

//Starting Wifi Protocols
void wifiSetup()
{
     WiFi.begin(ssid, password);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED) 
    { // Wait for the Wi-Fi to connect
        delay(1000);
        Serial.print(++i); Serial.print(' ');
    } 
    Serial.println(WiFi.localIP());
    timeClient.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("WebSocket server started.");
    server.on("/",[](){server.send(200, "text/html",webpage);});
    server.onNotFound(ifNotFound);
    server.begin();
    MDNS.begin(mdnsName);
    Serial.print("mDNS responder started: http://");
    Serial.print(mdnsName);
    Serial.println(".local");
     ArduinoOTA.onStart([]() 
              { 
    Serial.println("Arduino OTA started");
              });
    ArduinoOTA.begin();
    
}

//Functions needed to maintain the wifi conections
void wifiMaintain()
{
    timeClient.update();
    MDNS.update();
    server.handleClient();
    ArduinoOTA.handle();
    webSocket.loop();
}

//Handling wifi not found
void ifNotFound(){
  server.send(404,"text/plain", "404: Not Found");
  }
