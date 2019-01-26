/*
   en double résolution droit : 330 TIC /tour ; diamètre = 65 mm
   gauche : 320 TIC /tour ; diamètre = 65 mm

   4,5m - 90° - aller jusqu'à 1m puis 1,5 et aller-retours
   TIC totation = 672
*/


// MOTEUR - Déclaration des PINs et des fonctions

int PWM_G = 5; // Moteur gauche : pin de controle de la PWM
int sens_G = 4; // Moteur gauche : pin de direction
int PWM_D = 6; // Moteur droit : pin de controle de la PWM
int sens_D = 7; // Moteur droit : pin de direction

const int interuptG = 2; // pin d'interruption du moteur gauche
const int interuptD = 3; // pin d'interruption du moteur droit
const int canalBG = 9;
const int canalBD = 10; // voie B de l'encodeur, permettrait de trouver le sens de rotation : elle est en 1/4 de phase par rapport à la voie A, on s'en occupe pas

void avance(double mm_); // fonction pour faire avancer le robot x mm
void recule(double mm_); // fonction pour faire reculer le robot de x mm
void tourne_a_gauche(double degre_); // fonction pour faire tourner le robot vers la gauche du nombre de degrés indiqués
void tourne_a_droite(double degre_); // fonction pour faire tourner le robot vers la gauche du nombre de degrés indiqués


// ENCODEUR - Déclaration des PINs et des fonctions

int comptD = 0; // compteur de l'encodeur droit
int comptG = 0; // compteur de l'encodeur gauche

void augmenter_comptG(); //fonction qui augmente le compteur lié à la roue gauche
void augmenter_comptD(); // fonction qui augmente le compteur lié à la roue droite


// VITESSE et GRANDEURS PHYSIQUES (longueurs...)

double diametre = 65; // # mm
double empattement = 185; // # mm

int vitesse = 140;
int vitesse_D = vitesse; // vitesse du moteur gauche
int vitesse_G = vitesse; // vitesse du moteur droit
// Vitesses non égales car l'un des deux est monté à l'envers donc est légèrement plus lent que l'autre

double mm_par_TIC_G = PI * diametre / 320.0; // # m/TIC
double mm_par_TIC_D = PI * diametre / 330.0; // # m/TIC
double TIC_lim;


/* PARTIE CORRECTION DEFINITION DES VAR*/

int buffer_cptG = 0; // variable compteur temporaire
int buffer_cptD = 0;

int diffG = 0;  // différence à chaque boucle entre la valeur actuelle et la valeur précédente
int diffD = 0;

int correc = 0; //coefficient correcteur de vitesse
int correcMemo =correc;

/************************************************/


void setup()
{
  pinMode(sens_G, OUTPUT);
  pinMode(sens_D, OUTPUT);
  Serial.begin(9600);

  pinMode(canalBG, INPUT_PULLUP);
  pinMode(canalBD, INPUT_PULLUP);
  pinMode(interuptG, INPUT_PULLUP);
  pinMode(interuptD, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interuptG), augmenter_comptG, CHANGE); // fronts montants et descendants
  attachInterrupt(digitalPinToInterrupt(interuptD), augmenter_comptD, CHANGE); // fronts montants et descendants

  // Sens de rotation des moteurs, pour aller en avant, mettre LOW x2
  digitalWrite(sens_G, LOW);
  digitalWrite(sens_D, LOW);

  // Vitesse donnée aux moteurs
  analogWrite(PWM_G, 0);
  analogWrite(PWM_D, 0);

}


void loop()
{
  avance(1000); // # mm
  tourne_a_droite(360);  // # degre
  
  while (1);
  /*pas besoin des "{}" ici
   * permet "d'arreter" le programme en faisant une boucle infinie
   */

}

void correction(){
  /*cette fonction effectue une correction en vitesse. La vitesse du moteur gauche est fixe,
  le droit s'adapte à sa vitesse.*/

  diffD = comptD - buffer_cptD; /* */
  diffG = comptG - buffer_cptG;

  if (diffG - diffD > 0) {
    correc = correc + 1;
  }

  else if (diffG - diffD < 0) {
    correc = correc - 1;
  }


  /* On applique la correction de vitesse */
  if ((vitesse > 5) || (vitesse < 250)) {
      vitesse_D = vitesse + correc;
  }

  return;
}

