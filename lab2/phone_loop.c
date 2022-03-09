#include <stdio.h>

int main(){
    
    char n[11];
    scanf("%s", n);
    int i;
    int error = 0;
    while (scanf("%d", &i) != EOF){

        if (i == -1){
            printf("%s\n", n);
        }
        else if((-1 < i) & (i <= 9)){
            printf("%c\n", n[i]);
        }
    
        else {
            printf("%s\n", "ERROR");
            error = 1;
        }
    }
    return error;

}