#include <stdio.h>
#include <stdlib.h>
int main(){
    char n[11];
    int i;

    scanf("%s", n);
    scanf("%d", &i);

    if (i == -1){
        printf("%s\n", n);
    }
    else if((-1 < i) & (i <= 9)){
        printf("%c\n", n[i]);
    }
    
    else {
        printf("%s\n", "ERROR");
        return 1;
    }
    return 0;
}