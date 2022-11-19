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
int lim_mini_bp1 = NUM_LEDS-1;
// Limite maxi bp1
int lim_maxi_bp1 = NUM_LEDS-10;
// Limite mini bp2
int lim_mini_bp2 = 0;
// Limite maxi bp2
int lim_maxi_bp2 = 10;

// Type de vitesse
String type_speed;

// Vitesse de déplacemen de la led / delay d'execution
int vitesse_high   = 0;
int vitesse_medium = 10;
int vitesse_low    = 20;

// Couleur 
CHSV bleu    = CHSV(240,255,150); 
CHSV rouge   = CHSV(340,255,150);
CHSV orange  = CHSV(30,255,150);

//test
boolean state = true;

void setup() {
  // N'est utile que pour du débug 
  Serial.begin(115200);
  // Définit le bouton du J1
  pinMode(bp1, INPUT_PULLUP);
  // Définit le bouton du J2
  pinMode(bp2, INPUT_PULLUP);
  //Setup des leds
  LEDS.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
}

void loop() {
  //debug 
  if (state)test_pal_vitess();
  /*
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
 */
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
    for(nbrLED ; nbrLED <= NUM_LEDS ; nbrLED+=1){
      limites();
      // Ajoute un délai avant de repassé à sombre
      leds.fadeToBlackBy(180);
      
      if(type_speed == "high"){
        //Affiche la couleur
        leds[nbrLED] = rouge;
        // Vitesse d'actualisation
        FastLED.delay(vitesse_high);
      } else if (type_speed == "low") {
        //Affiche la couleur
        leds[nbrLED] = orange;
        // Vitesse d'actualisation
        FastLED.delay(vitesse_medium);
      } else {
        //Affiche la couleur
        leds[nbrLED] = bleu;
        // Vitesse d'actualisation
        FastLED.delay(vitesse_low);
      }
      
      //-- Inversion de sens --//
      if(nbrLED<lim_mini_bp1 && nbrLED>=lim_maxi_bp1  && digitalRead(bp1) == 0){
        if(nbrLED<=lim_mini_bp1-1 && nbrLED<=lim_mini_bp1-4){
          type_speed = "high";
        } else if (nbrLED<lim_mini_bp1-4 && nbrLED<=lim_mini_bp1-7){
          type_speed = "medium";
        } else {
          type_speed = "low";
        }
        sens = !sens;
        break;
      }

      //-- en cas d'erreur : si on attteint la "limite_score" qui est la dernière led, ou si on appuie sur le bp avant d'étre dans la zone de renvoie, alors on affiche qui à gagner en vert et perdant en rouge
      if(nbrLED >=lim_mini_bp1 || (nbrLED<lim_maxi_bp1 && digitalRead(bp1)==0)){
        Serial.println("BP2 FIN");
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
  }
  
  
  //de bp1 à bp2
  else if(sens == false && starter == true){
    for (nbrLED; nbrLED >= lim_mini_bp2; nbrLED-=1){
      leds.fadeToBlackBy(180);
      limites();

      if(type_speed == "high"){
        //Affiche la couleur
        leds[nbrLED] = rouge;
        // Vitesse d'actualisation
        FastLED.delay(vitesse_high);
      } else if (type_speed == "low") {
        //Affiche la couleur
        leds[nbrLED] = orange;
        // Vitesse d'actualisation
        FastLED.delay(vitesse_medium);
      } else {
        //Affiche la couleur
        leds[nbrLED] = bleu;
        // Vitesse d'actualisation
        FastLED.delay(vitesse_low);
      }

      //-- Inversion de sens --//
      if(nbrLED<=lim_maxi_bp2 && nbrLED>lim_mini_bp2 && digitalRead(bp2) == 0){
        if(nbrLED<=lim_mini_bp2+1 && nbrLED<=lim_mini_bp2+4){
          type_speed = "high";
        } else if (nbrLED<lim_mini_bp2+4 && nbrLED<=lim_mini_bp2+7){
          type_speed = "medium";
        } else {
          type_speed = "low";
        }
        sens = !sens;
        break; 
      }

      //-- En cas d'erreur: limite atteinte ou pression trop tôt --//
      if(nbrLED <= lim_mini_bp2 || (nbrLED>lim_maxi_bp2 && digitalRead(bp2)== 0)){
        Serial.println("BP1 FIN");
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
        leds[NUM_LEDS-1]= CRGB(60,0,0);
        leds[0]= CRGB(0,60,0);
        FastLED.show();
        FIN = true;
        break;
      }
    }
  }
}


void limites(){
  leds[lim_mini_bp1]=CHSV(32,255,125);
  leds[lim_maxi_bp1]=CHSV(32,255,125);
  leds[lim_mini_bp2]=CHSV(32,255,125);
  leds[lim_maxi_bp2]=CHSV(32,255,125);
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


//test des palier de vitesse
void test_pal_vitess (){
  for (nbrLED=0; nbrLED<=NUM_LEDS+1;nbrLED+=1){
    if(nbrLED>=lim_maxi_bp1 && nbrLED<=lim_mini_bp1-7) {
      leds[nbrLED] = CRGB(60,0,0);
      Serial.print("LEd nbr = ");Serial.print(nbrLED);Serial.print("// couleur = ");
      Serial.println("bleu");
    }else if(nbrLED>lim_mini_bp1-7 && nbrLED<=lim_mini_bp1-4){
      leds[nbrLED] = CRGB(0,60,0);
      Serial.print("LEd nbr = ");Serial.print(nbrLED);Serial.print("// couleur = ");
      Serial.println("Vert");
    } else if(nbrLED<lim_mini_bp1 && nbrLED<lim_mini_bp1-4){
      leds[nbrLED] = CRGB(0,0,60);
      Serial.print("LEd nbr = ");Serial.print(nbrLED);Serial.print("// couleur = ");
      Serial.println("Rouge");
    } 
    /*
    else {
      leds[nbrLED] = CRGB(10,60,60);  
    }
    */
    //FastLED.show();
    FastLED.delay(15);
  }
  leds[14] = CRGB(60,60,60);
  FastLED.delay(15);
  state = false;
}

//debug//
/*
Serial.print("nbrLED = ");Serial.println(nbrLED);
Serial.print("Etat bp1 = ");Serial.println(digitalRead(bp1));
Serial.print("lim_mini_bp1 = ");Serial.println(lim_mini_bp1);
Serial.print("lim_maxi_bp1 = ");Serial.println(lim_maxi_bp1);
*/
//Pour debug
/*
void checkbp(){
  if (digitalRead(bp1) == 0){
    Serial.println("BP1");
  }
  if (digitalRead(bp2) == 0){
    Serial.println("BP2");
  } 
}
*/
/*
De Bp2 à BP1
void trap(){
  //-- Piège à vitesse max --//
  if(vitesse <=vitesse_max){
    randomLED = random(limite_score,NUM_LEDS-limite_score);
    if(r==0 || r==2){
      if(randomLED == nbrLED ){
        leds[nbrLED]=CHSV(160,255,150);
        delay(300); 
      }
    }
    if(r==6|| r==4){
      if(nbrLED<=45 && nbrLED>=43){
        sens=!sens;
        break;
      }
    }
  }  
}
*/



/*
De Bp2 à BP1
    //-- Piège à vitesse max --//
    if(vitesse <=vitesse_max){
      randomLED = random(limite_score,NUM_LEDS-limite_score);
      if(r==0 || r==2){
        if(randomLED == nbrLED ){
        leds[nbrLED]=CHSV(160,255,150);
        delay(300); 
        }
      }
      if(r==4 || r==6){
        if(nbrLED<=(NUM_LEDS/2)+5 && nbrLED>=(NUM_LEDS/2)-5){
          sens=!sens;
          delay(500);
          break;
        }
      }
    }
*/
