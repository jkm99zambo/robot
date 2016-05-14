/*
 * La funzione fermo() è uguale al mot.Off() ma con un delay dopo, in modo tale da evitare i movimenti per inerzia
 */

void fermo() {
  ferma();
  delay(100);
}


int dim_letture_us=10;
int letture_us[10];
float pesi[10]={5.5,6,6.5,7,7.5,8,8.5,9,9.5,10}; //Pesi empirici presi da Pallaoro e suggeriti da Ballerin

float sommapesi=77.5; //Semplicemente la somma dei pesi, per evitare conti all'arduino
float multime2=0;

bool vai_al_muro=0;

float media_palla(int x)//la funzione per vedere le palle coi sensori senza usare la media vecchia ma questa
{ /*
  definiamo due termini: 
  la lettura "più recente" è l'ultima lettura eseguita delle dieci tenute in memoria, e sta nel vettore all' indice x
  la lettura "presa in esame" è la lettura che sto prendendo in considerazione, e che è tanto più antica quanto "ipesi" è piccolo

  ora
  sto bordello fa: parte dalla lettura più antica e le assegna un peso piccolo nel calcolo della media
                  poi prende le più recenti e le assegna un peso sempre più grande. (secondo i pesi assegnati nel vettore)
  */
float m=0;
multime2=0;
float peso=0;
int ilett = (x+1)%dim_letture_us;
int ipesi=0;
while(ilett != x){
      m = m +(letture_us[ilett] * pesi[ipesi]);
  
      if(ipesi>=8)
        multime2+=letture_us[ilett];
      ilett=(ilett+1)%dim_letture_us;
      ipesi++;
}
     //assert (ilett == x && ipesi ==9)
     m = m + (letture_us[x] * pesi[9]);
     multime2+=letture_us[x];
     multime2 = multime2 / 2;
      
return m/sommapesi; 
}

void inizializza_media(int val){
  for(int i=0; i<dim_letture_us; i++)
    {
      letture_us[i] = val;
    }
}

