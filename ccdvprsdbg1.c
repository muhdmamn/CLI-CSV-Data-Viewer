#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define content_size 256
#define MAX_STRING_LENGTH 1024
char table_ptr[4][4][MAX_STRING_LENGTH]; 
int Row_No = 4;
int Col_No = 4;
char content[content_size];
char content_table ;



void parse_info(FILE *stream){
    int i = 0;
    int k = 0;
    int c = 0;
    int r = 0;
    while (fgets(content,content_size, stream)) {
        for (;content[i] != '\0'; i++) {
            if (content[i] != '\n' && content[i] != ','){
                table_ptr[r][c][k] = content[i];
                k++;
                continue;
            }
            else if (content[i] == ',') {
                table_ptr[r][c][k] = '\0';
                c++;
                k = 0;
                continue;
            }
            else if (content[i] == '\n') {
                table_ptr[r][c][k] = '\0';
                r++;
                c = 0;
                k = 0;
                i = 0;
                break;
            }
        }
    }
}

int main(){
    parse_info(stdin);
    printf("%s\n",table_ptr[3][0]);
}