#include "mcc_generated_files/mcc.h"
//Varíaveis utilizadas ---------------------------------------------------------
uint8_t dado_rx = 0;        // Recebe 1 byte para atualizar andar destino
uint8_t dado_tx[5];         // Envia 5 bytes para atualizar dados do elevador
//Bytes transmitidos
uint8_t estado_motor;       // Parado = 0; 1 = subindo; 2 = descendo  [byte 0(5:4)]
uint8_t andar = 1;          // Andar atual                            [byte 0(1:0)]
float distancia = 0;        // Distância/2 em relação ao andar 1 (mm) [byte 1(6:0)]
uint16_t velocidade = 0;    // Velocidade do motor*4 (mm/s)           [byte 2(6:0)]
float corrente_ADC = 0;     // Corrente do motor/4 (mA)               [byte 3(6:0)]
uint16_t temperatura = 0;   // Temperatura do motor*2 (°C)            [byte 4(6:0)]
//condições iniciais
uint8_t andar_destino = 1;  // Andar inicial
int Enc_pulso = 0;          // Contagem de pulsos do enconder
uint16_t Enc_pulso_velocidade = 0;// Contagem do encoder variável, exclusivo para velocidade
uint16_t dist_velocidade = 0;     // Distância para o cálculo da velocidade
int pwm_motor = 0;                // Pwm atual do motor
uint8_t pos_andar = 60;     // Posição do andar destino
uint8_t dist_andar = 0;     // Distância até o andar destino
float porcentagem;
uint16_t PWM_LED;

//Declaração de rotinas --------------------------------------------------------

//Movimentos do elevador-------------------------------------|
void sobe(){
    Dir_SetHigh();    //Vai para cima
    pwm_motor = 409;
    estado_motor = 1;   //Dado byte 0 (movimento ascendente)
    PWM3_LoadDutyValue(pwm_motor);    // Motor ativo 40%
}
void desce(){
    Dir_SetLow();   // Vai para baixo
    pwm_motor = 409;
    estado_motor = 2;   //Dado byte 0 (movimento descendente)
    PWM3_LoadDutyValue(pwm_motor);    // Motor ativo 40%
}
void para(){
    pwm_motor = 0;
    estado_motor = 0;   //Dado byte 0 (parado)
    PWM3_LoadDutyValue(pwm_motor);
}

// Atualzação dos parâmetros para controle da LED ----|
void LED_start(){
    pos_andar = 60*andar_destino - 59;  // 59 impede divisão por 0 no andar 1
    porcentagem = (0x3FF/pos_andar);    // Coeficiente para cálculo do PWm
    PWM_LED = 0x3FF;                    // Ao atualizar a led, começa vermelha
    EPWM1_LoadDutyValue(PWM_LED);       // 1023 vermelho e 0 verde
}

//Sensores dos andares ------------------------------------------------|
void andar1(){
    //Se este sensor é acionado, o elevador para incondicionalmente
    para();
    andar = 1;          // Atualiza andar atual (1)
    __delay_ms(2000);   // Espera 2 Segundos quando chega no andar 1 (valor 2 para simulação apenas)
}
void andar2(){
    //Se este sensor é acionado e o andar destino é 2, o elevador para
    andar = 2;          // Atualiza andar atual (2)
    if(andar_destino == 2){
    para();
    andar_destino = 1;  // Volta para o andar 1 em seguida
    __delay_ms(2000);   // Espera 2 Segundos quando chega no andar 2 (valor 2 para simulação apenas)
    }
}
void andar3(){
    //Se este sensor é acionado e o andar destino é 3, o elevador para
    andar = 3;          // Atualiza andar atual (3)
    if(andar_destino == 3){
    para();
    andar_destino = 1;  // Volta para o andar 1 em seguida
    __delay_ms(2000);   // Espera 2 Segundos quando chega no andar 3 (valor 2 para simulação apenas)
    }
}
void andar4(){
    //Se este sensor é acionado, o elevador para incondicionalmente
    para();
    andar = 4;          //Atualiza andar atual (4)
    andar_destino = 1;  //Volta para o andar 1 em seguida
    __delay_ms(2000);   //Espera 2 Segundos quando chega no andar 4 (valor 2 para simulação apenas)
}

