// Import de la librairie responsable de la lumière
#include <FastLED.h>
// Pin Data de la bande led
#define DATA_PIN 14
// Pin des boutons
#define bp1 4
#define bp2 13
// Définir le nombre de LED
#define NUM_LEDS 144
// Définit le type de led
CRGBArray<NUM_LEDS> leds;
// Postion de la led allumé
int nbrLED;
// Définit le sens du "déplacement" de la led
boolean sens;
// Définit le status de la partie (Terminé/En cours)
boolean FIN = true;
// Contrôle l'affichage du mode Transition
boolean starter = false;

/* Définit les limites de la zone de renvoie */
// Limite mini bp1(rappel: -1, on commence à 0)
int lim = 18;
int lim_mini_bp1 = NUM_LEDS-1;
// Limite maxi bp1
int lim_maxi_bp1 = NUM_LEDS-lim;
// Limite mini bp2
int lim_mini_bp2 = 0;
// Limite maxi bp2
int lim_maxi_bp2 = lim;

// Type de vitesse
String type_speed;

// Vitesse de déplacemen de la led / delay d'execution
int vitesse_high   = 0;
int vitesse_medium = 10;
int vitesse_low    = 20;

// Couleur 
CHSV bleu    = CHSV(240,200,60); 
CHSV rouge   = CHSV(360,200,60);
CHSV orange  = CHSV(60,200,60);

//test
//boolean state = true;

// Déclenche le piège
boolean trig_trap = false;

// Dé
int r = 0;
int r_led = 0;

// Power Up
int powerup1 = 0;
int powerup2 = 0;

// Nombre de stack à cumuler
int max_powerUp =3;

void setup() {
  // N'est utile que pour du débug 
  //Serial.begin(115200);
  Serial.begin(9600);
  // Définit le bouton du J1
  pinMode(bp1, INPUT_PULLUP);
  // Définit le bouton du J2
  pinMode(bp2, INPUT_PULLUP);
  //Setup des leds
  LEDS.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
}

void loop() {
  //debug 
  //if (state)test_pal_vitess();
  //test_zone();
  if(FIN == true){
    // Affiche la transition  
    transition();
  }
  if(digitalRead(bp1) == 0 && digitalRead(bp2) == 0 && FIN == true){
    // Procédure d'initialisation d'une partie  
    inits();
  }
  if(FIN == false && starter == true){
    // Le Jeu héhé  
    game();
  }
}



void inits(){
  // Définit les valeurs initiaux
  FIN = false;
  nbrLED = NUM_LEDS/2;

  // On attribue un sens aléatoire
  int sensrandom = random(0,2);
  if(sensrandom == 1){
    sens = true;
  }
  if(sensrandom == 0){
    sens = false;
  }

  // Reset la vitesse du coup
  type_speed = "medium";

  // Reset powerups
  powerup1 = 0;
  powerup2  = 0;
  
  // On s'assure que toutes les leds sont éteintes
  for(int i=0; i<NUM_LEDS/2;i++){
    leds[i] = CHSV(0,0,0);
    leds(NUM_LEDS/2,NUM_LEDS) = leds(NUM_LEDS/2,0);
  }
  // On lance le départ
  for(int i=0; i<3;i++){
    if(i == 0){
      leds[NUM_LEDS/2]= CRGB(0,120,0); //leds[int x]=CHSV(H,S,V);   H=Hue-angle roue de couleur,  S=Saturation,  V=Value-Valeur de brillance
      leds[(NUM_LEDS/2)-1]= CRGB(0,120,0);
      FastLED.show();
    }
    if(i == 1){
      leds[NUM_LEDS/2]= CRGB(75,164,0);
      leds[(NUM_LEDS/2)-1]= CRGB(75,164,0);
      FastLED.show();
    }
    if(i == 2){
      leds[NUM_LEDS/2]= CRGB(120,0,0);
      leds[(NUM_LEDS/2)-1]= CRGB(120,0,0);
      FastLED.show();
    }
    delay(500);
    leds[NUM_LEDS/2]= CRGB(0,0,0);
    leds[(NUM_LEDS/2)-1]= CRGB(0,0,0);
    FastLED.show();
    delay(750);
  }
  starter = true;
}



