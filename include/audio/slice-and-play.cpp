#include <stdio.h>
#include <string.h>
#include "adpcm.h"
#include "player.h"
#include "slice-and-play.h"
#include "suoni/array-sound.h"
#define MAX_TOKEN 13

//creo la classe ring

//lunghezza attuale della stringa 

const unsigned char *tot[MAX_TOKEN];
unsigned int ar_len[MAX_TOKEN];
int cont=0;
ADPCMSound *neue[MAX_TOKEN];




ring& ring::instance()
{
	static ring singleton;
	return singleton;
}

//funzione che permette di scrivere le unità
void ring::say_unit (int unit){


	switch(unit){

		case 1: {
			
			addItemToVector(uno_bin,uno_bin_len);
			
			}	
		break;

		case 2:
                    addItemToVector( due_bin,due_bin_len);
		
		break;

		case 3: 
                     
                    addItemToVector( tre_bin,tre_bin_len);
			
		break;

		case 4: 
                         
			addItemToVector( quattro_bin,quattro_bin_len);
		break;

		case 5: 
                         
			addItemToVector( cinque_bin,cinque_bin_len);
		break;

		case 6: 

                    addItemToVector( sei_bin,sei_bin_len);
                    
		break;

		case 7: 
                         
			addItemToVector( sette_bin,sette_bin_len);;
		break;

		case 8: 
                         
			addItemToVector( otto_bin,otto_bin_len);
		break;

		case 9: 
                         
			addItemToVector( nove_bin,nove_bin_len);
		break;

		}

	

}
//permette di scrivere le decine
void ring::say_tens (int tens){
	



	switch(tens){

		case 2: 
                         
			addItemToVector( venti_bin,venti_bin_len);
		break;

		case 3: 
                         
			addItemToVector( trenta_bin,trenta_bin_len);
		break;

		case 4: 
                         
			addItemToVector( quaranta_bin,quaranta_bin_len);
		break;

		case 5: 
                         
			addItemToVector( cinquanta_bin,cinquanta_bin_len);
		break;

		case 6: 
                         
			addItemToVector( sessanta_bin,sessanta_bin_len);
		break;

		case 7: 
                         
			addItemToVector( settanta_bin,settanta_bin_len);
		break;

		case 8:
                         
			addItemToVector( ottanta_bin,ottanta_bin_len);
		break;

		case 9: 
                         
                    addItemToVector( novanta_bin,novanta_bin_len);
		break;
	}	

	
}
// permette di scrivere i numeri dal 10 al 19
void ring::say_tens_and_unit(int tensun){




	switch(tensun){
			
		case 0: 
                         
			addItemToVector( dieci_bin,dieci_bin_len);
		break;


		case 1: 
                         
			addItemToVector( undici_bin,undici_bin_len);
		break;

		case 2: 
                         addItemToVector( dodici_bin,dodici_bin_len);
			
		break;

		case 3: 
                         
			addItemToVector( tredici_bin,tredici_bin_len);
		break;

		case 4: 
                         
			addItemToVector( quattordici_bin,quattordici_bin_len);
		break;

		case 5: 
                         
			addItemToVector( quindici_bin,quindici_bin_len);
		break;

		case 6:
                         
			addItemToVector( sedici_bin,sedici_bin_len);
		break;

		case 7: 
                         
			addItemToVector( diciassette_bin,diciassette_bin_len);
		break;

		case 8:
                         
			addItemToVector( diciotto_bin,diciotto_bin_len);
		break;

		case 9: 
                         
			addItemToVector( diciannove_bin,diciannove_bin_len);
		break;

		}

	
}


void ring :: addItemToVector( const unsigned char *file,const unsigned int new_len){
   

   	tot[cont]=file;
    	
	ar_len[cont] = new_len;
	
	neue[cont] = new ADPCMSound(file,new_len);
	
	cont++;



}

// corpo centrale 
//prende il numero e ne fa la divisione intera per le potenze di 10, scomponendo così il numero nelle sue cifre 



