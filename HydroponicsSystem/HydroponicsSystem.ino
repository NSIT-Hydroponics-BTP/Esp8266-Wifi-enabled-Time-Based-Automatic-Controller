#include<cstring>
#include<string.h>
#include <LittleFS.h>
#include <FS.h>
#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>

#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

#include <ESPAsyncWebServer.h>
LiquidCrystal_PCF8574 lcd(0x27);


byte hotspott[2][8] = {{    0b01010,    0b10100,   0b11000,   0b10000,    0b10000,    0b10111,    0b11111,    0b01111},
    {    0b10101,    0b11010,    0b10100,    0b11000,    0b10000,    0b10111,   0b11111,    0b01111}
};


byte wifii[5][8] = {{    0b00000,    0b00000,    0b00000,    0b00000,    0b00000,    0b00000,    0b00000,    0b00001},
    {    0b00000,    0b00000,  0b00000,    0b00000,    0b00000,    0b00000,    0b00001,    0b00001},
    {    0b00000,   0b00000,    0b00000,    0b00000,    0b00000,    0b00001,    0b00001,    0b00001},
    {    0b00000,    0b00000,    0b00000,    0b00001,    0b00010,    0b00101,    0b00001,    0b00001},
    {    0b00000,    0b00001,    0b00010,    0b00101,    0b01010,    0b10101,    0b00001,    0b00001}
};



//#include <WiFi.h>   using this causes error beacuse ESPAsyncWEBSERVER uses different WIFI.h


//GPIO 12 to LIGHT D6 b
//GPIO 13 to Motor D7
//
//char* ssid = "FTTH-72D1";
//char* password =  "Pajid1123";

// timing should be strictly between start of morning 00::00(startof morning..night) 23:59 (end of Night..)
// startTime Should be ALways past 00::00 and end Time Should always before 23:59 on the same day ..
bool mRestart = false;



const char* defssid = "HPsystem";
const char* defpassword = "HPsystem";

String currentSsid = String("HPsystem");
String currentPass = String("HPsystem");



char* ssid;
char* password;




//const char* PARAM_INPUT_1 = "output";
//const char* PARAM_INPUT_2 = "state";


bool noTime = false;
String startH = String("01");
String startM = String("20");
String endH = String("01");
String endM = String("20");
int lightActive = 0;
int motorActive = 0;
int tempMotorActive = 0;
int tempLightActive = 0;

String lightA = "0";
String motorA = "0";

AsyncWebServer server(80);


//later on store this on progemm..
const char  webPage1[] PROGMEM = "<!DOCTYPE html><html version=\"5.0\"><head><style>body{font-family: 'Lucida Sans', 'Lucida Sans Regular', 'Lucida Grande', 'Lucida Sans Unicode', Geneva, Verdana, sans-serif;}.centered-box{ margin: 0; position: absolute; top: 50%; left: 50%; min-width: 300px; transform: translate(-50%,-50%) ;padding: 30px; background-color:rgba(240,240,240, 1); }span{ display: block; text-align: center;}input[type=text], select { width: 100%; padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box;}input[type=password], select { width: 100%; padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box;}input[type=submit] { width: 100%; background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 4px; cursor: pointer;}input[type=submit]:hover { background-color: #45a049;}</style></head> <body> <div class=\"centered-box\"> <hr> <h3 style=\"text-align: center;\"> Connectivity Configuration </h3> <hr> <form action=\"/credentials\" method=\"POST\"> <label >Wifi SSID:</label> <input type=\"text\" id=\"SSID\" name=\"SSID\" minlength=\"4\" required> <hr> <label >Password:</label> <input type=\"password\" id=\"setPassword\" name=\"setPassword\" minlength=\"8\" required><input type=\"checkbox\" onclick=\"myFunction()\">Show Password<br><br><br> <input type=\"radio\" id=\"CTW\" name=\"CTW\" value=\"station\" checked> <label for=\"CTW\">Connect to WIFI</label> <br> <br> <input type=\"radio\" id=\"CTW\" name=\"CTW\" value=\"AP\" > <label for=\"CW\">Create WIFI</label><br> <br> <input type=\"submit\" value=\"Submit\"></form></div><script> function myFunction() { var x = document.getElementById(\"setPassword\"); if (x.type === \"password\") { x.type = \"text\"; } else { x.type = \"password\"; }}</script></body></html>" ;



