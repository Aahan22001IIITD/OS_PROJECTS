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
    printf("HELLO 2");
    printf("%d\n",fibonacci(9));
    printf("%d\n",fibonacci(45));

    return 0;
}
