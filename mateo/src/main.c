#include "S32K144.h" /* include peripheral declarations S32K144 */
#include "math.h"
#define pi 3.1416
unsigned char enviar1;          //Lo enviado a LPUART1

unsigned char retro[7][4];                //Recibido de LPUART1 (x,y,theta)
unsigned char mensaje[4];             //Recibido de LPUART2 (10 instrucciones máx)
unsigned char largo;
unsigned char Tx1,Rax,Ray,contador=0;     //Variables para ordenar mensajes de LPUART1
unsigned char Tx2,Rbx,Rby=0;              //Variables para ordenar mensajes de LPUART2
unsigned short trayectoria=0;                     //Comparación con valores de simulink (Valores "altos")
unsigned char bateria,descarga,voltaje=0;      //Valores mandados desde simulink
unsigned short theta=0;                   // (0-360) -> short
unsigned char Y,pos_x=0;
unsigned char DATA1,DATA2=0;              //Variables temporales donde se almacena cada caracter recibido por LPUART
unsigned short RPM_I,RPM_D=0;             //Revoluciones de ambas llantas
unsigned char cuadrante=0;
signed short ca,co=0;
signed short msj;
unsigned long i=0;
unsigned char temp2[]={"A0\r\n1"};
unsigned char comodin=0;
unsigned char temp[23];                //Variable temporales
unsigned char permiso=1;
unsigned char rango=5;

void Juntar_numeros(){                //Junto los números del arreglo en uno solo
	msj=0;
	Rby=1;                            //Empezamos del segundo porque con el primero separamos la instrucción
	while(Rby!=largo){          //Añadimos hasta que sea igual que el contador
		msj+=mensaje[Rby]-48;
		Rby++;
		if(Rby!=largo) msj*=10;
	}
}

void Confirma (){
    if(temp2[1]=='1'){ //Adelante
    	if(cuadrante==1){ //cuadrante 1
    		if(Y<co || pos_x<ca){
    			permiso=0;
    		}else {
    			permiso=1;
    			temp2[1]='5';
    		}
    	}
    	if(cuadrante==2){ //cuadrante 2
    	    if(Y<co || pos_x>ca){
    	    	permiso=0;
    	    }else {
    	    	permiso=1;
    	    	temp2[1]='5';
    	    }
    	}
    	if(cuadrante==3){ //cuadrante 3
    	    if(Y>co || pos_x>ca){
    	    	permiso=0;
    	    }else {
    	    	permiso=1;
    	    	temp2[1]='5';
    	    }
    	}
    	if(cuadrante==4){ //cuadrante 4
    	    if(Y>co || pos_x<ca){
    	    	permiso=0;
    	    }else {
    	    	permiso=1;
    	    	temp2[1]='5';
    	    }
    	 }
    }

	if(temp2[1]=='2'){ //Giro Derecha
		if(theta>(msj+rango) && theta<(msj-rango)){
			permiso=0;
		}else {
			permiso=1;
			temp2[1]='5';
		}
	}

	if(temp2[1]=='3'){ //Giro Izquierda
		if(theta>(msj+rango) && theta<(msj-rango)){
			permiso=0;
	    }else {
	    	permiso=1;
	    	temp2[1]='5';
	    }
	}

	if(temp2[1]=='4'){ //Atrás
		if(cuadrante==1){ //cuadrante 1
		    if(Y<co || pos_x<ca){
		    	permiso=0;
		    }else {
		    	permiso=1;
		    	temp2[1]='5';
		    }
		}
		if(cuadrante==2){ //cuadrante 2
		    if(Y<co || pos_x>ca){
		    	permiso=0;
		     }else {
		    	 permiso=1;
		    	 temp2[1]='5';
		     }
		}
		if(cuadrante==3){ //cuadrante 3
		    if(Y>co || pos_x>ca){
		    	 permiso=0;
		      }else {
		    	  permiso=1;
		    	  temp2[1]='5';
		      }
		}
		if(cuadrante==4){ //cuadrante 4
		    if(Y>co || pos_x<ca){
		    	 permiso=0;
		      }else {
		    	  permiso=1;
		    	  temp2[1]='5';
		      }
	    }
	}
}

