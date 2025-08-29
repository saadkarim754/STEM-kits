#include <WiFi.h>
#include <WebServer.h>

// Motor Pins
#define IN3 33
#define IN4 32
#define IN1 26
#define IN2 25

// Create web server on port 80
WebServer server(80);

// HTML page
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>2WD ESP32 Remote</title>
<style>
body { font-family: Arial; text-align: center; margin: 0; padding: 0; background: #f5f5f5; }
.navbar { background: #333; overflow: hidden; display: flex; justify-content: center; }
.navbar button { background: #555; color: white; border: none; padding: 14px 20px; margin: 4px; cursor: pointer; }
.navbar button:hover { background: #777; }
.top-buttons { margin: 20px; }
.top-buttons button { padding: 12px 20px; font-size: 16px; margin: 5px; cursor: pointer; }
.control-grid { display: grid; grid-template-columns: repeat(3, 80px); gap: 10px; justify-content: center; }
.control-grid button { padding: 20px; font-size: 18px; cursor: pointer; border-radius: 8px; }
.green { background-color: #4CAF50 !important; color: white; }
</style>
</head>
<body>
<div class="navbar">
  <button>Remote Mode</button>
  <button>Collision Avoidance</button>
  <button>Human Follow</button>
</div>

<div class="top-buttons">
  <button onclick="sendCmd('ARM')">Arm</button>
  <button onclick="sendCmd('DISARM')">Disarm</button>
</div>

<div class="control-grid">
  <div></div>
  <button id="up" onclick="toggleButton(this, 'UP')">&#9650;</button>
  <div></div>
  <button id="left" onclick="toggleButton(this, 'LEFT')">&#9664;</button>
  <div></div>
  <button id="right" onclick="toggleButton(this, 'RIGHT')">&#9654;</button>
  <div></div>
  <button id="down" onclick="toggleButton(this, 'DOWN')">&#9660;</button>
</div>

<script>
function sendCmd(cmd){
  fetch('/cmd?c='+cmd);
}

function toggleButton(el, cmd){
  // Toggle green color
  el.classList.add('green');
  sendCmd(cmd);
  setTimeout(() => { el.classList.remove('green'); sendCmd('STOP'); }, 200);
}
</script>

</body>
</html>
)rawliteral";

// Function to handle commands
void handleCommand() {
  if (server.hasArg("c")) {
    String cmd = server.arg("c");
    if(cmd=="UP") { digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); }
    else if(cmd=="DOWN") { digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH); digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); }
    else if(cmd=="LEFT") { digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH); digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW); }
    else if(cmd=="RIGHT") { digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW); digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH); }
    else { digitalWrite(IN3,LOW); digitalWrite(IN4,LOW); digitalWrite(IN1,LOW); digitalWrite(IN2,LOW); } // STOP
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  // Motor pins as outputs
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Stop motors initially
  digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);
  digitalWrite(IN1,LOW); digitalWrite(IN2,LOW);

  // Start ESP32 in AP mode
  WiFi.softAP("ESP32_2WD_Remote", "12345678");
  Serial.println("Access Point started! Connect to ESP32_2WD_Remote");

  // Server routes
  server.on("/", [](){ server.send(200, "text/html", htmlPage); });
  server.on("/cmd", handleCommand);

  server.begin();
}

void loop() {
  server.handleClient();
}