// Controle comunicação EUSART --------------------------------------|
void envia(){
    if(EUSART_is_tx_ready()){
        for(int i = 0; i<5; i++){
            EUSART_Write(dado_tx[i]);   // Envia todos os bytes do array
        }
    }
}
void recebe(){
    dado_rx = 0;
    if(EUSART_is_rx_ready())                                            
        {
            dado_rx= EUSART_Read();
            if(dado_rx!=0){
                andar_destino = ((int)((dado_rx) & 0x03)+1); // Andar solicitado
                LED_start();    // Atualização de parâmetros PWM da LED
            }
        }
}
void atualiza(){
    velocidade = (Enc_pulso_velocidade<<4);    //Valor em mm/s desenvolvido em 100 ms fixos         
    Enc_pulso_velocidade = 0;                  //Zera a contagem de pulsos (dist_velocidade = 0)
    corrente_ADC = ((uint16_t)ADC_GetConversion(0)*0.39);   //Valor em mA
    distancia = Enc_pulso*0.83;                //Valor em mm
    temperatura = ((uint16_t)ADC_GetConversion(1))>>3; //Valor em °C (10mV)) Multiplicado por 0.125 (aproximação de 0.130)
    dado_tx[0] = 0x80|(((estado_motor<<4)|(andar-1))&0xB3);   // (informação + "10000000") AND "10110011"
    dado_tx[1] = ((((uint8_t)distancia)>>1)&0x7F);
    dado_tx[2] = (((uint8_t)velocidade<<2)&0x7F);
    dado_tx[3] = ((((uint8_t)corrente_ADC)>>2)&0x7F);
    dado_tx[4] = (((temperatura)<<1)&0x7F);
    envia();
}

//Contagem de pulsos totais no percursos e pulsos em 100 ms ----------------|
void pulso(uint16_t ccp4_valor){
    Enc_pulso_velocidade++;         // Exclusivo para cálculo da velocidade
    if(Dir_GetValue() == 0){        // Se descendo, decrementa Enc_pulso
        if(Enc_pulso != 0){         // Garante que não exceda o vetor
            Enc_pulso--;
        }
        else{
            Enc_pulso = 0;
        }
    }else{                          // Se subindo, incrementa Enc_pulso
        if(Enc_pulso != 215){       // Garante que não exceda o vetor
            Enc_pulso++;
        }
        else{
            Enc_pulso = 215;
        }
    }
}

void main(void)
{
   //Inicio do programa -------------------------------------------------------
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    
    //Interrupções que irão ser atendidas pelas seguintes funções --------------
    IOCBF0_SetInterruptHandler(andar1);    //S1
    IOCBF3_SetInterruptHandler(andar2);    //S2
    IOCBF4_SetInterruptHandler(andar3);    //S3
    IOCBF5_SetInterruptHandler(andar4);    //S4
    TMR4_SetInterruptHandler(atualiza);    //Timer 100 ms para enviar dados
    CCP4_SetCallBack(pulso);               //Interrupção do pulso em RA4
    
    while (S1_GetValue()!= 0){
        //Caso o sensor s1 não detecte o elevador no incio, ele desce
        desce();
        Enc_pulso = 0;//Garante o início da contagem no fundo do percurso
    }
    
    while (1)
    {
        //Verifica a entrada EUSART
        recebe();
        //Calcula a distância até o andar destino e controla LED -----------|
        dist_andar = abs((uint8_t)distancia - pos_andar); // Disancia atual entre elevador e andar destino
        PWM_LED = (uint16_t)(porcentagem*dist_andar);     // Cálculo baseado no coeficiente "porcentagem"
        if(andar_destino==1){
            EPWM1_LoadDutyValue(1023 - PWM_LED);    // Mudança gradual para verde na ida para o andar 1
        }else{
            EPWM1_LoadDutyValue(PWM_LED);   // Mudança gradual para verde na ida para os demais andares
        }
        //Controla o motor quando o andar destino é atualizado -------------|
        if(andar_destino != andar){
            if(andar < andar_destino){
                if((pwm_motor != 0) && (Dir_GetValue() == 0)){
                    //Se tiver decendo, espera 500 ms e sobe
                    para();
                    __delay_ms(500); // (valor 5 para simulação apenas)
                    sobe();
                }else{
                    //Se estiver parado ou indo pra cima, sobe
                    sobe();
                }
            }else if(andar > andar_destino){
                if((pwm_motor != 0) && (Dir_GetValue() == 1)){
                    //Se tiver subindo, espera 500 ms e desce
                    para();
                    __delay_ms(500); // (valor 5 para simulação apenas)
                    desce();
                }else{
                    desce();
                }
            }else{
                para();  // Elevador para caso andar = andar_destino
            }
        }
    }
}
/**
 End of File
*/