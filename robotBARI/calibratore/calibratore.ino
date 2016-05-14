#include <Light.h>
Light qtr;
long times[8];
long m[8] = {5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000};
long M[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int pins_qtr[8] = {26,27,28,29,30,31,32,33}; //PINS DEL SENSORE DI LINEA

short dir=20;

void setup()
{
  Serial.begin(9600);
  qtr.init(pins_qtr, 7000); //INIZIALIZZAZIONE SENSORE DI LINEA: VIENE PASSATO UN VETTORE INDICANTE I PINS
}

void loop()
{
  long t = millis();
  
  while (millis() - t < 8000) {
    qtr.readSensors(times);

    for (int i = 0; i < 8; i++)
    {
      if (times[i] < m[i])
        m[i] = times[i];
      if (times[i] > M[i])
        M[i] = times[i];

      Serial.print(m[i]);
      Serial.print(",");
    }
    Serial.println();
    for (int i = 0; i < 8; i++)
    {
      Serial.print(M[i]);
      Serial.print(",");
    }
    Serial.println();
    Serial.println();
  }
}