/*
 ██████╗███████╗██████╗  ██████╗ █████╗ ██████╗  █████╗ ██╗     ██╗      █████╗ 
██╔════╝██╔════╝██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔══██╗██║     ██║     ██╔══██╗
██║     █████╗  ██████╔╝██║     ███████║██████╔╝███████║██║     ██║     ███████║
██║     ██╔══╝  ██╔══██╗██║     ██╔══██║██╔═══╝ ██╔══██║██║     ██║     ██╔══██║
╚██████╗███████╗██║  ██║╚██████╗██║  ██║██║     ██║  ██║███████╗███████╗██║  ██║
 ╚═════╝╚══════╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝
*/
long cercapalla(int n) {
  int centroperfetto_edit=centroperfetto;
  if(n==2){
    centroperfetto_edit=centroperfetto-27;
  }
  else if(n==1){
    centroperfetto_edit=centroperfetto+27;
  }
  
  //Dichiaro limiti settore
  int settore[4];
  settore[0] = 40;
  settore[1] = 40;
  settore[2] = 40;
  settore[3] = 40;
  
  if(n==2){
    settore[0]=40;
    settore[3]=20;
  }
  else{
    settore[0]=20;
    settore[3]=40;
  }
  
  int nspazzolata=0; //Serve per contare il numero di spazzolate
  bool volta = 1; //Booleano per la larghezza della spazzolata (1 se è la prima, 0 se è oltre)
  fermo();


  int trovatopalla=0;

  float max_diff;
  
  cercapalla:;  // Inizio del "goto" cercapalla, dove cerca la palla a settori, a meno che non sia appena entrato
  /*nspazzolata++;
  if(nspazzolata>4 && n==0){
      posizionati(centroperfetto);
      setspeeds(-100,-100);
      while(backsonar.ping_cm()>=30){
        delay(50);
      }
      fermo();
      //ho_la_palla=0;
      
      rotate(180);
      
      asta.attach(asta_pin);
      asta.write(140);
      delay(500);
      pinza_attach();
      setPinza(12);
      return 0;
  }
  else if(nspazzolata>4){
      if(n==1){
        vai_al_muro=1;
        return 0;
      }
      else if(n==2){
        vai_al_muro=1;
        return 0;
      }
  }*/
  int media;
  time = millis();
  setspeeds(-100, 100);
  media = sonar.ping_cm();
  int distanza_pall;

  int idx=0;
  bool ever_read = false;

  long tempo_last_palla;
  int dist_last_palla;
  int limite;
  
  
  ///
  ///
  /// QUI INIZIO A CERCARE LA PALLA PER LA PRIMA VOLTA
  ///
  ///
  ///leggiNANO();
  /// int yawsum_iniziogiro = yawsum;
  if(n==0)
    max_diff=5;
  else
    max_diff=7.6;
  leggiNANO();
  int temp_osum=osum;
  //while (millis() - time <= (girotondo * 2 + volta * 900)) {
  while(temp_osum - 180 - volta*10 <= osum){

    if(n==0 && temp_osum-55 > osum)
      max_diff=7.6;
    
    delay(50);
    int sensore = sonar.ping_cm();
    if(!ever_read){
        inizializza_media(sensore); //è solo un for che inizializza tutti i valori di letture_us a sensore
        ever_read=true;
    }
    //Divisione in settori

    if (n == 1) {
      limite = settore[0];
      if (millis() - time > girotondo / 2)
        limite = settore[1];
      if (millis() - time > girotondo)
        limite = settore[2];
      if (millis() - time > girotondo * 1.5)
        limite = settore[3];
    }
    else if (n == 2) {
      limite = settore[3];
      if (millis() - time > girotondo / 2)
        limite = settore[2];
      if (millis() - time > girotondo)
        limite = settore[1];
      if (millis() - time > girotondo * 1.5)
        limite = settore[0];
    }
    else
      limite = 38;

    //Media per vedere se è stata trovata la pallina
    letture_us[idx]=sensore;
    media_palla(idx);
    Serial.print("Sensore: ");
    Serial.print(sensore);
    Serial.print(", media2: ");
    Serial.print(multime2);
    Serial.print(", mediaTOT: ");
    Serial.println(media_palla(idx));
    leggiNANO();
    
    if(sensore < limite && media_palla(idx) - multime2 > max_diff){ //CONTROLLA SE LA "MEDIA" E' TALE DA VEDER PALLE
        /*
        delay(1200); ///vai avanti a vedere se per caso dopo la palla c'era qualcosa
        mot.Off();
        if(ultra.Forward() > distanza_pall + 4){ //: se dopo la palla la distanza aumenta allora era una palla e non un muro
          mot.setSpeeds(50,-50);
          delay(1200);
          mot.Off();
          break; 
        }
        */
        Serial.print("TROVATA PALLA a ");
        Serial.print(sensore);
        Serial.print(" cm");
        distanza_pall = sensore;
        delay(200);
        trovatopalla=1;
        break;
    }
    else {
        idx=(idx+1)%dim_letture_us; 
    }
    /*
    if(millis() - time <= girotondo * 1.5){
      limite-=5;
    }
    */
  }
  ///
  ///
  /// E QUI TERMINO DI CERCARLA
  /// Quindi adesso devo controllare se effettivamente l'ho trovata o sono andato fuori limite massimo, finendo quindi lo spazio in cui posso cercarla
  ///
  ///
  
  volta = 0;
  delay(200);
  
  //if (millis() - time > girotondo * 2 + volta * 900) {
  if(!trovatopalla){
        Serial.println("Non ho trovato la pallina, rispazzolo"); //Mi riposiziono e ricomincio la spazzolata
        Serial.print("#mainagioia, questa era la spazzolata n° ");
        Serial.println(nspazzolata);
        //Cambio
        
        if(n==2){
          settore[0] = settore[0] - 5;
          settore[3] += 5;
        }
        else if(n==1){
          settore[0] += 5;
          settore[3] = settore[3]- 5;
        }
        
        posizionati(centroperfetto_edit);
        setspeeds(100, 100);
        delay(400);
        fermo();
        rotate(100);
        fermo();
        goto cercapalla;
  }
  
  ferma();
  long tempogiro = millis() - time;
  delay(500);

  //pallina trovata
  Serial.println("mi avvicino alla pallina, con le distanze computate a ca");
  if (distanza_pall > 40)
    distanza_pall = 40;
  else if (distanza_pall < 10 && distanza_pall > 5)
    distanza_pall -= 3;
  else if (distanza_pall <= 5)
    distanza_pall = 0;

  distanza_pall = distanza_pall * 115 - 500;
  if (distanza_pall < 0)
    distanza_pall = 0;
  setspeeds(-100, -100);
  delay(200);
  fermo();
  rotate(180);
  delay(100);

  //posiziona pinza
/*
  asta.attach(asta_pin);
  asta.write(140);
  delay(500);
  pinza_attach();
  setPinza(55);
  delay(500);
*/
  /*
   * PATCH DI CONTROLLO
   * 
   * Andiamo a prendere la pallina
   * 
   * Ma prima di andare mi segno l'angolo a cui mi trovo, per riposizionarmici prima di tornare in posizione
   * Questo perché potrei aver letto una finta pallina, ma con una pallina buona vicina, quindi mi avvicino, becco la pallina, e poi però, dato che la
   * pallina era finta mi trovo ad un angolo diverso, che dipende dalla pallina vera che mi sono trovato vicino.
   * Per evitare di tornare indietro a cazzo supponendo di aver trovato la pallina che stavo puntando inizialmente ma che in realtà era falsa, mi riposiziono all'angolo
   * precedente alla "caccia"
   */
   
  leggiNANO();
  int angolo_di_salvezza=o;
  
  setspeeds(-100, -100);
  delay(distanza_pall /4.1); //1.8 Trovato in modo empirico
  ferma();
  setspeeds(100, -100);
  delay(1000);
  setspeeds(-100, 100); //era (-70, 70)
  int distanza_pall_culo;
  media = backsonar.ping_cm();
  
  /*
   * PATCH DI SICUREZZA
   * 
   * Ce ne siamo sempre sbattuti il cazzo, ma se per caso la pallina era finta e non ce n'è nessuna di vera nelle vicinanze, meglio evitare un loop infinito 
   * Si potrebbe anche chiamare il lack, però meglio dare la possibilità di scelta in gara
   * 
   */
  Serial.println("POSIZIONATo, inizio la ricerca di culo");
  long quantomigiro=millis();
  while (millis()-quantomigiro < girotondo*6) {
    int sensore = backsonar.ping_cm();
    if (sensore < 19 && media - sensore > 2) {
      distanza_pall_culo = sensore;
      Serial.println("penso di averla trovata col culo");
      break;
    }
    else
      media = (media * 5 + sensore * 3)/8;
    delay(50);
  }

  if(millis()-quantomigiro >=girotondo*6){ //Cazzooooooo LOOP INFINITO!!!! (Accade di rado);
    fermo();
    Serial.println("NO LA HABE TROVATA, HO GIRATO TROPPO");
    for(int i=0;i<2;i++){
      delay(200);
    }
    posizionati(angolo_di_salvezza);
    /*asta.write(140);
    delay(500);
    setPinza(180);
    delay(500);
    asta.write(120);
    delay(500);
    asta.detach();
    pinza_detach();*/
    /*setspeeds(100,100);
    delay(distanza_pall/1.8);*/
    fermo();
    posizionati(centroperfetto_edit);
    setspeeds(100, 100);
    delay(1000);
    fermo();
    rotate(100);
    fermo();
    goto cercapalla;
  }
  
  Serial.println("vado a prendere la palla col culo");
  
  //Creo la variabile "quantovadoavanti" per sapere di quanto sono andato avanti dopo la metà di "distanza_pall", nel caso avessi mosso una pallina
  //In questo modo mi assicuro di tornare al punto di partenza
  
  long quantovadoavanti = millis();
  while (true) {
    if (backsonar.ping_cm() < 13)
      break;
    else
      delay(50);
  }
  delay(20); //ruota leggermente di più dato che vede la pallina presto
  fermo();
  quantovadoavanti = millis() - quantovadoavanti;
  /*asta.write(90);
  delay(300);
  setPinza(12);
  delay(500);
  asta.write(140);
  delay(500);*/
  prendipalla();
  
  /* PATCH DI CONTROLLO parte 2
   * 
   * Pallina presa, torna al posto
   * Però, come dicevo prima, meglio riposizionarsi all'angolo di rilevata pallina iniziale
   */
  
  posizionati(angolo_di_salvezza);
  
  setspeeds(100, 100);
  //Controllo per vedere se effettivamente ho spinto la pallina e applico la correzione
  
  if (quantovadoavanti > 800) //Se sono andato avanti tanto dopo la metà, allora è meglio dirlo aggiungendo un po' al distanza_pall, altrimenti posso anche lasciare stare
    distanza_pall += (quantovadoavanti - 500) * 0.8;
  
  //Fine del controllo
  delay(distanza_pall);
  
  fermo();
  posizionati(centroperfetto_edit + 180);

}

