/*
What is the output of the program?
Ans:- 10Hello World
Play around with return values in the below function.

*/


#include <stdio.h>
int print_a(int a)
{
    printf("%d",a);
    return 1;
}

int print_b(int b)
{
    printf("%d",b);
    return 0;
}

int main()
{ 
    int a=10,b=20;
    if(print_a(a) || print_b(b))
    {
        printf("Hello World");
    }

    return 0;
}