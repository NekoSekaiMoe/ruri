#include <cstdio>
#include <cstdlib>

int main() {
    int *p = (int *)malloc(sizeof(int));
    free(p);
    *p = 42; // 触发 ASan 报错
    return 0;
}

