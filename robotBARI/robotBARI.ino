#include <NewPing.h>
#include <LineFinder.h>
#include <PID.h>
#include <Servo.h>
LineFinder line;
PID pidous;
Servo bigservo;
Servo smallservo;

#define TIPOSTAZIONE 1 //1=ingresso lato corto, 2=ingresso lato lungo



void swap(int &_1,int &_2) {
  int temp=_1;
  _1=_2;
  _2=temp;
}



/*************************************/
//       SETTAGGIO ULTRASUONI        //
/*************************************/
#define MAX_DISTANCE 200  //Distanza massima rilevabile dal sensore di ultrasuoni
#define TRIGGER_PIN  42
#define ECHO_PIN     43
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

#define TRIGGER_PINBACK  44
#define ECHO_PINBACK     45
NewPing backsonar(TRIGGER_PINBACK, ECHO_PINBACK, MAX_DISTANCE);
/**************************************/



/*************************************/
//           ULTIMA STANZA           //
/*************************************/
int alto,basso,destra,sinistra;   //4 variabili in cui saranno segnati gli angoli che il robot formerà posizionandosi parallelo a u
int posizionetriangolo=0;
int posiniziale;
int centroperfetto;
/*************************************/



/************************************/
//             VERDE                //
/************************************/
const float costanteverde1 = 0.4;
const float costanteverde2 = 0.4;

const byte sclPin = A7;  // pin scl                 I PIN NON VANNO CAMBIATI
const byte sdaPin = A4;  // pin sda

const byte sclPin2 = A5;  // pin scl
const byte sdaPin2 = A6;  // pin sda

bool verde1 = false;                  //1 SX, 2 DX
bool verde2 = false;
/************************************/

/*************************************/
//             COSTANTI              //
/*************************************/
#define MAXWHITE 400                  //valore massimo con cui considerare il bianco
#define MINBLACK 700                  //valore minimo con cui considerare il nero
#define _90 500                       //tempo in cui il robot gira di 90 gradi !!!!ATTENZIONE!!!! Modificare questo valore in base all'attrito !!!!ATTENZIONE!!!!
#define distgap 355                  //una costante che rappresenta il tempo in cui i motori fanno 20 cm, e quindi aver superato la massima lunghezza di un gap
#define MINTIME 10                   //il tempo impiegato per superare la distanza della larghezza della linea
#define postime 3000                 //tempo di posizionamento al muro
/*************************************/



/*************************************/
//        VARIABILI GLOBALI          //
/*************************************/
bool dgap=false;     //Booleana che afferma se abbiamo già fatto un gap
bool flag=false;     //Booleana che afferma la presenza o no della linea
int f_speed=50;      //Velocità di forward
int d_speed=110;      //Velocita' di deviazione il quale valore e' poi alterato proporzionalmente all'errore in setspeeds() (corrisponde alla massima)
int v_min=2;
int maxValue=5000;   //Valore massimo che i sensori di luce possono leggere
int dist;            //Distanza rilevata dal sensore di ultrasuoni posto sulla parte anteriore
int divSPM;          //Divisore della velocita' dei motori perno (Divider Speed Pivot Motors) - definito in setspeeds() e utilizzato per le deviazioni proporzionali
int cases,pidVal;
float error;
int conGreen;        //Controllo sul verde
int previouserror=0;
int timef,time;

int t_servo = 1000;

char chr;     //Per la IMU
int o;
int v,osum, lasto = 0, partsum = 0, oprecedente;

int girotondo = 2400;
/*************************************/

/*************************************/
//         VETTORI GLOBALI           //
/*************************************/
int _pins[]={26,27,28,29,30,31,32,33}; //Pin a cui sono attaccatti i sensori di luce
long times[8];    //Vettore al cui interno verranno salvati i valori letti dai sensori di luce
long minvals[8]={240,192,240,240,240,284,288,348};
long maxvals[8]={1208,884,884,936,824,992,1156,1524};
float rap[8];
/*************************************/


