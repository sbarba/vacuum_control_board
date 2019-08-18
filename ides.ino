#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 112 };
EthernetServer server(80);

const String buttonState[2] = {"Closed", "Open"};
const int num_valves = 4;
const int valves[num_valves] = {4, 5, 6, 7};;
const int num_gauges = 4; // These run from 0 up so no need for an array identifying the pins as with valves.

String readString = ""; // Will hold the request from the browser.

void setup(){
  for (int i = 0; i < num_valves; i++) {
    pinMode(valves[i], OUTPUT);
  }

  Ethernet.begin(mac, ip);
  server.begin();
  Serial.begin(9600);
  Serial.println("Server is up at 192.168.0.112.");
}

void loop(){
  EthernetClient client = server.available(); // Create a client connection.
  // Poll for a client.
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (readString.length() < 100) {
          readString += c; // Store characters to readString.
        } 

        // If HTTP request has ended...
        if (c == '\n') {
          Serial.println(readString);
          if (readString.indexOf("Valve") == -1 && readString.indexOf("gauges") == -1) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
  
            client.println("<html>");
            client.println("<head>");
            client.println("<title>IDES</title>");
            client.println("<link rel='icon' href='data:,'>"); // Supress favico request.
  
            client.println("<script type='text/javascript'>");
            client.println("  function toggle_valve(v) {");
            client.println("    let xhr = new XMLHttpRequest();");
            client.println("    xhr.open('get', 'Valve+' + v + Math.random() * 999999);");
            client.println("    xhr.send();");
  
            client.println("    xhr.onreadystatechange = () => {");
            client.println("      if (xhr.readyState === XMLHttpRequest.DONE) {");
            client.println("        document.getElementById('valve_' + v).innerHTML = xhr.responseText");
            client.println("      }");
            client.println("    }");
            client.println("  }");

            client.println("  function gauges() {");
            client.println("    let xhr = new XMLHttpRequest();");
            client.println("    xhr.open('get', 'gauges_' + Math.random() * 999999);");
            client.println("    xhr.send();");

            client.println("    xhr.onreadystatechange = () => {");
            client.println("      if (xhr.readyState === XMLHttpRequest.DONE) {");
            client.println("        document.getElementById('gauges').innerHTML = xhr.responseText");
            client.println("      }");
            client.println("    }");
            client.println("    setTimeout(gauges, 500);");
            client.println("  }");
            client.println("</script>");
            client.println("</head>");

            client.println("<body onload='gauges()'><pre>");
            client.println("<table cellpadding='10'>");
            client.println("<tr>");
            for (int i = 1; i <= num_valves; i++) {
              client.println("<td>Valve " + String(i) + ": <button style='width:55px' id='valve_" + String(i) +  "' onclick='toggle_valve(" + String(i) + ")'>" + buttonState[digitalRead(valves[i - 1])] + "</button></td>");
            }
            client.println("</tr>");
            client.println("<tr id='gauges'>");
            client.println("</tr>");
            client.println("</table>");
            client.println("</pre></body>");
            client.println("</html>");
          }

          if (readString.indexOf("Valve") != -1) {
            int valve = readString.substring(11, 12).toInt(); // Pick the valve number out of the get request.
            digitalWrite(valves[valve-1], !digitalRead(valves[valve-1]));
            client.println(buttonState[digitalRead(valves[valve-1])]);
          }
          if (readString.indexOf("gauges") != -1) {
            for (int i = 1; i <= num_gauges; i++) {
              client.println("<td>Gauge " + String(i) + ": " + String(analogRead(i-1)) + "</td>");
            }
          }
          // Erase last request, wait 5 ms, kill the client & go to top of loop to poll again.
          readString = "";
          delay(5);
          client.stop();
        }
      }
    }
  }
}
