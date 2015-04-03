//Convolutional encoding and decoding using the Viterbi algorithm



#define DEBUG		0

#include "ViterbiFunctions.h"

int viterbidecode(int iHexValue) {
	
	char cStages0[8];
	char cStages1[8];
	char cStages2[8];

	char cTValue0[8] = {0, 0, 1, 1, 1, 1, 0, 0};
	char cTValue1[8] = {0, 1, 0, 1, 1, 0, 1, 0};
	char cTValue2[8] = {0, 1, 1, 0, 1, 0, 0, 1};

	//Each Node has it's most efficient nodal history stored here, if the most efficient path to node 0 comes from node 1, it takes on
	//Node 1's history and adds the latest branch length to it.
	char cNode0V[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	char cNode1V[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	char cNode2V[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	char cNode3V[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	//These are used as buffers while passing histories between nodes.
	char cNode0T[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	char cNode1T[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	char cNode2T[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	char cNode3T[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	//Final Node length, length of correct path
	int cNodeF = 0;

	char i,j;

	//Stores the length of each of the 8 possible branches, they are counted 0-7 from their destination not their origin.  So the
	//first two branches both end at node 0
	char cBLength[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	//The length of the nodal histories are all stored here.
	char cNLength[4] = {0, 0, 0, 0};

	//This is a buffer for the length of the nodal histories
	char cNLTemp[4] = {0, 0, 0, 0};

	if (DEBUG) {
		printf("Input Data Was: %x\n", iHexValue);
	}

	for(i=0;i<8;i++){
		cStages0[i] = (iHexValue & 0x01);
		cStages1[i] = !!(iHexValue & 0x02);
		cStages2[i] = !!(iHexValue & 0x04);
		iHexValue = iHexValue >> 3;
	}

	// gets the length of the first 2 branches, as these are the only two that get a length for the first step
	cBLength[0] = (cStages0[0]==cTValue0[0]) ? 0:1;
	cBLength[0] += (cStages1[0]==cTValue1[0]) ? 0:1;
	cBLength[0] += (cStages2[0]==cTValue2[0]) ? 0:1;

	cBLength[2] = (cStages0[0]==cTValue0[2]) ? 0:1;
	cBLength[2] += (cStages1[0]==cTValue1[2]) ? 0:1;
	cBLength[2] += (cStages2[0]==cTValue2[2]) ? 0:1;

    

	//Stores paths so far of the nodes, these are only 000 for node 0 and 101 for node 1
	cNode0V[0] = 0;
	cNode1V[0] = 5; 

	//Takes the nodal to the two possible nodes so far
	cNLength[0] = (cBLength[0]);
	cNLength[1] = (cBLength[2]);

	//Takes the length of the 4 possible branches at this stage.
	cBLength[0] = (cStages0[1]==cTValue0[0]) ? 0:1;
	cBLength[0] += (cStages1[1]==cTValue1[0]) ? 0:1;
	cBLength[0] += (cStages2[1]==cTValue2[0]) ? 0:1;

	cBLength[2] = (cStages0[1]==cTValue0[2]) ? 0:1;
	cBLength[2] += (cStages1[1]==cTValue1[2]) ? 0:1;
	cBLength[2] += (cStages2[1]==cTValue2[2]) ? 0:1;

	cBLength[4] = (cStages0[1]==cTValue0[4]) ? 0:1;
	cBLength[4] += (cStages1[1]==cTValue1[4]) ? 0:1;
	cBLength[4] += (cStages2[1]==cTValue2[4]) ? 0:1;

	cBLength[6] = (cStages0[1]==cTValue0[6]) ? 0:1;
	cBLength[6] += (cStages1[1]==cTValue1[6]) ? 0:1;
	cBLength[6] += (cStages2[1]==cTValue2[6]) ? 0:1;

	//All nodes now have a path that arrived there, thus they are all now given that path, all starting with either 000 or 101
	//as the paths are passed between the nodes and those are the only two possible values at the first stage
	cNode3V[0] = cNode1V[0];
	cNode3V[1] = 2;  
	cNode2V[0] = cNode1V[0];
	cNode2V[1] = 7;
	cNode1V[0] = cNode0V[0];
	cNode1V[1] = 5;
	cNode0V[1] = 0;

	//The nodal lengths are now calculated, there is no culling as of yet.
	cNLength[3] = (cNLength[1]+cBLength[6]);
	cNLength[2] = (cNLength[1]+cBLength[4]);
	cNLength[1] = (cNLength[0]+cBLength[2]);
	cNLength[0] = (cNLength[0]+cBLength[0]);

	//The remaining 6 stages are done in this loop, they are each effectively the same thing repeated
	for(i=2;i<8;i++){

		//For each stage the new branch 0 length is found
		cBLength[0] = (cStages0[i]==cTValue0[0]) ? 0:1;
		cBLength[0] += (cStages1[i]==cTValue1[0]) ? 0:1;
		cBLength[0] += (cStages2[i]==cTValue2[0]) ? 0:1;

		//For each stage the new branch 1 length is found
		cBLength[1] = (cStages0[i]==cTValue0[1]) ? 0:1;
		cBLength[1] += (cStages1[i]==cTValue1[1]) ? 0:1;
		cBLength[1] += (cStages2[i]==cTValue2[1]) ? 0:1;

		//For each stage the new branch 2 length is found
		cBLength[2] = (cStages0[i]==cTValue0[2]) ? 0:1;
		cBLength[2] += (cStages1[i]==cTValue1[2]) ? 0:1;
		cBLength[2] += (cStages2[i]==cTValue2[2]) ? 0:1;

		//For each stage the new branch 3 length is found
		cBLength[3] = (cStages0[i]==cTValue0[3]) ? 0:1;
		cBLength[3] += (cStages1[i]==cTValue1[3]) ? 0:1;
		cBLength[3] += (cStages2[i]==cTValue2[3]) ? 0:1;

		//For each stage the new branch 4 length is found
		cBLength[4] = (cStages0[i]==cTValue0[4]) ? 0:1;
		cBLength[4] += (cStages1[i]==cTValue1[4]) ? 0:1;
		cBLength[4] += (cStages2[i]==cTValue2[4]) ? 0:1;

		//For each stage the new branch 5 length is found
		cBLength[5] = (cStages0[i]==cTValue0[5]) ? 0:1;
		cBLength[5] += (cStages1[i]==cTValue1[5]) ? 0:1;
		cBLength[5] += (cStages2[i]==cTValue2[5]) ? 0:1;

		//For each stage the new branch 6 length is found
		cBLength[6] = (cStages0[i]==cTValue0[6]) ? 0:1;
		cBLength[6] += (cStages1[i]==cTValue1[6]) ? 0:1;
		cBLength[6] += (cStages2[i]==cTValue2[6]) ? 0:1;

		//For each stage the new branch 7 length is found
		cBLength[7] = (cStages0[i]==cTValue0[7]) ? 0:1;
		cBLength[7] += (cStages1[i]==cTValue1[7]) ? 0:1;
		cBLength[7] += (cStages2[i]==cTValue2[7]) ? 0:1;

		//The culling occurs next
		//The nodal length of node 0 is set to the shortest nodal length of either node 0 or node 2 in it's buffer
		if((cNLength[0]+cBLength[0])<=(cNLength[2]+cBLength[1])){
			for(j=0;j<8;j++){
				cNode0T[j] = cNode0V[j];
			}
		}
		else{
			for(j=0;j<8;j++){
				cNode0T[j] = cNode2V[j];
			}
		}

		//The nodal length of node 1 is set to the shortest nodal length of either node 0 or node 2 in it's buffer
		if((cNLength[0]+cBLength[2])<=(cNLength[2]+cBLength[3])){
			for(j=0;j<8;j++){
				cNode1T[j] = cNode0V[j];
			}
		}
		else{
			for(j=0;j<8;j++){
				cNode1T[j] = cNode2V[j];
			}
		}

		//The nodal length of node 2 is set to the shortest nodal length of either node 3 or node 1 in it's buffer
		if((cNLength[1]+cBLength[4])<=(cNLength[3]+cBLength[5])){
			for(j=0;j<8;j++){
				cNode2T[j] = cNode1V[j];
			}
		}
		else{
			for(j=0;j<8;j++){
				cNode2T[j] = cNode3V[j];
			}
		}

		//The nodal length of node 3 is set to the shortest nodal length of either node 3 or node 1 in it's buffer
		if((cNLength[1]+cBLength[6])<=(cNLength[3]+cBLength[7])){
			for(j=0;j<8;j++){
				cNode3T[j] = cNode1V[j];
			}
		}
		else{
			for(j=0;j<8;j++){
				cNode3T[j] = cNode3V[j];
			}
		}

		for(j=0;j<8;j++){
			cNode3V[j] = cNode3T[j];
			cNode2V[j] = cNode2T[j];
			cNode1V[j] = cNode1T[j];
			cNode0V[j] = cNode0T[j];
		}

		//The next step in the path for each node is selected, from 000 to 111
		cNode0V[i] = ((cNLength[0]+cBLength[0])<=(cNLength[2]+cBLength[1])) ? 0:6;
		cNode1V[i] = ((cNLength[0]+cBLength[2])<=(cNLength[2]+cBLength[3])) ? 5:3;
		cNode2V[i] = ((cNLength[1]+cBLength[4])<=(cNLength[3]+cBLength[5])) ? 7:1;
		cNode3V[i] = ((cNLength[1]+cBLength[6])<=(cNLength[3]+cBLength[7])) ? 2:4;

		//Each Node's buffer is filled with it's next nodal length
		cNLTemp[0] = ((cNLength[0]+cBLength[0])<=(cNLength[2]+cBLength[1])) ? (cNLength[0]+cBLength[0]):(cNLength[2]+cBLength[1]);
		cNLTemp[1] = ((cNLength[0]+cBLength[2])<=(cNLength[2]+cBLength[3])) ? (cNLength[0]+cBLength[2]):(cNLength[2]+cBLength[3]);
		cNLTemp[2] = ((cNLength[1]+cBLength[4])<=(cNLength[3]+cBLength[5])) ? (cNLength[1]+cBLength[4]):(cNLength[3]+cBLength[5]);
		cNLTemp[3] = ((cNLength[1]+cBLength[6])<=(cNLength[3]+cBLength[7])) ? (cNLength[1]+cBLength[6]):(cNLength[3]+cBLength[7]);
		
		//The buffer values are now transfered over to the proper variables
		cNLength[0] = cNLTemp[0];
		cNLength[1] = cNLTemp[1];
		cNLength[2] = cNLTemp[2];
		cNLength[3] = cNLTemp[3];
	}

	//If node 0 has the shortest nodal length it is selected and the value is created from the node path
	if((cNLength[0]<cNLength[1]) & (cNLength[0]<cNLength[2]) & (cNLength[0]<cNLength[3])){
		if (DEBUG) {
			printf("Node0 Was Selected");
		}
		for(j=8;j>0;j--){
			if (DEBUG) {
				printf("Node0 Step Value: %x\n", cNodeF);
			}
			if((cNode0V[j-1] == 0) | (cNode0V[j-1] == 1) | (cNode0V[j-1] == 6) | (cNode0V[j-1] == 7)){
				cNodeF = cNodeF * 2;
			}
			else {
				cNodeF += 1;
				cNodeF = cNodeF * 2;
			}	
		}
	}
	

	//If node 1 has the shortest nodal length it is selected and the value is created from the node path
	else if((cNLength[1]<cNLength[0]) & (cNLength[1]<cNLength[2]) & (cNLength[1]<cNLength[3])){
		if (DEBUG) {
			printf("Node1 Was Selected");
		}
		for(j=8;j>0;j--){
			if (DEBUG) {
				printf("Node1 Step Value: %x\n", cNodeF);
			}
			if((cNode1V[j-1] == 0) | (cNode1V[j-1] == 1) | (cNode1V[j-1] == 6) | (cNode1V[j-1] == 7)){
				cNodeF = cNodeF * 2;
			}
			else {
				cNodeF += 1;
				cNodeF = cNodeF * 2;
			}
		}
	}

	//If node 2 has the shortest nodal length it is selected and the value is created from the node path
	else if((cNLength[2]<cNLength[1]) & (cNLength[2]<cNLength[0]) & (cNLength[2]<cNLength[3])){
		if (DEBUG) {
			printf("Node2 Was Selected");
		}
		for(j=8;j>0;j--){
			if (DEBUG) {
				printf("Node2 Step Value: %x\n", cNodeF);
			}
			if((cNode2V[j-1] == 0) | (cNode2V[j-1] == 1) | (cNode2V[j-1] == 6) | (cNode2V[j-1] == 7)){
				cNodeF = cNodeF * 2;
			}
			else {
				cNodeF += 1;
				cNodeF = cNodeF * 2;
			}
		}
	}

	//If none of the other nodes were selected node 3 has the shortest nodal length it is selected and the value is 
	//created from the node path
	else{
		if (DEBUG) {
			printf("Node3 Was Selected");
		}
		for(j=8;j>0;j--){
			if (DEBUG) {
				printf("Node3 Step Value: %x\n", cNodeF);
			}
			if((cNode3V[j-1] == 0) | (cNode3V[j-1] == 1) | (cNode3V[j-1] == 6) | (cNode3V[j-1] == 7)){
				cNodeF = cNodeF * 2;
			}
			else {
				cNodeF += 1;
				cNodeF = cNodeF * 2;
			}
		}
	}

	//As a for loop is used it will muliply by 2 once too many times, that is fixed here.
	cNodeF = cNodeF/2;

	printf("Character: %c\r\n", cNodeF);
	return cNodeF;
}


/*-----------------------------------------------------------*/
/* CLI Encode Function */

int viterbiencode(char cData) {

	//char cData;
	char cBytes[8];
	int cEncoded;
	char cPrevious[2] = {0,0};
	char i;
	//Reads typed value if not using entered value
	//scanf("%c", cData);

	//Seperates byte into bits
	cBytes[0] = (cData & 0x01);	
	cBytes[1] = !!(cData & 0x02);
	cBytes[2] = !!(cData & 0x04);
	cBytes[3] = !!(cData & 0x08);
	cBytes[4] = !!(cData & 0x10);
	cBytes[5] = !!(cData & 0x20);
	cBytes[6] = !!(cData & 0x40);
	cBytes[7] = !!(cData & 0x80);

	for(i=0; i<8; i++) {
		//Encodes each bit seperately
		cData = (((cBytes[i]+cPrevious[1])%2));
		cData += (((cPrevious[1]+cPrevious[0])%2)<<1);
		cData += (((cBytes[i] + cPrevious[1] + cPrevious[0])%2)<<2);
		cPrevious[0] = cPrevious[1];
		cPrevious[1] = cBytes[i];
		cBytes[i] = cData;
	}
	//Puts the different sets of data together
	cEncoded = 0;
	for(i=1; i<8; i++) {
		cEncoded += (cBytes[i] <<(3*i));
	}
	cEncoded += cBytes[0];
	printf("\r\n%02x\r\n", cEncoded);
	return cEncoded;
}
