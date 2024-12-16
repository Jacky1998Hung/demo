#include <stdio.h>

int main() {
    int i = 0, sum = 0;

    while (i < 10) { // Loop header
        if (i % 2 == 0) {
            i++;        // Path 1: Increment i and jump back to the header
        } else if (sum > 15) {
            i += 2;     // Path 2: Increment i differently and jump back
        } else {
            sum += i;   // Path 3: Normal increment after adding i
            i++;
        }
    }

    return 0;
}

