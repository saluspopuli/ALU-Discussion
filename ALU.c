#include <stdio.h>

// GLOBAL VARIABLES ==========================================================================
unsigned char SF, OF, ZF, CF;
unsigned int ACC;

// PROTOTYPES ================================================================================
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal);
void setFlags (unsigned int ACC);
void printFlags ();
unsigned char twosComp(unsigned char operand);
void printBin(unsigned int data, unsigned char data_width);

// MAIN FUNCTION =============================================================================
int main(){

    // 4 + 1 = 5 (00000101)
    printBin(ALU(0b00000100, 0b00000001, 0x01), 16);
    printf("\n");
    printFlags();
    printf("=====================================");
    printf("\n");

    // 255 + 255 = 510 (idk)
    printBin(ALU(255, 255, 0x01), 16);
    printf("\n");
    printFlags();
    printf("=====================================");
    printf("\n");

    // 10 - 5 = 5 (00000101)
    printBin(ALU(10, 5, 0x02), 16);
    printf("\n");
    printFlags();
    printf("=====================================");
    printf("\n");

    // 56 - 123 = -67 (10111101)
    printBin(ALU(56, 123, 0x02), 16);
    printf("\n");
    printFlags();
    printf("=====================================");
    printf("\n");

    // 255 - 255 = 0
    printBin(ALU(255, 255, 0x02), 16);
    printf("\n");
    printFlags();
    printf("=====================================");
    printf("\n");

    // 28 + (-28) = 0
    printBin(ALU(28, 0b11100100, 0x01), 16);
    printf("\n");
    printFlags();
    printf("=====================================");
    printf("\n");

    //test output from the pdf
    printBin(ALU(0b11000000, 0b00001010, 0x03), 16);
    printf("\n");
    printFlags();
    printf("=====================================");
    printf("\n");

    return 0;
}

// FUNCTIONS =================================================================================
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal) {
    unsigned char temp_OP1 = 0x00, temp_OP2 = 0x00, temp_prod = 0x0000;
    unsigned int n = 0, Q_n1 = 0;
   
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

    } else if (control_signal == 0x03){ // MUL 

        temp_OP2 = operand2;
        printf("Fetching operands...\n");
        printf("OP1: ");
        printBin(operand1, 8);
        printf("\n");
        printf("OP2: ");
        printBin(operand2, 8);
        printf("\nOperation: MUL\n");
        printf("            A               Q          Q_n1        M            n\n");
        for(n = 0; n < 9; n++){

            printf("\t");
            printBin(temp_prod, 8);
            printf("\t");
            printBin(temp_OP2, 8);
            printf("\t");
            printf("%d", Q_n1);
            printf("\t");
            printBin(operand1, 8); 
            printf("\t");
            printf("%d", n);
            printf("\n");
            
            //check Q, Q_n1 conditions
            unsigned int MSB_A = (temp_prod >> 8) & 0x01; // most significant bit of temp_prod (A)
            unsigned int LSB_A = temp_prod & 0x01; // least significant bit of temp_prod (A)
            unsigned int LSB_Q = temp_OP2 & 0x01; // least significant bit of temp_OP2 (Q)
 
            if(LSB_Q > Q_n1){   

                temp_OP1 = twosComp(operand1);             // 10
                temp_prod = temp_OP1 + temp_prod; // A <-- A - M

            } else if(LSB_Q < Q_n1){  
                              // 01
                temp_prod = operand1 + temp_prod ; // A <-- A + M
            }
                //Arithmetic Shift Right
            MSB_A = (temp_prod >> 7) & 0x01; // most significant bit of temp_prod (A)
            LSB_A = temp_prod & 0x01; // least significant bit of temp_prod (A)
            LSB_Q = temp_OP2 & 0x01;
            temp_prod = temp_prod >> 1;
            temp_OP2 = temp_OP2 >> 1;

            temp_OP2 = temp_OP2 | (LSB_A << 7); // LSB of temp_prod (A) assigned to MSB of temp_OP2 (Q)

            Q_n1 = LSB_Q; // Q_n1 receives LSB of temp_OP2
                
            temp_prod = temp_prod | (MSB_A << 7); // LSB of temp_prod (A) assigned to MSB of temp_prod (A) afer shifting

        }
        ACC = temp_prod << 8 | temp_OP2;

    } else if (control_signal == 0x04){ // AND

        ACC = temp_OP1 & temp_OP2;

    } else if (control_signal == 0x05){ // OR

        ACC = temp_OP1 | temp_OP2;

    } else if (control_signal == 0x06){ // NOT

        ACC = ~temp_OP1;
        
    } else if (control_signal == 0x07){ // XOR

        ACC = temp_OP1 ^ temp_OP2;
        
    } else if (control_signal == 0x08){ // SHIFT RIGHT

        ACC = temp_OP1 >> temp_OP2;
        
    } else if (control_signal == 0x09){ // SHIFT LEFT
        
        ACC = temp_OP1 << temp_OP2;
        
    }
    
    setFlags(ACC);

    if (control_signal == 0x01 || control_signal == 0x02)
        ACC = ACC & 0x00FF;
    
    return(ACC);
}

void setFlags (unsigned int ACC){
    
    unsigned char tmp_ACC = ACC;

    // CHECKING IF ACC IS ZERO
    if (tmp_ACC == 0x0000)
        ZF = 1;
    else
        ZF = 0;

    // CHECKING IF SIGN BIT IS NEGATIVE
    if ((ACC & 0x0080) == 0x0080)
        SF = 1;
    else
        SF = 0;

    // CHECKING IF ACC IS GREATER THAN 127 (Last positive number)
    if (tmp_ACC > 0x7F)
        OF = 1;
    else
        OF = 0;
    
    // CHECKING IF ACC IS GREATER THAN 255 (Beyond 8-bit range)
    if (ACC > 0xFF && ZF == 0)
        CF = 1;
    else
        CF = 0;
}

void printFlags(){

    printf("ZF = %c, SF = %c, OF = %c, CF = %c\n", ZF + '0', SF + '0', OF + '0', CF + '0');

    return;
}

unsigned char twosComp(unsigned char operand){
    operand = ~operand; // Operand is bitwise inverted
    operand = operand + 1; // 1 is added to finish twos complement
    
    return operand;
}

void printBin(unsigned int data, unsigned char data_width){
    
    unsigned char tmp_data;

    for (int i = data_width-1; i >= 0; i--){    
        tmp_data = (data >> i) & 0x01;

        printf("%c", tmp_data + '0');
    }

    return;
}