void LPUART1_RxTx_IRQHandler (void){ //Handler para interrupciones de LPUART1 (Simulink)

	if ((LPUART1->STAT & (1<<23))==(1<<23)){ //Interrupción para transmitir a simulink
		if(i!=4){
		LPUART1->DATA=temp2[i++];
		}else{
			i=0;
			LPUART1->CTRL&=1<<19;
		}
			    }

	if ((LPUART1->STAT & (1<<21))==(1<<21)){   //Interrupción para recibir de simulink         //LISTO
		DATA1=LPUART1->DATA;
		if(DATA1=='X'){ //Dato empieza con X (Es la posición x)
			Rax=0;
			contador=0;
		}
        if(DATA1=='Y'){ //Dato empieza con Y (Es la posición y)
        	Rax=1;
        	contador=0;
        }
        if(DATA1=='Z'){ //Dato empieza con Z (Es el ángulo theta)
        	Rax=2;
        	contador=0;
        }
        if(DATA1=='B'){ //Dato empieza con B (Es la señal de descarga)
        	Rax=3;
        	contador=0;
        }
        if(DATA1=='V'){
        	Rax=4;
        	contador=0;
        }
        if(DATA1=='D'){
        	Rax=5;
        	contador=0;
        }
        if(DATA1=='I'){
        	Rax=6;
        	contador=0;
        }

        if(Rax==0){ //X
        	if(DATA1!='.' && DATA1!='X'){
        		retro[Rax][Ray++]=DATA1;
        		contador++;
        	}else{
        		pos_x=0;
        		Ray=0;
                while(Ray!=contador){
                	 pos_x+=retro[Rax][Ray++]-48; //Convertimos de ASCII a dígito regular y lo guardamos en X
                	if(Ray!=contador) pos_x*=10;
                }
                Ray=0;
                contador=0;
        	}
        }
        if(Rax==1){ //Y
            if(DATA1!='.' && DATA1!='Y'){
            	retro[Rax][Ray++]=DATA1;
            	contador++;
            }else{
            	Y=0;
            	Ray=0;
                while(Ray!=contador){
                    Y+=retro[Rax][Ray++]-48; //Convertimos de ASCII a dígito regular y lo guardamos en Y
                    if(Ray!=contador) Y*=10;
                }
                Ray=0;
                contador=0;
            }
         }
        if(Rax==2){ //Z (Theta)
            if(DATA1!='.' && DATA1!='Z'){
                retro[Rax][Ray++]=DATA1;
                contador++;
            }else{
                theta=0;
                Ray=0;
                while(Ray!=contador){
                    theta+=retro[Rax][Ray++]-48; //Convertimos de ASCII a dígito regular y lo guardamos en theta
                    if(Ray!=contador) theta*=10;
                }
                Ray=0;
                contador=0;
            }
         }
        if(Rax==3){ //Recarga?
            if(DATA1!='.' && DATA1!='B'){
                retro[Rax][Ray++]=DATA1;
           		contador++;
           	}else{
                descarga=0;
                Ray=0;
                while(Ray!=contador){
                    descarga+=retro[Rax][Ray++]-48; //Convertimos de ASCII a dígito regular y lo guardamos en descarga
                    if(Ray!=contador) descarga*=10;
                }
                Ray=0;
                contador=0;
           }
                }
        if(Rax==4){ //Voltaje
              if(DATA1!='.' && DATA1!='V'){
                retro[Rax][Ray++]=DATA1;
                contador++;
            }else{
                voltaje=0;
                Ray=0;
                while(Ray!=contador){
                     voltaje+=retro[Rax][Ray++]-48; //Convertimos de ASCII a dígito regular y lo guardamos en voltaje
                     if(Ray!=contador) voltaje*=10;
                }
                Ray=0;
                contador=0;
           }
                }
        if(Rax==5){ //Velocidad Angular Derecha (rad/s)
                      if(DATA1!='.' && DATA1!='D'){
                        retro[Rax][Ray++]=DATA1;
                        contador++;
                    }else{
                        RPM_D=0;
                        Ray=0;
                        while(Ray!=contador){
                             RPM_D+=retro[Rax][Ray++]-48; //Convertimos de ASCII a dígito regular y lo guardamos en RPM_D
                             if(Ray!=contador) RPM_D*=10;
                        }
                        Ray=0;
                        contador=0;
                   }
                        }
        if(Rax==6){ //Velocidad Angular Izquierda (rad/s)
                              if(DATA1!='.' && DATA1!='I'){
                                retro[Rax][Ray++]=DATA1;
                                contador++;
                            }else{
                                RPM_I=0;
                                Ray=0;
                                while(Ray!=contador){
                                     RPM_I+=retro[Rax][Ray++]-48; //Convertimos de ASCII a dígito regular y lo guardamos en RPM_D
                                     if(Ray!=contador) RPM_I*=10;
                                }
                                Ray=0;
                                contador=0;
                           }
                                }
	}
}