void avance(double mm_) { // fait avancer le robot de mm_ en millimetre
  comptG = 0;
  comptD = 0;
  buffer_cptG = comptG;
  buffer_cptD = comptD;
  
  TIC_lim = mm_ / mm_par_TIC_G; // se cale sur la roue GAUCHE
  
  //Serial.print(TIC_lim);
  digitalWrite(sens_G, LOW); // le moteur tourne dans le sens avant
  digitalWrite(sens_D, LOW); // le moteur tourne dans le sens avant
  analogWrite(PWM_G, vitesse_G);   //PWM Speed Control
  analogWrite(PWM_D, vitesse_D);   //PWM Speed Control
  int compteur = 0;
  while (comptG <= TIC_lim) { // le robot avance jusqu'a la la valeur de TIC demandée
    /* Ne me demandez pas pourquoi, si la boucle est vide, elle ne s'arrête jamais
       et lorsqu'on affiche le compteur du nombre de boucle, même débranché de l'ordi...
       Bah ça se met à marcher correctement
    */

    /*calcul et application de la correction*/
    correction();
    analogWrite(PWM_G, vitesse_G);   //PWM Speed Control
    analogWrite(PWM_D, vitesse_D);   //PWM Speed Control
    buffer_cptG = comptG;
    buffer_cptD = comptD;
    /**************END CORRECTION*************************/
    
    Serial.println(compteur);
    compteur++;
  }
  analogWrite(PWM_G, 0);   // arret des moteurs
  analogWrite(PWM_D, 0);   // arret des moteurs
  return;
}



void recule(double mm_) { // fait reculer le robot de mm_ en millimetre
  comptG = 0;
  comptD = 0;
  TIC_lim = mm_ / mm_par_TIC_G; // se cale sur la roue GAUCHE
  
  //Serial.print(TIC_lim);
  digitalWrite(sens_G, HIGH); // le moteur tourne dans le sens arrière
  digitalWrite(sens_D, HIGH); // le moteur tourne dans le sens arrière
  analogWrite(PWM_G, vitesse_G);   //PWM Speed Control
  analogWrite(PWM_D, vitesse_D);   //PWM Speed Control
  int compteur = 0;
  while (comptG <= TIC_lim) { // le robot recule jusqu'a la la valeur de TIC demandée
    /* Ne me demandez pas pourquoi, si la boucle est vide, elle ne s'arrête jamais
       et lorsqu'on affiche le compteur du nombre de boucle, même débranché de l'ordi...
       Bah ça se met à marcher correctement
    */

    /*calcul et application de la correction*/
    correction();
    digitalWrite(sens_G, HIGH); // le moteur tourne dans le sens avant
    digitalWrite(sens_D, HIGH); // le moteur tourne dans le sens avant
    analogWrite(PWM_G, vitesse_G);   //PWM Speed Control
    analogWrite(PWM_D, vitesse_D);   //PWM Speed Control
    buffer_cptG = comptG;
    buffer_cptD = comptD;
    /**************END CORRECTION*************************/

    
    Serial.println(compteur);
    compteur++;
  }
  analogWrite(PWM_G, 0);   // arret des moteurs
  analogWrite(PWM_D, 0);   // arret des moteurs
  return;
}


void tourne_a_gauche(double degre_) {
  comptG = 0;
  comptD = 0;
  double dist = PI * empattement * degre_ / 360.0;
  TIC_lim = dist / mm_par_TIC_G; // se cale sur la roue GAUCHE
  
  digitalWrite(sens_G, HIGH); // le moteur tourne dans le sens arrière
  digitalWrite(sens_D, LOW); // le moteur tourne dans le sens avant
  analogWrite(PWM_G, vitesse_G);   //PWM Speed Control
  analogWrite(PWM_D, vitesse_D);   //PWM Speed Control
  int compteur = 0;
  while (comptG <= TIC_lim) { // le robot tourne jusqu'a la la valeur de TIC demandée
    /* Ne me demandez pas pourquoi, si la boucle est vide, elle ne s'arrête jamais
       et lorsqu'on affiche le compteur du nombre de boucle, même débranché de l'ordi...
       Bah ça se met à marcher correctement
    */
    Serial.println(compteur);
    compteur++;
  }
  analogWrite(PWM_G, 0);   // arret des moteurs
  analogWrite(PWM_D, 0);   // arret des moteurs
  return;
}

void tourne_a_droite(double degre_) {
  comptG = 0;
  comptD = 0;
  double dist = PI * empattement * degre_ / 360.0;
  TIC_lim = dist / mm_par_TIC_G; // se cale sur la roue GAUCHE

  digitalWrite(sens_G, LOW); // le moteur tourne dans le sens avant
  digitalWrite(sens_D, HIGH); // le moteur tourne dans le sens arrière
  analogWrite(PWM_G, vitesse_G);   //PWM Speed Control
  analogWrite(PWM_D, vitesse_D);   //PWM Speed Control
  int compteur = 0;
  while (comptG <= TIC_lim) { // le robot tourne jusqu'a la la valeur de TIC demandée
    /* Ne me demandez pas pourquoi, si la boucle est vide, elle ne s'arrête jamais
       et lorsqu'on affiche le compteur du nombre de boucle, même débranché de l'ordi...
       Bah ça se met à marcher correctement
    */
    Serial.println(compteur);
    compteur++;
  }

  analogWrite(PWM_G, 0);   // arret des moteurs
  analogWrite(PWM_D, 0);   // arret des moteurs
  return;
}


void augmenter_comptG() // incrémente le compteur gauche
{
  comptG++;
  return;
}
void augmenter_comptD() // incrémente le compteur droit
{
  comptD ++;
  return;
}
