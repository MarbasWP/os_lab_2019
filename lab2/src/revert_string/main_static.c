#include <stdio.h>
#include "revert_string.h"

int main() {
    char str[] = "Hello, World!";
    RevertString(str);
    printf("Reverted (Static): %s\n", str);
    return 0;
}