void Separar_mensaje2(){//Mensaje recibido Bluetooth
	switch (mensaje[0]){ //El primer caracter indica qué tipo de movimiento se va a hacer

	case '1': //Movimiento hacia enfrente
		Juntar_numeros(); //Juntamos el resto de los números es una sola variable

		co=(sin(theta)*msj)+Y; //Y
		ca=(cos(theta)*msj)+pos_x; //X

		if(theta>=0 && theta<90){ //Cuadrante 1 X-pos, Y-pos
			cuadrante=1;
		}else if (theta>=90 && theta<180){ //Cuadrante 2  X-neg, Y-pos
			cuadrante=2;
		}else if (theta>=180 && theta<270){ //Cuadrante 3 X-neg, Y-neg
			cuadrante=3;
		}else if (theta>=270 && theta<359){ //Cuadrante 4 X-pos, Y-neg
			cuadrante=4;
		}

		temp2[1]='1';
		LPUART1->CTRL|=(1<<19);
		LPUART1->CTRL|=(1<<23);
		break;

	case '2':                 //Giro Derecha
		PTD->PTOR=1;
		Juntar_numeros();
		msj=theta-msj;           //Ejemplo theta es 45° y queremos que gire 50° a la derecha -> estará a 355° grados a partir del eje x
		if(msj<0) msj+=360;     //Ejemplo theta es 235 y queremos que gire 90° a la derecha -> estará a 145° grados a partir del eje x
		temp2[1]='2';
		LPUART1->CTRL|=(1<<19);
		LPUART1->CTRL|=(1<<23);
		break;

	case '3':                 //Giro Izquierda
		Juntar_numeros();
		msj=theta+msj;                 //Ejemplo theta es 45° y queremos que gire 50° a la izquierda -> estará a 95° grados a partir del eje x
		if(msj>=360) msj=msj-360;      //Ejemplo theta es 300° y queremos que gire 100° a la izquierda -> estará a 40° grados a partir del eje x
		temp2[1]='3';
		LPUART1->CTRL|=(1<<19);
				LPUART1->CTRL|=(1<<23);
		break;

	case '4': //Movimiento hacia atrás
		Juntar_numeros();
		co=((sin(theta)*msj)+Y)*-1; //Y
	    ca=((cos(theta)*msj)+pos_x)*-1; //X

	    if(theta>=0 && theta<90){ //Cuadrante 1 X-pos, Y-pos
	    	cuadrante=3;
		}else if (theta>=90 && theta<180){ //Cuadrante 2  X-neg, Y-pos
			cuadrante=4;
		}else if (theta>=180 && theta<270){ //Cuadrante 3 X-neg, Y-neg
			cuadrante=1;
		}else if (theta>=270 && theta<359){ //Cuadrante 4 X-pos, Y-neg
			cuadrante=2;
		}
	    temp2[1]='4';
	    LPUART1->CTRL|=(1<<19);
	    		LPUART1->CTRL|=(1<<23);
		break;
	     }
	largo=0;
	Rbx=0;
	Rby=0;
	temp2[0]='A';
	temp2[2]='\r';
	temp2[3]='\n';
}

