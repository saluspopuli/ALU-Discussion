// ===========================================================================================
//                                       == ALU.c ==
//          Team 6:
//              Berondo, Jamille
//              Cortez, Andre
//              Francis, Mohan Nuelle
//              Gere, Rodjean
//
//          Description: 
//              C code that emulates the basic functions of an ALU
//
//          Revision History:
//              1.0 - Created ALU function with addition and subtraction
//              1.1 - Added MUL function
//              1.2 - Added bitwise operations
//              1.3 - Added documentation
//
//          Last Revision Date:
//              February 28, 2024
//
// ===========================================================================================
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

    unsigned int tmp;
    
    // ADDITION AND SUBTRACTION TEST CASES
    tmp = ALU(0b00000011, 0b00000101, 0x02);
    printf("\nALU: ");
    printBin(tmp, 16);
    printf("\n");
    printf("=====================================");

    tmp = ALU(-120, -123, 0x01);
    printf("\nALU: ");
    printBin(tmp, 16);
    printf("\n");
    printf("=====================================");
    

    //MUL TEST CASES
    printBin(ALU(0b11000000, 0b00001010, 0x03), 16);
    printf("\n");
    printFlags();
    printf("=====================================");
    printf("\n");

    printBin(ALU(0b01000110, 0b00000010, 0x03), 16);
    printf("\n");
    printFlags();
    printf("=====================================");
    printf("\n");

    return 0;
}

// FUNCTIONS =======================================================================================

// =================================================================================================
// ALU
// Description: Function that emulates the basic operation of an ALU
//
// INPUT:
// - unsigned char operand1 = Stores the first operand as an 8-bit value
// - unsigned char operand2 = Stores the second operand as an 8-bit value
// - unsigned char control_signal = Stores the control signal that determines the type of operation
// OUTPUT:
// - ACC = returns an unsigned int which is the result of the operations done
// =================================================================================================
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal) {
    unsigned char temp_OP1 = 0x00, temp_OP2 = 0x00, temp_prod = 0x0000;
    unsigned int n = 0, Q_n1 = 0;
   
    // Setting ACC and flags to initial values
    ACC = 0x0000; SF = 0, CF = 0, ZF = 0, OF = 0;       

    printf("\nFetching operands...\n");
    printf("OP1: ");
    printBin(operand1, 8);
    printf("\n");
    printf("OP2: ");
    printBin(operand2, 8);

    if (control_signal == 0x01 || control_signal == 0x02) { // ADD or SUB

        temp_OP1 = operand1;

        if (control_signal == 0x02){            // Checks if operation is Subtraction
            
            temp_OP2 = twosComp(operand2);
            
            printf("\nOperation: SUB");
            printf("\n2's Complement OP2\n");
        }   
        else {                                  // If not subtraction, just set operand2 to temp_OP2
            
            temp_OP2 = operand2;

            printf("\nOperation: ADD\n");
        }
        // 8-bit adder
        printf("Adding OP1 & OP2");
        
        ACC = temp_OP1 + temp_OP2;

    } else if (control_signal == 0x03){ // MUL 

        temp_OP2 = operand2;
        
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
    
    return(ACC);
}

// =================================================================================================
// setFlags
// Description: Function that changes flag values based on ALU results, function is only used within
//              the ALU function
//
// INPUT:
// - unsigned int ACC = The result of ALU functions
// =================================================================================================
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

// =================================================================================================
// printFlags
// Description: Utility function that just prints the flags              
// =================================================================================================
void printFlags(){

    printf("ZF = %c, SF = %c, OF = %c, CF = %c\n", ZF + '0', SF + '0', OF + '0', CF + '0');

    return;
}

// =================================================================================================
// twosComp
// Description: Function that performs twos complement on a given operand
//
// INPUT:
// - unsigned char operand = operand that is to be 2's complemented.
// OUTPUT:
// - operand = The operand that is now 2's complemented
// =================================================================================================
unsigned char twosComp(unsigned char operand){
    operand = ~operand; // Operand is bitwise inverted
    operand = operand + 1; // 1 is added to finish twos complement
    
    return operand;
}

// =================================================================================================
// printBin
// Description: Utility function that prints the binary of given data
//
// INPUT:
// - unsigned int data = Given data that is to be printed
// - unsigned char data_width = The number of bits to be printed from the data
// =================================================================================================
void printBin(unsigned int data, unsigned char data_width){
    
    unsigned char tmp_data;

    for (int i = data_width-1; i >= 0; i--){    
        tmp_data = (data >> i) & 0x01;

        printf("%c", tmp_data + '0');
    }

    return;
}
