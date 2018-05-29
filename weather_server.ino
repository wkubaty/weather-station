
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
  #include <time.h>
  #define ONE_WIRE_BUS 2 
  #include <Wire.h>
  #include "my_htu21d.h"
  unsigned long global_millis;
  time_t time_5s[100];
  time_t time_1m[100];
  time_t time_1h[100];
  double temp1_5s[100];
  double temp1_1m[100];
  double temp1_1h[100];
  
  double temp2_5s[100];
  double temp2_1m[100];
  double temp2_1h[100];
  
  double hum_5s[100];
  double hum_1m[100];
  double hum_1h[100];
  double average_temp1_1m=0;
  double average_temp2_1m=0;
  double average_hum_1m=0;
  
  double average_temp1_1h=0;
  double average_temp2_1h=0;
  double average_hum_1h=0;
  
  int counter_1m=0;
  int counter_1h=0;
  
  OneWire oneWire(ONE_WIRE_BUS);
  DallasTemperature sensors(&oneWire);
  const char* ssid="***";
  const char* password="***";
  ESP8266WebServer server(80);
  //HTU21D myHumidity;
  String webSite;
  String javaScript;
  String XML;
  double temperature1;
  double temperature2;
  double humidity;
  int nOfP_5s;
  int nOfP_1m;
  int nOfP_1h;
  my_htu21d myhtu;
  void buildWebsite(){

    buildJavascript();

    webSite="<!DOCTYPE HTML>\n";
    webSite+="<html>";
    webSite+="<head>";
    webSite+="  <meta charset='UTF-8'>";
    webSite+=javaScript;

    webSite+="</head>";
    webSite+="<BODY onload='process()'>\n";
    webSite+="<center>PROJEKT ZSC TEMPERATURA I WILGOTNOŚĆ</center><BR>\n";
    webSite+="<center><div id='readings'></div></center>\n";
    webSite+="<center><div id='steps1'>\n";
    webSite+="Wybierz częstotliwość:  \n";
    webSite+="<input type='radio' name='rBC1' value='5s' checked>5s \n";
    webSite+="<input type='radio' name='rBC1' value='1m' >1m \n";
    webSite+="<input type='radio' name='rBC1' value='1h' >1h<br><br>\n";
    webSite+="</div></center>\n";
    webSite+="<center><canvas id='c1' align='center' width='800' height='400'></canvas></center>\n";
    webSite+="<center><canvas id='c2' align='center' width='800' height='400'></canvas></center>\n";
    //webSite+=javaScript;
    webSite+="</BODY>\n";
    webSite+="</HTML>\n";
  }
  
  
  void buildJavascript(){

    javaScript="<SCRIPT>\n";
    javaScript+="var xmlHttp=createXmlHttpObject();\n";
  
    javaScript+="function createXmlHttpObject(){\n";
    javaScript+=" if(window.XMLHttpRequest){\n";
    javaScript+="    return new XMLHttpRequest();\n";
    javaScript+=" }else{\n";
    javaScript+="    return new ActiveXObject('Microsoft.XMLHTTP');\n";
    javaScript+=" }\n";
    javaScript+="}\n";
  
    javaScript+="function process(){\n";
    javaScript+=" if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
    javaScript+="   xmlHttp.open('PUT','xml',true);\n";
    javaScript+="   xmlHttp.onreadystatechange=handleServerResponse;\n";
    javaScript+="   xmlHttp.send(null);\n";
    javaScript+=" }\n";
    javaScript+=" setTimeout('process()',500);\n";
    javaScript+="}\n";
    
    javaScript+="function handleServerResponse(){\n";
    javaScript+="if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
    javaScript+="xmlResponse=xmlHttp.responseXML;\n";
    javaScript+="xmldoc=xmlResponse;\n";
    javaScript+="message=xmldoc.getElementsByTagName('readings')[0].firstChild.nodeValue;\n";
    javaScript+="document.getElementById('readings').innerHTML=message; \n";
    javaScript+="var rB=document.getElementsByName('rBC1');\n";
    
    javaScript+="var stepTimeString=rB[0].value; \n";
    javaScript+="if(rB[1].checked){stepTimeString=rB[1].value;} \n";
    javaScript+="if(rB[2].checked){stepTimeString=rB[2].value;} \n";

    javaScript+="var nOfP=xmldoc.getElementsByTagName('nOfP_'+stepTimeString)[0].firstChild.nodeValue;\n";
    javaScript+="var t1List=xmldoc.getElementsByTagName('temp1_'+stepTimeString)[0].firstChild.nodeValue.split(',').slice(0,nOfP);\n";
    javaScript+="var t2List=xmldoc.getElementsByTagName('temp2_'+stepTimeString)[0].firstChild.nodeValue.split(',').slice(0,nOfP);\n";
    javaScript+="var hList=xmldoc.getElementsByTagName('hum_'+ stepTimeString)[0].firstChild.nodeValue.split(',').slice(0,nOfP);\n";

    javaScript+="var c1=document.getElementById('c1');\n";
    javaScript+="var c2=document.getElementById('c2');\n";  
    javaScript+="plot(c1,t1List,nOfP,'red','TEMPERATURA (DS18B20)',true);\n";
    javaScript+="plot(c2,t2List,nOfP,'red','',true);\n";
    javaScript+="plot(c2,hList,nOfP,'blue','TEMPERATURA I WILGOTNOŚĆ (HTU21)',false);\n"; 
    javaScript+=" }\n";
    javaScript+="}\n";
    javaScript+="function plot(c,values,nOfP,colour,title,clear){\n";
    
          javaScript+="var ctx=c.getContext('2d');\n";
          javaScript+="if(clear==true){ctx.clearRect(0,0,c.width,c.height)};\n";
          javaScript+="c.style.display='block';\n";
          javaScript+="c.style.position='100';\n";
  
          javaScript+="var stPX=0;\n";
          javaScript+="var stPY=0;\n";
          javaScript+="var fSize=50;\n";
          javaScript+="var sX=c.width-2*fSize;\n";
          javaScript+="var sY=c.height-2*fSize;\n";
          
          javaScript+="ctx.beginPath();\n";
          javaScript+="ctx.strokeStyle='rgb(220,220,220)';\n";
          javaScript+="ctx.lineWidth=20;\n";
          javaScript+="ctx.rect(stPX,stPY,c.width,c.height);\n";
          javaScript+="ctx.stroke();\n";
          javaScript+="ctx.closePath();\n";
          
          javaScript+="ctx.beginPath();\n";
          javaScript+="ctx.lineWidth=2;\n";
          //ctx.rect(stPX+fSize,stPY+fSize,c.width-2*fSize,c.height-2*fSize)
        
          javaScript+="var x=[];\n";
          
          javaScript+="for(var i=0; i<nOfP; i++){\n";
          javaScript+=" x.push(i);\n}\n";
          javaScript+="var y=values;\n";
          javaScript+="var maxY=Math.max.apply(Math,y);\n";
          javaScript+="var minY=Math.min.apply(Math,y);\n";
          
          javaScript+="var stepX=sX/Math.max(0.01,nOfP-1);\n";
          javaScript+="var stepY=sY/Math.max(0.01,maxY-minY);\n";
          
          javaScript+="var fontS=14;\n";
          javaScript+="ctx.font=fontS+'px Arial';\n";
          javaScript+="var yOffset=sY/4;\n";
                  
          //draw lines and values on Y axis
          javaScript+="var stepYAxis=(maxY-minY)/4;\n";
          javaScript+="ctx.fillStyle='black';\n";
          javaScript+="ctx.strokeStyle='rgb(120,120,120)';\n";
          
          javaScript+="for(var i=0;i<5;i++){\n";
          javaScript+=" if(clear){ctx.fillText((maxY-i*stepYAxis).toFixed(2),fSize/4,fSize+fontS/2+i*yOffset);\n";
          javaScript+=" ctx.moveTo(stPX+fSize,stPY+fSize+i*sY/4);\n";
          javaScript+=" ctx.lineTo(stPX+fSize+sX,stPY+fSize+i*sY/4);}\n";
          javaScript+=" else{ctx.fillText((maxY-i*stepYAxis).toFixed(2),sX+fSize+3,fSize+fontS/2+i*yOffset);}\n";
          javaScript+="}\n";
          
          //print title
          javaScript+="ctx.font=fontS+' 18 px Arial';\n";
          javaScript+="if(clear){ctx.fillText(title,0.4*sX,fSize/2+9);}\n";
          javaScript+="else{ctx.fillText(title,sX/3,fSize/2+fontS/2);}\n";
          javaScript+="ctx.font='12px Arial';\n";
          javaScript+="ctx.fillText('°C',fSize/2,fSize/2+6);\n";
          javaScript+="if(clear==false){ctx.fillText('%',sX+fSize+10,fSize/2+fontS/2);}\n";
          javaScript+="ctx.stroke();\n";
          javaScript+="ctx.closePath();\n";
         
          javaScript+="ctx.beginPath();\n";
          javaScript+="ctx.strokeStyle=colour;\n";
          //plot line chart
          javaScript+="for(var i=nOfP-2;i>=0;i--){\n";
          javaScript+=" ctx.moveTo(fSize+stPX+stepX*(nOfP-2-i),Math.floor(stPY+fSize+sY-(y[i+1]-minY)*stepY));\n";
          javaScript+=" ctx.lineTo(fSize+stPX+stepX*((nOfP-2-i)+1),Math.floor(stPY+fSize+sY-(y[i]-minY)*stepY));\n";
          javaScript+="}\n";

          //plot points on chart";
          
          javaScript+="for(var i=nOfP-1;i>=0;i--){\n";
         
          javaScript+="var xx=Math.floor(fSize+stPX+stepX*(nOfP-1-i));\n";
          javaScript+="var yy=Math.floor(stPY+fSize+sY-(y[i]-minY)*stepY);\n";
          javaScript+="ctx.moveTo(xx,yy);\n";
          javaScript+="ctx.arc(xx,yy,2,0,2* Math.PI,true);\n";
          javaScript+="ctx.fillStyle=colour;\n";
          javaScript+="ctx.fill();\n";
     
          javaScript+="}\n";
          
          javaScript+="ctx.stroke();\n";
          javaScript+="ctx.closePath();\n";
          
      javaScript+="}\n";
      
    javaScript+="</SCRIPT>\n";

  }
  
  String buildXMLDiv(double tab[100]){
    String XMLDiv="";
    for(int i=0;i<99;i++){
      XMLDiv+=tab[i];
      XMLDiv+=",";
    }
    XMLDiv+=tab[99];
    return XMLDiv;
  }
  void buildXML(){

    XML="<?xml version='1.0'?>";
    XML+="<divs>";
    
    XML+="<readings>";
    XML+=getMeasures();
    XML+="</readings>";

    XML+="<nOfP_5s>";
    XML+=nOfP_5s;
    XML+="</nOfP_5s>";
    
    XML+="<temp1_5s>";
    XML+=buildXMLDiv(temp1_5s);
    XML+="</temp1_5s>";
  
    XML+="<temp2_5s>";
    XML+=buildXMLDiv(temp2_5s);
    XML+="</temp2_5s>";

    XML+="<hum_5s>";
    XML+=buildXMLDiv(hum_5s);
    XML+="</hum_5s>";
    
    XML+="<nOfP_1m>";
    XML+=nOfP_1m;
    XML+="</nOfP_1m>";
    
    XML+="<temp1_1m>";
    XML+=buildXMLDiv(temp1_1m);
    XML+="</temp1_1m>";
  
    XML+="<temp2_1m>";
    XML+=buildXMLDiv(temp2_1m);
    XML+="</temp2_1m>";

    XML+="<hum_1m>";
    XML+=buildXMLDiv(hum_1m);
    XML+="</hum_1m>";

    XML+="<nOfP_1h>";
    XML+=nOfP_1h;
    XML+="</nOfP_1h>";
    
    XML+="<temp1_1h>";
    XML+=buildXMLDiv(temp1_1h);
    XML+="</temp1_1h>";
  
    XML+="<temp2_1h>";
    XML+=buildXMLDiv(temp2_1h);
    XML+="</temp2_1h>";

    XML+="<hum_1h>";
    XML+=buildXMLDiv(hum_1h);
    XML+="</hum_1h>";
    XML+="</divs>";

  }
  String getMeasures(){
    String measures="Temperatura DS18B20: ";
    measures += temperature1;
    measures +="°C\n";
    measures += "   Temperatura HTU21: ";
    measures += temperature2;
    measures +="°C\n";
    measures += "   Wilgotnosc:";
    measures += humidity;
    measures +="%";
    return measures;
  }
  
  void handleWebsite(){
    buildWebsite();
    Serial.print("\n\n\nsize of webSite: ");
    Serial.println(webSite.length());
    Serial.println("\n\n");
    server.send(200,"text/html",webSite);
  }
  
  void handleXML(){
    buildXML();
    server.send(200,"text/xml",XML);

  }
  
  void setup() {
   Serial.begin(115200);
    delay(10);
    Serial.println("Starting ");
  
    sensors.begin();
    sensors.setWaitForConversion(false);
    sensors.setResolution(12); 
    sensors.requestTemperatures();
    //myHumidity.begin();
    myhtu.begin();
    delay(100);
    //byte b=0x81;
    //myHumidity.setResolution(b);
    
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin (ssid,password);
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected");
    Serial.print("Station IP address: ");
    Serial.println(WiFi.localIP());
    server.on("/",handleWebsite);
    server.on("/xml",handleXML);
    server.begin();  
    configTime(3 * 3600,0,"time.nist.gov","pool.ntp.org");
    Serial.println("\nWaiting for time");
    while (!time(nullptr)) {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("");
    for(int i=0;i<100;i++){
      time_5s[i]=time_1m[i]=time_1h[i]=0;
      temp1_5s[i]=temp2_5s[i]=hum_5s[i]=0;
      temp1_1m[i]=temp2_1m[i]=hum_1m[i]=0;
      temp1_1h[i]=temp2_1h[i]=hum_1h[i]=0;
    }
    global_millis=millis();
  }
  
    
  
  const int I2C_TIMEOUT_ERROR=998;
  const int BAD_CRC_ERROR=999;
  
  void set_time_and_readings(time_t now,double temperature1,double temperature2,double humidity){
      struct tm *now_tm=localtime(&now);
      now_tm->tm_hour-=2;
      now_tm->tm_hour+=now_tm->tm_isdst;
  
      if(now_tm->tm_sec%5==0 and now!=time_5s[0]){
      for(int i=99;i>0;i--){
        time_5s[i]=time_5s[i-1];
        temp1_5s[i]=temp1_5s[i-1];
        temp2_5s[i]=temp2_5s[i-1];
        hum_5s[i]=hum_5s[i-1];
      }
      temp1_5s[0]=temperature1;
      temp2_5s[0]=temperature2;
      hum_5s[0]=humidity;
      time_5s[0]=now;
      
      counter_1m++;
      Serial.print("counter 1m: ");
      Serial.println(counter_1m);
      average_temp1_1m+=temperature1;
      average_temp2_1m+=temperature2;
      average_hum_1m+=humidity;  

      if(counter_1m>=12){
        for(int i=99;i>0;i--){
        time_1m[i]=time_1m[i-1];
        temp1_1m[i]=temp1_1m[i-1];
        temp2_1m[i]=temp2_1m[i-1];
        hum_1m[i]=hum_1m[i-1];
        }
        
        temp1_1m[0]=average_temp1_1m/(double) counter_1m; //temperature1;
        temp2_1m[0]=average_temp2_1m/(double) counter_1m; //temperature2;
        hum_1m[0]=average_hum_1m/(double) counter_1m; //humidity;
        counter_1m=0;
        time_1m[0]=now;
        average_temp1_1m=0;
        average_temp2_1m=0;
        average_hum_1m=0;

        counter_1h++;
        Serial.print("counter 1h: ");
        Serial.println(counter_1h);
        if(counter_1h>=60){
          for(int i=99;i>0;i--){
          time_1h[i]=time_1h[i-1];
          temp1_1h[i]=temp1_1h[i-1];
          temp2_1h[i]=temp2_1h[i-1];
          hum_1h[i]=hum_1h[i-1];
          }
          temp1_1h[0]=average_temp1_1h/(double) counter_1h; //temperature1;
          temp2_1h[0]=average_temp2_1h/(double) counter_1h; //temperature2;
          hum_1h[0]=average_hum_1h/(double) counter_1h; //humidity;
          time_1h[0]=now;
          counter_1h=0;
          average_temp1_1h=0;
          average_temp2_1h=0;
          average_hum_1h=0;
        }
      }
    }
    nOfP_5s=0;
    nOfP_1m=0;
    nOfP_1h=0;
    for(int i=0;i<100;i++){
        if(time_5s[i]!=0){
          nOfP_5s++;
        }
        if(time_1m[i]!=0){
          nOfP_1m++;
        }
        if(time_1h[i]!=0){
          nOfP_1h++;
        }
     }
  }

  void loop() {
    Serial.print("heap size:");
    Serial.println(ESP.getFreeHeap());
    time_t rawTime;
    time(&rawTime);
    sensors.requestTemperatures();
    temperature1=sensors.getTempCByIndex(0);
    temperature2=myhtu.readTemperature();

    humidity=myhtu.readHumidity();
    set_time_and_readings(rawTime,temperature1,temperature2,humidity);
    Serial.println("DS18B20 Temperature: "+String(temperature1)+"°C");
    Serial.println("HTU21 Temperature: "+String(temperature2)+"°C");
    Serial.println("HTU21 Humidity: "+String(humidity)+"%"); 

    server.handleClient();

    delay(800);
  }
