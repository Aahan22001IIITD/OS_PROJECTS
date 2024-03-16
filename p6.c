#include <stdio.h>

// Function to calculate the nth Fibonacci number using recursion
int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

int main() {
    int n;
    printf("HELLO 6");
    printf("%d\n",fibonacci(19));
    printf("%d\n",fibonacci(55));

    return 0;
}
