//Jonathan Evans
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include "compiler.h"

char * reservedWords[13] = {"begin","end","if","then","while","do","call","const","var","procedure","write","read","else"};
char * lexemeList[500];
char * indentifierList[50];
int tokenCount;
int identListCount;

int GetReservedIndex(char * string);
void PrintTokenList();
int SearchIdentifierList(char * string);
void PrintLexemeTable();

lexeme * tokenList;

lexeme * lexical_analyzer(char * input)
{
    tokenList = malloc(sizeof(lexeme) * MAX_ARRAY_SIZE);
    char buffer[50];

    char c, next;
    int bufferCount = 0, hasNext = 0, inputCount = 0; 
    tokenCount = 0;
    identListCount = 0;

    printf("Lexeme Table:\n");
    printf("lexeme\t\ttoken type\n");

    while (1)
    {
        if(hasNext == 1)
        {
            bufferCount = 0;
            c = next;
            hasNext = 0;
            buffer[bufferCount] = next;
            bufferCount++;
        }
        else
        {
            bufferCount = 0;
            buffer[bufferCount] = input[inputCount];
            inputCount++;
            c = buffer[bufferCount];
            bufferCount++;
        }

        if(c == '\0')
        {
            break;
        }

        //Read into buffer until either white space or a special character is hit
        while (!isspace(c) && (isalpha(c) || isdigit(c)))
        {
            buffer[bufferCount] = input[inputCount];
            inputCount++;
            c = buffer[bufferCount];
            bufferCount++;
        }
        buffer[bufferCount-1] = '\0';

        //Check if next value read in is a reserved word
        int reservedIndex = GetReservedIndex(buffer);
        if (reservedIndex != -1)
        {   
            switch (reservedIndex)
            {
                case 0:
                    //begin
                    tokenList[tokenCount].type = beginsym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;
                
                case 1:
                    //end
                    tokenList[tokenCount].type = endsym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;

                case 2:
                    //if
                    tokenList[tokenCount].type = ifsym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;

                case 3:
                    //then
                    tokenList[tokenCount].type = thensym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;

                case 4:
                    //while
                    tokenList[tokenCount].type = whilesym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;

                case 5:
                    //do
                    tokenList[tokenCount].type = dosym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;
                
                case 6:
                    //call
                    tokenList[tokenCount].type = callsym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;

                case 7:
                    //const
                    tokenList[tokenCount].type = constsym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;
                
                case 8:
                    //var
                    tokenList[tokenCount].type = varsym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;
                
                case 9:
                    //procedure
                    tokenList[tokenCount].type = procsym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;
                
                case 10:
                    //write
                    tokenList[tokenCount].type = writesym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;

                case 11:
                    //read
                    tokenList[tokenCount].type = readsym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;

                case 12:
                    //else
                    tokenList[tokenCount].type = elsesym;
                    strcpy(tokenList[tokenCount].name, buffer);
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    break;
                
                default:
                    break;
            
            }
        }
        else
        {
            //Check if first character is a letter
            if(isalpha(buffer[0]))
            {

                //Check that the identifier is a valid length
                int identCount = 0;
                while (buffer[identCount] != '\0')
                {
                    identCount++;
                }

                if (identCount > 11)
                {
                    tokenList[tokenCount].error_type = 2;
                    tokenCount++;
                }
                else
                {
                    //Check if identifier is in identifier list
                    if(SearchIdentifierList(buffer) == -1)
                    {
                        //Add identifier to list
                        indentifierList[identListCount] = malloc(sizeof(char) * strlen(buffer));
                        strcpy(indentifierList[identListCount], buffer);

                        tokenList[tokenCount].type = identsym;
                        strcpy(tokenList[tokenCount].name, buffer);
                        tokenList[tokenCount].error_type = 0;
                        tokenCount++;
                        identListCount++;
                    }
                    else
                    {
                        tokenList[tokenCount].type = identsym;
                        strcpy(tokenList[tokenCount].name, indentifierList[SearchIdentifierList(buffer)]);
                        tokenList[tokenCount].error_type = 0;
                        tokenCount++;
                    }
                }
            }
            else if(isdigit(buffer[0]))
            {
                //Check that the number is a valid length
                int numCount = 0, numError = 0;
                while (isdigit(buffer[numCount]))
                {
                    numCount++;
                }

                if(isalpha(buffer[numCount]))
                {
                    tokenList[tokenCount].error_type = 1;
                    tokenCount++;
                }
                else
                {
                    if (numCount > 5)
                    {
                        numError = 1;
                    }

                    if(numError == 1)
                    {
                        tokenList[tokenCount].error_type = 1;
                    }
                    else
                    {
                        tokenList[tokenCount].type = numbersym;
                        strcpy(tokenList[tokenCount].name, buffer);
                        tokenList[tokenCount].value = atoi(buffer);
                        tokenList[tokenCount].error_type = 0;
                    }
                    tokenCount++;
                }
            }
        }

        //Special characters
        if(!(isdigit(c) || isalpha(c)) && !isspace(c))
        {
            switch (c)
            {
            case '+':
                //Plus symbol
                buffer[50] = input[inputCount];
                inputCount++;
                next = buffer[50];
                if(next == '=')
                {
                    tokenList[tokenCount].type = addassignsym;
                    strcpy(tokenList[tokenCount].name, "+=");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                }
                else
                {
                    tokenList[tokenCount].type = plussym;
                    strcpy(tokenList[tokenCount].name, "+");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    hasNext = 1;
                }
                break;

            case '-':
                //Minus symbol
                buffer[50] = input[inputCount];
                inputCount++;
                next = buffer[50];
                if(next == '=')
                {
                    tokenList[tokenCount].type = subassignsym;
                    strcpy(tokenList[tokenCount].name, "-=");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                }
                else
                {
                    tokenList[tokenCount].type = minussym;
                    strcpy(tokenList[tokenCount].name, "-");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    hasNext = 1;
                }
                break;

            case '*':
                //Mult symbol
                buffer[50] = input[inputCount];
                inputCount++;
                next = buffer[50];
                if(next == '=')
                {
                    tokenList[tokenCount].type = timesassignsym;
                    strcpy(tokenList[tokenCount].name, "*=");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                }
                else
                {
                    tokenList[tokenCount].type = multsym;
                    strcpy(tokenList[tokenCount].name, "*");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    hasNext = 1; 
                }
                break;

            case '/':
                //Slash symbol
                buffer[50] = input[inputCount];
                inputCount++;
                next = buffer[50];
                if(next == '*')
                {
                    buffer[1] = input[inputCount];
                    inputCount++;
                    while (input[inputCount] != '\0' && buffer[0] != '*' && buffer[1] != '/')
                    {
                        buffer[0] = buffer[1];
                        buffer[1] = input[inputCount];
                        inputCount++;
                        if(input[inputCount - 1] == '\0')
                        {
                            tokenList[tokenCount].error_type = 5;
                            tokenCount++;
                        }
                    }
                }
                else if(next == '=')
                {
                    tokenList[tokenCount].type = divassignsym;
                    strcpy(tokenList[tokenCount].name, "/=");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                }
                else
                {
                    tokenList[tokenCount].type = slashsym;
                    strcpy(tokenList[tokenCount].name, "/");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    hasNext = 1;
                }
                break;

            case '%':
                //Mod symbol
                buffer[50] = input[inputCount];
                inputCount++;
                next = buffer[50];
                if(next == '=')
                {
                    tokenList[tokenCount].type = modassignsym;
                    strcpy(tokenList[tokenCount].name, "%=");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                }
                else
                {
                    tokenList[tokenCount].type = modsym;
                    strcpy(tokenList[tokenCount].name, "%");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    hasNext = 1;
                }
                break;

            case '=':
                //EQL symbol
                tokenList[tokenCount].type = eqlsym;
                strcpy(tokenList[tokenCount].name, "=");
                tokenList[tokenCount].error_type = 0;
                tokenCount++;
                break;

            case '!':
                //NEQL symbol
                buffer[50] = input[inputCount];
                inputCount++;
                next = buffer[50];
                if(next == '=')
                {
                    tokenList[tokenCount].type = neqsym;
                    strcpy(tokenList[tokenCount].name, "!=");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                }
                else
                {
                    tokenList[tokenCount].error_type = 3;
                    tokenCount++;
                }
                break;

            case '<':
                //Less symbol
                buffer[50] = input[inputCount];
                inputCount++;
                next = buffer[50];
                if(next == '=')
                {
                    tokenList[tokenCount].type = leqsym;
                    strcpy(tokenList[tokenCount].name, "<=");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                }
                else
                {
                    tokenList[tokenCount].type = lessym;
                    strcpy(tokenList[tokenCount].name, "<");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    hasNext = 1;
                }
                break;

            case '>':
                //GTR symbol
                buffer[50] = input[inputCount];
                inputCount++;
                next = buffer[50];
                if(next == '=')
                {
                    tokenList[tokenCount].type = geqsym;
                    strcpy(tokenList[tokenCount].name, ">=");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                }
                else
                {
                    tokenList[tokenCount].type = gtrsym;
                    strcpy(tokenList[tokenCount].name, ">");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                    hasNext = 1;
                }
                break;

             case '(':
                //LParent symbol
                tokenList[tokenCount].type = lparentsym;
                strcpy(tokenList[tokenCount].name, "(");
                tokenList[tokenCount].error_type = 0;
                tokenCount++;
                break;

             case ')':
                //RParent symbol
                tokenList[tokenCount].type = rparentsym;
                strcpy(tokenList[tokenCount].name, ")");
                tokenList[tokenCount].error_type = 0;
                tokenCount++;
                break;
            
             case ',':
                //Comma symbol
                tokenList[tokenCount].type = commasym;
                strcpy(tokenList[tokenCount].name, ",");
                tokenList[tokenCount].error_type = 0;
                tokenCount++;
                break;

             case ';':
                //Semicolon symbol
                tokenList[tokenCount].type = semicolonsym;
                strcpy(tokenList[tokenCount].name, ";");
                tokenList[tokenCount].error_type = 0;
                tokenCount++;
                break;

             case '.':
                //Period symbol
                tokenList[tokenCount].type = periodsym;
                strcpy(tokenList[tokenCount].name, ".");
                tokenList[tokenCount].error_type = 0;
                tokenCount++;
                break;

             case ':':
                //Becomes symbol
                buffer[50] = input[inputCount];
                inputCount++;
                next = buffer[50];
                if(next == '=')
                {
                    tokenList[tokenCount].type = becomessym;
                    strcpy(tokenList[tokenCount].name, ":=");
                    tokenList[tokenCount].error_type = 0;
                    tokenCount++;
                }
                else
                {
                    tokenList[tokenCount].error_type = 3;
                    tokenCount++;
                }
                break;
            
            default:
                if(c > 0)
                {
                    tokenList[tokenCount].error_type = 3;
                    tokenCount++;
                    break;
                }
            }
        }
    }
    
    PrintLexemeTable();
    PrintTokenList();

    return tokenList;
}

