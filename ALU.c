#include <stdio.h>

// GLOBAL VARIABLES ==========================================================================
unsigned char SF, OF, ZF, CF;
unsigned int ACC;

// PROTOTYPES ================================================================================
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal);
void setFlags (unsigned int ACC);
unsigned char twosComp(unsigned char operand);
void printBin(unsigned char data, unsigned char data_width);

// MAIN FUNCTION =============================================================================
int main(){

    

    return 0;
}

// FUNCTIONS =================================================================================
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal) {
    unsigned char temp_OP1 = 0x00, temp_OP2 = 0x00;

    // Setting ACC and flags to initial values
    ACC = 0x0000; SF = 0, CF = 0, ZF = 0, OF = 0;

    if (control_signal == 0x01 || control_signal == 0x02) { // ADD or SUB

        // Sign and Operation Check Logic
        temp_OP1 = operand1;

        if (control_signal == 0x02)
            temp_OP2 = twosComp(operand2);
        else
            temp_OP2 = operand2;

        // 8-bit adder
        ACC = temp_OP1 + temp_OP2;

    } else if (control_signal == 0x02){ // MUL
        //Mohan's code here
    }

    setFlags(ACC);

    return(ACC);
}

void setFlags (unsigned int ACC){
    
    // CHECKING IF ACC IS ZERO
    if (ACC == 0x0000)
        ZF = 1;
    else
        ZF = 0;

    // CHECKING IF SIGN BIT IS NEGATIVE
    if ((ACC & 0x0080) == 0x0080)
        SF = 1;
    else
        SF = 0;

    // CHECKING IF ACC IS GREATER THAN 127 (Last positive number)
    if (ACC > 0x7F)
        OF = 1;
    else
        OF = 0;
    
    // CHECKING IF ACC IS GREATER THAN 255 (Beyond 8-bit range)
    if (ACC > 0xFF)
        CF = 1;
    else
        CF = 0;
}

unsigned char twosComp(unsigned char operand){
    operand = ~operand; // Operand is bitwise inverted
    operand = operand + 1; // 1 is added to finish twos complement
    
    return operand;
}

void printBin(unsigned char data, unsigned char data_width){
    
    unsigned char tmp_data;

    for (int i = 0; i < data_width; i++){     
        tmp_data = data & 0x90;
        tmp_data = data >> 7;

        printf("%c", tmp_data + '0');

        data = data << 1;
    }

    return;
}