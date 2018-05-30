# define _CRT_SECURE_NO_WARNINGS
# define  size_of_chunk_bit 35
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<string.h>
struct metadata
{
	int no_of_tables;
};
void strcopy(char *dest, char *source)
{
	int i;
	for (i = 0; source[i]; i++)
	{
		dest[i] = source[i];
	}
	dest[i] = '\0';
}
int parse_char_to_int(char *s)
{
	int n = 0;
	for (int i = 0; s[i]; i++)
	{
		n *= 10;
		n += s[i] - 48;
	}
	return n;
}
struct column_node
{
	int col_id;
	char val[20];
	int version;
	column_node *next_col, *down;
};
struct row_node
{
	column_node *next_col;
	row_node *next_row;
};
struct table
{
	char cols[20][20],title[20];
	row_node *head_row;
	int max_columns;
};

void display_total_columns(table *table_temp)
{
	for (int i = 0; i < table_temp->max_columns; i++)
	{
		printf(" %s ",table_temp->cols[i]);
	}
	printf("\n");
	for (int i = 0; i < table_temp->max_columns; i++)
	{
		printf(" %d ",i);
	}
}
row_node *create_row_node()
{
	row_node *temp = (row_node *)malloc(sizeof(row_node) * 1);
	temp->next_col = NULL;
	temp->next_row = NULL;
	return temp;
}
column_node *create_column_node(char *value, int col_id)
{
	column_node *temp = (column_node *)malloc(sizeof(column_node) * 1);
	temp->down = NULL;
	temp->next_col = NULL;
	strcopy(temp->val, value);
	temp->col_id = col_id;
	temp->version = 0;
	return temp;
}
column_node *get_prev_column_node(column_node *curr_col, row_node *curr_row)
{
	column_node *prev_col = curr_row->next_col;
	while (prev_col)
	{
		if (prev_col->next_col == curr_col)
		{
			return prev_col;
		}
		prev_col = prev_col->next_col;
	}
	return NULL;
}
row_node *get_prev_row_node(row_node *curr_row, table *table_temp)
{
	row_node *prev_row = table_temp->head_row;
	while (prev_row)
	{
		if (prev_row->next_row == curr_row)
		{
			return prev_row;
		}
		prev_row = prev_row->next_row;
	}
	return NULL;
}
row_node *get_row_node_from_id(int id, table *table_temp)
{
	//int counter = 1;
	int row_id = 0;
	row_node *row = table_temp->head_row;
	while (row)
	{
		/*if (counter == id)
		return row;
		counter++;*/
		row_id = parse_char_to_int(row->next_col->val);
		if (row_id == id)
		{
			return row;
		}
		row = row->next_row;
	}
	return NULL;
}
void Delete(table *table_temp)
{
	int id;
	printf("Enter the userID you want to Delete : \n");
	scanf("%d", &id);
	if (parse_char_to_int(table_temp->head_row->next_col->val) == id) //case of deleting first row
	{
		table_temp->head_row = table_temp->head_row->next_row;
	}
	else
	{
		row_node *row_curr = get_row_node_from_id(id, table_temp);
		row_node *row_prev = get_prev_row_node(row_curr, table_temp);
		row_prev->next_row = row_curr->next_row;
	}
}
void scan_table_id(int *table_id)
{
	int id;
	printf("Enter the table id : \n");
	scanf("%d", &id);
	*table_id = id;
}
void extend_version(column_node *version_col,char *value,int col_id,row_node *row)
{
	column_node *latest_version_node = create_column_node(value, col_id);
	latest_version_node->next_col = version_col->next_col;
	latest_version_node->down = version_col;
	version_col->next_col = NULL;
	column_node *prev = get_prev_column_node(version_col,row);
	prev->next_col = latest_version_node;
}
void create_version(column_node *version_col,char *value,int col_id)
{
	column_node *new_col, *temp_col;
	new_col = create_column_node(value, col_id);
	temp_col = version_col->next_col;
	version_col->next_col = new_col;
	new_col->next_col = temp_col;
}
column_node *get_version_column_in_row(row_node *row,int col_id)
{
	column_node *col = row->next_col,*prev_col=NULL;
	while (col)
	{
		if (col->col_id == col_id)
		{
			return col;
		}
		if (col->col_id > col_id)
		{
			return prev_col;
		}
		prev_col = col;
		col = col->next_col;
	}
	return prev_col;
}
void put_helper(table *table_temp,row_node *row, int col_id, char *value)
{
	column_node *col = row->next_col;
	column_node *version_col = get_version_column_in_row(row, col_id),*temp_col=NULL,*new_col=NULL;
	if (version_col->col_id == col_id)  // found column in row
	{
		extend_version(version_col, value, col_id,row);
	}
	else                               // create a new column in middle or at end
	{
		create_version(version_col,value,col_id);
	}
}
void put(table *table_temp)
{
	int id;
	char choice = 0;
	int col_id = 0;
	char **values = (char **)malloc(sizeof(char*) * (table_temp->max_columns+1));
	for (int i = 0; i < 10; i++)
	{
		values[i] = (char *)malloc(sizeof(char) * 10);
		values[i][0] = '\0';
	}
	printf("Enter the user_id you want to update : \n");
	scanf("%d", &id);
	row_node *row = get_row_node_from_id(id,table_temp);
	do
	{
		printf("Do you want to add new versions : [y/n] ");
		scanf(" %c", &choice);
		//printf("choice = %c\n",choice);
		if (choice == 'Y' || choice == 'y')
		{
			printf("Enter the column id : ");
			scanf("%d", &col_id);
			printf("Enter the value of %s : ", table_temp->cols[col_id]);
			scanf("%s", values[col_id]);
			put_helper(table_temp, row, col_id, values[col_id]);
		}
	} while (choice == 'Y' || choice == 'y');
}
void display_data_in_table(table *table_temp)
{
	row_node *temp_row = table_temp->head_row;
	column_node *temp_col;
	while (temp_row)
	{
		temp_col = temp_row->next_col;
		while (temp_col)
		{
			printf("%s:%s  ,",table_temp->cols[temp_col->col_id],temp_col->val);
			temp_col = temp_col->next_col;
		}
		temp_row = temp_row->next_row;
		printf("\n");
	}
}
void display_data_in_row(row_node *row,table *table_temp)
{
	column_node *col = row->next_col;
	while (col)
	{
		printf(" %s:%s  ,", table_temp->cols[col->col_id], col->val);
		col = col->next_col;
	}
	printf("\n");
}
void display_tables(table **tables,metadata *md)
{
	for (int i = 0; i < md->no_of_tables; i++)
	{
		display_data_in_table(tables[i]);
	}
}
column_node *get_row_of_column_nodes(table *table_temp, char **values)
{
	//printf("heyy -- %s\n",values[0]);
	column_node *head = (column_node*)malloc(sizeof(column_node) * 1);
	column_node *prev = head, *curr;
	for (int i = 0; i<table_temp->max_columns; i++)
	{
		if (values[i][0])
		{
			curr = create_column_node(values[i],i);
			prev->next_col = curr;
			prev = curr;
		}
	}
	prev->next_col = NULL;
	return head->next_col;
}
void create_table(table **tables, metadata *md)
{
	int max_columns;
	char *name = (char *)malloc(sizeof(char) * 20);
	tables[md->no_of_tables]->head_row = NULL;
	printf("Enter the table title : \n");
	scanf("%s", tables[md->no_of_tables]->title);
	printf("Enter the max no of columns and column names : \n");
	scanf("%d", &max_columns);
	tables[md->no_of_tables]->max_columns = max_columns;
	for (int i = 0; i < max_columns; i++)
	{
		scanf("%s", name);
		strcopy(tables[md->no_of_tables]->cols[i], name);
	}
	md->no_of_tables += 1;
}
void insert(table *table_temp)
{
	char **values = (char **)malloc(sizeof(char*) * (table_temp->max_columns+1));
	char choice;
	int col_id=0;
	for (int i = 0; i < 10; i++)
	{
		values[i] = (char *)malloc(sizeof(char) * 20);
		values[i][0] = '\0';
	}
	printf("Enter the values for column with column id as : \n");
	display_total_columns(table_temp);
	do
	{
		printf("\n\nDo you want to add columns : [y/n] ");
		scanf(" %c",&choice);
		//printf("choice = %c\n",choice);
		if (choice == 'Y' || choice == 'y')
		{
			printf("Enter the column id : ");
			scanf("%d",&col_id);
			printf("Enter the value of %s : ",table_temp->cols[col_id]);
			scanf("%s", values[col_id]);
		}
	} while (choice=='Y'||choice=='y');
	//return;
	column_node *col_head = get_row_of_column_nodes(table_temp, values);
	//printf("value = %s\n",col_head->val);
	//return;
	if (table_temp->head_row == NULL)
	{
		row_node *first_row_node = create_row_node();
		first_row_node->next_col = col_head;
		first_row_node->next_row = NULL;
		table_temp->head_row = first_row_node;
	}
	else
	{
		row_node *last_row_node = table_temp->head_row;
		while (last_row_node->next_row)
			last_row_node = last_row_node->next_row;
		row_node *new_row_node = create_row_node();
		last_row_node->next_row = new_row_node;
		new_row_node->next_col = col_head;
		new_row_node->next_row = NULL;
	}
}
void get(table *table_temp)
{
	int id;
	printf("Enter the user ID you want to get : ");
	scanf("%d", &id);
	row_node *row = get_row_node_from_id(id, table_temp);
	display_data_in_row(row,table_temp);
}
char *get_file_name(char *table_title,char *column_name)
{
	char *filename = (char *)malloc(sizeof(char)*20);
	int top = 0;
	for (int i = 0; table_title[i]; i++)
	{
		filename[top++] = table_title[i];
	}
	filename[top++] = '_';
	for (int i = 0; column_name[i]; i++)
	{
		filename[top++] = column_name[i];
	}
	filename[top] = '\0';
	return filename;
}
void write_to_file_helper(FILE *fp,table *table_temp,int col_id)
{
	row_node *row = table_temp->head_row;
	column_node *col = NULL;
	char *primary_key = (char *)malloc(sizeof(char)*20);
	while (row)
	{
		col = row->next_col;
		strcopy(primary_key,col->val);
		while (col)
		{
			if (col->col_id == col_id)
			{
				while (col)
				{
					fprintf(fp, "%s,%s\n", primary_key,col->val);
					col = col->down;
				}
			}
			col = col->next_col;
		}
		row = row->next_row;
	}
}
void write_to_file(table *table_temp)
{
	char *filename;
	for (int i = 1; i < table_temp->max_columns; i++)
	{
		filename = get_file_name(table_temp->title, table_temp->cols[i]);
		FILE *fp = fopen(filename, "w");
		write_to_file_helper(fp, table_temp, i);
		fclose(fp);
	}
}
int main()
{
	int choice = 0;
	int table_id;
	int no_of_columns = 0;
	struct metadata *md = (metadata *)malloc(sizeof(metadata) * 1);
	md->no_of_tables = 0;
	struct table **tables = (table **)malloc(sizeof(table*) * 10);
	for (int i = 0; i < 10; i++)
	{
		tables[i] = (table *)malloc(sizeof(table) * 10);
	}
	while (1)
	{
		printf("\nEnter an option :\n0.create table\n1.insert\n2.put\n3.get\n4.Delete\n5.save table to file\n\n");
		scanf("%d", &choice);
		if (choice == 0)
		{
			create_table(tables,md);
			//dipaly_table_row_attributes();
		}
		else if (choice == 1)
		{
			scan_table_id(&table_id);
			insert(tables[table_id]);
			display_data_in_table(tables[table_id]);
		}
		else if (choice == 2)
		{
			scan_table_id(&table_id);
			put(tables[table_id]);
			display_data_in_table(tables[table_id]);
			//update(table1);
		}
		else if (choice == 3)
		{
			scan_table_id(&table_id);
			get(tables[table_id]);
		}
		else if (choice == 4)
		{
			scan_table_id(&table_id);
			Delete(tables[table_id]);
			display_data_in_table(tables[table_id]);
		} 
		else if (choice == 5)
		{
			scan_table_id(&table_id);
			write_to_file(tables[table_id]);
		}
		else
		{
			break;
		}
	}
	display_tables(tables, md);
	system("pause");
	return 0;
}