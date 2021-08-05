/*BAJA UFSCar - 2020
 * Eletrônica - Renan - André - Jack - Paloma
pinos em uso 
Display (D7[Video] e D9 [Sync])
velocidade (D2)
rotação(D3)--Sensor não deve estar presente no arquivo 
freio (A0)
temperatura(A2)
SD (SCK[13], MISO[12], MOSI[11], CS[10])
  */
//Bibliotecas
#include <Wire.h> 
#include <TVout.h> //Biblioteca utilizada para mostrar os dados no display
//#include <font8x8.h> //Bibliotecas utilizadas para alterar o tamanho da fonte no display
//#include <font6x8.h>
//#include <font4x6.h> 
#include <SD.h> //Bibliotecas utilizadas para o módulo SD
#include <SPI.h>


//Variáveis globais
TVout tela; //Declaração da variável "tela" para executar as ações do display
int pinoCS = 10, tempPin = A2, velPin=2, fdiaPin=A0, ftraPin=A1; //rotPin=3 retirado 
// float pause = 10, rpm = 0,celsius, km, f_dianteiro, f_traseiro; (Variáveis alteradas)
File dataTemp, dataRot, dataVel, dataFD, dataFT;

//Sensor de Temperatura 
  int leitura;
  float voltagem;
  float temperatura;

//Velocimetro JACK 
  #define pi 3.14
  float vel=0;

//Variaveis dentro da função freio_tra
  int val1 = 0;  
  float pressao1 = 0;
  float volt1 =0;
  
//Variaveis dentro da função freio_dia
  int val2 = 0;
  float pressao2 = 0;
  float volt2 =0;
 
//Inicialização
void setup() {
  tela.begin(NTSC,120,96);
//Serial.begin(115200);
  pinMode(pinoCS, OUTPUT);
//Velocimetro JACK
  Serial.begin(9600); //Dúvida sobre dois seriais 
  pinMode(velPin,INPUT);
//Sensor pressao de via de freio dianteiro e traseiro
  pinMode(pinoCS, OUTPUT);


}

//Rotina de dados
void loop() {
temp();
vel=velocimetro();
freio_dia();
freio_tra();
datalog(); 
}

//Programa responsável por registrar os valores dos sensores no cartão SD
void datalog(){
  dataTemp = SD.open("Temp.txt",FILE_WRITE);
  if (dataTemp) { // Se o Arquivo abrir imprime:
    dataTemp.println(temperatura); // Escreve no Arquivo
   dataTemp.close(); // Fecha o Arquivo após escrever
  }
 /* dataRot = SD.open("Rot.txt",FILE_WRITE);            //Será realizado na rede CAN em uma placa individual
  if (dataRot) { // Se o Arquivo abrir imprime:
    dataRot.println(rpm); // Escreve no Arquivo
    dataRot.close(); // Fecha o Arquivo após escrever
  }*/
  dataVel = SD.open("Vel.txt",FILE_WRITE);
  if (dataVel) { // Se o Arquivo abrir imprime:
    dataVel.println(vel); // Escreve no Arquivo
    dataVel.close(); // Fecha o Arquivo após escrever
  }
  dataFD = SD.open("FDia.txt",FILE_WRITE);
  if (dataFD) { // Se o Arquivo abrir imprime:
    dataFD.println(pressao2); // Escreve no Arquivo
    dataFD.close(); // Fecha o Arquivo após escrever
  }
  dataFT = SD.open("FTra.txt",FILE_WRITE);
  if (dataFT) { // Se o Arquivo abrir imprime:
    dataFT.println(pressao1); // Escreve no Arquivo
    dataFT.close(); // Fecha o Arquivo após escrever
  }
}

//Programa responsável por armazenar os valores do sensor de temperatura na CVT
void temp(){
 /* double t;
  double fct;
  fct=0.004828125*100; // fator de calibração 
  t=analogRead(tempPin)*fct;
  delayMicroseconds(pause);
  return t;
}*/
//Sensor de temperatura ANDRE
  leitura=analogRead(tempPin);
  voltagem=leitura*(5.0/1023);
  temperatura=(voltagem*100)-50;
  Serial.print("temp: ");  
  Serial.print(temperatura);
  Serial.println("C");
  delayMicroseconds(200);
}   


//Programa responsável por armazenar os valores do sensores de pressão nas vias do freio dianteiro
void freio_dia(){
//Programação do sensor do freio dianteiro DANTE
  val2 = analogRead(fdiaPin);  
  volt2 = 0.004887*val2;
  pressao2 = (2*volt2 - 0.9)-0.07375; 
  
//Filtro de ruídos 
   if (pressao2 <= 0.1)
  {
    pressao2 = 0;
  }
  
  Serial.print("Freio_dia_press: ");  
  Serial.print(pressao1);
  Serial.println("Pa.");
  delayMicroseconds(200);
  }

//Programa responsável por armazenar os valores do sensores de pressão nas vias do freio traseiro
void freio_tra(){

//Programação do sensor do freio traseiro DANTE
  val1 = analogRead(ftraPin);   
  volt1 = 0.004887*val1;
  pressao1 = (2*volt1 - 0.9)-0.03; 
  
//Filtro de ruídos 
    if (pressao1 <= 0.1)
  {
    pressao1 = 0;
  }
  
  Serial.print("Freio_tra_press: ");  
  Serial.print(pressao1);
  Serial.println("Pa.");
  delayMicroseconds(200);
}

//Programa responsável por mostrar os dados no display para o piloto
void comPiloto(){
  
}

//Programa responsável por mostrar os dados no display para a equipe
void comEquipe(){
  
}

//Programa responsável por armazenar os valores recebidos do ATMEGA escravo relacionados à velocidade do veículo
double velocimetro(){

//Variaveis entro da função
int state=0, cont=0,t;
float rpm=0, d=0.5334, r=0;
long lastmillis=0;

//Programação do sensor velocimetro JACK
    state = digitalRead(velPin);
  if(state==1){
    cont++;
}
  t=cont;

  if(millis()-lastmillis>=1000){
    r=d/2;
    rpm = t*60;
    vel = rpm*pi*2*r*3.6/60;
    cont=0;
    lastmillis=millis();
}
  Serial.print("Velocidade: ");
  Serial.print(vel); 
  Serial.print("km/h");
  Serial.println(".");
  delayMicroseconds(200);
  return vel;//duvidas se utiliza este comando juntamente com a troca da função de float para double
}

//Programa responsável por armazenar os valores recebidos do ATMEGA escravo relacionados à rotação do motor
/*void isr(){ // ISR - interrrupt service routine
  rev++;
}  
float rot(){
int rpm;
  if (millis() - lastmillis==1000){
     detachInterrupt(0);
     rpm=(rev)*60;
     lastmillis=millis();
     Serial.print("rpm: ");
     Serial.println(rpm);
     rev=0;
     attachInterrupt(0,isr,RISING);

    //global
    int lastmillis=0;
    int rev=0;
     // Rotação do motor - sensor Hall
attachInterrupt(0,isr,RISING);  //attaching the interrupt
}*/