void lslinea() {          //Questa funzione riempie il vettore times[] con i valori ricavati dai sensori di linea
    int i;
    for(i=0;i<8;i++)
    {
        times[i]=maxValue;
        pinMode(_pins[i], OUTPUT);
        digitalWrite(_pins[i], HIGH);
    }
    delayMicroseconds(10);
    for(i=0;i<8;i++)
    {
        pinMode(_pins[i], INPUT);
        digitalWrite(_pins[i], LOW);
    }
    unsigned long startTime = micros();
    while (micros()-startTime<maxValue)
    {
        unsigned int time= micros()-startTime;
        for (i=0;i<8;i++)
        {
            if (digitalRead(_pins[i])==LOW && time<times[i])
                times[i] = time;
        }
    }
}



bool isostacolo(){               //Verifica che ciò che ha visto sia effettivamente un ostacolo e non un bumper/rampa
  sprint();
  delay(500);
  setspeeds(-50,-50);
  delay(10);
  ferma();
  if(sonar.ping_cm()<5 && sonar.ping_cm()!=0)
    return true;
  return false;
}




void rotateto(int angolo) {
  leggiNANO();
  if(angolo>=0 && o>=0){
      rotate(angolo-o);
    }

    else if(angolo<=0 && o<=0){
      rotate(angolo-o);
    }

    else if(angolo<=0 && o>=0){
      if(180-o+180+angolo<-angolo+o)
        rotate(180-o+abs(180+angolo));
      else
        rotate(-o-abs(angolo));
    }
    else if(angolo>=0 && o<=0){
      if(180+o+180-angolo<angolo-o)
        rotate(-abs(180+o)-180+angolo);
      else
        rotate(abs(o)+angolo);
    }
    ferma();
}


void rotate(int deg){
  leggiNANO();
  int tolleranza = 12;
  int angolofinale = osum + deg;
  if (deg > 0) {
    setspeeds(130, -130);
    while (osum < angolofinale - tolleranza)
      leggiNANO();
    //mot.setSpeeds(20, -20);
    //delay(100);
    ferma();
  }
  else {
    setspeeds(-130, +130);
    while (osum > angolofinale + tolleranza)
      leggiNANO();
    //mot.setSpeeds(-20, 20);
    //delay(100);
    ferma();
  }
}
/*******************************************************************************************************************************
*setspeeds(sensore che vede di piu' la linea)
*******************************************************************************************************************************/

void setspeeds(int v1,int v2) {
    int v1_abs=abs(v1);
    int v2_abs=abs(v2);
    if(v1_abs>255) v1_abs=255;
    if(v2_abs>255) v2_abs=255;
    if(v2<0){
      analogWrite(4,v2_abs/*/divSPM0*/);
      analogWrite(7,0);
    }
    else{
      analogWrite(4,0);
      analogWrite(7,v2_abs/*/divSPM0*/);
    }

    if(v1>0){
      analogWrite(6,0);
      analogWrite(5,v1_abs);
    }
    else{
      analogWrite(6,v1_abs);
      analogWrite(5,0);
    }

}


void left90() {                      //Ruota il robot di 90 gradi verso sinistra
    int meta, delta_o, vel;
    leggiNANO();
    if(o>-90)       //meta non sara' mai -180
      meta=o-90;
    else
      meta=270+o;
    rotateto(meta);
}

void right90() {                     
    int meta;                                         //Decide quale deve essere l'angolo META da raggiungere basandosi sulla IMU
    leggiNANO();
    if(o<=90)       //al massimo meta puo' essere 180 (positivo, altrimenti -180)
      meta=o+90;
    else
      meta=-270+o;
    rotateto(meta);
}

void sprint() {
  setspeeds(100+f_speed,100+f_speed);
}

void ferma() {
  setspeeds(0,0);
}

void ostacolo(){                              //Funzione che si occupa di gestire gli ostacoli avendo solo un sensore di ultrasuoni
  ferma();                              //Si ferma per controllare accuratamente l'angolo
  rotate(-30);
  setspeeds(f_speed+30,f_speed+30);
  delay(750);                                 //DA SOSTITUIRE QUANDO AVREMO IL SENSORE DI ULTRASUONI LATERALE
  ferma();
  rotate(30);
  setspeeds(f_speed+30,f_speed+30);
  delay(600);
  rotate(90);
  setspeeds(f_speed+30,f_speed+30);
  delay(100);
  lslinea();
  line.FindError(times,rap,&cases);
  while(cases==1) {
    lslinea();
    line.FindError(times,rap,&cases);
  }
  rotate(-90);
}

