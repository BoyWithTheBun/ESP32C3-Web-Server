#include <WiFi.h>
#include <WebServer.h>

const char* ssid = ""; //The WIFI your labtop is connected to
const char* password = ""; //The pass word

WebServer server(80);
volatile unsigned long test = 0;
unsigned long past = 0;

void handleRoot() {
//Create a string titled HTML that can be sent to the webserver with all the necessary
//HTML magic to create the website
String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Live Scrolling Log</title>
</head>
<body>
  <h1>Live Variable Stream</h1>
  <pre id="val"></pre>

<script>
let logBox = document.getElementById("val");

setInterval(() => {
  fetch("/value")
    .then(r => r.text())
    .then(val => {

      logBox.innerText += val + "\n";   

      let lines = logBox.innerText.split("\n");
      if (lines.length > 300) {
        logBox.innerText = lines.slice(-300).join("\n");
      }

      logBox.scrollTop = logBox.scrollHeight; 
    });
}, 300);
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

//This function takes the current value in test makes it readable by HTML
void handleValue() {
  server.send(200, "text/plain", "Velocity" + String(test));
}

void setup() {
  Serial.begin(115200);
  delay(200); 

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");

//Waiting to Connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

//Every time fetch is looped to, these functions will trigger to update values
  server.on("/", handleRoot);
  server.on("/value", handleValue);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  unsigned long timer = millis();

  //Simple loop to count up test without the use of delay
  if ((timer - past) > 300) {
    test++;
    past = timer;
  }
}

