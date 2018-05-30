# define _CRT_SECURE_NO_WARNINGS
# define  size_of_chunk_bit 35
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<string.h>
#include"myhead.h"
int offset = 0,no_of_symbols=0,no_of_constants=0,no_of_instructions=0,pc = 0,no_of_labels;
int memory[1000],register_memory[10];
struct symbol_table
{
	char name;
	int address;
	int size;
}*symbol_table_rows[20];
struct constant_table
{
	char name;
	int address;
	int size;
	int value;
}*constant_table_rows[10];
struct intermediate_lang
{
	int opcode;
	int parameters[6];
	int top = 0;
}*intermediate_table_rows[20];
struct label_table
{
	char name;
	int address;
}*label_table_rows[20];
struct opcode
{
	int opcode_no;
	char opearation[10];
}; 
int parse_char_to_int(char *s)
{
	int n = 0;
	for (int i = 0; s[i]&&s[i]!='\n'; i++)
	{
		n *= 10;
		n += s[i] - 48;
	}
	return n;
}
void strcopy(char *dest, char *source)
{
	int i;
	for (i = 0; source[i]; i++)
	{
		dest[i] = source[i];
	}
	dest[i] = '\0';
}
int strlength(char *s)
{
	int i;
	for (i = 0; s[i]; i++) {}
	return i;
}
int strcomp(char *s1, char *s2)
{
	if (strlength(s1) != strlength(s2))
		return 0;
	for (int i = 0; s1[i]; i++)
	{
		if (s1[i] != s2[i])
			return 0;
	}
	return 1;
}
void set_opcode_table(opcode **opcode_table)
{
	char operations[15][10] = {"$$$","MOV","MOV","ADD","SUB","MUL","JMP","IF","EQ","LT","GT","LTEQ","GTEQ","PRINT","READ"};
	for (int i = 0; i < 15; i++)
	{
		opcode_table[i]->opcode_no = i + 1;
		strcopy(opcode_table[i]->opearation,operations[i]);
	}
}
void get_DATA_operands(char *buff,char *operand1)
{
	int i,top=0;
	for (i = 0; buff[i]!='\n'; i++)
	{
		operand1[top++] = buff[i];
	}
	operand1[top] = '\0';
}
void get_CONST_operands(char *buff,char *operand1,char *operand2)
{
	int i, top = 0;
	for (i = 0; buff[i] != '='; i++)
	{
		operand1[top++] = buff[i];
	}
	operand1[top] = '\0';
	top = 0;
	for (i++ ; buff[i]; i++)
	{
		operand2[top++] = buff[i];
	}
	operand2[top] = '\0';
}
void get_READ_operands(char *buff,char *operand1)
{
	int i, top = 0;
	for (i = 0; buff[i] != '\n'; i++)
	{
		operand1[top++] = buff[i];
	}
	operand1[top] = '\0';
}
void get_MOV_operands(char *buff,char *operand1,char *operand2)
{
	int i, top = 0;
	for (i = 0; buff[i] != ','; i++)
	{
		operand1[top++] = buff[i];
	}
	operand1[top] = '\0';
	top = 0;
	for (i++; buff[i]!='\0'&&buff[i] != '\n'; i++)
	{
		operand2[top++] = buff[i];
	}
	operand2[top] = '\0';
}
void get_ADD_operands(char *buff, char **operand1, char **operand2,char **operand3)
{
	*operand1 = strtok(buff,",");
	*operand2 = strtok(NULL,",");
	*operand3 = strtok(NULL, "\n");
}
void get_IF_operands(char *buff, char **operand1, char **operand2, char **operand3)
{
	*operand1 = strtok(buff, " ");
	*operand2 = strtok(NULL, " ");
	*operand3 = strtok(NULL, " ");
}
/* it also gives register id if it is a register*/
int is_register(char *operand)
{
	char registers[8][3] = {"AX","BX","CX","DX","EX","FX","GX","HX"};
	for (int i = 0; i < 8; i++)
	{
		if (strcomp(registers[i],operand))
		{
			return i;
		}
	}
	return -1;
}
int give_memory_of_operand(char operand)
{
	for (int i = 0; i < no_of_symbols; i++)
	{
		if (symbol_table_rows[i]->name == operand)
		{
			return symbol_table_rows[i]->address;
		}
	}
	for (int i = 0; i < no_of_constants; i++)
	{
		if (constant_table_rows[i]->name == operand)
		{
			return constant_table_rows[i]->address;
		}
	}
	//printf("yoo\n");
	return -1;
}
int give_memory_of_label(char operand)
{
	for (int i = 0; i < no_of_labels; i++)
	{
		if (label_table_rows[i]->name == operand)
		{
			printf("fucking - %d\n", label_table_rows[i]->address);
			return label_table_rows[i]->address;
		}
	}
}
int get_opcode(char *operation,opcode **opcode_table)
{
	for (int i = 1; i < 15; i++)
	{
		if(strcomp(opcode_table[i]->opearation,operation))
			return i;
	}
	return -1;
}
void resolve(char *operation,char *buff,opcode **opcode_table)
{
	struct symbol_table *row = (symbol_table *)malloc(sizeof(symbol_table));
	struct constant_table *ROW = (constant_table *)malloc(sizeof(constant_table));
	struct intermediate_lang *temp = (intermediate_lang *)malloc(sizeof(intermediate_lang));
	struct label_table *temp_label = (label_table*)malloc(sizeof(temp_label));
	char *operand1 = (char *)malloc(sizeof(char)*20);
	char *operand2 = (char *)malloc(sizeof(char) * 20);
	char *operand3 = (char *)malloc(sizeof(char) * 20);
	int shift = 1;
	int register_id;
	if (strcomp("DATA", operation))
	{
		get_DATA_operands(buff,operand1);
		//strcopy(row->name,operand1);
		row->name = operand1[0];
		if (strlen(operand1) > 1)
		{
			shift = operand1[2]-'0';
		}
		row->address = offset;
		row->size = shift;
		offset += shift ;
		symbol_table_rows[no_of_symbols++] = row;
		//printf("--%s--%c", operand1,row->name);
	}
	else if (strcomp("CONST", operation))
	{
		get_CONST_operands(buff,operand1,operand2);
		ROW->name = operand1[0];
		ROW->address = offset++;
		ROW->size = 0;
		ROW->value = parse_char_to_int(operand2);
		//printf("ooy -- %d--%s", ROW->value, operand2);
		constant_table_rows[no_of_constants++] = ROW;
		//printf("%s--%s\n", operand1, operand2);
	}
	else if (strcomp("READ", operation))
	{
		//printf("you are back\n");
		get_READ_operands(buff,operand1);
		register_id = is_register(operand1);
		temp->opcode = get_opcode("READ", opcode_table);
		if (register_id!=-1)
		{
			temp->parameters[0] = register_id;
			temp->top = 1;
		}
		intermediate_table_rows[no_of_instructions++] = temp;
		//printf("%s\n",operand1);
	}
	else if (strcomp("PRINT", operation))
	{
		//printf("you are back\n");
		get_READ_operands(buff, operand1);
		register_id = is_register(operand1);
		temp->opcode = get_opcode("PRINT", opcode_table);
		if (register_id != -1)
		{
			temp->parameters[0] = register_id;
			temp->top = 1;
		}
		intermediate_table_rows[no_of_instructions++] = temp;
		//printf("%s\n",operand1);
	}
	else if (strcomp("MOV", operation))
	{
		get_MOV_operands(buff,operand1,operand2);
		//printf("heyyy  %s--%s\n", operand1, operand2);
		register_id = is_register(operand1);
		temp->opcode = get_opcode("MOV", opcode_table);
		if (register_id != -1)
		{
			//rg,mem
			temp->opcode = 2;
			temp->parameters[0] = register_id;
			temp->parameters[1] = give_memory_of_operand(operand2[0]);
			printf("check - %d,%d", temp->parameters[0], temp->parameters[1]);
		}
		else
		{
			//mem,rg
			if (strlength(operand1) > 2)
			{
				//printf("YOu got this\n");
				temp->opcode = 1;
				temp->parameters[0] = give_memory_of_operand(operand1[0])+(operand1[2]-'0');
				temp->parameters[1] = is_register(operand2);
			}
			else
			{
				temp->opcode = 1;
				temp->parameters[0] = give_memory_of_operand(operand1[0]);
				temp->parameters[1] = is_register(operand2);
			}
		}
		temp->top = 2;
		intermediate_table_rows[no_of_instructions++] = temp;
	}
	else if (strcomp("ADD", operation))
	{
		get_ADD_operands(buff,&operand1,&operand2,&operand3);
		temp->opcode = get_opcode("ADD", opcode_table);
		temp->parameters[0] = is_register(operand1);
		temp->parameters[1] = is_register(operand2);
		temp->parameters[2] = is_register(operand3);
		temp->top = 3;
		intermediate_table_rows[no_of_instructions++] = temp;
		//printf("%s--%s--%s", operand1, operand2,operand3);
	}
	else if (strcomp("SUB", operation))
	{
		get_ADD_operands(buff, &operand1, &operand2, &operand3);
		temp->opcode = get_opcode("SUB", opcode_table);
		temp->parameters[0] = is_register(operand1);
		temp->parameters[1] = is_register(operand2);
		temp->parameters[2] = is_register(operand3);
		temp->top = 3;
		intermediate_table_rows[no_of_instructions++] = temp;
		//printf("%s--%s--%s", operand1, operand2, operand3);
	}
	else if (strcomp("MUL", operation))
	{
		get_ADD_operands(buff, &operand1, &operand2, &operand3);
		temp->opcode = get_opcode("MUL", opcode_table);
		temp->parameters[0] = is_register(operand1);
		temp->parameters[1] = is_register(operand2);
		temp->parameters[2] = is_register(operand3);
		temp->top = 3;
		intermediate_table_rows[no_of_instructions++] = temp;
		//printf("%s--%s--%s", operand1, operand2, operand3);
	}
	else if (strlen(operation) == 2 && operation[1] == ':')
	{
		temp_label->name = operation[0];
		temp_label->address = no_of_instructions;
		//printf("fuck - %d\n", temp_label->address);
		label_table_rows[no_of_labels++] = temp_label;
	}
	else if (strcomp("JMP", operation))
	{
		//printf("Buffer = %c\n",buff[0]);
		temp->opcode = get_opcode("JMP", opcode_table);
		temp->parameters[0] = give_memory_of_label(buff[0]);
		temp->top = 1;
		intermediate_table_rows[no_of_instructions++] = temp;
	}
	else if (strcomp("IF",operation))
	{
		//printf("ooyy\n");
		get_IF_operands(buff,&operand1,&operand2,&operand3);
		temp->opcode = get_opcode("IF", opcode_table);
		temp->parameters[0] = is_register(operand1);
		temp->parameters[1] = is_register(operand3);
		temp->parameters[2] = get_opcode(operand2, opcode_table);
		temp->parameters[3] = -1;
		temp->top = 4;
		intermediate_table_rows[no_of_instructions++] = temp;
		//printf("i am keeping at %d\n", no_of_instructions-1);
		push(no_of_instructions-1);
		//printf("%s--%s--%s--\n", operand1, operand2, operand3);
	}
	else if (strcomp("ELSE",operation))
	{
		int jump_to_address = pop();
		printf("jta  %d\n", jump_to_address);
		intermediate_table_rows[jump_to_address]->parameters[3] = no_of_instructions+1;
		push(no_of_instructions);
		temp->opcode = get_opcode("JMP", opcode_table);
		temp->parameters[0] = -1;
		temp->top = 1;
		intermediate_table_rows[no_of_instructions++] = temp;
	}
	else if (strcomp("ENDIF",operation))
	{
		int jump_to_address = pop();
		intermediate_table_rows[jump_to_address]->parameters[0] = no_of_instructions + 1;
	}

}
void tokenise_input_line(char *buff,opcode **opcode_table)
{
	//printf("%s", buff);
	char *operation = (char *)malloc(sizeof(char)*10);
	int i,top=0,no_of_operands=0;
	for (i = 0; buff[i] != ' ' && buff[i]!='\n'; i++)
	{
		operation[top++] = buff[i];
	}
	operation[top] = '\0';
	//printf("operaiton = %s\n",operation);
	resolve(operation,buff+i+1,opcode_table);
}
void execute_MOV1(int ad1,int ad2)
{
	memory[ad1] = register_memory[ad2];
	printf("MOV1 - value - %d\n", register_memory[ad1]);
}
void execute_MOV2(int ad1, int ad2)
{
	register_memory[ad1] = memory[ad2];
	printf("MOV2 - %d\n", memory[ad1]);
}
void execute_ADD_SUB_MUL(int opcode_id,int ad1, int ad2, int ad3)
{
	if (opcode_id == 3)
	{
		register_memory[ad1] = register_memory[ad2] + register_memory[ad3];
	}
	else if (opcode_id == 4)
	{
		register_memory[ad1] = register_memory[ad2] - register_memory[ad3];
	}
	else
	{
		register_memory[ad1] = register_memory[ad2] * register_memory[ad3];
	}
}
void execute_PRINT(int ad1)
{
	printf("printing - %d\n",register_memory[ad1]);
}
void execute_READ(int ad1)
{
	scanf("%d", &register_memory[ad1]);
}
int execute_IF(int ad1, int code, int ad2)
{
	int op1 = register_memory[ad1];
	int op2 = register_memory[ad2];
	if (code == 8)
	{
		return op1 == op2;
	}
	if (code == 9)
	{
		return op1 < op2;
	}
	if (code == 10)
	{
		return op1 > op2;
	}
	if (code == 11)
	{
		return op1 <= op2;
	}
	if (code == 12)
	{
		return op1 >= op2;
	}
}
void execute()
{
	//printf("Entered\n");
	int counter = 0;
	for (int i = 0; i < no_of_instructions; i++)
	{
		//counter++;
		//if (counter == 50)
			//break;
		printf("i-%d\n",i);
		int opcode_id = intermediate_table_rows[i]->opcode;
		if (opcode_id == 1)
		{
			execute_MOV1(intermediate_table_rows[i]->parameters[0], intermediate_table_rows[i]->parameters[1]);
		}
		else if (opcode_id == 2)
		{
			execute_MOV2(intermediate_table_rows[i]->parameters[0], intermediate_table_rows[i]->parameters[1]);
		}
		else if (opcode_id == 3 || opcode_id == 4 || opcode_id == 5)
		{
			execute_ADD_SUB_MUL(opcode_id,intermediate_table_rows[i]->parameters[0], intermediate_table_rows[i]->parameters[1], intermediate_table_rows[i]->parameters[2]);
		}
		else if (opcode_id == 6)
		{
			i = intermediate_table_rows[i]->parameters[0]-1;
			continue;
		}
		else if (opcode_id == 7)
		{
			if (execute_IF(intermediate_table_rows[i]->parameters[0], intermediate_table_rows[i]->parameters[2], intermediate_table_rows[i]->parameters[1]))
			{
				continue;
			}
			else
			{
				i = intermediate_table_rows[i]->parameters[3] - 1;
				continue;
			}
		}
		else if (opcode_id == 13)
		{
			execute_PRINT(intermediate_table_rows[i]->parameters[0]);
		}
		else if (opcode_id == 14)
		{
			//printf("Exited\n");
			execute_READ(intermediate_table_rows[i]->parameters[0]);
			//printf("Exited\n");
		}
	}
	//printf("Exited\n");
}
void constants_in_memory()
{
	for (int i = 0; i < no_of_constants; i++)
	{
		memory[constant_table_rows[i]->address] = constant_table_rows[i]->value;
	}
}
void write_to_file()
{
	FILE *fp = fopen("executable","w");

	/*  writing constants code to file */
	fprintf(fp, "%d\n", no_of_constants);
	for (int i = 0; i < no_of_constants; i++)
	{
		fprintf(fp,"%c,%d,%d\n",constant_table_rows[i]->name, constant_table_rows[i]->address, constant_table_rows[i]->size);
	}

	/*  writing intermediate code to file */
	fprintf(fp, "%d\n", no_of_instructions);
	for (int i = 0; i < no_of_instructions; i++)
	{
		fprintf(fp,"%d,",intermediate_table_rows[i]->opcode);
		for (int j = 0; j < intermediate_table_rows[i]->top; j++)
		{
			fprintf(fp,"%d", intermediate_table_rows[i]->parameters[j]);
			if (j != intermediate_table_rows[i]->top - 1)
			{
				fprintf(fp,",");
			}
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
}
int main()
{
	char *buff = (char *)malloc(sizeof(char)*100);
	FILE *fp = fopen("input1.txt","r");
	opcode **opcode_table = (opcode **)malloc(sizeof(opcode *)*20);
	for (int i = 0; i < 20; i++)
	{
		opcode_table[i] = (opcode *)malloc(sizeof(opcode)*20);
	}
	symbol_table **table_rows = (symbol_table **)malloc(sizeof(symbol_table*) * 20);
	for (int i = 0; i < 20; i++)
	{
		table_rows[i] = (symbol_table *)malloc(sizeof(symbol_table)*20);
	}
	set_opcode_table(opcode_table);
	while (fgets(buff,90,fp))
	{
		printf("%s",buff);
		int ind = 0;
		while (buff[ind] == '\t')
			ind++;
		tokenise_input_line(buff+ind,opcode_table);
		pc++;
	}
	printf("\n");
	printf("\t\t\tSYMBOL TABLE\n");
	for (int i = 0; i < no_of_symbols; i++)
	{
		printf("%c , %d , %d\n", symbol_table_rows[i]->name, symbol_table_rows[i]->address, symbol_table_rows[i]->size);
	}
	printf("\t\t\tCONSTANT TABLE\n");
	for (int i = 0; i < no_of_constants; i++)
	{
		printf("%c , %d , %d\n", constant_table_rows[i]->name, constant_table_rows[i]->address, constant_table_rows[i]->size);
	}
	printf("\t\t\tINTERMEDIATE TABLE\n");
	for (int i = 0; i < no_of_instructions; i++)
	{
		printf("%d , ",intermediate_table_rows[i]->opcode);
		for (int j = 0; j < intermediate_table_rows[i]->top; j++)
		{
			printf("%d ,", intermediate_table_rows[i]->parameters[j]);
		}
		printf("\n");
	}
	printf("\t\t\tLABEL TABLE\n");
	for (int i = 0; i < no_of_labels; i++)
	{
		printf("%c , %d\n", label_table_rows[i]->name, label_table_rows[i]->address);
	}
	constants_in_memory();
	//printf("hello --- %d\n",memory[0]);
	execute();
	
	write_to_file();

	system("pause");
	return 0;
}