const char webPage2[] PROGMEM = "<!DOCTYPE html><html version=\"5.0\"><head><style>body{font-family: 'Lucida Sans', 'Lucida Sans Regular', 'Lucida Grande', 'Lucida Sans Unicode', Geneva, Verdana, sans-serif;}.centered-box{ margin: 0; position: absolute; top: 50%; left: 50%; transform: translate(-50%,-50%) ;padding: 30px; background-color:rgba(240,240,240, 1); }span{ display: block; text-align: center;}</style></head> <body> <hr> <h3 style=\"text-align: center;\"> SuccessFully Saved Configuration </h3> <hr><h4> Please connect to wifi Network with Name %TEMP0% and follow Ip shown in LCD Display </h4>SSID: %TEMP1% <br>PassWORD : %TEMP2%<br> %TEMP3% <br><div class=\"centered-box\"> <h2 > Restarting in </h2> <h1 style=\"text-align: center;\" id=\"timer\"> 10 </h1></div> <script>var timer=document.getElementById(\"timer\");var seconds = 10;setInterval(function() {timer.innerHTML = seconds--;if(seconds<=0){ location.reload(); seconds=0;}}, 1000);</script></body></html>";

const char mainPage[] PROGMEM =   R"rawliteral(
<!DOCTYPE HTML>
<html>

    <head>

        <title>
            HP Control
        </title>

        <style>

            html {
                font-family: Arial;
                display: inline-block;
                text-align: center;
            }
            h2 {
                font-size: 3.0rem;
            }
            p {
                font-size: 3.0rem;
            }
            .switch {
                position: relative;
                display: inline-block;
                width: 120px;
                height: 68px;
            }
            .switch input {
                display: none;
            }
            .slider {
                position: absolute;
                top: 0;
                left: 0;
                right: 0;
                bottom: 0;
                background-color: #ccc;
                border-radius: 6px;
            }
            .slider:before {
                position: absolute;
                content: "";
                height: 52px;
                width: 52px;
                left: 8px;
                bottom: 8px;
                background-color: #fff;
                -webkit-transition: 0.4s;
                transition: 0.4s;
                border-radius: 3px;
            }
            input:checked+.slider {
                background-color: #15b300;
            }
            input:checked+.slider:before {
                -webkit-transform: translateX(52px);
                -ms-transform: translateX(52px);
                transform: translateX(52px);
            }

            .button2 {
                background-color: white;
                float: right;
                padding: 10px;
                color: black;
                border: 2px solid #25ba00;
            }

            .button2:hover {
                background-color: #00ba28;
                color: white;
            }

            .block::after {
                content: "";
                clear: both;
                display: table;
            }
            .GPIOs {
                background-color: rgb(212, 212, 212);
                display: inline-block;
                padding: 80px;
            }

            .Auto {
                padding: 0;
                margin: 0;
                height: 425px;
                min-width: 300px;

                display: inline-block;
                text-align: center;
                background-color: rgb(212, 212, 212);

            }

            input[type=submit] {
                width: 50%;
                background-color: #4CAF50;
                color: white;
                padding: 14px 20px;
                margin: 8px 0;
                border: none;
                border-radius: 4px;
                cursor: pointer;
            }

            input[type=submit]:hover {
                background-color: #45a049;
            }

            nav{
                padding: 40px;

            }

             .button3 {
                background-color: white;
                float: right;
                padding: 10px;
                color: black;
                border: 2px solid #e90606;
            }

            .button3:hover {
                background-color: #c01a04;
                color: white;
            }
        </style>
    </head>

    <body>

        <div class="block">
            <a class="button3" style="margin-left:10px ;"href="/reset">Reset</a>
            <a class="button2" href="/configureConnectivity">Configure Connectivity</a>
           
        </div>
        <h2>Hydroponics System
        </h2>
        <hr>
        <div style="text-align:justify;">
            <div class="GPIOs">

                %PLACEHOLDER%

            </div>
            <div class="Auto" style="float: right;">
                <form action="/setAuto" method="GET">

                    <nav>
                        <label for="startTime">Select Start time:</label>
                        <input type="time" id="startTime" name="startTime" value= %TEMPtime1%  required >
                    </nav>

                    <br>

                    <nav>
                        <label for="endTime">Select End time:</label>
                        <input type="time" id="endTime" name="endTime" value= %TEMPtime2%  required>

                    </nav>
                    <input type="submit" value="SetAuto">
                </form>

            </div>

        </div>

        <script>
            function toggleCheckbox(element) {
                var xhr = new XMLHttpRequest();
                if (element.checked) {
                    xhr.open("GET", "/update?output=" + element.id + "&state=1", true);
                } else {
                    xhr.open("GET", "/update?output=" + element.id + "&state=0", true);
                }
                xhr.send();
            }
        </script>
    </body>
