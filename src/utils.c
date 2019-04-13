#include "../header/utils.h"

/* debug itoa per numeri decimali */
void itoa(unsigned int x, char aux[]){
	int i = 0, j = 0 ;

	if (x == 0){
		aux[0] = '0' ;
		aux[1] = '\0' ;
	}
	else {
		while (x != 0){
			aux[i++] = (x % 10) + '0' ;
			x = x / 10 ;
			j++;
		}
		j--;
	}
	//algoritmo di xor swap
	for (i = 0; i < j ; i++){
		aux[i] = aux[i] ^ aux[j] ;
		aux[j] = aux[i] ^ aux[j] ;
		aux[i] = aux[i] ^ aux[j] ;
		j--;
	}
}