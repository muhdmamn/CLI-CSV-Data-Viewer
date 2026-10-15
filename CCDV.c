#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define content_size 1024
#define MAX_STRING_LENGTH 1024

char content[content_size];
char content_table;
typedef struct{
char ***table_ptr; //A pointer is used instead of an array directly because you cannot use Variable length Array Declaration at file scope.
//so a pointer is used because it allows you to use malloc to allocate its size as if it was an array later in the code.
int Row_No;
int Col_No;
int rows_opt;
int *counter;
}CSVTable;
void init_counter(CSVTable *Table){
Table->counter = malloc(Table->Col_No * sizeof(int));
if (Table->counter == NULL) {
    free(Table->counter);
    fprintf(stderr,"Memory Allocation Failed");
    exit(1);
}//Memory Allocation Check To Prevent Segfaults
for (int i = 0; i < Table->Col_No; i++){
    Table->counter[i] = 0;
}
}

void free_counter(CSVTable *Table){
    free(Table->counter);
}

void file_info(FILE *stream,CSVTable *Table){
    bool quoted = false;
    bool no_content = false;
    while (fgets(content,content_size,stream)){
        
        for (int i = 0; content[i] != 0; i++){
            if (content[i] == '\n'){
                if (quoted) {
                    continue;
                }
                Table->Row_No = Table->Row_No + 1;
            }
            if (content[i] == ',' && Table->Row_No == 0 && quoted == false){
                Table->Col_No = Table->Col_No + 1;
            }
            if (content[i] == '"'){
                quoted = !quoted;
            }
        } // In the for loop content[i] != 0 is used instead of i < content_size 
        // so the functions reads to the end of the exact file and not just to a given buffer size.
        
    }
    if (no_content == false && strlen(content) > 0 && content[strlen(content)-1] != '\n'){
        Table->Row_No++;
    }//Handles the case where the last character in a CSV is not a newline
    //The option checks if the array isn't empty then if there is info in the content buffer (to avoid segfaults on a file that only has \n) and if the last character in the last line isn't \n if so we add 1 to Row_No

    //To-do: Handle Empty file case.
    rewind(stream);//Rewinds the file stream so it can be reused for reading
    if (fgets(content,content_size,stream) == NULL) {
            Table->Row_No = 0;
            Table->Col_No = 0;
            no_content = true;
        } //Handles empty file case 
    
    if (Table->Col_No >= 0 && no_content ==  false) {
        Table->Col_No++;
    } 
    rewind(stream);//Rewinds the stream again


}//Takes the file stream to be read from as an argument

