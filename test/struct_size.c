#include<stdio.h>

typedef struct {
float x;
float y;
} data;

int main(){
    printf("size of struct data is %zu\n", sizeof(data));

}