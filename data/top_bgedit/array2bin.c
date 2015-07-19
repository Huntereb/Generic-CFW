#include <stdio.h>
#include "array.h"

int main(){
    FILE *fp;
    fp = fopen("out.bin", "wb");
    if(fp){
        fwrite(array, 1, 0x46500, fp);
    }
    fclose(fp);
    return 0;
}