#include <msp430f5529.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * main.c
 */
typedef struct _som_node_{
	double elements[4];
} Node;
typedef struct _data_{
	double attribute[4];
	int tag;
} Data;
/*************************************
 * MSP430 Variable
 *************************************/
	// Buffer for serial receiver
	char 				inputArray[11];
	// All purpose iterator
	volatile int 		it  			= 0;
	volatile int		itt				= 0;
	volatile int		ptr				= 0; //Pointer for serial input
	volatile int 		input_enable	= 1; //Input Enable for Serial comm
/*************************************
 * Global constant and Variable
 *************************************/
	unsigned const int  ROW             = 15;
    unsigned const int  COL             = 15;
    unsigned int        iteration_count = 0;
    const int           element_count   = 4;
	int 				x				= 0;
	int 				y				= 0;
	int 				result			= 0;
/*************************************
 * Data Structure for a node
 *************************************/
	//NOTHING HERE
/*************************************
 * Function
 *************************************/

//UART SERIAL COMMUNICATION //////////////////
void uart_putchar(char input){
	while (!(UCA1IFG&UCTXIFG));
	UCA1TXBUF = input;
}
void uart_newline(){
	uart_putchar('\n');
	uart_putchar('\r');
}
void sendACK(){
	uart_putchar('@');
	uart_newline();
	ptr=0;
	input_enable = 1;
		//CLEAR BUFFER FOR NEXT INPUT
		inputArray[0] = ' ';
		inputArray[1] = ' ';
		inputArray[2] = ' ';
		inputArray[3] = ' ';
		inputArray[4] = ' ';
		inputArray[5] = ' ';
		inputArray[6] = ' ';
		inputArray[7] = ' ';
		inputArray[8] = ' ';
		inputArray[9] = ' ';
		inputArray[10] = ' ';
		//TURN LED OFF
		P1OUT = 0b00000000;
}


void initialize(){
	 // P3.3,4 = USCI_A1 TXD/RXD (USB TX RX on PORT 3.3,3.4)
	 P4SEL |= BIT5+BIT4;
	 // Reset UCA1CTL1
	 UCA1CTL1 |= UCSWRST;
	 // Select Source For UART => SMCLK (1.5Mhz)
	 UCA1CTL1 |= UCSSEL_3;
	 // Set Baud rate
	 UCA1BR1 = 0x00; UCA1BR0 = 0x6D; // 1.5MHz/9600=0x006D
	 UCA1MCTL |= UCBRS_2 + UCBRF_0; // Modulation UCBRSx=2, UCBRFx=0 (FROM DATASHEET)
	 UCA1CTL1 &= ~UCSWRST; // Initialize the state machine
	 // Enable USCI_A1 RX interrupt
	 UCA1IE |= UCRXIE;
	 // LED Initialization
	 P1DIR |= 0x01;
	 P4DIR |= 0x80;
	 P1OUT = 0b00000000;
	 P4OUT = 0b00000000;
}

double euclidian_distance_point_square(int x1, int y1 , int x2 , int y2 ){
	return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
}

double euclidian_distance_vector_squre(double input1[] , double input2[]){
	int i;
	double dist;
	for(i = 0 ; i < element_count ; i++){
		dist += (input1[i] - input2[i])*(input1[i] - input2[i]);
	}
	return dist;
}

void blinking(int times){
	int t;
	for (t = 0 ; t < times ; t++){
		P1OUT = 0b00000001;
		_delay_cycles(500000);
		P1OUT = 0b00000000;
		_delay_cycles(500000);

	}
}


/*************************************
 * Main Function
 *************************************/
int main(void) {
	// Stop watchdog timer
    	WDTCTL = WDTPW | WDTHOLD;
    // Initialize Ports
    	initialize();
	// Receiver Interrupt
		__bis_SR_register(GIE); //interrupts enabled
		__no_operation();

////////////////////////////////////////////////////////////////////
		// Receive RAW Data via serial
			Node som_map[15][15];
		    int plotty[15][15];
			int i , j , e;
			//Trained Network Data
				for(i = 0 ; i < ROW ; i++){
					for(j = 0 ; j < COL ; j++){
						for(e=0 ; e < element_count ; e++){
							while(input_enable == 1);
								som_map[i][j].elements[e] = atof(inputArray);
							sendACK();
						}
					}
				}
			//Classification Array (position of trained data)
				for(i = 0 ; i < ROW ; i++){
					for(j = 0 ; j < COL ; j++){
						while(input_enable == 1);
							plotty[i][j] = atoi(inputArray);
						sendACK();
					}
				}
		//GREEN LED SHOW THAT MODEL HAS FINISHED LOADING
			P4OUT = 0b10000000;//GREEN[7]-ON
			P1OUT = 0b00000000;//RED[0]-OFF
////////////////////////////////////////////////////////////////////

		//Q&A Process
		double minimum = 10000;
		double temp = 0;
		double question[4];

		while(1){
			P1OUT = 0b00000000;

			//Receive Question
			for(e = 0 ; e < element_count ; e++){
				while (input_enable == 1);
					question[e] = atof(inputArray);
				sendACK();
			}
			//Find winner node
			for (i = 0; i < ROW; i++) {
				for (j = 0; j < COL; j++) {
					temp = euclidian_distance_vector_squre(question,som_map[i][j].elements);
					if (temp < minimum) {
						minimum = temp;
						x = j;
						y = i;
					}
				}
			}
				minimum = 10000;
				temp = 0;
			//See if the winner node located near which trained data
				for (i = 0; i < ROW; i++) {
					for (j = 0; j < COL; j++) {
						if(plotty[i][j] != 0 ){
							temp = euclidian_distance_point_square(x,y,j,i);
							if(temp < minimum){
								result = plotty[i][j];
								minimum = temp;
							}
						}
					}
				}
			//Blink the LED according to the class number
				blinking(result);
			//FULL LED
				P4OUT = 0b10000000;
				P1OUT = 0b00000001;
				minimum = 10000;
				temp = 0;
				x=0;
				y=0;
		}
}


// RECEIVER ( INTERRUPT ON RECEIVE )
// This is the RX UART ISR and is entered when the RX buffer is full
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void){
	//Input
	while (!(UCA1IFG&UCTXIFG)); // Is the USCI_A0 TX buffer ready?
		//Return text to Screen
	UCA1TXBUF = UCA1RXBUF;
	if(input_enable){
		if(UCA1RXBUF == ','){
			input_enable = 0;
			P1OUT = 0b00000001;
		}else{
			inputArray[ptr++] = UCA1RXBUF;
		}
	}else{
		P1OUT = 0b00000000;
	}
}
