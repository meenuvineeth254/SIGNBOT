#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// Servo Configuration
Servo s1, s2, s3, s4, s5;
const int servoPins[5] = {D1, D2, D5, D6, D7};



ESP8266WebServer server(80);

// Alphabet mapping (keep your original entries)
struct SignLanguageAlphabet {
  char letter;
  int angles[5];
};

SignLanguageAlphabet alphabetMap[] = {
  {'A', {180,180,180,0,0}},
  {'B', {180,0,0,180,180}},
  {'C', {45,90,90,90,90}},
  {'D', {0,180,0,180,180}},
  {'E', {0,0,0,0,0}},
  {'F', {45,180,180,180,180}},
  {'G', {180,180,0,180,180}},
  {'H', {180,180,180,0,0}},
  {'I', {0,0,0,0,180}},
  {'J', {90,0,0,0,180}},
  {'K', {90,180,0,0,0}},
  {'L', {0,180,0,180,180}},
  {'M', {0,0,0,0,180}},
  {'N', {0,0,0,180,180}},
  {'O', {90,90,90,90,90}},
  {'P', {90,180,180,0,0}},
  {'Q', {180,180,0,180,180}},
  {'R', {0,180,180,0,0}},
  {'S', {0,0,0,0,0}},
  {'T', {0,0,0,0,180}},
  {'U', {0,180,180,180,180}},
  {'V', {0,180,180,0,0}},
  {'W', {0,180,180,180,180}},
  {'X', {0,45,0,0,0}},
  {'Y', {180,0,0,0,180}},
  {'Z', {90,180,180,0,0}},
};

void setup() {
  Serial.begin(115200);
 
  // Attach servos
  for(int i=0; i<5; i++){
    switch(i){
      case 0: s1.attach(servoPins[i]); break;
      case 1: s2.attach(servoPins[i]); break;
      case 2: s3.attach(servoPins[i]); break;
      case 3: s4.attach(servoPins[i]); break;
      case 4: s5.attach(servoPins[i]); break;
    }
  }

  WiFi.mode(WIFI_STA);   // fun to set esp as wifi station
  WiFi.begin("iot","123456789"); // wifi credentials of hotspot to which esp need to get connected
  Serial.println("connecting");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.print(WiFi.localIP());
  delay(2000);
  s1.write(0);
  s2.write(0);
  s3.write(0);
  s4.write(180);
  s5.write(180);
 
  // Configure server
  server.onNotFound(handleGesture);
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleGesture() {
  String path = server.uri();
  path.toLowerCase();
 
  if(path.startsWith("/gesture/")) {
    String inputText = path.substring(9); // Skip "/gesture/"
    inputText.replace("%20", " ");        // Decode spaces
    inputText.toUpperCase();
   
    Serial.print("Processing: ");
    Serial.println(inputText);

    processText(inputText);
    server.send(200, "text/plain", "Processed: " + inputText);
  }
  else {
    server.send(404, "text/plain", "Use /gesture/YOUR_TEXT");
  }
}

void processText(String text) {
  for(int i=0; i<text.length(); i++){
    char c = text[i];
    bool found = false;
   
    for(auto &entry : alphabetMap){
      if(entry.letter == c){
        moveServos(entry.angles);
        found = true;
        delay(2000); // 2s between letters
        break;
      }
    }
   
    if(!found) Serial.print("Unknown: "); Serial.println(c);
  }
}

void moveServos(int angles[5]) {
  s1.write(angles[0]);
  s2.write(angles[1]);
  s3.write(angles[2]);
  s4.write(angles[3]);
  s5.write(angles[4]);
  delay(300); // Servo stabilization
}
