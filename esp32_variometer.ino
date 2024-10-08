#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Parametry ekranu OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//https://github.com/abishur/ms5x/blob/main/examples/BasicMS5x/BasicMS5x.ino
#include <MS5x.h>
MS5x barometer(&Wire);

// Pin głośnika
#define SPEAKER_PIN 16

// Częstotliwości dźwięków
#define HIGH_TONE_FREQ 2000  // Wysoki dźwięk
#define LOW_TONE_FREQ 500    // Niski dźwięk

// Tablica do przechowywania ostatnich 3 pomiarów ciśnienia w celu określenia trendu
float pressureQueue[3] = {0, 0, 0};

// Z ilu elementów tablicy będzie obliczany trend
int queueSize = 2;

// Tolerancja szumów ciśnienia
#define TOLERANCE 0.07



//Ciśnienie na startowisku wczytywane po uruchomieniu
float pressureStart = 0.0;


//-------------------------------------------------------------------------------


void updatePressureQueue(float newPressure, float* pressureQueue, int queueSize) {
  // Przesuwanie danych w kolejce
  for (int i = 0; i < queueSize - 1; i++) {
    pressureQueue[i] = pressureQueue[i + 1];
  }
  pressureQueue[queueSize - 1] = newPressure;
}

String analyzeTrend(float* pressureQueue, int queueSize) {
  bool isStable = true;
  bool isRising = true;
  bool isFalling = true;

  for (int i = 1; i < queueSize; i++) {
    float diff = pressureQueue[i] - pressureQueue[i - 1];
    if (abs(diff) > TOLERANCE) {
      isStable = false;
    }
    if (diff < -TOLERANCE) {
      isFalling = false;
    }
    if (diff > TOLERANCE) {
      isRising = false;
    }
  }

  if (isStable) {
    return "Stable";
  } else if (isRising) {
    return "Rising";
  } else if (isFalling) {
    return "Falling";
  } else {
    return "Unstable";
  }
}

void generateSound(String trend) {
  if (trend == "Rising") {
    tone(SPEAKER_PIN, HIGH_TONE_FREQ, 500); // Wysoki dźwięk przez 1 sekundę
  } else if (trend == "Falling") {
    tone(SPEAKER_PIN, LOW_TONE_FREQ, 500); // Niski dźwięk przez 1 sekundę
  } else {
    noTone(SPEAKER_PIN); // Brak dźwięku
  }
}

// Funkcja obliczająca różnicę wysokości
float calculateAltitudeDifference(float P0, float P) {
    // Stałe
    const float g = 9.80665;    // Przyspieszenie ziemskie w m/s²
    const float rho = 1.225;    // Gęstość powietrza w kg/m³

    // Oblicz różnicę wysokości w metrach
    float deltaH = (P0 - P) / (g * rho);
    return deltaH;
}

//-------------------------------------------------------------------------------


void setup() {
  // Start I2C Communication SDA = 5 and SCL = 4 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
  Wire.begin(5, 4);

  // Inicjalizacja ekranu OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    for (;;); // Zatrzymanie programu w przypadku błędu inicjalizacji ekranu
  }
  delay(2000); // Pauza na 2 sekundy

  while(barometer.connect()>0) { // barometer.connect starts wire and attempts to connect to sensor
		display.print("Check your BMP180 Interface and I2C Address.");
    display.display();
    delay(3000); // Pauza na 3 sekundy
	}
  
  display.print("Vario inited...");
  display.display();
  delay(3000); // Pauza na 3 sekundy
  
  // Wyczyszczenie ekranu
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(1, 1);

  // Ustawienie pinu głośnika jako wyjście
  pinMode(SPEAKER_PIN, OUTPUT);
}


void loop() {
  // Wyświetlenie pobranej wysokości i temperatury na ekranie
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  barometer.checkUpdates();

  if (barometer.isReady()) { 

          display.setCursor(10, 10);
          display.print("Temp: " + String(barometer.GetTemp()) + " C");

          float currentPressure = barometer.GetPres() / 100;
          //float currentPressure = bmp180.getPressure() / 100; // Konwersja na hPa
          updatePressureQueue(currentPressure, pressureQueue, queueSize);

          display.setCursor(10, 20);
          display.print("Pres: " + String(currentPressure) + " hPa");

          String trend = analyzeTrend(pressureQueue, queueSize);
          display.setCursor(10, 30);
          display.print("Trend: " + trend);
          // Generowanie dźwięku zależnie od trendu
          generateSound(trend);

          if (pressureStart == 0.0) pressureStart = barometer.GetPres();
          // Oblicz różnicę wysokości od startowiska

          display.setCursor(10, 40);
          float QFE = calculateAltitudeDifference(pressureStart, barometer.GetPres());
          display.print("QFE: " + String(QFE));

          display.setCursor(10, 50);
          float QNH = calculateAltitudeDifference(101325.0, barometer.GetPres());
          display.print("QNH: " + String(QNH));

          display.display();
  }


  // Pauza na 0.1 sekundy
  delay(100);
}
