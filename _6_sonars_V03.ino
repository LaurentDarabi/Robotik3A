// ***********************************************************************
//
// Programme de detection a 6 sonars CLUB ROBOTIK ENSIL ENSCI
//
// MAJ : 6 avril 2018 - David GROSSOLEIL
//
// **********************************************************************

// definition des 6 entrees analogique en provenance des sonars
#define PinAnloS1 0
#define PinAnloS2 1
#define PinAnloS3 2
#define PinAnloS4 3
#define PinAnloS5 4
#define PinAnloS6 5

// definition des sorties TOR pour les 6 LED
#define PinLedS1 8
#define PinLedS2 7
#define PinLedS3 9
#define PinLedS4 10
#define PinLedS5 12
#define PinLedS6 11

#define PinInterupt 2 // pin d'interruption en provenance du dernier sonar
#define PinStop 3 // envoie HIGHT si un sonar detecte
#define PinTrigger 4 // pin pour trigger les sonars
#define PinAV 5 // envoie HIGHT si un sonar AVANT detecte
#define PinAR 6 // envoie HIGHT si un sonar ARRIERE detecte

volatile byte Trigger = LOW;
volatile int ARG = 0;
volatile int AR = 0;
volatile int ARD = 0;
volatile int AVG = 0;
volatile int AV = 0;
volatile int AVD = 0;
volatile unsigned long temps;

// **********************************************************************
float distance_seuil = 50; // distance de detection en cm
// **********************************************************************

void setup()
{
  pinMode(PinLedS1, OUTPUT);
  pinMode(PinLedS2, OUTPUT);
  pinMode(PinLedS3, OUTPUT);
  pinMode(PinLedS4, OUTPUT);
  pinMode(PinLedS5, OUTPUT);
  pinMode(PinLedS6, OUTPUT);

  pinMode(PinTrigger, OUTPUT);
  pinMode(PinStop, OUTPUT);
  pinMode(PinAV, OUTPUT);
  pinMode(PinAR, OUTPUT);

  digitalWrite(PinTrigger, Trigger);
  digitalWrite(PinStop, LOW);
  digitalWrite(PinAV, LOW);
  digitalWrite(PinAR, LOW);

  pinMode(PinInterupt, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinInterupt), mesure, RISING);

  distance_seuil = distance_seuil * 0.8; // mise a l echelle 1,25 cm / quanta
  temps = millis();
}

// timer a 400 ms qui sert de trigger pour le sonar 1 ARG
void loop()
{
  if ((millis() - temps) > 200)
  {
    Trigger = !Trigger;
    temps = millis();
    digitalWrite(PinTrigger, Trigger);
  }
}

// interruption en provenance du sonar 6 AVG
void mesure()
{
  ARG = (analogRead(PinAnloS1) < distance_seuil);
  digitalWrite(PinLedS1, ARG);

  AR = (analogRead(PinAnloS2) < distance_seuil);
  digitalWrite(PinLedS2, AR);

  ARD = (analogRead(PinAnloS3) < distance_seuil);
  digitalWrite(PinLedS3, ARD);

  AVD = (analogRead(PinAnloS4) < distance_seuil);
  digitalWrite(PinLedS4, AVD);

  AV = (analogRead(PinAnloS5) < distance_seuil);
  digitalWrite(PinLedS5, AV);

  AVG = (analogRead(PinAnloS6) < distance_seuil);
  digitalWrite(PinLedS6, AVG);

  digitalWrite(PinAV, ((AVG || AV) || AVD));
  digitalWrite(PinAR, ((ARG || AR) || ARD));
  digitalWrite(PinStop, ((AVG || AV) || AVD) || ((ARG || AR) || ARD));
}