int GetReservedIndex(char * string)
{
    for(int i = 0; i < 13; i++)
    {
        if(strcmp(string, reservedWords[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

void PrintTokenList()
{
    printf("\nToken List:\n");

    for (int i = 0; i < tokenCount; i++)
    {
        if(tokenList[i].error_type == 0)
        {
            printf("%d ", (int)tokenList[i].type);
            if(tokenList[i].type == identsym)
            {
                printf("%s ", tokenList[i].name);
            }
            if(tokenList[i].type == numbersym)
            {
                printf("%d ", tokenList[i].value);
            }
        }
    }
    printf("\n");

}

int SearchIdentifierList(char * string)
{
    for(int i = 0; i < identListCount; i++)
    {
        if (strcmp(string, indentifierList[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

void PrintLexemeTable()
{
    for (int i = 0; i < tokenCount; i++)
    {
        if(tokenList[i].error_type == 0)
        {
            if(tokenList[i].type == identsym)
            {
                printf("%s ", tokenList[i].name);
            }
            else if(tokenList[i].type == numbersym)
            {
                printf("%d ", tokenList[i].value);
            }
            else
            {
                printf("%s ", tokenList[i].name);
            }
            printf("\t\t%d\n", (int)tokenList[i].type);
        }
        else
        {
            switch (tokenList[i].error_type)
            {
            case 1:
                printf("Lexical Analyzer Error: Invalid Identifier\n");
                break;

            case 2:
                printf("Lexical Analyzer Error: Identifier Length\n");
                break;

            case 3:
                printf("Lexical Analyzer Error: Invalid Symbol\n");
                break;

            case 4:
                printf("Lexical Analyzer Error: Number Length\n");
                break;

            case 5:
                printf("Lexical Analyzer Error: Never-ending comment\n");
                break;
            
            default:
                break;
            }
        }
    }
}