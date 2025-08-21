#include "esp_camera.h" 
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "BUM";
const char* password = "bum12321";

WebServer server(80);

// Gelen veri: P1, P2, P3, P4, P5, P6
// Gösterim sırası: P5, P4, P6, P3, P1, P2 --> A1, A2, A3, B1, B2, B3
String parkDurumu = "000000";  // Başlangıçta tüm park yerleri boş

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<meta http-equiv='refresh' content='0.3'>";  // Sayfayı 300ms'de bir yenile
  html += "<title>Park Durumu</title>";
  html += "<style>body{font-family:Arial;text-align:center;} .box{display:inline-block;width:100px;height:100px;margin:10px;font-size:20px;line-height:100px;border-radius:10px;} .dolu{background:red;color:white;} .bos{background:green;color:white;}</style></head><body>";
  html += "<h1>Bilge Kaan Öztürk</h1>";

  // A1-A2-A3-B1-B2-B3 görsel sırasına göre: P5, P4, P6, P3, P1, P2
  int goruntuSirasi[6] = {4, 3, 5, 2, 0, 1};  
  String label[6] = {"A1", "A2", "A3", "B1", "B2", "B3"};

  for (int i = 0; i < 6; i++) {
    char durum = parkDurumu[goruntuSirasi[i]];
    String sinif = (durum == '1') ? "dolu" : "bos";
    html += "<div class='box " + sinif + "'>" + label[i] + "</div>";
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(9600);

  // ESP32'yi Access Point modunda başlat
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("ESP32 IP Adresi: ");
  Serial.println(IP);

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();

  // Arduino'dan gelen UART verisini oku
  if (Serial.available()) {
    String gelenVeri = Serial.readStringUntil('\n');
    gelenVeri.trim();  // \n, \r gibi karakterleri temizle

    // Gelen veri 6 karakter uzunluğunda ve sadece 0/1 içermelidir
    if (gelenVeri.length() == 6 && gelenVeri.indexOf('1') != -1 || gelenVeri.indexOf('0') != -1) {
      parkDurumu = gelenVeri;
      Serial.println("Yeni Park Verisi: " + parkDurumu);
    }
  }
}
