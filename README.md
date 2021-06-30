# Esp8266-Wifi-enabled-Time-Based-Automatic-Controller
This controller Allows the setting HP system time and once set can be left free and will show the content on display


initially To setup As intended You need a TFT display 16*2 and a RTC module DS1307(time should be already set)
[Note : RTC is needed just at the start of ESP8266 , post which ESP8266 sync its internal clock to that of RTC clock] you may remove RTC post connection with wifi has been estblished
[Note : If Time from RTC can't be during the ESP8266 start read/( faluty connection may or RTC damaged) TFT display will show "** RTC not WORKING ** " error]

Step 1: connect modules through I2C 
Step 2: You may configure (Default )wifi settings in the Arduino code or you may upload the code and change the final connectivity configuration through the webpage 
Step 3: follow the TFT display 
            1st line on the left side shows the WIFI name [Note : if configured wifi not present, Esp8266 will run in accesspoint mode with default connection parameters]
            1st line on the right side shows the configured time {if not in operating phase: it will show start time else if in operating phase it will show end Time}
            2nd line to the left side shows the IP you need to access through your browser to configure the system
            2nd line to the right will show animation [2 type of animation are possible : 1 for when esp8266 operating in station mode and other for when it is operating in accesspoint mode ]
            2nd line left from the animation: will show time [only the ongoing minute hand]