void ring::play_n_of_step(int num,int volume){

    ADPCMSound zero (zero_bin,zero_bin_len);
    ADPCMSound haifatto (haifatto_bin,haifatto_bin_len);	
    ADPCMSound passi (passi_bin,passi_bin_len);
    ADPCMSound haivinto (haivinto_bin,haivinto_bin_len);
    
   	//conversione del valore del voume con controolo dei bound
	if (volume>100) volume =100;
	if (volume<0) volume = 0;
	
	volume = volume -100;
    




	//suona "Hai fatto"
	//Player::instance().play(haifatto,volume);

	addItemToVector(haifatto_bin, haifatto_bin_len);
	
	int div=10; // potenza del 10 che divide
	int res=0; //risultato della divisione intera
	short int exp = 1; // esponente della potenza del dieci
	
      
        
        
        
	//bool like per il caso particolare dei numeri dal 10 al 19 (in questo caso vengono dette due cifre in una volta sola)
	short int just_said; 

	int mask = num;	//mask è una coppia di num che rimane inalterata per capire se le migliaia o i milioni sono singolari o plurali
	
	
	
	just_said=0;
	// se il numero è zero pronuncio lo zero e esco
	if (num == 0) {
		
                //zero 
		Player::instance().play(zero,volume);
		
	}
	//pongo il massimo numero di passi a 9 milioni 999999		
	div = div * 100000;
	exp = 6;
	
	// quando vengono esaurite le divisioni si esce
	while(exp != -1){
		
		//calcolo la cifra da far dire
		res = num/div;
		// controllo di non avere errori (dal 10 al 19 è gestito in seguito)
		if (res>9){
			
			 //troppi passi
			Player::instance().play(haivinto,volume);
		}
		
		// se la cifra è diversa da zero bisogna solo controllare che non si sia da dire "mille" o "milione" 
 		if (res != 0){
			
			
			//le differenze su cosa dire si basano sulla posizione della cifra all'interno del numero
			
			//centinaia
			if ((exp % 3) == 2){
				
				if (res>1){
					
					say_unit(res );
					
                                       
                                        addItemToVector( cento_bin,cento_bin_len);
                                        
                                       
				}else{
					
                                         
                                       addItemToVector( cento_bin,cento_bin_len);
				}

			}
			//decine
			if ((exp % 3) == 1){
				if (res>1){
                                         say_tens(res );
				}else{
					num -= res * div;
					div = div/10;
					exp--;
					res = num / div;
					say_tens_and_unit(res );
					just_said = 1;
				}

			}
			//milioni
			if (exp == 6){
				
                                         say_unit(res );

				if ( (mask/div) > 1 ){
						
					
                                         //milioni
                                       addItemToVector( milioni_bin,milioni_bin_len);
					
				}else{ 
                                         //milione
                                        addItemToVector( milione_bin,milione_bin_len);
				}
				mask = mask - (res * div);
			}
			//migliaia
			if (exp == 3 ){
				 
				if (just_said == 1){
				 	
                                         // migliaia plurale
                                        addItemToVector( mila_bin,mila_bin_len);
				}else{
					if ( (mask/div) > 1 ){
						
                                                say_unit(res );
						
                                                 // migliaia plurale
                                                 addItemToVector( mila_bin,mila_bin_len);
					
					}else{ 
                                                 //migliaia singolare
                                                 addItemToVector( mille_bin,mille_bin_len);
					}	
				}

			}
			//unità
			if (exp == 0){
				
				if (just_said==0)  {say_unit(res );}

			}
			


		}else{
			// se il risultato della divisione è uguale a zero ma bisogna integrare con "mille " o "milioni"
			// e.g. 50475
			if (exp == 3 ){		
				if ( (mask/div) > 1 ){
                                   
                                   //migliaia plurale  
                                    addItemToVector( mila_bin,mila_bin_len);
                                
                                }	
				

			}
		}
		num -= res * div;
		div = div/10;
		exp--;
		just_said = 0;
		
	}
        
	
	
	

		
	addItemToVector(passi_bin, passi_bin_len);
	
       

		
		Player::instance().play_V(neue,&cont,volume);
	
	
      


	//Player::instance().play(passi,volume);	
}
//funzione che permette di riprodurre il segnale di vittoria
void ring::victory_Song(int volume){
    ADPCMSound haivinto (haivinto_bin,haivinto_bin_len);
	//conversione del valore del voume con controolo dei bound
	if (volume>100) volume =100;
	if (volume<0) volume = 0;
	
	volume = volume -100;
                         
			Player::instance().play(haivinto,volume);
    
    
}

//funzione che permette di riprodurre il segnale di sconfitta

void ring::looser_Song(int volume){
    
ADPCMSound haiperso  (haiperso_bin,haiperso_bin_len);
//conversione del valore del voume con controolo dei bound
	if (volume>100) volume =100;
	if (volume<0) volume = 0;
	
	volume = volume -100;
                         
			Player::instance().play(haiperso,volume);
}