void game(){
  //--De bp2 à bp1--//
  if(sens == true && starter == true){
    reset_trap();
    for(nbrLED ; nbrLED <= NUM_LEDS-1 ; nbrLED+=1){
      limites();
      showPowerUP();
      // Ajoute un délai avant de repassé à sombre
      leds.fadeToBlackBy(180);
      
      if(type_speed == "high"){
        //Affiche la couleur
        leds[nbrLED] = rouge;
        // Vitesse d'actualisation
        FastLED.delay(vitesse_high);
      } else if (type_speed == "medium") {
        //Affiche la couleur
        leds[nbrLED] = orange;
        // Trap 
        trap();
        // Vitesse d'actualisation
        FastLED.delay(vitesse_medium);
      } else if (type_speed == "low") {
        //Affiche la couleur
        leds[nbrLED] = bleu;
        // Vitesse d'actualisation
        FastLED.delay(vitesse_low);
      }
      
      //-- Inversion de sens --//
      if(nbrLED<lim_mini_bp1 && nbrLED>=lim_maxi_bp1  && digitalRead(bp1) == 0 || trig_trap == true){
        if(nbrLED>=lim_maxi_bp1 && nbrLED<(lim_maxi_bp1+lim/3)){
          type_speed = "low";
          powerup1 = 0;
        } else if (nbrLED<lim_mini_bp1-(lim/3) && nbrLED>=lim_maxi_bp1+(lim/3)){
          type_speed = "medium";
          powerup1 = 0;
          r = random(1,10);
        } else if(nbrLED<=lim_mini_bp1 && nbrLED>=lim_mini_bp1-(lim/3)){
          powerup1 = powerup1 +1;
          type_speed = "high";
        }
        sens = !sens;
        break;
      }
      
      // Power Up
      /*
      if((nbrLED<lim_maxi_bp1 && digitalRead(bp1)==0) && powerup1>=3){
        trig_trap = true;
      }
      */
      //-- en cas d'erreur : si on attteint la "limite_score" qui est la dernière led, ou si on appuie sur le bp avant d'étre dans la zone de renvoie, alors on affiche qui à gagner en vert et perdant en rouge
      if(nbrLED >=lim_mini_bp1 || (nbrLED<lim_maxi_bp1 && digitalRead(bp1)==0)){
        //on fait clignoté 3 fois, un coter rouger et l'autre vert pour montré qui a gagné
        for(int i=0;i<3;i++){
          for(int j=0;j<NUM_LEDS/2;j++){
            leds[(NUM_LEDS/2)+j]=CRGB(0,6,0);
            leds[((NUM_LEDS/2)-1)-j]=CRGB(6,0,0);
          }
          FastLED.show();
          delay(500);
          for(int j=0;j<NUM_LEDS/2;j++){
            leds[(NUM_LEDS/2)+j]=CRGB(0,0,0);
            leds[((NUM_LEDS/2)-1)-j]=CRGB(0,0,0);
          }
          FastLED.show();
          delay(500);
        }
        FIN = true;
        break;
      }
    }
    //r =  random(0,5);
  }
  
  
  //de bp1 à bp2
  else if(sens == false && starter == true){
    reset_trap();
    
    for (nbrLED; nbrLED >= lim_mini_bp2; nbrLED-=1){
      limites();
      showPowerUP();
      leds.fadeToBlackBy(180);
      if(type_speed == "high"){
        //Affiche la couleur
        leds[nbrLED] = rouge;
        // Vitesse d'actualisation
        FastLED.delay(vitesse_high);
      } else if (type_speed == "medium") {
        //Affiche la couleur
        leds[nbrLED] = orange;
        // Trap 
        trap();
        // Vitesse d'actualisation
        FastLED.delay(vitesse_medium);
      } else if (type_speed == "low"){
        //Affiche la couleur
        leds[nbrLED] = bleu;
        // Vitesse d'actualisation
        FastLED.delay(vitesse_low);
      }
      
      //-- Inversion de sens --//
      if(nbrLED<=lim_maxi_bp2 && nbrLED>lim_mini_bp2 && digitalRead(bp2) == 0 || trig_trap == true){
        if(nbrLED<lim_maxi_bp2 && nbrLED>=lim_maxi_bp2-(lim/3)){
          type_speed = "low";
          powerup2 = 0;
        } else if (nbrLED>=lim_mini_bp2+(lim/3) && nbrLED<lim_maxi_bp2-(lim/3)){
          type_speed = "medium";
          powerup2 = 0;
        } else if(nbrLED>=lim_mini_bp2 && nbrLED<lim_mini_bp2+(lim/3)){
          powerup2 = powerup2 +1;
          type_speed = "high";
        }
        sens = !sens;
        break; 
      }
      // Power Up
      /*
      if((nbrLED>lim_maxi_bp2 && digitalRead(bp2)== 0) && powerup2>=3){
        trig_trap = true;
      }
      */
      //-- En cas d'erreur: limite atteinte ou pression trop tôt --//
      if(nbrLED <= lim_mini_bp2 || (nbrLED>lim_maxi_bp2 && digitalRead(bp2)== 0)){
        /*
        //on fait clignoté 3 fois, un coter rouger et l'autre vert pour montré qui a gagné
        for(int i=0;i<3;i++){
          for(int j=0;j<NUM_LEDS/2;j++){
            leds[(NUM_LEDS/2)+j]=CRGB(6,0,0);
            leds[((NUM_LEDS/2)-1)-j]=CRGB(0,6,0);
          }
          FastLED.show();
          delay(500);
          for(int j=0;j<NUM_LEDS/2;j++){
            leds[(NUM_LEDS/2)+j]=CRGB(0,0,0);
            leds[((NUM_LEDS/2)-1)-j]=CRGB(0,0,0);
          }
          FastLED.show();
          delay(500);
        }

        FIN = true;
        */
        //r =  random(1,10);
        type_speed = "medium";
        sens = !sens;
        powerup2 = powerup2 +1;
        break;
      }
    }
    //r =  random(1,10);
  }
}