void Calcular_salidas (void){
	//Empezamos con los datos de RPM_I   (Mandaremos 4 caracteres) temp[0],temp[1],temp[2],temp[3]
	RPM_I=(RPM_I/2*pi)*60;  //Convertimos de Rad/s  a Rev/m  no damos importancia a decimales
			if(RPM_I<1000){
				temp[0]='0';
				if(RPM_I<100){ //Menor a 100 (99-0)
				temp[1]='0';
				if(RPM_I<10){ //Menor a 10 (9-0) (Un dígito)
					temp[2]='0';
					temp[3]=RPM_I+'0'; //Lo convierto en ASCII
				}else{ //Mayor o igual a 10, menor que 100  (Dos dígitos)
					temp[3]=RPM_I%10;
					temp[2]=(RPM_I-temp[3])/10;
					temp[3]+='0';
					temp[2]+='0';
				}
			}else{ //Mayor o igual a 100 (Tres dígitos)
				temp[3]=RPM_I%10;
			    temp[2]=(RPM_I-temp[3])/10;
			    temp[2]=temp[2]%10;
				temp[1]=((((RPM_I-temp[3])/10))-temp[2])/10;
				temp[1]+='0';
				temp[2]+='0';
				temp[3]+='0';
			}

			}else{//Mayor o igual a 1000 (cuatro dígitos) Ejemplo
				temp[3]=RPM_I%10;
				temp[2]=(RPM_I-temp[3])/10;
				temp[2]=temp[2]%10;
				temp[1]=(((RPM_I-temp[3])/10)-temp[2])/10;
				temp[1]=temp[1]%10;
				temp[0]=(((((RPM_I-temp[3])/10)-temp[2])/10)-temp[1])/10;
				temp[0]+='0';
				temp[1]+='0';
				temp[2]+='0';
				temp[3]+='0';
			}
	//Acaba datos RPM_I

			//Empezamos con los datos de RPM_D   (Mandaremos 4 caracteres) temp[4],temp[5],temp[6],temp[7]
			RPM_I=(RPM_I/2*pi)*60;
						if(RPM_D<1000){
							temp[4]='0';
							if(RPM_D<100){ //Menor a 100 (99-0)
							temp[5]='0';
							if(RPM_D<10){ //Menor a 10 (9-0) (Un dígito)
								temp[6]='0';
								temp[7]=RPM_D+'0'; //Lo convierto en ASCII
							}else{ //Mayor o igual a 10, menor que 100  (Dos dígitos)
								temp[7]=RPM_D%10;
								temp[6]=(RPM_D-temp[7])/10;
								temp[7]+='0';
								temp[6]+='0';
							}
						}else{ //Mayor o igual a 100 (Tres dígitos)
							temp[7]=RPM_D%10;                            //Ejemplo número 233   temp3=3
						    temp[6]=(RPM_D-temp[7])/10;                    //  temp2= 23
						    temp[6]=temp[6]%10;                            //  temp2= 3
							temp[5]=((((RPM_D-temp[7])/10))-temp[6])/10;     //  temp1= 2
							temp[5]+='0';
							temp[6]+='0';
							temp[7]+='0';
						}
						}else{//Mayor o igual a 1000 (cuatro dígitos) Ejemplo 4321
							temp[7]=RPM_D%10;                //432    ->1
							temp[6]=(RPM_D-temp[7])/10;         // 432
							temp[6]=temp[6]%10;              // 43->   2
							temp[5]=((((RPM_D-temp[7]))/10)-temp[6])/10;  // 43
							temp[5]=temp[5]%10;                          // 4 -> 3
							temp[4]=((((((RPM_D-temp[7]))/10)-temp[6])/10)-temp[5])/10;
							temp[4]+='0';
							temp[5]+='0';
							temp[6]+='0';
							temp[7]+='0';
						}

		//Empezamos con datos de X (Dos dígitos) temp[8],temp[9]
						if(pos_x<10){
							temp[8]='0';
							temp[9]=pos_x+'0';
						}else{
							temp[9]=pos_x%10;
							temp[8]=(pos_x-temp[9])/10;
							temp[9]+='0';
							temp[8]+='0';
						}
		//Acaban datos de X

		//Empezamos con datos de Y  temp[10],temp[11]
						if(Y<10){
							temp[10]='0';
							temp[11]=Y+'0';
						}else{
							temp[11]=Y%10;
							temp[10]=(Y-temp[11])/10;
							temp[11]+='0';
							temp[10]+='0';
						}
		//Acaban datos de Y

		//Empezamos con los datos de theta temp[12],temp[13],temp[14]
						if(theta<100){
							temp[12]='0';
						if(theta<10){
							temp[13]='0';
							temp[14]=theta+'0';
						}else{
							temp[14]=theta%10;
							temp[13]=(theta-temp[14])/10;
							temp[14]+='0';
							temp[13]+='0';
						}
						}else{
							temp[14]=theta%10;
							temp[13]=(theta-temp[14])/10;
							temp[13]=temp[13]%10;
							temp[12]=((((theta-temp[14])/10))-temp[13])/10;
							temp[12]+='0';
							temp[13]+='0';
							temp[14]+='0';
						}
		 //Acaba datos theta

		//Empiezan datos bateria temp[15], temp[16], temp[17], temp[18]
						bateria=42-voltaje;
						bateria = 9-bateria;
						bateria = (bateria*100)/9;
						if(bateria<100){
							temp[15]='0';
						if(theta<10){
							temp[16]='0';
							temp[17]=bateria+'0';
							temp[18]='%';
						}else{
							temp[17]=bateria%10;
							temp[16]=(bateria-temp[17])/10;
							temp[17]+='0';
							temp[16]+='0';
							temp[18]='%';
						}
						}else{
							temp[17]=bateria%10;
							temp[16]=(bateria-temp[17])/10;
							temp[16]=temp[16]%10;
							temp[15]=((((bateria-temp[17])/10))-temp[16])/10;
							temp[15]+='0';
							temp[16]+='0';
							temp[17]+='0';
							temp[18]='%';
						}
		//Acaban datos batería

		//Empiezan datos trayectoria temp[19], temp[20], temp[21], temp[22]
						if(trayectoria<100){
							temp[19]='0';
						if(theta<10){
							temp[20]='0';
							temp[21]=trayectoria+'0';
							temp[22]='%';
						}else{
							temp[21]=trayectoria%10;
							temp[20]=(trayectoria-temp[21])/10;
							temp[21]+='0';
							temp[20]+='0';
							temp[22]='%';
						}
						}else{
							temp[21]=trayectoria%10;
							temp[20]=(trayectoria-temp[21])/10;
							temp[20]=temp[20]%10;
							temp[19]=((((trayectoria-temp[21])/10))-temp[20])/10;
							temp[19]+='0';
							temp[20]+='0';
							temp[21]+='0';
							temp[22]='%';
												}
		//Acaban datos trayectoria
}

