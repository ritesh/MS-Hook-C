#include <stdlib.h>
#include <stdio.h>

int getAge(void);


int main(int argc, char* argv[])
{
    int r = arc4random() % 10000;
    printf("[i] Test application for hooking. ARC: %p, gA: %p\n", &arc4random, &getAge);
    printf("[+] Grant is %d years old.\n[+] Random number is: %d\n\n", getAge(), r);
    return 0;
}


int getAge(void)
{
    return 21;
}