/*
██╗   ██╗██╗  ████████╗██╗███╗   ███╗ █████╗ ███████╗████████╗ █████╗ ███╗   ██╗███████╗ █████╗ 
██║   ██║██║  ╚══██╔══╝██║████╗ ████║██╔══██╗██╔════╝╚══██╔══╝██╔══██╗████╗  ██║╚══███╔╝██╔══██╗
██║   ██║██║     ██║   ██║██╔████╔██║███████║███████╗   ██║   ███████║██╔██╗ ██║  ███╔╝ ███████║
██║   ██║██║     ██║   ██║██║╚██╔╝██║██╔══██║╚════██║   ██║   ██╔══██║██║╚██╗██║ ███╔╝  ██╔══██║
╚██████╔╝███████╗██║   ██║██║ ╚═╝ ██║██║  ██║███████║   ██║   ██║  ██║██║ ╚████║███████╗██║  ██║
 ╚═════╝ ╚══════╝╚═╝   ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝
*/

void ultimastanza(int n) {
  int distanza_pall;
  int check = 0;
  //ingresso e posizionamento al centro

  if (n == 1) { //Entrata dal lato corto
    setspeeds(-100, -100);
    delay(400);
    fermo();
    rotate(90);
    fermo();
    setspeeds(-100, -100);
    delay(1000);
    setspeeds(100, 100);
    delay(700);
  }

  if (n == 2) { //Entrata dal lato lungo
    //mot.setSpeeds(-50, -50);
    //delay(400);
  }


  fermo();
  rotate(-90);
  fermo();
  setspeeds(-100, -100);
  delay(2000);
  ferma();
  delay(1000);
  leggiNANO();
  centroperfetto = o;
  setspeeds(100, 100);
  //delay(2000);
  delay(300);
  fermo();
  rotate(120);
  fermo();
  int media;

  
  //inizio ricerca prima pallina
  cercapalla(0);
  fermo();
  //Inizio a cercare dove si trova la stazione
  setspeeds(100, 100);
  while (true) {
    if (sonar.ping_cm() <= 30)
      break;
    delay(50);
  }
  fermo();
  setspeeds(-100, -100);
  while (true) {
    if (sonar.ping_cm() >= 30)
      break;
    delay(50);
  }
  fermo();
  rotate(90);
  ferma();
  delay(100);
  setspeeds(100, 100);
  while (true) {
    if (backsonar.ping_cm() > 30)
      break;
    delay(50);
  }
  setspeeds(-100, -100);
  while (true) {
    if (backsonar.ping_cm() <= 30)
      break;
    delay(50);
  }
  fermo();
  rotate(45);
  fermo();

  //Check posizione n° 1

  if (backsonar.ping_cm() < 30) {
    check = 1; //la stazione si trova nella posizione n°1 (o almeno lo credo
    ferma();
    delay(1000);
    rotate(45);
    bigservo.write(0);
    delay(t_servo);
    rotate(-90);
    bigservo.write(130);
    delay(t_servo);
    rotate(45);
    if(backsonar.ping_cm()>20){
      setspeeds(100,100);
      delay(500);
      check=2;
    }
  }

  if(check==2) {   //Se la stazione NON si trova nella posizione n°1
    rotate(135);
    ferma();
    delay(100);
    setspeeds(100, 100);
    while (true) {
      if (sonar.ping_cm() < 5)
        break;

      delay(50);
    }
    delay(2000);
    fermo();
    setspeeds(-100, -100);
    delay(400);
    fermo();
    rotate(-90);
    fermo();
    setspeeds(100, 100);
    delay(2900);  ///////Da misurare!!!!!!! (È la lunghezza del campo, fino alla posizione n°2// Ultima calbrazione: 19/07/15 10:05 Hefei
    fermo();
    
    //Posizionamento Check 2
    rotate(90);
    fermo();
    setspeeds(100, 100);
    delay(1000);
    fermo();
    setspeeds(-100, -100);
    delay(500);
    while (true) {
      if (sonar.ping_cm() > 30)
        break;

      delay(50);
    }
    fermo();
    rotate(90);
    fermo();
    setspeeds(-100, -100);
    while (true) {
      if (backsonar.ping_cm() < 30)
        break;

      delay(50);
    }
    fermo();
    rotate(45);
    ferma();
    delay(100);

    if (backsonar.ping_cm() < 30) { //POTREBBE esserci il recinto, ma non ne sono sicuro, meglio spazzolare
      setspeeds(-100, -100);
      delay(1500);
      ferma();
      rotate(-90);
      bigservo.write(0);
      delay(t_servo);
      rotate(180);
      bigservo.write(130);
      delay(t_servo);
      rotate(-90);
    }
    check = 2;

    //Ricontrollo dopo aver spazzolato se c'è effettivamente il muro, altrimenti era una pallina che ho appena spazzolato via. Poco male.

    if (backsonar.ping_cm() > 15) {
      
      /*
      * Se la stazione NON si trova nella posizione n°2
      * e già mi sono assicurato con il controllo precedente che non si trova nella posizione n°1
      * allora si trova sicuramente nella 3
      */
      
      setspeeds(100, 100);
      delay(1200);
      ferma();
      check = 3;
      rotate(-135);
      fermo();
      setspeeds(-100, -100);
      delay(1000);
      fermo();  //La parte della spazzata per la palla parte da qui
      rotate(90);
      bigservo.write(0);
      delay(t_servo);
      rotate(-180);
      bigservo.write(130);
      delay(t_servo);
      rotate(135);
      fermo();  //E finisce qui
    }
  }



  //Consegna della palla, valida per tutte le possibili posizioni della stazione
  
  setspeeds(-100, -100);
  while (true) {
    if (backsonar.ping_cm() < 5) {
      delay(1000);
      fermo();
      deposita();
      /*asta.write(125); ///APRI COSO
      delay(500);
      for (int i = 15; i < 30; i++) {
        setPinza(i);
        delay(50);
      }
      asta.write(140);
      delay(500);
      setPinza(180);
      delay(500);
      pinza_detach();
      asta.write(120);
      delay(500);
      asta.detach();*/
      break;
    }
    delay(50);
  }
  
  //Palla consegnata, controllo adesso se ci sono palline rasenti al muro

  //Commentato, tanto a Bari le palline sono nel mezzo. Semmai vanno messe alla fine della ricerca, così non si perde tempo.
  /*
    
  if(check==1){ //Se mi trovo nella posizione n°1 è bene controllare solo sul lato lungo, a destra
    alMuro1(1);
  }
  else if(check==3){ //Se mi trovo nella posizione n°2 è bene controllare su entrambi i lati
    alMuro1(1);
    alMuro2(1);
  }
  else if(check==2){  //Se mi trovo nella posizione n°3 è bene controllare su entrambi i lati
    alMuro1(-1);
    alMuro2(-1);
  }
  */
  
  /*
  ########################################################################
  ########################################################################
  ############################# ALTRE PALLE ##############################
  ########################################################################
  ########################################################################
  */
  while (true) {

    //Mi allontano dalla stazione

    setspeeds(-100, -100);
    delay(500);
    ferma();
    delay(1000);
    leggiNANO();
    centroperfetto = o;
    setspeeds(100, 100);
    delay(500);

    //Mi giro di una certa quantità di gradi in modo tale da prendere la diagonale del parallelepipedo che è il campo (per esperienza, 27 gradi)

    int gradidiagonale=27;

    if (check == 1 || check == 3) //Posizione 1 e 3 (che tanto è uguale perché sono speculari)
      rotate(gradidiagonale);

    else    //Posizione 2
      rotate(-gradidiagonale);
    
    fermo();
    setspeeds(100, 100); //Vado avanti ancora un pochino
    delay(600);

    //Mi giro di 90° per prepararmi a cercare la palla

    fermo();
    rotate(120);
    fermo();

    //Cerco la palla a seconda dell'angolo del recinto
    
    if (check == 1 || check == 3)
      cercapalla(1);
    else
      cercapalla(2);

    //Mi riposiziono sulla diagonale

    fermo(); //E qui mi rifermo, non si sa mai con che cosa esco dal cercapalla
    
    if (check == 1 || check == 3)
      posizionati(centroperfetto + gradidiagonale);

    else
      posizionati(centroperfetto - gradidiagonale);

    fermo();
    
    //Torno indietro finché non sono abbastanza vicino alla stazione
    
    setspeeds(-100, -100);
    while (true) {
      if (backsonar.ping_cm() < 20) {
        break;
      }
      delay(50);
    }

    
    //Mi giro in modo tale da essere perpendicolare alla stazione
    
    posizionati(centroperfetto);
    
    fermo();

    //Qui mi metto al centro del recinto
    
    rotate(45);
    setspeeds(100, 100);
    while (true) {
      if (backsonar.ping_cm() >= 20)
        break;
      delay(100);
    }
    setspeeds(-100, -100);
    while (true) {
      if (backsonar.ping_cm() <= 20)
        break;
      delay(100);
    }
    rotate(-90);
    setspeeds(100, 100);
    while (true) {
      if (backsonar.ping_cm() >= 20)
        break;
      delay(100);
    }
    setspeeds(-100, -100);
    while (true) {
      if (backsonar.ping_cm() <= 20)
        break;
      delay(100);
    }
    rotate(45);

    
    //Mi avvicino e abbasso la pinza
    
    setspeeds(-100, -100);
    delay(2500);
    fermo();
    if(!vai_al_muro){
        delay(500);
        deposita();
    }
    /*else{
        if(check==1){ //Se mi trovo nella posizione n°1 è bene controllare solo sul lato lungo, a destra
          alMuro1(1);
        }
        else if(check==3){ //Se mi trovo nella posizione n°2 è bene controllare su entrambi i lati
          alMuro1(1);
          alMuro2(1);
        }
        else if(check==2){  //Se mi trovo nella posizione n°3 è bene controllare su entrambi i lati
          alMuro1(-1);
          alMuro2(-1);
        }
    }*/
  }
}
void prendipalla(){
    /*
    Ricerca della precisione
    Vari comandi per i servi 21
    */
    bigservo.write(0);
    delay(t_servo);
    bigservo.write(130);
    delay(t_servo);
}

void deposita(){
    /*
    Vari comandi per i servi   20
    */
    //smallservo.write(10);
    smallservo.write(100);
    delay(1111);
    smallservo.write(10);
}


void riposiziona(){
    /*
    Vari comandi per i servi
    */
    smallservo.write(10);
    delay(t_servo);
    bigservo.write(130);
    delay(t_servo);
}

int ispalla() {/*
  if(backsonar.ping_cm()<distanza-5){                //Osserva se ha trovato una pallina
                trovata=o;
                Serial.println("trovata");
                ferma();
                rotateto(trovata);
                Serial.println("Ho ruotato");
                delay(10);
                if(backsonar.ping_cm()<distanza-5){            //Ricontrolla dato che i sensori di ultrasuoni non sono affidabili
                    Serial.println("trovata2");
                    setspeeds(-50,-50);
                    while(backsonar.ping_cm()>20){}          //Si avvicina alla pallina
                    while(backsonar.ping_cm()>20){}          //Ricontrolla
                    leggiNANO();
                    trovata=o;
                    ferma();}}*/
}



