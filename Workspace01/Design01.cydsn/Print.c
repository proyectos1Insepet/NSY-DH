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
#include <stdlib.h>
#include <string.h>
#include "Variables.h"
#include "I2C.h"
#include "protocol.h"
#include "Logos.h"

/*
*********************************************************************************************************
*                                            MENSAJES
*********************************************************************************************************
*/
uint8 msn_fecha[13]       = "Fecha      : ";
uint8 msn_hora[13]        = "Hora       : ";
uint8 msn_placa[13]       = "Placa      : ";
uint8 PRN_MILLEAGE[13]    = "Kilometraje: ";
uint8 PRN_TRANSTYPE[13]   = "Tipo       : ";
uint8 msn_pos[13]         = "Posicion   : ";
uint8 PRN_PRODUCT[13]     = "Producto   : ";
uint8 msn_ppu[13]         = "Ppu        : ";
uint8 msn_vol[13]         = "Volumen    : ";
uint8 msn_din[13]         = "Dinero     : ";
uint8 msn_numero[13]      = "Consecutivo: ";
uint8 PRN_BALANCE[13]     = "Saldo      : ";
uint8 PRN_COMPANY[13]     = "Empresa    : ";
uint8 PRN_ACCOUNT[13]     = "Cuenta     : ";
uint8 PRN_SERIAL[13]      = "Serial     : ";
uint8 PRN_ACCOUNTTYPE[13] = "Tipo Cuenta: ";
uint8 PRN_VISITSDAY[13]   = "Visitas Dia: ";
uint8 PRN_VISITSWEEK[13]  = "Visitas Sem: ";
uint8 PRN_VISITSMONTH[13] = "Visitas Mes: ";
uint8 PRN_VOLUMEDAY[13]   = "Volumen Dia: ";
uint8 PRN_VOLUMEWEEK[13]  = "Volumen Sem: ";
uint8 PRN_VOLUMEMONTH[13] = "Volumen Mes: ";
uint8 PRN_MONEYDAY[13]    = "Dinero Dia : ";
uint8 PRN_MONEYWEEK[13]   = "Dinero Sem : ";
uint8 PRN_MONEYMONTH[13]  = "Dinero Mes : ";
uint8 PRN_PRESET[13]      = "Valor Prog.: ";
uint8 PRN_SIGNATURE[13]   = "Firma      : ";
uint8 PRN_ID[13]          = "Cedula     : ";
uint8 PRN_CURRENCY[1]     = "$";
uint8 SEPARATOR[30]       = "******************************";


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
void printLogoK(uint8 val, uint8 logo){
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
void printLogoP(uint8 val, uint8 logo){
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

void imprimir(uint8 val, uint8 pos){ //val, puerto de impresora
    uint8 x;
    
    if(PrinterType ==1){
        printLogoP(printPortA,0);
    }			
    write_psoc1(val,10);
    for(x = 0; x < 30; x++){
        write_psoc1(val,Encabezado1[x]);
    }
    write_psoc1(val,10);
    
    for(x = 0; x < 30; x++){
        write_psoc1(val,Encabezado2[x]);
    }
    write_psoc1(val,10);
    
    for(x = 0; x < 30; x++){
        write_psoc1(val,Encabezado3[x]);
    }
    write_psoc1(val,10);
    
    for(x = 0; x < 30; x++){
        write_psoc1(val,Encabezado4[x]);
    }
    write_psoc1(val,10);
    
    for(x = 0; x < 30; x++){
        write_psoc1(val,SEPARATOR[x]);
    }
    write_psoc1(val,10);
    for(x=0;x < 13;x++){										//FECHA								
		write_psoc1(val,msn_fecha[x]);
	}
    if(leer_fecha()==1){
		write_psoc1(val,(((dateDownHandle[0]&0x30)>>4)+48));
		write_psoc1(val,((dateDownHandle[0]&0x0F)+48));
		write_psoc1(val,'/');
		write_psoc1(val,(((dateDownHandle[1]&0x10)>>4)+48));
		write_psoc1(val,((dateDownHandle[1]&0x0F)+48));	
		write_psoc1(val,'/');
		write_psoc1(val,(((dateDownHandle[2]&0xF0)>>4)+48));
		write_psoc1(val,((dateDownHandle[2]&0x0F)+48));			
	}		
    write_psoc1(val,10);
    for(x=0;x < 13;x++){										//HORA								
		write_psoc1(val,msn_hora[x]);
	}
    if(leer_hora()==1){										//HORA
		write_psoc1(val,(((timeDownHandle[1]&0x10)>>4)+48));
		write_psoc1(val,((timeDownHandle[1]&0x0F)+48));
		write_psoc1(val,':');
		write_psoc1(val,(((timeDownHandle[0]&0xF0)>>4)+48));
		write_psoc1(val,((timeDownHandle[0]&0x0F)+48));		
	}
    write_psoc1(val,10);
    for(x = 0; x < 30; x++){
        write_psoc1(val,SEPARATOR[x]);
    }
    write_psoc1(val,10);
    
    for(x=0;x< 13;x++){										    //POSICION								
		write_psoc1(val,msn_pos[x]);
	}
    write_psoc1(val,((pos/10)+48));
	write_psoc1(val,((pos%10)+48));
    write_psoc1(val,10);
    
    
    for(x=0;x < 13;x++){										    //NUMERO DE VENTA							
		write_psoc1(val,msn_numero[x]);
	}    	
    write_psoc1(val,10);
	if(pos==side.a.dir){
        for(x=0;x < 13;x++){										//Producto							
    		write_psoc1(val,PRN_PRODUCT[x]);
    	}    	
        write_psoc1(val,10);
        
        for(x=0;x < 13;x++){										//Ppu							
    		write_psoc1(val,msn_ppu[x]);
    	}   
        for(x=1;x <= side.a.ppuSale[0];x++){						   							
    		write_psoc1(val,side.a.ppuSale[x]);
    	}
        write_psoc1(val,0x20);
        write_psoc1(val,PRN_CURRENCY[0]);
        write_psoc1(val,0x2F);
        for(x=0;x < 5;x++){										//Simbolos $/G							
    		write_psoc1(val,VolUnit[x]);
    	}
        write_psoc1(val,10);
        
        
        for(x=0;x < 13;x++){										//Volumen							
    		write_psoc1(val,msn_vol[x]);
    	} 
        ///////////////////////////////////////////////////////////////
        if(side.a.volumeSale[1] == 0x30)
            side.a.volumeSale[1] = 0x00;
        if(side.a.volumeSale[1] == 0x00 && side.a.volumeSale[2] ==0x30)
            side.a.volumeSale[2] = 0x00;
        ///////////////////////////////////////////////////////////////
        for(x=1;x <= side.a.volumeSale[0];x++){						   							
            write_psoc1(val,side.a.volumeSale[x]);
            if(x == VolDec)
                write_psoc1(val,'.');
    	}
        write_psoc1(val,0x20);
        for(x=0;x < 5;x++){										//Simbolos $/G							
    		write_psoc1(val,VolUnit[x]);
    	}
        write_psoc1(val,10);
        
       
        for(x=0;x < 13;x++){										//Dinero							
    		write_psoc1(val,msn_din[x]);
    	}
        /////////////////////////////////////////////////////////////
        if(side.a.moneySale[1] ==0x30)
            side.a.moneySale[1] = 0x00;
        if(side.a.moneySale[1] ==0x00 && side.a.moneySale[2] ==0x30)
            side.a.moneySale[2] = 0x00;
        /////////////////////////////////////////////////////////////
        for(x=1;x <= side.a.moneySale[0];x++){						   							
    		write_psoc1(val,side.a.moneySale[x]);
            if(x == MoneyDec)
                write_psoc1(val,'.');
    	}
        write_psoc1(val,0x20);
        write_psoc1(val,PRN_CURRENCY[0]);        
        write_psoc1(val,10);
        
        if(bufferDisplay1.saleType==1){
    		for(x=0;x<13;x++){																		
    			write_psoc1(val,msn_placa[x]);                          //PLACA
    		} 
    		for(x=1;x<=bufferDisplay1.licenceSale[0];x++){
    			write_psoc1(val,bufferDisplay1.licenceSale[x]);	
    		}
            write_psoc1(val,10);
            for(x=0;x<13;x++){																		
    			write_psoc1(val,PRN_PRESET[x]);                          //Preset
    		} 
    		for(x=1;x<=bufferDisplay1.presetValue[0][0];x++){
    			write_psoc1(val,bufferDisplay1.presetValue[0][x]);	
    		}
            write_psoc1(val,10);
        }
        
        for(x = 0; x < 30; x++){
            write_psoc1(val,SEPARATOR[x]);
        }
        write_psoc1(val,10);
        
  	    if(bufferDisplay1.saleType==2){		
            for(x = 0;x < 13;x++){																		
				write_psoc1(val,PRN_MILLEAGE[x]);
			}	
			for(x=1;x<=bufferDisplay1.mileageSale[0];x++){
				write_psoc1(val,bufferDisplay1.mileageSale[x]);	
			}
            write_psoc1(val,10);
            		
			for(x= 0;x < 13;x++){										//DATOS IBUTTON								
				write_psoc1(val,PRN_SERIAL[x]);
			} 
			for(x=1;x<=bufferDisplay1.idSerial[0];x++){
				write_psoc1(val,bufferDisplay1.idSerial[x]);	
			}			
			write_psoc1(val,10);	
			for(x=0;x < 13;x++){																		
				write_psoc1(val,msn_placa[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
            
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_BALANCE[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				//write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
            
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_COMPANY[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				//write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
            
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_ACCOUNT[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				//write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
			
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_VISITSDAY[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				//write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
            
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_VISITSWEEK[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				//write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
            
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_VISITSMONTH[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				//write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
            
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_MONEYDAY[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				//write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
            
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_MONEYWEEK[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				//write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
            
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_MONEYMONTH[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				//write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
            
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_ACCOUNTTYPE[x]);
			} 
			for(x=1;x<=bufferDisplay1.identySale[0];x++){
				//write_psoc1(val,bufferDisplay1.identySale[x]);	
			}
            write_psoc1(val,10);
            write_psoc1(val,10);
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_SIGNATURE[x]);
			} 			
            write_psoc1(val,10);
            write_psoc1(val,10);
            write_psoc1(val,10);
            
            for(x=0;x < 13;x++){																		
				write_psoc1(val,PRN_ID[x]);
			} 			
            write_psoc1(val,10);
            write_psoc1(val,10);
            write_psoc1(val,10);
            for(x = 0; x < 30; x++){
                write_psoc1(val,SEPARATOR[x]);
            }
            write_psoc1(val,10);
	    }
    }
    
	
    
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