void init_table(CSVTable *Table){
    Table->table_ptr = malloc(Table->Row_No * sizeof(char **)); // Allocate memory for every row 
    if (Table->table_ptr == NULL){
        free(Table->table_ptr);
        fprintf(stderr,"Memory Allocation Failed");
        exit(1);
    }//Memory Allocation Check To Prevent Segfaults
    for (int i = 0; i < Table->Row_No; i++){
        Table->table_ptr[i] = malloc(Table->Col_No * sizeof(char *)); //Allocate memory for every column
        if (Table->table_ptr[i] == NULL){
            free(Table->table_ptr[i]);
            fprintf(stderr,"Memory Allocation Failed");
            exit(1);
        }//Memory Allocation Check To Prevent Segfaults
        for (int j = 0; j < Table->Col_No; j++){
            Table->table_ptr[i][j] = malloc(MAX_STRING_LENGTH * sizeof(char));//Allocate memory for every character
            if (Table->table_ptr[i][j] == NULL){
                free(Table->table_ptr[i][j]);
                fprintf(stderr,"Memory Allocation Failed");
                exit(1);
            }//Memory Allocation Check To Prevent Segfaults
        }
    }
}//This function is to allocate space for a table we will store the CSV content, 
//It works by using malloc to assign a size to the array pointer at row location first we use the No of rows multiplied by sizeof char ** the reason for the
//2 pointers in front is because we are storing columns in the rows which is a pointer and text in the columns which is also a pointer, we then use a
//for loop to allocate memory for every column in a row and another one to allocate for every text in a column.
void free_table(CSVTable *Table){
    for (int i = 0; i < Table->Row_No ; i++) {
        for (int j = 0; j < Table->Col_No; j++){
            free(Table->table_ptr[i][j]);
        }
        free(Table->table_ptr[i]);
    }
    free(Table->table_ptr);
}
//Parse Pseudocode
//1. Read file
//2. counter rows
//3. counter columns
//4. Read characters
//5. When ',' is detected iterate column by one.
//6. When '\n' is detected iterate rows by one.
//7. When '"' make sure ',' doesn't iterate rows until the next '"'.
void parse_info(FILE *stream, CSVTable *Table){
    int i = 0;
    int k = 0;
    int c = 0;
    int r = 0;
    bool quoted = false; //Boolean for handling quotes
    while (fgets(content,content_size, stream)) {                   
        i = 0;
        for (;content[i] != '\0'; i++) {
            if (content[i] != '\n' && content[i] != ',' && content[i] != '"'){
                Table->table_ptr[r][c][k] = content[i];
                k++;
                if (k >= MAX_STRING_LENGTH) {
                    fprintf(stderr,"Column [%d] more than 1024 characters", c);//Handles the case where a column has more characters than the MAX STRING LENGTH
                    exit(1);
                }
                //printf("r:%d,c:%d,k:%d,i:%d\n",r,c,k,i);
            }
            else if (content[i] == ',') {
                if (quoted){
                    Table->table_ptr[r][c][k++] = content[i];//Adds the comma to the column string in the array
                    //printf("r:%d,c:%d,k:%d,i:%d\n",r,c,k,i);
                    continue;
                }
                else {
                    Table->table_ptr[r][c][k] = '\0';
                    if (Table->counter[c] < k) {
                        Table->counter[c] = k;
                    }
                    k = 0;
                    c++;
                    if (c >= Table->Col_No) {
                        fprintf(stderr,"Malformed CSV\n");
                        exit(1);
                    }//Handles the case where a row has more columns than the header row
                    //printf("r:%d,c:%d,k:%d,i:%d\n",r,c,k,i);
                    continue;
               }
            }
            else if (content[i] == '\n') {
                if (quoted) {
                    Table->table_ptr[r][c][k] = '\n';
                    k++;
                    continue;
                }
                Table->table_ptr[r][c][k] = '\0';
                r++;
                if (r > Table->Row_No) {
                    fprintf(stderr,"Malformed CSV\n");
                    exit(1);
                }
                if (Table->counter[c] < k) {
                    Table->counter[c] = k;
                }
                c = 0;
                k = 0;
                //printf("r:%d,c:%d,k:%d,i:%d\n",r,c,k,i);
                break;
            }
            else if (content[i] == '"') {
                quoted = !quoted; //This toggles the boolean true or false.
                continue;
            }
        }
    }
    if (quoted) {
            fprintf(stderr,"Malformed CSV: Improper Quotation");
            exit(1);
        } 
        
    if (c > 0 || k > 0) {
        Table->table_ptr[r][c][k] = '\0';
        if (Table->counter[c] < k) {
            Table->counter[c] = k;
        }
    }
    //printf("Row No:%d,%d\nCol No:%d,%d\nString:%d,%d\n",Table->Row_No,r,Col_No,c,MAX_STRING_LENGTH,k);
}

void print_info(int rows,CSVTable *Table){
    int r;
    printf("Rows:%d,Columns:%d\n",Table->Row_No,Table->Col_No);
    for (r = 0; r < (rows); r++) {
        for (int c = 0; c < Table->Col_No; c ++){
            printf("%-*s ",Table->counter[c],Table->table_ptr[r][c]); //%-*s right aligns a string along a 15 character wide column, %-15s left aligns.
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]){
    if (argc == 1){
       fprintf(stderr,"No Arguments Passed.\n");
        return 2;

    }
    //FILE *in_file = fopen(argv[1], "r"); //The file stream is the first argument passed to the program
    FILE *in_file;
    if (!(in_file = fopen(argv[1], "r"))){
        fprintf(stderr,"Can't Open File %s\n",argv[1]);
        return 1;
    }
    CSVTable Table1;
    file_info(in_file, &Table1);
    if (Table1.Row_No == 0 && Table1.Col_No == 0) {
        fprintf(stderr, "No Rows or Columns Detected\n");
        return 3;
    }
    //printf("Rows:%d,Columns;%d\n",Table1.Row_No,Table1.Col_No);
    init_counter(&Table1);
    init_table(&Table1);
    parse_info(in_file,&Table1);
    //printf("%s\n\n",Table->table_ptr[0][0]);
    print_info(Table1.Row_No,&Table1);
    free_table(&Table1);
    free_counter(&Table1);
    fclose(in_file);


    return 0;
}