void verde(){   //1 SX, 2 DX

  int meta;
  leggiNANO();
  if(verde2){         //RIGHT

    if(o<=150)       //al massimo meta puo' essere 180 (positivo, altrimenti -180)
      meta=o+30;
    else
      meta=-330+o;  /////////////////////////////-180+30-(180-o)
    //rotateto(meta);
    clock();
    while(o!=meta)  //o!=meta     FORSE E' MEGLIO UN RANGE!!
      leggiNANO();
    ferma();
  }

  else{                //LEFT
    if(o>-150)       //meta non sara' mai -180
      meta=o-30;
    else
      meta=330+o;     //////////////////////////180-(30-(o+180))
    //rotateto(meta);
    counterclock();
    while(o!=meta)
      leggiNANO();
    ferma();
  }
}

int posizionati(int deg) {
  leggiNANO();
  int gradi = deg - o;
  if (gradi > 180)
    gradi = gradi - 360;
  else if (gradi < -180)
    gradi = gradi + 360;
  if (gradi != 0) {
    Serial.print("Mi trovo a gradi ");
    Serial.print(o);
    Serial.print(" e voglio andare a gradi ");
    Serial.print(deg);
    Serial.print(" quindi ruoto di ");
    Serial.print(gradi);
    Serial.println(" gradi");
    rotate(gradi);
  }

}

/******************************************
*                  DEBUG                  *
******************************************/

void DEBUG_SEGUILINEA(){
  while(true) {
    for(int i=0; i<8; i++){
      Serial.print(times[i]);
      Serial.print("\t");
    }
    Serial.println();
    lslinea();
  }
}
//////////////////////////////////////////


void setup() {
   pinMode(4,OUTPUT);     //settaggio dei pin dei motori
   pinMode(5,OUTPUT);
   pinMode(6,OUTPUT);
   pinMode(7,OUTPUT);
   pinMode(13,OUTPUT);
   pinMode(38,OUTPUT);
   pinMode(21,OUTPUT);
   //inizializzazione degli i2c del VERDE
   init_TCS34725();
   init_TCS347252();

   //servi
   bigservo.attach(21);
   smallservo.attach(20);
   bigservo.write(130);
   smallservo.write(10);

   line.init(minvals,maxvals);
   pidous.init(55,10,0);
   delay(200);
   Serial.begin(9600);
   delay(200);
   Serial2.begin(9600);////NANO
}



void loop() {      
    leggiNANO();    
    dist=sonar.ping_cm();
    if (dist<5 && dist!=0) {
      if(isostacolo()){
        setspeeds(-80, -80);
        delay(200);
        ostacolo();
      }
    }
    lslinea();
    error=line.FindError(times,rap,&cases);

    
    //Serial.print(cases);
    //Serial.print("\t");
    //Serial.println(error);
    if(cases==0){                             //c'e' la linea
      pidVal=pidous.Compute(error);
      Serial.println(pidVal);
      else {
        setspeeds(f_speed+pidVal,f_speed-pidVal);
      }
    }
    else if(cases==1){                        //tutto bianco
      int tempomax=millis()+800;
      setspeeds(f_speed+100,f_speed+100);
      do{
      lslinea();
      error=line.FindError(times,rap,&cases);
      }
      while(cases==1 && millis()<tempomax);   
      /*******************************************************/
      //                  ULTIMA STANZA                      //
      /*******************************************************/
      if (cases==1){
        ultimastanza(TIPOSTAZIONE);
      }
      /*******************************************************/
    }
    else{                                     //tutto nero
      verde1=false;
      verde2=false;
      setspeeds(0,0);
      if(conGreen<5){
          for(int i=0;i<50 && !verde1 && !verde2;i++) {
            setspeeds(-30,-30);
            get_Colors();
            get_Colors2();
          }
          conGreen++;
          if(verde1||verde2) {                   //se hai visto il verde ...
            setspeeds(f_speed,f_speed);
            delay(50);
            verde();
          }
        }
      else {                                //altrimenti vai dritto
        setspeeds(f_speed,f_speed);
        delay(MINTIME);
        conGreen=0;
      }


    }

}
