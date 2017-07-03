#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    int cnt;

    cnt = unlink("tmp.aaa");
    if (cnt == -1){