</html>
)rawliteral"; 


String content = "";
String storedContent[20];

String processor(const String& var)
{

    Serial.println("called...");
    if (var == "TEMP1")
        return currentSsid;
    if (var == "TEMP2")
        return currentPass;

    if(var=="TEMP3")
      return "";

    return String();
}

String processor2(const String& var)
{

    if (var == "PLACEHOLDER")
    {
        String buttons = "";                                                                                                           // id is being sent
        buttons += "<h4>Output AT - GPIO 12 - Light</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"12\" " + outputState(12) + "><span class=\"slider\"></span></label>";
        buttons += "<h4>Output AT - GPIO 13 - Motor</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"13\" " + outputState(13) + "><span class=\"slider\"></span></label>";
        return buttons;
    }

    if(var == "TEMPtime1"){
      if(noTime){
        return String();
        }else{
            
        String t=String("\"")+startH+String(":")+ startM+ String("\"");
        return t;}
      }

    if(var == "TEMPtime2") {
        if(noTime){
          return String();
          }else {
           String t= String("\"") + endH + String(":")+ endM+  String("\"");
        return t;
          }
      } 
    return String();
// avoiding empty string return;..

}

String outputState(int output)
{


if(output==12){
  
  if(lightActive)
      return "checked";
    else
    return"";
 }else if(output==13){
  
  if(motorActive)
      return "checked";
     else
     return"";
    
  }

  
//    if (digitalRead(output))
//    {
//        return "checked";
//    }
//    else
//    {
//        return "";
//    }




    
}


void splitContent(char* str, char* delim)
{


    for (int i = 0; i < 20; i++)
    {
        storedContent[i] = String("");
    }

    char *ptr = strtok(str, delim);


    for (int i = 0; ptr != NULL; i++)
    {
        //String to(ptr);
        storedContent[i] = String(ptr);
        ptr = strtok(NULL, delim);
    }


}






void eraseFile(String namee)
{
    LittleFS.remove(namee);
}

String pathToCredentials = "/credentials.txt";
String pathToStatus = "/status.txt";


void clearAllData(){
  
  LittleFS.remove(pathToCredentials);
  LittleFS.remove(pathToStatus);
  
  
  }



void writeConnectionData(String mmode, String sssid, String password)
{
    String ct = mmode + String("\n") + sssid + String("\n") + password;
    WriteToFileWithErase(pathToCredentials, ct);

}


// Writes content to File named namee .. Seprated content Should be delemeted by delimeter "\n";
void WriteToFileWithErase(String namee, String content)
{
    eraseFile(namee);
    File f = LittleFS.open(namee, "w+");
    if (!f)
    {
        Serial.println("can't write to File");
        return;
    }
    f.write(content.c_str(), content.length());
    f.close();
}

