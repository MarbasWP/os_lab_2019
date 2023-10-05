#include <stdio.h>
#include <dlfcn.h>
#include "revert_string.h"

int main() {
    void *handle = dlopen("librevert_string.so", RTLD_LAZY);
    void (*RevertString)(char *str) = dlsym(handle, "RevertString");

    char str[] = "Hello, World!";
    RevertString(str);

    printf("Reverted (Dynamic): %s\n", str);

    dlclose(handle);
    return 0;
}