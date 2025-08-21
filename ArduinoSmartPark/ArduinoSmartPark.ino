#include <Servo.h>

// TCRT5000 sensör ve LED pinleri
const int tcrtPins[6] = {A0, A1, A2, A3, A4, A5}; // TCRT5000 sensör pinleri
const int ledPins[6]  = {3, 4, 2, 6, 5, 7};       // Her sensöre karşılık gelen LED

// Ultrasonik sensör pinleri
const int trigGiris = 8;
const int echoGiris = 9;
const int trigCikis = 10;
const int echoCikis = 11;

// Servo motorlar
Servo servoGiris;
Servo servoCikis;

// Servo pinleri
const int servoGirisPin = 12;
const int servoCikisPin = 13;

int kapasite = 6;
int doluYer = 0;

void setup() {
  Serial.begin(9600); // Hem bilgisayara hem ESP'ye veri gönderir

  // TCRT ve LED pin ayarları
  for (int i = 0; i < 6; i++) {
    pinMode(tcrtPins[i], INPUT);
    pinMode(ledPins[i], OUTPUT);
  }

  // Ultrasonik sensör pinleri
  pinMode(trigGiris, OUTPUT);
  pinMode(echoGiris, INPUT);
  pinMode(trigCikis, OUTPUT);
  pinMode(echoCikis, INPUT);

  // Servo motor bağlantıları
  servoGiris.attach(servoGirisPin);
  servoCikis.attach(servoCikisPin);

  // Servolar başlangıçta kapalı konumda
  servoGiris.write(90);  // Giriş servosu ters yönde
  servoCikis.write(0);   // Çıkış servosu düz yönde
}

void loop() {
  doluYer = 0;
  String ledDurum = "";

  // TCRT5000 ile park yeri kontrolü
  for (int i = 0; i < 6; i++) {
    int tcrtValue = digitalRead(tcrtPins[i]);

    if (tcrtValue == LOW) {
      digitalWrite(ledPins[i], HIGH);
      doluYer++;
      ledDurum += "1";
    } else {
      digitalWrite(ledPins[i], LOW);
      ledDurum += "0";
    }
  }

  // Durumu hem bilgisayara hem ESP'ye gönder
  Serial.print("Dolu Yer: ");
  Serial.println(doluYer);
  Serial.println(ledDurum); // Örnek: 101100

  // GİRİŞ KONTROLÜ
  if (aracVarMi(trigGiris, echoGiris)) {
    if (doluYer < kapasite) {
      Serial.println("Giriş: Araç algılandı ve boş yer var.");
      kapiyiYavasAcKapatTers(servoGiris);
    } else {
      Serial.println("Giriş: Kapasite dolu, kapı açılmıyor.");
    }
  }

  // ÇIKIŞ KONTROLÜ
  if (aracVarMi(trigCikis, echoCikis)) {
    Serial.println("Çıkış: Araç algılandı.");
    kapiyiYavasAcKapat(servoCikis);
  }

  delay(500);
}

// Ultrasonik sensör ile araç algılama
bool aracVarMi(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long sure = pulseIn(echoPin, HIGH, 20000);
  int mesafe = sure * 0.034 / 2;

  return (mesafe > 0 && mesafe < 15);
}

// DÜZ çalışan servo motor için yavaş aç-kapat
void kapiyiYavasAcKapat(Servo &servoMotor) {
  for (int pos = 10; pos <= 65; pos++) {
    servoMotor.write(pos);
    delay(15);
  }

  delay(3000);

  for (int pos = 65; pos >= 10; pos--) {
    servoMotor.write(pos);
    delay(15);
  }
}

// TERS çalışan servo motor için yavaş aç-kapat
void kapiyiYavasAcKapatTers(Servo &servoMotor) {
  for (int pos = 65; pos >= 10; pos--) {
    servoMotor.write(pos);
    delay(15);
  }

  delay(3000);

  for (int pos = 10; pos <= 65; pos++) {
    servoMotor.write(pos);
    delay(15);
  }
}