// this opens file with name and assign the content Content;
void setContentFromFileToGlobal(String namee)
{
    File f = LittleFS.open(namee, "r");
    if (!f)
    {
        Serial.println("can't open  File to read");
        return;
    }

    content = "";
    while (f.available())
    {
        content = content + String(f.readString());
    }
    f.close();
}


//assign the content  with delemiter newLine to Global storedContent
void assignContent(String namee)
{
     for (int i = 0; i < 20; i++)
    {
        storedContent[i] = String("");
    }
     content="";  
    setContentFromFileToGlobal(namee);
    splitContentStringVersion(content,"\n");
}

void splitContentStringVersion(String content,char *delim){
  
  int temp1 = content.length();
    char tempContent[temp1 + 1];
    strcpy(tempContent, content.c_str());
    //                tempContent=(char *)tempContent;
    splitContent(tempContent, delim);
  }







void wifiNotFoundFunction(){
  Serial.println("Default Connection Parameters..");
   IPAddress IP;
            WiFi.softAP(defssid, defpassword);
            IP = WiFi.softAPIP();
            Serial.print("AP IP address: ");
            Serial.println(IP);
            Serial.print("Wifi: ");
            Serial.println(defssid);
            Serial.print("Pass: ");
            Serial.println(defpassword);
            lcd.setCursor(0, 0);
        lcd.print(defssid);
         lcd.setCursor(0, 1);
        lcd.print(IP);
  }


void writeStatusToFile(){
    String tempc=String(lightActive)+String("\n")+String(motorActive)+String("\n");
    
    tempc=tempc+ String(startH)+String("\n")+String(startM)+String("\n")+ String(endH)+String("\n")+String(endM);

     WriteToFileWithErase(pathToStatus,tempc); 
  
  }



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool STOPFORNOW=false;
bool RTCWORKING=false;

bool RouterMode=false;







