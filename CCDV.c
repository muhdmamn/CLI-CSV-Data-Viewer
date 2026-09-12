#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define content_size 1024
#define MAX_STRING_LENGTH 1024
char ***table_ptr; //A pointer is used instead of an array directly because you cannot use Variable length Array Declaration at file scope.
//so a pointer is used because it allows you to use malloc to allocate its size as if it was an array later in the code.
int Row_No;
int Col_No;
char content[content_size];
char content_table;
int rows_opt;
int *counter;

void init_counter(){
counter = malloc(Col_No * sizeof(int));
for (int i = 0; i < Col_No; i++){
    counter[i] = 0;
}
}

void free_counter(){
    free(counter);
}

void file_info(FILE *stream){
    bool quoted = false;
    while (fgets(content,content_size,stream)){
        for (int i = 0; content[i] != 0; i++){
            if (content[i] == '\n'){
                Row_No = Row_No + 1;
            }
            if (content[i] == ',' && Row_No == 0 && quoted == false){
                Col_No = Col_No + 1;
            }
            if (content[i] == '"'){
                quoted = !quoted;
            }
        } // In the for loop content[i] != 0 is used instead of i < content_size 
        // so the functions reads to the end of the exact file and not just to a given buffer size.
        
    }
    if (Col_No > 0) {
        Col_No++;
    }
    rewind(stream);//Rewinds the file stream so it can be reused for reading

}//Takes the file stream to be read from as an argument

void init_table(){
    table_ptr = malloc(Row_No * sizeof(char **)); // Allocate memory for every row 

    for (int i = 0; i < Row_No; i++){
        table_ptr[i] = malloc(Col_No * sizeof(char *)); //Allocate memory for every column
        for (int j = 0; j < Col_No; j++){
            table_ptr[i][j] = malloc(MAX_STRING_LENGTH * sizeof(char));//Allocate memory for every string
        }
    }
}//This function is to allocate space for a table we will store the CSV content, 
//It works by using malloc to assign a size to the array pointer at row location first we use the No of rows multiplied by sizeof char ** the reason for the
//2 pointers in front is because we are storing columns in the rows which is a pointer and text in the columns which is also a pointer, we then use a
//for loop to allocate memory for every column in a row and another one to allocate for every text in a column.
void free_table(){
    for (int i = 0; i < Row_No ; i++) {
        for (int j = 0; j < Col_No; j++){
            free(table_ptr[i][j]);
        }
        free(table_ptr[i]);
    }
    free(table_ptr);
}
//Parse Pseudocode
//1. Read file
//2. counter rows
//3. counter columns
//4. Read characters
//5. When ',' is detected iterate column by one.
//6. When '\n' is detected iterate rows by one.
//7. When '"' make sure ',' doesn't iterate rows until the next '"'.
void parse_info(FILE *stream){
    int i = 0;
    int k = 0;
    int c = 0;
    int r = 0;
    while (fgets(content,content_size, stream)) {
        i = 0;
        bool quoted = false; //Boolean for handling quotes
        for (;content[i] != '\0'; i++) {
            if (content[i] != '\n' && content[i] != ',' && content[i] != '"'){
                table_ptr[r][c][k] = content[i];
                k++;
                //printf("r:%d,c:%d,k:%d,i:%d\n",r,c,k,i);
            }
            else if (content[i] == ',') {
                if (quoted){
                   // printf("r:%d,c:%d,k:%d,i:%d\n",r,c,k,i);
                    continue;
                }
                else {
                    table_ptr[r][c][k] = '\0';
                    if (counter[c] < k) {
                        counter[c] = k;
                    }
                    k = 0;
                    c++;
                    //printf("r:%d,c:%d,k:%d,i:%d\n",r,c,k,i);
                    continue;
               }
            }
            else if (content[i] == '\n') {
                table_ptr[r][c][k] = '\0';
                r++;
                if (counter[c] < k) {
                    counter[c] = k;
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
    printf("Row No:%d,%d\nCol No:%d,%d\nString:%d,%d\n",Row_No,r,Col_No,c,MAX_STRING_LENGTH,k);
}

void print_info(int rows){
    bool cont = false;
    int PAGE = 1;
    char opt;
    int r;
    printf("Rows:%d,Columns:%d\n",Row_No,Col_No);
    for (r = 0; r < (rows); r++) {
        for (int c = 0; c < Col_No; c ++){
            printf("%-*s ",counter[c],table_ptr[r][c]); //%-*s right aligns a string along a 15 character wide column, %-15s left aligns.
        }
        printf("\n");
    }
}

void tbd(int rows){
    int PAGE = 1;
    char opt;

    printf("PAGE: %d\n",PAGE);
    printf("Continue? y/n\n");
    fgets(&opt, 1 , stdin);
    if (opt == 'Y' || opt == 'y') {
        rows = rows + rows;
        print_info(rows);
    }
    else if (opt == 'n' || opt == 'N') {
        printf("Okay");
    }
    else {
        printf("Invalid Input");
    }
}

int main(int argc, char *argv[]){
    if (argc == 1){
       fprintf(stderr,"No Arguments Passed.\n");
        return 2;

    }
    char arr[content_size];
    //FILE *in_file = fopen(argv[1], "r"); //The file stream is the first argument passed to the program
    FILE *in_file;
    if (!(in_file = fopen(argv[1], "r"))){
        fprintf(stderr,"Can't Open File %s\n",argv[1]);
        return 1;
    }
    file_info(in_file);
    if (Row_No == 0 && Col_No == 0) {
        fprintf(stderr, "No Rows or Columns Detected");
        return 3;
    }
    init_counter();
    init_table();
    parse_info(in_file);
    print_info(Row_No);
    free_table();
    free_counter();
    fclose(in_file);


    return 0;
}