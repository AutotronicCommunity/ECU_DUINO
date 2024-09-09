// ------------------------------------------------------------------------
// PERHATIAN!!! SKETCH INI BELUM TERUJI JADI DIBUTUHKAN RISET LEBIH LANJUT
// ------------------------------------------------------------------------
// Pin Definitions
const int tpsPin = A0;              // Pin untuk sensor TPS (Analog Input)
const int crankshaftSensorPin = 2;   // Pin untuk sensor crankshaft (Digital Input)
const int injectorPin = 9;           // Pin kontrol untuk injektor bahan bakar (PWM Output)
const int ignitionPin = 10;          // Pin kontrol untuk pengapian (Digital Output)
const int fuelPumpPin = 11;          // Pin kontrol untuk Fuel Pump
// Tabel RPM dan TPS (35 x 25)
const int numRPMPoints = 35;         // Jumlah titik data RPM
const int numTPSPoints = 25;         // Jumlah titik data TPS

// Tabel RPM dan TPS
int rpmTable[numRPMPoints] = {1000, 1200, 1400, 1600, 1800, 2000, 2200, 2400, 2600, 2800, 3000, 3200, 3400, 3600, 3800, 4000, 4200, 4400, 4600, 4800, 5000, 5200, 5400, 5600, 5800, 6000, 6200, 6400, 6600, 6800, 7000};
int tpsTable[numTPSPoints] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 100};

// Tabel pengapian (35 x 25), berisi data derajat pengapian
float ignitionTable[numRPMPoints][numTPSPoints] = {
  {10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58},
  // Baris selanjutnya diisi sesuai banyak data. contoh sketch ini 35 x 25 (rpm vs tps)
};

// Tabel fuel (35 x 25), berisi timing injector (dalam milidetik atau persen duty cycle)
float fuelTable[numRPMPoints][numTPSPoints] = {
  {1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.2, 4.4, 4.6, 4.8, 5.0, 5.2, 5.4, 5.6, 5.8},
  // Baris selanjutnya diisi sesuai banyak data data. contoh sketch ini 35 x 25  (rpm vs tps)
};

// Variabel untuk RPM dan timing crankshaft
volatile unsigned long lastPulseTime = 0;
volatile unsigned long crankPulseInterval = 0;
int rpm = 0;
int tps = 0;
float ignitionTiming = 0;
float fuelTiming = 0;

void setup() {
  pinMode(injectorPin, OUTPUT);     // Pin untuk kontrol injektor
  pinMode(ignitionPin, OUTPUT);     // Pin untuk kontrol pengapian
  pinMode(fuelPumpPin, OUTPUT);     // Pin untuk kontrol pengapian
  pinMode(crankshaftSensorPin, INPUT_PULLUP);  // Sensor crankshaft
  digitalWrite(fuelPumpPin,HIGH); // Fuel Pump Aktif
  // Interrupt untuk mendeteksi pulsa dari sensor crankshaft
  attachInterrupt(digitalPinToInterrupt(crankshaftSensorPin), crankshaftISR, RISING);
}

void loop() {
  // Membaca nilai TPS dari sensor dan mengkonversinya menjadi persentase (0-100%)
  tps = analogRead(tpsPin) / 10.23;
  
  // Menghitung RPM berdasarkan interval pulsa crankshaft
  if (crankPulseInterval > 0) {
    rpm = 60000000 / crankPulseInterval;  // Konversi mikrodetik ke RPM
  }

  // Mencari indeks terdekat untuk RPM dan TPS pada tabel
  int rpmIndex = findNearestIndex(rpmTable, numRPMPoints, rpm);
  int tpsIndex = findNearestIndex(tpsTable, numTPSPoints, tps);

  // Mengambil nilai pengapian (derajat) dan fuel timing dari tabel
  ignitionTiming = ignitionTable[rpmIndex][tpsIndex];
  fuelTiming = fuelTable[rpmIndex][tpsIndex];

  // Kontrol pengapian dan injektor bahan bakar
  controlIgnition(ignitionTiming);
  controlFuelInjector(fuelTiming);
}

// Fungsi Interrupt Service Routine untuk sensor crankshaft
void crankshaftISR() {
  unsigned long currentTime = micros();
  crankPulseInterval = currentTime - lastPulseTime;  // Interval antara dua pulsa
  lastPulseTime = currentTime;
}

// Fungsi untuk mencari indeks terdekat pada tabel RPM atau TPS
int findNearestIndex(int *table, int size, int value) {
  int nearestIndex = 0;
  for (int i = 0; i < size; i++) {
    if (abs(table[i] - value) < abs(table[nearestIndex] - value)) {
      nearestIndex = i;
    }
  }
  return nearestIndex;
}

// Fungsi untuk mengontrol pengapian berdasarkan derajat pengapian
void controlIgnition(float timingDegrees) {
  // Placeholder untuk kontrol pengapian
  if (timingDegrees > 0) {
    digitalWrite(ignitionPin, HIGH);  // Simulasi pengapian pada derajat tertentu
    delayMicroseconds((int)timingDegrees * 100);  // Delay untuk representasi waktu pengapian
    digitalWrite(ignitionPin, LOW);
  }
}

// Fungsi untuk mengontrol injektor bahan bakar berdasarkan timing
void controlFuelInjector(float fuelTiming) {
  // Konversi timing injeksi ke nilai PWM atau waktu
  int pulseWidth = map(fuelTiming, 0, 10, 0, 255);  // Misal map dari 0-10 ms ke 0-255 (PWM)
  analogWrite(injectorPin, pulseWidth);  // Kontrol injektor menggunakan PWM
}