void trap() {
  /*
   Si le joueur décide de lancer la balle à vitesse max, alors il 
   y a une probabilité qu'un piège se lance.
  */ 
  if (nbrLED>=(NUM_LEDS/2-10) && nbrLED<=(NUM_LEDS/2+10) && nbrLED == r_led && r>=5) {
    Serial.println("Trigger trap");
    trig_trap = true;  
  }
}

void reset_trap(){
  r_led = random((NUM_LEDS/2-20),(NUM_LEDS/2+20));
  Serial.print("r_led=");Serial.println(r_led);
  r = random(1,10);
  //Serial.print("R=");Serial.println(r);
  if(trig_trap){
    trig_trap = false;  
  }
}

void limites(){
  leds[lim_mini_bp1]=CHSV(32,255,125);
  leds[lim_maxi_bp1]=CHSV(32,255,125);
  leds[lim_mini_bp2]=CHSV(32,255,125);
  leds[lim_maxi_bp2]=CHSV(32,255,125);
  showPowerUP();
}
void transition(){
  int fadeColor;
  fadeColor = random(0,255);
  leds[(NUM_LEDS/2)-1]=CHSV(fadeColor,255,150);
  leds[NUM_LEDS/2]=CHSV(fadeColor,255,150);
 
  for (int i = 1; i<((NUM_LEDS-1)/2); i++){
    leds.fadeToBlackBy(40);
    leds[((NUM_LEDS/2)-2)-i]=CHSV (fadeColor,255,60); //CRGB(75,164,0); orange
    leds[((NUM_LEDS/2)+1)+i]=CHSV (fadeColor,255,60); //CRGB(75,164,0); orange
    FastLED.delay(15);
    if( digitalRead(bp1) == 0 && digitalRead(bp2) == 0) break;
  }
  delay(15);
}



