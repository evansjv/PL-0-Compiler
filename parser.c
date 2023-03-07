//Jonathan Evans
/*
	skeleton for Dr. Montagne's Systems Software project Summer 2022
	you may alter all of this code if desired, but you must provide clear
	instructions if you alter the compilation process or wish us to
	use different files during grading than the ones provided 
	(driver.c, compiler.h, vm.o/.c, or lex.o/.c)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

lexeme *tokens;
int lIndex = 0;
instruction *code;
int cIndex = 0;
symbol *table;
int tIndex = 0;

int level;

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void mark();
int multipledeclarationcheck(char name[]);
int findsymbol(char name[], int kind);

void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

void Program();
void Block();
void Statement();
void ConstDeclaration();
int VarDeclaration();
void ProcedureDeclaration();
void Condition();
void Expression();
void Term();
void Factor();

int mainIndex = 0;
int errorFlag = 0;

instruction *parser_code_generator(lexeme *list)
{
	int i;
	tokens = list;
	code = malloc(sizeof(instruction) * MAX_ARRAY_SIZE);
	table = malloc(sizeof(symbol) * MAX_ARRAY_SIZE);
	
	// Your code here. 
	// Make sure to remember to free table before returning always
	// and to free code and return NULL if there was an error


	Program();
	
	if(errorFlag == 1)
	{
		free(code);
		free(table);
		return NULL;
	}

	printsymboltable();
	printassemblycode();

	free(table);
	code[cIndex].op = -1;
	return code;
}

	void Program()
	{
		emit(7, 0, 0);
		addToSymbolTable(3, "main", 0, 0, 0, 0);
		level = -1;
		Block();
		if(tokens[lIndex].type != periodsym)
		{
			printparseerror(1); //Missing period error
		}
		else
		{
			emit(9, 0, 3);
			//Fix first JMP statement
			table[0].addr = mainIndex;
			code[0].m = mainIndex * 3;
			//Fix call statements
			for(int i = 0; i < cIndex; i++)
			{
				if(code[i].op == 5)
				{
					
					code[i].m = table[code[i].m].addr * 3;
				}
			}
		}
	}

	void Block()
	{
		int procedureIndex = tIndex - 1;
		level = level + 1;
		ConstDeclaration();
		int varCount = VarDeclaration();
		while (tokens[lIndex].type == procsym)
		{
			ProcedureDeclaration();
		}
		mainIndex = cIndex;

		//Fix procedure address
		table[procedureIndex].addr = cIndex;

		emit(6, 0, varCount + 3);
		Statement();
		mark();
		level = level - 1;
	}

	void ConstDeclaration()
	{
		if(tokens[lIndex].type == constsym)
		{
			do
			{
				lIndex++;
				int symbolIndex = multipledeclarationcheck(tokens[lIndex].name);
				if(tokens[lIndex].type != identsym)
				{
					printparseerror(2); //Missing identifier
				}
				else if(symbolIndex != -1)
				{
					printparseerror(18);//Conflicting symbol
				}
				else
				{
					char * constName = tokens[lIndex].name;
					lIndex++;
					if(tokens[lIndex].type != becomessym)
					{
						printparseerror(2); //Missing := symbol
					}
					else
					{
						lIndex++;
						if(tokens[lIndex].type != numbersym)
						{
							printparseerror(2); //Missing number value
						}
						else
						{
							addToSymbolTable(1, constName, tokens[lIndex].value, level, 0, 0);
							lIndex++;
						}
					}
				}

			} while (tokens[lIndex].type == commasym);
			
			if(tokens[lIndex].type != semicolonsym)
			{
				if(tokens[lIndex].type == identsym)
				{
					printparseerror(13);//Missing comma between identifier declarations
				}
				else
				{
					printparseerror(14);//Missing semicolon
				}
			}
			else
			{
				lIndex++;
			}
		}
	}

	int VarDeclaration()
	{
		int addressCount = 3;
		if(tokens[lIndex].type == varsym)
		{
			do
			{
				lIndex++;
				if(tokens[lIndex].type != identsym)
				{
					printparseerror(3);//Var declarion should follow ident {"," ident}
				}
				int symbolIndex = multipledeclarationcheck(tokens[lIndex].name);
				if(symbolIndex != -1)
				{	
					printparseerror(18);//Conflicting symbol
				}
				else
				{
					addToSymbolTable(2, tokens[lIndex].name, 0, level, addressCount, 0);
					addressCount++;
				}
				lIndex++;
			} while (tokens[lIndex].type == commasym);

			if(tokens[lIndex].type != semicolonsym)
			{
				if(tokens[lIndex].type == identsym)
				{
					printparseerror(13);//Missing comma between identifier declarations
				}
				else
				{
					printparseerror(14);//Missing semicolon
				}
			}
			else
			{
				lIndex++;
			}
		}
		return addressCount - 3;
	}

	void ProcedureDeclaration()
	{
		if(tokens[lIndex].type == procsym)
		{
			lIndex++;
			if(tokens[lIndex].type != identsym)
			{
				printparseerror(4);//Missing identifier
			}
			else
			{
				int symbolIndex = multipledeclarationcheck(tokens[lIndex].name);
				if(symbolIndex != -1)
				{
					printparseerror(18);
				}
				else
				{
					addToSymbolTable(3, tokens[lIndex].name, 0, level, 0, 0);
					lIndex++;
					if(tokens[lIndex].type != semicolonsym)
					{
						printparseerror(4);//Missing semicolon after procedure identifier
					}
					else
					{
						lIndex++;
						Block();
						if(tokens[lIndex].type != semicolonsym)
						{
							printparseerror(14);
						}
						else
						{
							lIndex++;
							emit(2, 0, 0);
						}
					}
				}
			}
		}
	}

	void Statement()
	{
		//Assignment Statements
		if(tokens[lIndex].type == identsym)
		{
			int symbolIndex = findsymbol(tokens[lIndex].name, 2);
			if(table[symbolIndex].kind != 2)
			{
				printparseerror(6);//Identifier not variable
			}
			if(symbolIndex == -1)
			{
				printparseerror(19);//Undeclared Identifier
			}
			else
			{
				lIndex++;

				switch (tokens[lIndex].type)
				{
				case becomessym:
					lIndex++;
					Expression();
					emit(4, level - table[symbolIndex].level, table[symbolIndex].addr);
					break;

				case addassignsym:
					lIndex++;
					emit(3, level - table[symbolIndex].level, table[symbolIndex].addr);
					Expression();
					emit(2, 0, 2);
					emit(4, level - table[symbolIndex].level, table[symbolIndex].addr);
					break;

				case subassignsym:
					lIndex++;
					emit(3, level - table[symbolIndex].level, table[symbolIndex].addr);
					Expression();
					emit(2, 0, 3);
					emit(4, level - table[symbolIndex].level, table[symbolIndex].addr);
					break;

				case timesassignsym:
					lIndex++;
					emit(3, level - table[symbolIndex].level, table[symbolIndex].addr);
					Expression();
					emit(2, 0, 4);
					emit(4, level - table[symbolIndex].level, table[symbolIndex].addr);
					break;

				case divassignsym:
					lIndex++;
					emit(3, level - table[symbolIndex].level, table[symbolIndex].addr);
					Expression();
					emit(2, 0, 5);
					emit(4, level - table[symbolIndex].level, table[symbolIndex].addr);
					break;

				case modassignsym:
					lIndex++;
					emit(3, level - table[symbolIndex].level, table[symbolIndex].addr);
					Expression();
					emit(2, 0, 6);
					emit(4, level - table[symbolIndex].level, table[symbolIndex].addr);
					break;
				
				default:
					printparseerror(5);//Missing assignment symbol
					break;
				}
			}
		}
		//Call statements
		else if(tokens[lIndex].type == callsym)
		{
			lIndex++;
			int procIndex = findsymbol(tokens[lIndex].name, 3);
			int varIndex = findsymbol(tokens[lIndex].name, 2);
			int constIndex = findsymbol(tokens[lIndex].name, 1);


			if(tokens[lIndex].type != identsym || varIndex > 0 || constIndex > 0)
			{
				printparseerror(7);//Call must be followed by procedure identifier
			}

			if(procIndex == -1)
			{
				printparseerror(19);//Undeclared identifier
			}
			emit(5, level - table[procIndex].level, procIndex);
			lIndex++;
		}
		//Begin statements
		else if(tokens[lIndex].type == beginsym)
		{
			lIndex++;
			Statement();
			do
			{
				lIndex++;
				Statement();
			} while (tokens[lIndex].type == semicolonsym);

			if(tokens[lIndex].type != endsym)
			{
				if(tokens[lIndex].type == identsym ||tokens[lIndex].type == readsym || 
				tokens[lIndex].type == writesym || tokens[lIndex].type == beginsym || tokens[lIndex].type == callsym ||
				tokens[lIndex].type == ifsym || tokens[lIndex].type == whilesym)
				{
					printparseerror(15);//Statements must be seperated by a semicolon
				}
				else
				{
					printparseerror(16);//Missing end
				}
			}
			else
			{
				lIndex++;
			}
		}
		//if statements
		else if(tokens[lIndex].type == ifsym)
		{
			lIndex++;
			Condition();
			if(tokens[lIndex].type != thensym)
			{
				printparseerror(8);//Missing then
			}
			else
			{
				lIndex++;
				int jpcIndex = cIndex;
				emit(8, 0, 0);
				Statement();

				if(tokens[lIndex].type == elsesym)
				{
					lIndex++;
					int jmpIndex = cIndex;
					emit(7, 0, 0);
					code[jpcIndex].m = cIndex * 3;
					Statement();
					code[jmpIndex].m = cIndex * 3;
				}
				else
				{
					code[jpcIndex].m = cIndex * 3;
				}
			}
		}
		//While statements
		else if(tokens[lIndex].type == whilesym)
		{
			int codeTemp1 = cIndex;
			lIndex++;
			Condition();
			int codeTemp2 = cIndex;
			emit(8, 0, 0);
			if (tokens[lIndex].type != dosym)
			{
				printparseerror(9);
			}
			else
			{
				lIndex++;
			}
			Statement();
			emit(7, 0, codeTemp1 * 3);
			code[codeTemp2].m = cIndex * 3;
		}
		//read statements
		else if(tokens[lIndex].type == readsym)
		{
			lIndex++;

			int procIndex = findsymbol(tokens[lIndex].name, 3);
			int varIndex = findsymbol(tokens[lIndex].name, 2);
			int constIndex = findsymbol(tokens[lIndex].name, 1);

			if(tokens[lIndex].type != identsym || procIndex > 0 || constIndex > 0)
			{
				printparseerror(6);//Missing Identifier or not variable
			}
			else if(varIndex < 0)
			{
				printparseerror(19);//undeclared identifier
			}
			else
			{
				emit(9, 0, 2);
				emit(4, level - table[varIndex].level, varIndex);
			}
			lIndex++;
		}
		//Write statements
		else if(tokens[lIndex].type == writesym)
		{
			lIndex++;
			Expression();
			emit(9, 0, 1);
		}
	}

	void Condition()
	{
		Expression();
		if(tokens[lIndex].type == eqlsym)
		{
			lIndex++;
			Expression();
			emit(2, 0, 7);
		}
		else if(tokens[lIndex].type == neqsym)
		{
			lIndex++;
			Expression();
			emit(2, 0, 8);
		}
		else if(tokens[lIndex].type == lessym)
		{
			lIndex++;
			Expression();
			emit(2, 0, 9);
		}
		else if(tokens[lIndex].type == leqsym)
		{
			lIndex++;
			Expression();
			emit(2, 0, 10);
		}
		else if(tokens[lIndex].type == gtrsym)
		{
			lIndex++;
			Expression();
			emit(2, 0, 11);
		}
		else if(tokens[lIndex].type == geqsym)
		{
			lIndex++;
			Expression();
			emit(2, 0, 12);
		}
		else
		{
			printparseerror(10);
		}
	}

	void Expression()
	{
		if(tokens[lIndex].type == minussym)
		{
			lIndex++;
			Term();
			emit(2, 0, 1);
		}
		else
		{
			Term();
		}
		while (tokens[lIndex].type == plussym || tokens[lIndex].type == minussym)
		{
			if(tokens[lIndex].type == plussym)
			{
				lIndex++;
				Term();
				emit(2, 0, 2);
			}
			else
			{
				lIndex++;
				Term();
				emit(2, 0, 3);
			}
		}

		if(tokens[lIndex].type == lparentsym || tokens[lIndex].type == identsym || tokens[lIndex].type == numbersym)
		{
			printparseerror(17);//Bad arithmetic
		}		
	}

	void Term()
	{
		Factor();
		while (tokens[lIndex].type == multsym || tokens[lIndex].type == slashsym || tokens[lIndex].type == modsym)
		{
			if(tokens[lIndex].type == multsym)
			{
				lIndex++;
				Factor();
				emit(2, 0, 4);
			}
			else if(tokens[lIndex].type == slashsym)
			{
				lIndex++;
				Factor();
				emit(2, 0, 5);
			}
			else
			{
				lIndex++;
				Factor();
				emit(2, 0, 6);
			}
		}
	}
	
	void Factor()
	{
		if(tokens[lIndex].type == identsym)
		{
			int varIndex = 0;
			int constIndex = 0;
			int isProcedure = 0;
			isProcedure = findsymbol(tokens[lIndex].name, 3);
			varIndex = findsymbol(tokens[lIndex].name, 2);
			constIndex = findsymbol(tokens[lIndex].name, 1);

			if(isProcedure > 0)
			{
				printparseerror(11);//Found Procedure
			}

			if(varIndex > 0 || constIndex > 0)
			{
				
				//If it finds both a constant and a variable with the identifier name pick the one with
				//the smallest difference in lexicographical level
				if(varIndex && constIndex > 0)
				{
					if((level - table[varIndex].level) < (level - table[constIndex].level))
					{
						emit(3, level - table[varIndex].level, table[varIndex].addr);
					}
					else
					{
						emit(1, 0, table[constIndex].value);
					}
					lIndex++;
				}
				else
				{
					if(constIndex > 0)
					{
						emit(1, 0, table[constIndex].value);
					}
					else
					{
						emit(3, level - table[varIndex].level, table[varIndex].addr);
					}
					lIndex++;
				}
			}
			else
			{
				printparseerror(19);//undeclared identifier
			}
		}
		else if(tokens[lIndex].type == numbersym)
		{
			emit(1, 0, tokens[lIndex].value);
			lIndex++;
		}
		else if(tokens[lIndex].type == lparentsym)
		{
			lIndex++;
			Expression();
			if(tokens[lIndex].type != rparentsym)
			{
				printparseerror(12);//Missing right parenthesis
			}
			else
			{
				lIndex++;
			}
		}
		else
		{
			printparseerror(11);//Arithmetic expression contains invalid symbol
		}
	}

// adds an instruction to the end of the code array
void emit(int opname, int level, int mvalue)
{
	code[cIndex].op = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

// adds a symbol to the end of the symbol table
void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].value = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

// starts at the end of the table and works backward (ignoring already
// marked entries (symbols of subprocedures)) to mark the symbols of
// the current procedure. it knows it's finished the current procedure
// by looking at level: if level is less than the current level and unmarked
// (meaning it belongs to the parent procedure) it stops
void mark()
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

// does a linear pass through the symbol table looking for the symbol
// who's name is the passed argument. it's found a match when the names
// match, the entry is unmarked, and the level is equal to the current
// level. it returns the index of the match. returns -1 if there are no
// matches
int multipledeclarationcheck(char name[])
{
	int i;
	for (i = 0; i < tIndex; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

// returns -1 if a symbol matching the arguments is not in the symbol table
// returns the index of the desired entry if found (maximizing the level value)
int findsymbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

void printparseerror(int err_code)
{
	errorFlag = 1;
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=, +=, -=, *=, /=, or %%=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
	exit(0);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].value, table[i].level, table[i].addr, table[i].mark); 
	printf("\n");
}

void printassemblycode()
{
	int i;
	printf("Assembly Code:\n");
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].op);
		switch (code[i].op)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("MOD\t");
						break;
					case 7:
						printf("EQL\t");
						break;
					case 8:
						printf("NEQ\t");
						break;
					case 9:
						printf("LSS\t");
						break;
					case 10:
						printf("LEQ\t");
						break;
					case 11:
						printf("GTR\t");
						break;
					case 12:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				printf("SYS\t");
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	printf("\n");
}