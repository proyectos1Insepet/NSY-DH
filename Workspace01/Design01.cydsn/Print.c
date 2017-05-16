/*
*********************************************************************************************************
*                                           GRP500 CODE
*
*                             (c) Copyright 2013; Sistemas Insepet LTDA
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                               GRP500 CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5969AXI-LP035
*
* Filename      : Print.c
* Version       : V1.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <device.h>
#include "Variables.h"
#include "I2C.h"
#include "protocol.h"
#include "Logos.h"

/*
*********************************************************************************************************
*                                            MENSAJES
*********************************************************************************************************
*/
uint8 msn_eds[15]="SERVICE STATION";
uint8 msn_nit[4]="NIT ";
uint8 msn_tel[4]="PH: ";
uint8 msn_numero[8]="Number: ";
uint8 msn_fecha[6]="Date: ";
uint8 msn_hora[7]="Hour:  ";
uint8 msn_pos[10]="Position: ";
uint8 msn_ppu[14]="PPU:      H/G ";
uint8 msn_vol[13]="Volumen:  G ";
uint8 msn_din[11]="Money:   H ";
uint8 msn_placa[10]="Plate:    ";
uint8 msn_cuenta[11]="Account:   ";
uint8 msn_km[11]="Km:        ";
uint8 msn_id[11]="Id:        ";
uint8 msn_copia[6]="COPY ";
uint8 msn_producto[9]="Product: ";
uint8 msn_fcorte[12]="Shift date: ";
uint8 msn_hcorte[13]="Shift time:  ";
uint8 msn_idcorte[16]="Consecutive:    ";
uint8 msn_ufcorte[17]="Last date shift: ";
uint8 msn_uhcorte[18]="Last time shift:  ";
uint8 msn_consecutivo[17]="Last time shift: ";