void showPowerUP(){
    // pour Bp1
    for(int x=0; x<powerup1; x++){
      leds[lim_maxi_bp1-x-1]=CHSV(0,0,100);
    }
    // pour Bp2
    for(int x=0; x<powerup2; x++){
      leds[lim_maxi_bp2+x+1]=CHSV(0,0,100);
    }
}

/**
  Test Zone
**/
/*
void test_zone(){
  leds.fadeToBlackBy(180);
  for (nbrLED=0; nbrLED<=NUM_LEDS+1;nbrLED+=1){
    if (nbrLED>=(NUM_LEDS/2-10) && nbrLED<=(NUM_LEDS/2+10)) {
      leds[nbrLED] = CRGB(60,0,0);      
    }
  } 
  FastLED.delay(10);
}
*/
/*
  Définir 2 variable INT powerup1 et powerup2

  - A chaque échange à pleine vitesse, on augmente powerup de 1.
  - si on réussi à avoir une série de 4 powerup alors on obtient le power.
  - si powerup est inférieur à 4 et qu'on met un coup à une vitesse inférieur, 
    alors on perd nos stack de powerup.
  - pour chaque stack, il faut alumé une led d'une couleurs différentes.
*/


//test des palier de vitesse
/*
void test_pal_vitess (){
  leds.fadeToBlackBy(180);
  for (nbrLED=0; nbrLED<=NUM_LEDS+1;nbrLED+=1){
    if(nbrLED>=lim_maxi_bp1 && nbrLED<(lim_maxi_bp1+lim/3)) {
      leds[nbrLED] = CRGB(60,0,0);
      Serial.print("LEd nbr = ");Serial.print(nbrLED);Serial.print("// couleur = ");
      Serial.println("bleu");
    }else if(nbrLED<lim_mini_bp1-(lim/3) && nbrLED>=lim_maxi_bp1+(lim/3)){
      leds[nbrLED] = CRGB(0,60,0);
      Serial.print("LEd nbr = ");Serial.print(nbrLED);Serial.print("// couleur = ");
      Serial.println("Vert");
    } else if(nbrLED<=lim_mini_bp1 && nbrLED>=lim_mini_bp1-(lim/3)){
      leds[nbrLED] = CRGB(0,0,60);
      Serial.print("LEd nbr = ");Serial.print(nbrLED);Serial.print("// couleur = ");
      Serial.println("Rouge");
    } 

  

    if(nbrLED<lim_maxi_bp2 && nbrLED>=lim_maxi_bp2-(lim/3)) {
      leds[nbrLED] = CRGB(60,0,0);
      Serial.print("LEd nbr = ");Serial.print(nbrLED);Serial.print("// couleur = ");
      Serial.println("bleu");
    }else if(nbrLED>=lim_mini_bp2+(lim/3) && nbrLED<lim_maxi_bp2-(lim/3)){
      leds[nbrLED] = CRGB(0,60,0);
      Serial.print("LEd nbr = ");Serial.print(nbrLED);Serial.print("// couleur = ");
      Serial.println("Vert");
    } else if(nbrLED>=lim_mini_bp2 && nbrLED<lim_mini_bp2+(lim/3)){
      leds[nbrLED] = CRGB(0,0,60);
      Serial.print("LEd nbr = ");Serial.print(nbrLED);Serial.print("// couleur = ");
      Serial.println("Rouge");
    }


    delay(50);
    FastLED.delay(10);
  }
  //leds[14] = CRGB(60,60,60);
  //FastLED.delay(60);
  state = false;
}
*/