void LPUART2_RxTx_IRQHandler (void){ //Handler para interrupciones de LPUART2 (Bluetooth)

	if ((LPUART2->STAT & (1<<23))==(1<<23)){ //Interrupción para transmitir al bluetooth          //LISTO
		LPUART2->DATA=temp[Tx2++];
	        if(Tx2==23){
	        	Tx2=0;
				//LPUART2->CTRL&=~(1<<23);
	        }
	    }
	if ((LPUART2->STAT & (1<<21))==(1<<21)){   //Interrupción para recibir del bluetooth         //LISTO
		DATA2=LPUART2->DATA;
		if(permiso==1){ //Si tiene permiso recibe, si no, no
				if(DATA2!='T'){
					mensaje[Rby++]=DATA2;
					largo++;
				}else {
					Separar_mensaje2();
					Rby=0;
				}
		     }
		 }
    }


void LPTMR0_IRQHandler (void)
{
	LPTMR0->CSR|=(1<<7);        //Apagar bandera
	Calcular_salidas();
	PTD->PTOR=1;
    Confirma();
	//LPUART1->CTRL|=1<<23;
}

int main(void){

	PCC->PCCn[PCC_PORTA_INDEX]=1<<30;    //Habilitar clock del periférico en Puerto A
	PCC->PCCn[PCC_PORTC_INDEX]=1<<30;    //Habilitar clock del periférico en Puerto C
    PCC->PCCn[PCC_PORTD_INDEX]=1<<30;    //Habilitar clock del periférico en Puerto D

    PORTD->PCR[0]=(1<<8);                //LED Azul GPIO
    PORTD->PCR[15]=(1<<8);
    PTD->PDDR=(1<<15)+1;
    PTD->PDOR=(1<<15)+1;

	SCG->SIRCDIV=1<<8;				     //SIRCDIV2: 8 MHz/1
	/*  LPTMR0 Config   */
	PCC->PCCn[PCC_LPTMR0_INDEX]=(1<<30);   //Peripheral Clock al módulo LPTMR0
	LPTMR0->CMR=10-1;
	LPTMR0->PSR=5;                         //Bypass prescaler, escogemos reloj fijo de 1KHz -> 1ms por cuenta
	LPTMR0->CSR=(1<<6)+1;                  //Enable y habilitamos interrupción
	S32_NVIC->ISER[58/32]=(1<<(58%32));    //Interrupción por ISER

	/*  LPUART1 Config  */
	PORTC->PCR[7]=2<<8;				    //LPUART1 TX   PIN 7 - PUERTO C (Conexión desde Micro USB)
	PORTC->PCR[6]=2<<8;				    //LPUART1 RX   PIN 6 - PUERTO C (Conexión desde Micro USB)
    PCC->PCCn[PCC_LPUART1_INDEX]=2<<24;		//SIRCDIV2
	PCC->PCCn[PCC_LPUART1_INDEX]|=1<<30;
	LPUART1->BAUD|=52;						//BAUD_SRG=CLK_UART/(16*9600)
	LPUART1->CTRL|=(1<<23)+(1<<21)+(1<<19)+(1<<18);	        //TE=RE=1 Habilitamos interrupciones tanto de Tx como de Rx, y habilitamos transmisión y recepción
    S32_NVIC->ISER[33/32]=(1<<(33%32));     //Habilitar interrupcion de LPUART1

    /*  LPUART2 Config  */
    PORTA->PCR[9]=2<<8;				    //LPUART2 TX   PIN 9 - PUERTO A
    PORTA->PCR[8]=2<<8;				    //LPUART2 RX   PIN 8 - PUERTO A
    PCC->PCCn[PCC_LPUART2_INDEX]=2<<24;		//SIRCDIV2
   	PCC->PCCn[PCC_LPUART2_INDEX]|=1<<30;
   	LPUART2->BAUD|=52;						//BAUD_SRG=CLK_UART/(16*9600)
   	LPUART2->CTRL|=(1<<23)+(1<<21)+(1<<19)+(1<<18);	        //TE=RE=1 Habilitamos interrupciones tanto de Tx como de Rx, y habilitamos transmisión y recepción
    S32_NVIC->ISER[35/32]=(1<<(35%32));     //Habilitar interrupciones de LPUART2
    while(1){
    }
	return 0;
}