/*
*********************************************************************************************************
*                          				void print_logo_k(uint8 val, uint8 logo)
*
* Description : Envia por I2C los datos de los vectores de los logos. Usando impresora Kiosco
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void print_logo_k(uint8 val, uint8 logo){
	uint16 i;
	switch(logo){
		case 0:
			for(i=0;i<=1511;i++){
        		write_psoc1(val,biomax[i]);			
			}
		break;

		case 1:
			for(i=0;i<=1511;i++){
        		write_psoc1(val,brio[i]);			
			}
		break;			

		case 2:
			for(i=0;i<=1511;i++){
        		write_psoc1(val,cencosud[i]);			
			}
		break;	
			
		case 3:
			for(i=0;i<=1511;i++){
        		write_psoc1(val,ecospetrol[i]);			
			}
		break;

		case 4:
			for(i=0;i<=1511;i++){
        		write_psoc1(val,esso[i]);			
			}
		break;
			
		case 5:
//			for(i=0;i<=944;i++){
//        		write_psoc1(val,exito[i]);			
//			}
		break;
			
		case 6:
			for(i=0;i<=1511;i++){
        		write_psoc1(val,National[i]);			
			}
		break;	
			
		case 7:
			for(i=0;i<=944;i++){
        		write_psoc1(val,mineroil[i]);			
			}
		break;	
			
		case 8:
			for(i=0;i<=944;i++){
        		write_psoc1(val,mobil[i]);			
			}
		break;	
			
		case 9:
			for(i=0;i<=1511;i++){
        		write_psoc1(val,petrobras[i]);			
			}
		break;
			
		case 10:
			for(i=0;i<=755;i++){
        		write_psoc1(val,plus[i]);			
			}
		break;	
			
		case 11:
			for(i=0;i<=1511;i++){
        		write_psoc1(val,terpel[i]);			
			}
		break;
			
		case 12:
			for(i=0;i<=1511;i++){
        		write_psoc1(val,texaco[i]);			
			}
		break;	
			
		case 13:
			for(i=0;i<=755;i++){
        		write_psoc1(val,zeus[i]);			
			}
		break;
			
		case 14:
			for(i=0;i<=1511;i++){
        		write_psoc1(val,petromil[i]);			
			}
		break;		
	}	
	write_psoc1(val,10);
	write_psoc1(val,0x1D);
	write_psoc1(val,0x4C);
	write_psoc1(val,0x07);
	write_psoc1(val,0x00);
	write_psoc1(val,10);
}



/*
*********************************************************************************************************
*                          				void print_logo_p(uint8 val, uint8 logo)
*
* Description : Envia por I2C los datos de los vectores de los logos. Para impresora panel
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void print_logo_p(uint8 val, uint8 logo){
	uint16 i;
	switch(logo){
		case 0:
			for(i=0;i<=943;i++){
        		write_psoc1(val,biomax1[i]);			
			}
		break;

		case 1:
			for(i=0;i<=943;i++){
        		write_psoc1(val,brio1[i]);			
			}
		break;			

		case 2:
			for(i=0;i<=943;i++){
        		write_psoc1(val,cencosud1[i]);			
			}
		break;	
			
		case 3:
			for(i=0;i<=943;i++){
        		write_psoc1(val,ecospetrol1[i]);			
			}
		break;

		case 4:
			for(i=0;i<=943;i++){
        		write_psoc1(val,esso1[i]);			
			}
		break;
			
		case 5:
//			for(i=0;i<=943;i++){
//        		write_psoc1(val,exito[i]);			
//			}
		break;
			
		case 6:
			for(i=0;i<=943;i++){
        		write_psoc1(val,National1[i]);			
			}
		break;	
			
		case 7:
			for(i=0;i<=943;i++){
        		write_psoc1(val,mineroil1[i]);			
			}
		break;	
			
		case 8:
			for(i=0;i<=943;i++){
        		write_psoc1(val,mobil1[i]);			
			}
		break;	
			
		case 9:
			for(i=0;i<=943;i++){
        		write_psoc1(val,petrobras1[i]);			
			}
		break;
			
		case 10:
			for(i=0;i<=943;i++){
        		write_psoc1(val,plus1[i]);			
			}
		break;	
			
		case 11:
			for(i=0;i<=943;i++){
        		write_psoc1(val,terpel1[i]);			
			}
		break;
			
		case 12:
			for(i=0;i<=943;i++){
        		write_psoc1(val,texaco1[i]);			
			}
		break;	
			
		case 13:
			for(i=0;i<=943;i++){
        		write_psoc1(val,zeus1[i]);			
			}
		break;
			
		case 14:
			for(i=0;i<=943;i++){
        		write_psoc1(val,petromil1[i]);			
			}
		break;		
	}	
	write_psoc1(val,0x1B);
	write_psoc1(val,0x40);
	write_psoc1(val,10);
	write_psoc1(val,10);
}




/*
*********************************************************************************************************
*                                         void imprimir(uint8 val)
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/

void imprimir(uint8 val, uint8 producto, uint8 copia, uint8 pos){
    uint8 digito,x;
    
	for(x=0;x<=((31-20)/2);x++){
		write_psoc1(val,0x20);			
	}		
	for(x=0;x<=14;x++){										//ESTACION DE SERVICIO
		write_psoc1(val,msn_eds[x]);
	}
    write_psoc1(val,10);
	
    write_psoc1(val,10);
    write_psoc1(val,10);
    write_psoc1(val,10);
    	for(x=0;x<=7;x++){										//NUMERO DE VENTA							
		write_psoc1(val,msn_numero[x]);
	}        	    
	write_psoc1(val,10);
	for(x=0;x<=5;x++){										//FECHA								
		write_psoc1(val,msn_fecha[x]);
	}	
	
	for(x=0;x<10;x++){										//POSICION								
		write_psoc1(val,msn_pos[x]);
	}
    write_psoc1(val,((pos/10)+48));
	write_psoc1(val,((pos%10)+48));
    write_psoc1(val,10); 
	for(x=0;x<9;x++){										//PRODUCTO								
		write_psoc1(val,msn_producto[x]);
	}
        	
	if(pos==side.a.dir){											//PLACA		
		write_psoc1(val,10);	
		for(x=0;x<10;x++){																		
			write_psoc1(val,msn_placa[x]);
		} 
		for(x=1;x<=bufferDisplay1.licenceSale[0];x++){
			write_psoc1(val,bufferDisplay1.licenceSale[x]);	
		}		
//		if((bufferDisplay1.idSerial[0])==0x04){						//DATOS IBUTTON
//			write_psoc1(val,10);	
//			write_psoc1(val,10);		
//			for(x=0;x<=10;x++){																		
//				write_psoc1(val,msn_cuenta[x]);
//			} 
//			for(x=1;x<=Buffer_LCD1.cuenta[0];x++){
//				write_psoc1(val,Buffer_LCD1.cuenta[x]);	
//			}			
//			write_psoc1(val,10);	
//			for(x=0;x<=10;x++){																		
//				write_psoc1(val,msn_placa[x]);
//			} 
//			for(x=1;x<=Buffer_LCD1.placa[0];x++){
//				write_psoc1(val,Buffer_LCD1.placa[x]);	
//			}
//			if(Buffer_LCD1.km[0]>0){
//				write_psoc1(val,10);	
//				for(x=0;x<=10;x++){																		
//					write_psoc1(val,msn_km[x]);
//				}
//				for(x=1;x<=Buffer_LCD1.km[0];x++){
//					write_psoc1(val,Buffer_LCD1.km[x]);	
//				}				
//			}
//			write_psoc1(val,10);	
//			for(x=0;x<=10;x++){																		
//				write_psoc1(val,msn_id[x]);
//				
//			}
//			for(x=6;x>=1;x--){
//				if(((Buffer_LCD1.id[x]>>4)&0x0F)>=10){
//					write_psoc1(val,(((Buffer_LCD1.id[x]>>4)&0x0F)+55));
//				}
//				else{
//					write_psoc1(val,(((Buffer_LCD1.id[x]>>4)&0x0F)+48));				
//				}
//				if((Buffer_LCD1.id[x]&0x0F)>=10){
//					write_psoc1(val,((Buffer_LCD1.id[x]&0x0F)+55));
//				}
//				else{
//					write_psoc1(val,((Buffer_LCD1.id[x]&0x0F)+48));				
//				}				
//			}	
//		}
	}
//	else{															//DATOS PLACA LCD 2
//		if(Buffer_LCD2.posventa==1){
//			write_psoc1(val,10);	
//			for(x=0;x<=10;x++){																		
//				write_psoc1(val,msn_placa[x]);
//			} 
//			for(x=1;x<=Buffer_LCD2.placa[0];x++){
//				write_psoc1(val,Buffer_LCD2.placa[x]);	
//			}
//		}
//		if((Buffer_LCD2.preset&0x04)==0x04){
//			write_psoc1(val,10);
//			write_psoc1(val,10);
//			for(x=0;x<=10;x++){																		
//				write_psoc1(val,msn_cuenta[x]);
//			} 
//			for(x=1;x<=Buffer_LCD2.cuenta[0];x++){
//				write_psoc1(val,Buffer_LCD2.cuenta[x]);	
//			}	
//			write_psoc1(val,10);
//			for(x=0;x<=10;x++){																		
//				write_psoc1(val,msn_placa[x]);
//			} 
//			for(x=1;x<=Buffer_LCD2.placa[0];x++){
//				write_psoc1(val,Buffer_LCD2.placa[x]);	
//			}
//			if(Buffer_LCD2.km[0]>0){
//				write_psoc1(val,10);
//				for(x=0;x<=10;x++){																		
//					write_psoc1(val,msn_km[x]);
//				}
//				for(x=1;x<=Buffer_LCD2.km[0];x++){
//					write_psoc1(val,Buffer_LCD2.km[x]);	
//				}				
//			}
//			write_psoc1(val,10);	
//			for(x=0;x<=10;x++){																		
//				write_psoc1(val,msn_id[x]);
//				
//			}			
//			for(x=6;x>=1;x--){
//				if(((Buffer_LCD2.id[x]>>4)&0x0F)>=10){
//					write_psoc1(val,(((Buffer_LCD2.id[x]>>4)&0x0F)+55));
//				}
//				else{
//					write_psoc1(val,(((Buffer_LCD2.id[x]>>4)&0x0F)+48));				
//				}
//				if((Buffer_LCD2.id[x]&0x0F)>=10){
//					write_psoc1(val,((Buffer_LCD2.id[x]&0x0F)+55));
//				}
//				else{
//					write_psoc1(val,((Buffer_LCD2.id[x]&0x0F)+48));				
//				}				
//			}
//		}	
//	}   
    write_psoc1(val,10); 
    write_psoc1(val,10); 
    write_psoc1(val,10);
    write_psoc1(val,10);
	write_psoc1(val,10);
	write_psoc1(val,0x1D);
	write_psoc1(val,0x56);
	write_psoc1(val,0x31);	
}




/* [] END OF FILE */
