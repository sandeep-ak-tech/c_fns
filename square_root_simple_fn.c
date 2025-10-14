#include <stdio.h>
#include <stdlib.h>

/*Function prototypes*/
float get_sqrt(int);

/*
Function name : main
parameters - void
Return type - int
*/
int main(){
    int num = 16;

    printf("Square root of %d is %f\r\n", num, get_sqrt(num));
}

/*
Function name : get_sqrt
parameters - int
Return type - float
*/
float get_sqrt(int num)
{
    int first =0, last = num, mid = 0;
    float ans = 0;

    while(first <= last)
    {
        mid = (first+last)/2;

        if(mid * mid == num)
        {
            ans = (float)mid;
            break;
        }
        else if (mid* mid <= num)
        {
            ans = first;
            first = mid+1;
        }
        else 
        {
            last = mid-1;
        }

    }

    float increment = 0.1;

    for(int i=0;i<5;i++)
    {
        while(ans * ans < num)
        {
            ans += increment;
        }

        ans-=increment;
        increment/=10;
        
    }
    return ans;
}