void setup()
{

  
    Serial.begin(115200);
     while (!Serial)
     
        ;
    RTC_DS1307 rtc;
    Wire.begin();
    Wire.beginTransmission(0x27);
    int error = Wire.endTransmission();
    
    if (error == 0)
    {
        Serial.println(": LCD found.");
        lcd.begin(16, 2); // initialize the lcd

    }
    else
    {
        Serial.println(": LCD not found.");
    }

    lcd.setBacklight(255);
    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 0);
    

    if (! rtc.begin()) {  
    Serial.println("Couldn't find RTC.. Better to Contact Developer Company..");
    
    
   // use this
   //  if((!STOPFORNOW)&&(!RTCNOTWORKING))
   //Also Use timeAlarm Library...
   
//    Serial.println(hour());
//   Serial.println(minute());
//   Serial.println(second());
    
  }else {
    RTCWORKING=true;
    DateTime now = rtc.now();
    setTime(now.hour(),now.minute(),now.second(),now.day(),now.month(),now.year());
    
    }
    



    
    pinMode(12,OUTPUT);
    pinMode(13,OUTPUT);
    if (!LittleFS.begin())
    {
        Serial.printf("Unable to begin(), aborting\n.. contact adminsitrator");
        //return;
    }
    IPAddress IP;

    // one END of String character

    assignContent(pathToCredentials);
    if (storedContent[0].length() == 0)
    {
        // password needs to be 8 character Long
        WiFi.softAP(defssid, defpassword);
        IP = WiFi.softAPIP();
        RouterMode=true;
        Serial.print("AP IP address: ");
        Serial.println(IP);
       
        Serial.print("Wifi: ");
        Serial.println(defssid);
        lcd.setCursor(0, 0);
        lcd.print(defssid);
         lcd.setCursor(0, 1);
        lcd.print(IP);
        Serial.print("Pass: ");
        Serial.println(defpassword);
        

    }
    else
    {
        if (strcmp(storedContent[0].c_str(), "0") == 0)
        {

            Serial.println("working **");
            WiFi.softAP(defssid, defpassword);
            IP = WiFi.softAPIP();
            RouterMode=true;
            Serial.print("AP IP address: ");
            Serial.println(IP);
            lcd.setCursor(0, 0);
        lcd.print(defssid);
         lcd.setCursor(0, 1);
        lcd.print(IP);
            Serial.print("Wifi: ");
            Serial.println(defssid);
            Serial.print("Pass: ");
            Serial.println(defpassword);

        }
        else if (strcmp(storedContent[0].c_str(), "1") == 0)
        {
            char temp1[storedContent[1].length() + 1];
            strcpy (temp1, storedContent[1].c_str());
            char temp2[storedContent[2].length() + 1];
            strcpy (temp2, storedContent[2].c_str());

            WiFi.begin(temp1, temp2);
            long timerTemp=millis();
            bool found=true;
            while (WiFi.status() != WL_CONNECTED)
            {
              
                delay(1000);
                Serial.println("..");
                // start as stationMode if wifiNot Found.. 10sec wait
                if((millis()-timerTemp)>10000){
                  Serial.println("WIfi Not Found starting as SationMode..");
                  WiFi.disconnect();
                  wifiNotFoundFunction();
                  RouterMode=true;
                  found=false;
                  break;
                  
                  }
                
            }

            if(found){            
            Serial.println("working 2");
            IP = WiFi.localIP();
            Serial.println(IP);
            RouterMode=false;
            currentSsid = storedContent[1];
            currentPass = storedContent[2];
            
            // does not Work Don't know why
            ssid = temp1;
            password = temp2;
            
            Serial.print("Wifi: ");
            Serial.println(temp1);
            Serial.print("Pass: ");
            Serial.println(temp2);
            lcd.setCursor(0, 0);
        lcd.print(ssid);
         lcd.setCursor(0, 1);
        lcd.print(IP);
            }
            // if WifiCOnnected..move ahed..else do something..

        }
        else if (strcmp(storedContent[0].c_str(), "2") == 0)
        {
            char temp1[storedContent[1].length() + 1];
            strcpy (temp1, storedContent[1].c_str());
            char temp2[storedContent[2].length() + 1];
            strcpy (temp2, storedContent[2].c_str());

            WiFi.softAP(temp1, temp2);
            IP = WiFi.softAPIP();
            RouterMode=true;
            Serial.print("AP IP address: ");
            Serial.println(IP);
            currentSsid = storedContent[1];
            currentPass = storedContent[2];
            // does not Work Don't know why
            ssid = temp1;
            password = temp2;
                
            Serial.print("Wifi: ");
            Serial.println(temp1);
            Serial.print("Pass: ");
            Serial.println(temp2);
             lcd.setCursor(0, 0);
        lcd.print(temp1);
         lcd.setCursor(0, 1);
        lcd.print(IP);
            // wifirouterMode

        }
        else if (strcmp(storedContent[0].c_str(), "3") == 0)
        {
            // no IOTmode

        }
    }
    

    //check for status and assign it..
    
    assignContent(pathToStatus);
    

    if (storedContent[0].length() == 0)
    {
        //First time opened...
        Serial.println("Running HEre 1 \n content= ");
        Serial.println(content);

        // set both pins to low..
          Serial.println("Setting Pin 12 to low");
          digitalWrite(12,LOW);
          
          Serial.println("Setting Pin 13 to low");
          digitalWrite(13,LOW);
          noTime=true;
          startH="";
          startM="";
          endH="";
          endM="";
          
    }
    else
    {
      //light active pin..
       Serial.println(content);
      lightActive=storedContent[0].toInt();
      motorActive=storedContent[1].toInt();
      tempLightActive=lightActive;
      tempMotorActive=motorActive;
      noTime=false;
      startH=storedContent[2];
      startM=storedContent[3];
      endH=storedContent[4];
      endM=storedContent[5];

  
  }

    Serial.println("working 1");












    server.on("/configureConnectivity", HTTP_GET, [](AsyncWebServerRequest * request)
    {
        request->send(200, "text/html", webPage1 );

    });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
    {
        request->send_P(200, "text/html", mainPage ,processor2 );

    });


    server.on("/setAuto", HTTP_GET, [](AsyncWebServerRequest * request)
    {
       String inputMessage1;
       String inputMessage2;
       bool done=false;
         if (request->hasArg("startTime") && request->hasArg("endTime"))
          { 
            inputMessage1 = request->arg("startTime");
            inputMessage2 = request->arg("endTime");
            String temp1,temp2,temp3,temp4;
            Serial.println(inputMessage1);
            Serial.println(inputMessage2);
            done=true;
            splitContentStringVersion(inputMessage1,":");
            temp1=storedContent[0];
            temp2=storedContent[1];

            splitContentStringVersion(inputMessage2,":");
            temp3=storedContent[0];
            temp4=storedContent[1];

                if(temp1.toInt()>temp3.toInt()){
                  done=false;
                  
                  
                  }else if((temp1.toInt()==temp3.toInt())&&(temp2.toInt()>temp4.toInt()))
                  {
                    done=false;
                    
                  
                    }else{
                      done=true;
                      startH=temp1;
                      startM=temp2;
                      endH=temp3;
                      endM=temp4;

                      motorActive=tempMotorActive;
                      lightActive =tempLightActive;

                      noTime=false;
                       writeStatusToFile();
                      }
            

                
          }

        if(done){
          request->send(200, "text/html", "<h1 style=\"text-align:center;\">AutoSet Complete...Restarting!!</h1>");
       // though restarting is not required..
           mRestart=true;
          
          }else{
              Serial.println("Timing is worong..");
             request->send(200, "text/html", "<h1 style=\"text-align:center;\">Error Occured maybe Time misMAtch</h1>");
              
            }



    });

    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest * request)
    {
        eraseFile(pathToCredentials);
        eraseFile(pathToStatus);
        Serial.println("Reseting complete");
        Serial.println("restarting..");
        mRestart = true;
        request->send(200,"text/plain","Reseting.. Post which System will restart..");
        
    });


    server.on("/update", HTTP_GET, [] (AsyncWebServerRequest * request)
    {
        String inputMessage1;
        String inputMessage2;
        
        // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
        if (request->hasArg("output") && request->hasArg("state"))
        {
            inputMessage1 = request->arg("output");
            inputMessage2 = request->arg("state");
            Serial.println(inputMessage1+ String(":")+inputMessage2);
             if(inputMessage1.toInt()==12){
              tempLightActive=inputMessage2.toInt();
              lightA=inputMessage2; //will be used when using PWM... no use right now
              }
            if(inputMessage1.toInt()==13){
              tempMotorActive=inputMessage2.toInt();
              motorA=inputMessage2;// will be used when using PWM... no use right now
              }
        }
        request->send(200, "text/plain", "OK");
    });


    server.on("/credentials", HTTP_POST, [](AsyncWebServerRequest * request)
    {
        String modee = String(0);

        String mo = request->arg("CTW");

        // problem here see this..
        if (strcmp(mo.c_str(), "station") == 0)
        {
            modee = String(1);
        }
        if (strcmp(mo.c_str(), "AP") == 0)
        {
            modee = String(2);
        }

        String sssid = request->arg("SSID");
        String ppassword = request->arg("setPassword");

        if ((mo.length() == 0) || (sssid.length() < 8) || (ppassword.length() < 8))
        {
            request->redirect("/");
        }
        else
        {
            currentSsid = sssid;
            currentPass = ppassword;
            Serial.print("SSID= ");
            Serial.println(sssid);

            Serial.print("password= ");
            Serial.println(ppassword);

            Serial.print("Mode= ");
            Serial.println(modee);
            writeConnectionData(modee, sssid, ppassword);

            request->send_P(200, "text/html", webPage2, processor);
            mRestart = true;
        }
    });


    server.begin();
    Serial.setTimeout(100);


    Serial.println("to erase All Data..'<' ");
     lcd.setCursor(13, 1);
  lcd.print(minute());
}



