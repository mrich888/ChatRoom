#include <stdio.h>

#define SIZE 10

int main()
{
    int a = SIZE;
    if (a > 0)
    {
        printf("is a 正数");
    }
    else
    {
        printf("是个负数");
    }



    int num = 0;
    num += SIZE;
    printf("num : %d\n", num);



    return 0;
}