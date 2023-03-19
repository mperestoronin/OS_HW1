#include <stdio.h>
#include <ctype.h>

int main() {
int count[256] = {0};
int c;
while ((c = getchar()) != EOF) {
    if (ispunct(c)) {
        count[c]++;
    }
}

for (int i = 0; i < 256; i++) {
    if (count[i] > 0) {
        printf("%c: %d\n", i, count[i]);
    }
}

return 0;
}
