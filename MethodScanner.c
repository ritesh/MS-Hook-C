/*
 *
 * Author: Grant Douglas (@Hexploitable)
 *
 * Description: Use this when the binary has been stripped and the function isn't exported.
 *              I.e. when you can't use MSFindSymbol().
 *
 * Usage:       Open app in disassembler, grab first 16 bytes (might need to tweak this)
 *              of your target method. Use this as signature. Define sigsize as number of bytes you're using.
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <mach/vm_map.h>
#include <mach-o/dyld_images.h>
#include <string.h>

int pid = 0;
int g_pid = 0;
unsigned char *needle;

mach_vm_address_t *scanMem(int pid, mach_vm_address_t addr, mach_msg_type_number_t size)
{
    task_t t;
    task_for_pid(mach_task_self(), pid, &t);
    mach_msg_type_number_t dataCnt = size;
    mach_vm_address_t max = addr + size;
    int bytesRead = 0;
    kern_return_t kr_val;
    pointer_t memStart;
    uint32_t sz;
    int len = strlen(needle);
    unsigned char buffer[len];
    int i = 0;


    while (bytesRead < size)
    {
        if ((kr_val = vm_read(t, addr, len, &memStart, &sz)) == KERN_SUCCESS)
        {
            memcpy(buffer, (const void *)memStart, len);
            if (memcmp(buffer, needle, len) == 0)
            {
                fflush(stdout);
                return (unsigned long long *)addr;
            }
            else
                printf("[-] %p ---> vm_read()\r", (void *) addr);
            fflush(stdout);
        }
        else
        {
            printf("[-] %p ---> vm_read().\r", (void *) addr);
            fflush(stdout);
        }
        addr += sizeof(unsigned char);
        bytesRead += sizeof(unsigned char);
    }
    printf("[i] Scanning ended without a match.\r\n");
    fflush(stdout);
    return NULL;
} 


unsigned int *getMemRegions(task_t task, vm_address_t address)
{
    kern_return_t kret;
    vm_region_basic_info_data_t info;
    vm_size_t size;
    mach_port_t object_name;
    mach_msg_type_number_t count;
    vm_address_t firstRegionBegin;
    vm_address_t lastRegionEnd;
    vm_size_t fullSize;
    count = VM_REGION_BASIC_INFO_COUNT_64;
    int regionCount = 0;
    int flag = 0;


    while (flag == 0)
    {
        //Attempts to get the region info for given task
        kret = mach_vm_region(task, &address, &size, VM_REGION_BASIC_INFO, (vm_region_info_t) &info, &count, &object_name);
        if (kret == KERN_SUCCESS)
        {
            if (regionCount == 0)
            {
                firstRegionBegin = address;
                regionCount += 1;
            }
            fullSize += size;
            address += size;
        }
        else
            flag = 1;
    }
    lastRegionEnd = address;
    printf("[i] Region to scan: %p - %p\n", (void *)firstRegionBegin, (void *)lastRegionEnd);
    unsigned int *ptrToFunc = (unsigned int *)scanMem(pid, firstRegionBegin, fullSize);
    return ptrToFunc;
}


int main(int argc, char** argv) {
    kern_return_t rc;
    mach_port_t task;
    mach_vm_address_t addr = 1;


    if (argc >= 3)
    {
        pid = atoi(argv[1]);
        g_pid = pid; //Required for fw >= 6.0    
        rc = task_for_pid(mach_task_self(), pid, &task);
        if (rc)
        {
            fprintf(stderr, "[-] task_for_pid() failed, error %d - %s", rc, mach_error_string(rc));
            exit(1);
        }
        needle = argv[2];
        printf("[+] PID: %d\n[i] Task: %d\n[+] Needle: \"%s\"\n", pid, task, needle);
        unsigned int *sym = getMemRegions(task, addr);
        if (sym != NULL)
            printf("\n\n\t[$] Located target function ---> %p\n\n", sym);
        else
            printf("[-] Didn\'t find the function.\n");
    }
    else
        fprintf(stderr, "[i] Usage: %s <pid> $'<needle>'\n", argv[0]);
    return 0;
}
