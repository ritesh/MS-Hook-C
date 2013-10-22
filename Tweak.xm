#include <substrate.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

/*
 *
 * Author: Grant Douglas (@Hexploitable)
 *
 * Description: Use this to hook exported functionality. Such as methods from libc etc or methods exported 
 *              within the clients binary. This Tweak assumes that _getAge is exported or binary hasn't been
 *              stripped.
 *
*/


static int (*original_getAge)(void);  // a function pointer to store the original.
int replaced_getAge(void) {
    int rVal;
    printf("[+] Enter a return value for getAge: ");
    scanf("%d", &rVal);
    return rVal;
}


static int (*original_arc4random)(void);
int replaced_arc4random(void)
{
    int rVal;
    printf("[+] Enter a return value for arc4random: ");
    scanf("%d", &rVal);
    return rVal;
}


%ctor {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    //If Exported || If we have symbols
            //MSImageRef image = MSGetImageByName("/var/root/a.out");
            //Look for our target method
            //void *sym = MSFindSymbol(image, "_getAge");
    //Else
    unsigned int *sym = (unsigned int *)0xdeadbeef;

    //Hook the symbol we found
    MSHookFunction((void *)sym, (void *)replaced_getAge, (void **)&original_getAge);
    //Hook arc4random using it's own symbol (We don't have to look this up)
    MSHookFunction((void *)arc4random, (void *)replaced_arc4random, (void **)&original_arc4random);
    [pool release];
}