void wifiAnimation(int x, int y)
{
    static int i = 0;
    
        lcd.createChar(0, wifii[i]);
        lcd.setCursor(x, y);
        lcd.write(0);
        if(i==4)
          i=-1;
        i++;
}

void hotspotAnimation(int x, int y)
{
    static int i = 0;
    
        lcd.createChar(1, hotspott[i]);
        lcd.setCursor(x, y);
        lcd.write(1);
        if(i==1)
          i=-1;
        i++;
}





bool ALLSTARTED=false;
bool stateRunning=false;

long animationM=millis();
long testmillis=millis();




void startSystem(){
  
  if(!ALLSTARTED){
              Serial.println("Motor Light COndition Active..");
              Serial.print("Setting light To " );
               Serial.println(lightActive);
               Serial.print("Setting Motor To " );
               Serial.println(motorActive);
               
               String st=String(" ")+String(endH)+String(endM);
               lcd.setCursor(11, 0);
               lcd.print(st);
              ALLSTARTED=true;
              
              digitalWrite(12,lightActive);
              digitalWrite(13,motorActive);
              

              
              } 
              
  
  }

void closeSystem(){
  
static bool runOnce=false;
if(!runOnce){
    String st=String(" ")+String(startH)+String(startM);
               lcd.setCursor(11, 0);
               lcd.print(st);
               runOnce=true;
  
  }
  
   if(ALLSTARTED){
              Serial.println("STOPPING everthing..");
              ALLSTARTED=false;
               String st=String(" ")+String(startH)+String(startM);
               lcd.setCursor(11, 0);
               lcd.print(st);

                digitalWrite(12,0);
              digitalWrite(13,0);
              }
              
  
  }

  

