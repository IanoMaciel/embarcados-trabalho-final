
#define MOT_SUBIR 3
#define MOT_DESCER 8
#define PORTA_ABRIR A4
#define PORTA_FECHAR A3

#define rdc 13
#define primeiro 12
#define segundo 11
#define terceiro 10
#define emergencia A5
#define A 5
#define B 4
#define C A2
#define D A1
#define E A0
#define F 6
#define G 7

int x=0;
int pretendido=0,atual=0;
int RDC,pri,seg,ter,quar,emer;

void setup()
{
  pinMode (A5,INPUT);
  pinMode (A0,OUTPUT);
  pinMode (A1,OUTPUT);
  pinMode (A2,OUTPUT);
  pinMode (4,OUTPUT);
  pinMode (5,OUTPUT);
  pinMode (6,OUTPUT);
  pinMode (7,OUTPUT);
  pinMode (3,OUTPUT);
  pinMode (8,OUTPUT);
  pinMode (A3,OUTPUT);
  pinMode (A4,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);

  Serial.begin(9600);
  attachInterrupt(0,interrupcao,RISING);

}

void loop()
{  
  do{   
    RDC=digitalRead(rdc);

    pri=digitalRead(primeiro); 

    seg=digitalRead(segundo);

    ter=digitalRead(terceiro);

    numero(pretendido);

  }while(pri==0 && seg==0 && ter==0 && RDC==0);


  if (RDC==1){
    pretendido=0;
  }

  if (pri==1){
    pretendido=1;
  }


  if (seg==1){
    pretendido=2;
  }

  if (ter==1){
    pretendido=3;
  }  

  if (pretendido>atual){
    Serial.println("atual < pretendido ");
    subir_andar();

    do{
      Serial.println("Subindo ");
      numero(atual);
      Serial.println(atual);

      atual++;
      delay(1000);

    }while (atual<=pretendido);
	
    stop_subir();
    abrir_portas();
    fechar_portas();


    atual=pretendido;
  }
  else if (pretendido<atual){
    Serial.println("atual < pretendido ");
    descer_andar();
    do{
      numero(atual);
      Serial.println("Descendo ");
      Serial.println(atual);
      atual--;
      delay(1000);
    }while (atual>=pretendido);

    stop_descer();
    abrir_portas();
    fechar_portas();
    atual=pretendido;

  }else if(atual==pretendido){
    Serial.println("andar atual ");
    atual=pretendido;
    abrir_portas();
    fechar_portas();
    interrupcao();

  }
}



void abrir_portas(){
  Serial.println("Abrindo as portas ");
  Serial.println(atual);
  digitalWrite(PORTA_ABRIR,HIGH);
  delay(1500);
  digitalWrite(PORTA_ABRIR,LOW);
  delay(1500);
}

void fechar_portas(){
  
  Serial.println("Fechando as portas ");
  Serial.println(atual);
  digitalWrite(PORTA_FECHAR,HIGH);
  delay(1500);
  digitalWrite(PORTA_FECHAR,LOW);
}

void subir_andar(){
  digitalWrite(MOT_SUBIR,HIGH);
}

void stop_subir(){
  digitalWrite(MOT_SUBIR,LOW); 
}

void descer_andar (){
  digitalWrite(MOT_DESCER,HIGH); 
}

void stop_descer (){
  digitalWrite(MOT_DESCER,LOW); 
}

void numero (int x) {
  if (x == 0) {
    digitalWrite (A, LOW);
    digitalWrite (B, LOW);
    digitalWrite (C, LOW);
    digitalWrite (D, LOW);
    digitalWrite (E, LOW);
    digitalWrite (F, LOW);
    digitalWrite (G,LOW);  

    digitalWrite (A, HIGH);
    digitalWrite (B, HIGH);
    digitalWrite (C, HIGH);
    digitalWrite (D, HIGH);
    digitalWrite (E, HIGH);
    digitalWrite (F, HIGH);

  }

  if (x == 1) {

    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    digitalWrite(G, LOW);
    digitalWrite(E, LOW);
    digitalWrite(D, LOW);
    digitalWrite(F,LOW);  
    digitalWrite (B, HIGH);
    digitalWrite (C, HIGH);
  }

  else if (x==2){

    digitalWrite (A, LOW);
    digitalWrite (B, LOW);
    digitalWrite (C, LOW);
    digitalWrite (D, LOW);
    digitalWrite (E, LOW);
    digitalWrite (F, LOW);
    digitalWrite (G,LOW);  


    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(E, HIGH);
    digitalWrite(D, HIGH);


  }

  else  if (x==3){

    digitalWrite (B, LOW);
    digitalWrite (C, LOW);
    digitalWrite (G, LOW);
    digitalWrite (F, LOW);
    digitalWrite (E,LOW);

    digitalWrite (A, HIGH);
    digitalWrite (B, HIGH);
    digitalWrite (C, HIGH);
    digitalWrite (D, HIGH);
    digitalWrite (G, HIGH);


  }

  else if (x==4){

    digitalWrite (A, LOW);
    digitalWrite (C, LOW);
    digitalWrite (D, LOW);
    digitalWrite (E, LOW);
    digitalWrite (F, LOW);
    digitalWrite (G, LOW); 

    digitalWrite (B, HIGH);
    digitalWrite (C, HIGH);
    digitalWrite (G, HIGH);
    digitalWrite (F, HIGH);


  }
}

void interrupcao(){

  digitalWrite(MOT_SUBIR,LOW);
  digitalWrite(MOT_DESCER,LOW);
  digitalWrite(PORTA_FECHAR,LOW);
  digitalWrite(PORTA_ABRIR,LOW);
 
}
