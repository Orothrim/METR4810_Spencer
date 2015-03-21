#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ViterbiFunctions.h"

using namespace std;

char toBeEncoded;
int Encoded;
int toBeDecoded;
int Decoded;



int main() {
	printf("Please enter a single character to have it encoded.\n");
	scanf(" %c", &toBeEncoded);
 
	Encoded = viterbiencode(toBeEncoded);
	
	printf("Please enter a hex value to be decoded.\n");
	scanf(" %x", &toBeDecoded);

	Decoded = viterbidecode(toBeDecoded);
}