void loop()
{
    if (mRestart)
    {

        delay(2000);
        ESP.restart();

    }

    // there will be less posibility to change of minute or hour inside the if condition id seconds is in between

if(RTCWORKING ){
    
if(second()>2 && second()<58){

   
    if((hour()>=startH.toInt())&&(hour()<endH.toInt()) && (minute()>startM.toInt()))
      {
      
          
          startSystem();
              
       
      
    }else if((hour()>startH.toInt())&&(hour()<=endH.toInt())&&(minute()<endM.toInt()) ){
      
          

              startSystem();
      
      }
    
    
    else if(         ((hour()==startH.toInt())&&(hour()==endH.toInt()))&&((minute()>=startM.toInt())&&(minute()<endM.toInt()))   ){


        
             startSystem();

      
      }
      else{

        
       closeSystem();
        
        
        
        }

}
else {

  
  }





    
}else{
         lcd.setCursor(0, 1);
        lcd.print("*Clock Not Set*");

   
  // RTC not working..
  }


    if (Serial.available() > 0)
    {
      Serial.println("Clearing..");
        //    // read the incoming byte:
        String st = Serial.readString();
        int n = st.length();
        char char_array[n];
        strcpy(char_array, st.c_str());
            if (char_array[0] == '<')
            {

               clearAllData();
               
               Serial.println("Cleared ALL Data..");
               mRestart=true;
            }

       
    }


if((millis()-animationM)>300){

  

  if(RouterMode){
  hotspotAnimation(15,1);
  
  }else{
    
    wifiAnimation(15,1);
    }
  animationM=millis();
}

if(second()>=58){
       lcd.setCursor(13, 1);
  lcd.print(minute());
  
  }

if(millis()-testmillis>1000){
  
  Serial.print(hour());
  Serial.println(":");
  Serial.println(minute());
  testmillis=millis();
  
  }  

    


}
