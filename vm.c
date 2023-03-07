//Jonathan Evans
#include<stdio.h>
#include<stdlib.h>
#include "compiler.h"

//The program counter, stack pointer, and base pointer registers represented by ints
int pc = 0, sp = 0, bp = 0, halt = 1;

//Instructions stored in the instruction register divided into 3 ints to 
//represent the op code, L, and M
int op = 0, L = 0, M = 0;

const int MAX_PAS_LENGTH = 500;

int pas[500] = {0};

int textLength;

int base(int L);
void PrintExecution(char * instructionName);
void PrintStack(int bp, int sp, int stackEnd);


void virtual_machine(instruction * code)
{
    int instructionIndex = 0;

    //Load instructions into pas
    textLength = 0;
 
    while (code[instructionIndex].op != -1)
    { 
        pas[textLength] = code[instructionIndex].op;
        pas[textLength + 1] = code[instructionIndex].l;
        pas[textLength + 2] = code[instructionIndex].m;

        textLength = textLength + 3;
        instructionIndex++;   
    }

    printf("VM Execution:\n");
    printf("\t  PC\tBP\tSP\tstack\n");

    bp = textLength;
    sp = bp - 1;

    printf("Initial Values: %d %d %d\n\n", pc, bp, sp);

    while (halt == 1)
    {
        //Fetch
        op = pas[pc];
        L = pas[pc + 1];
        M = pas[pc + 2];
        pc = pc + 3;

        //Execute
        switch (op)
        {
        case 1:
            //Literal
            sp = sp + 1;
            pas[sp] = M;
            PrintExecution("LIT");
            break;

        case 2:
            //OPR - Arimetic operations
            switch (M)
            {
            case 0:
                //Return
                sp = bp - 1;
                bp = pas[sp + 2];
                pc = pas[sp + 3];
                PrintExecution("RTN");
                break;

            case 1:
                //Negative
                pas[sp] = -1 * pas[sp];
                PrintExecution("ADD");
                break;

            case 2:
                //Add
                sp = sp - 1;
                pas[sp] = pas[sp] + pas[sp + 1];
                PrintExecution("ADD");
                break;

            case 3:
                //Subtract
                sp = sp - 1;
                pas[sp] = pas[sp] - pas[sp + 1];
                PrintExecution("SUB");
                break;

            case 4:
                //Multiply
                sp = sp - 1;
                pas[sp] = pas[sp] * pas[sp + 1];
                PrintExecution("MUL");
                break;

            case 5:
                //Divide
                sp = sp - 1;
                pas[sp] = pas[sp] / pas[sp + 1];
                PrintExecution("DIV");
                break;

            case 6:
                sp = sp - 1;
                pas[sp] = pas[sp] % pas[sp + 1];
                PrintExecution("MOD");
                break;

            case 7:
                //Equals
                sp = sp - 1;
                pas[sp] = pas[sp] == pas[sp + 1];
                PrintExecution("EQL");
                break;

            case 8:
                //Not Equal
                sp = sp - 1;
                pas[sp] = pas[sp] != pas[sp + 1];
                PrintExecution("NEQ");
                break;

            case 9:
                //Less Than
                sp = sp - 1;
                pas[sp] = pas[sp] < pas[sp + 1];
                PrintExecution("LSS");
                break;

            case 10:
                //Less Than or Equal to
                sp = sp - 1;
                pas[sp] = pas[sp] <= pas[sp + 1];
                PrintExecution("LEQ");
                break;

            case 11:
                //Greater Than
                sp = sp - 1;
                pas[sp] = pas[sp] > pas[sp + 1];
                PrintExecution("GTR");
                break;

            case 12:
                //Greater Than or Equal to
                sp = sp - 1;
                pas[sp] = pas[sp] >= pas[sp + 1];
                PrintExecution("GEQ");
                break;
            
            default:
                break;
            }

            break;

        case 3:
            //Load
            sp = sp + 1;
            pas[sp] = pas[base(L) + M];
            PrintExecution("LOD");
            break;

        case 4:
            //Store
            pas[base(L) + M] = pas[sp];
            sp = sp - 1;
            PrintExecution("STO");
            break;

        case 5:
            //Call
            pas[sp + 1] = base(L); //Stores Static Link
            pas[sp + 2] = bp; //Stores Dynamic Link
            pas[sp + 3] = pc; //Stores Return Address

            bp = sp + 1;
            pc = M;
            PrintExecution("CAL");
            break;

        case 6:
            //Increment
            sp = sp + M;
            PrintExecution("INC");
            break;

        case 7:
            //Jump
            pc = M;
            PrintExecution("JMP");
            break;

        case 8:
            //JPC
            if(pas[sp] == 0)
            {
                pc = M;
            }
            sp = sp - 1;
            PrintExecution("JPC");
            break;

        case 9:
            //SYS - System Calls
            switch (M)
            {
            case 1:
                //Read
                printf("Output result is: %d\n", pas[sp]);
                sp = sp - 1;
                PrintExecution("SYS");
                break;

            case 2:
                //Write
                sp = sp + 1;
                printf("Please Enter an Integer: ");
                scanf("%d", &pas[sp]);
                PrintExecution("SYS");
                break;

            case 3:
                //Halt - Ends program
                halt = 0;
                PrintExecution("SYS");
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }
    }
    printf("\n");
}

void PrintExecution(char * instructionName)
{
    printf("%s %d %d   %d\t%d\t%d\t", instructionName, L, M, pc, bp, sp);
    PrintStack(bp, sp, sp);
    printf("\n");
}

void PrintStack(int bp, int sp, int stackEnd)
{
    if(bp <= textLength + 1)
    {
        for(int i = bp; i < sp + 1; i++)
        {
            printf("%d ", pas[i]);
        }
        return;
    }

    PrintStack(pas[bp + 1], bp - 1, stackEnd);

    if (sp > bp)
    {
        printf("| ");
    }

    for(int i = bp; i < sp + 1; i++)
    {
        printf("%d ", pas[i]);
    }
    
    return;
}

int base(int L)
{
    int arb = bp; // arb = activation record base
    while ( L > 0)     //find base L levels down
    {
        arb = pas[arb];
        L--;
    }
    return arb;
}