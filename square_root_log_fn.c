#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
To find the square root of a given number N using the logarithm approach, you can use the log2() function. 
The method is based on the mathematical relationship between the square root and logarithms. 
Specifically, if d is the square root of N, then:

d = N^(1/2)

Applying the base-2 logarithm (log2) to both sides:

log2(d) = log2(N^(1/2))

Using the logarithm power rule, this simplifies to:

log2(d) = (1/2) * log2(N)

Solving for d:

d = 2^((1/2) * log2(N))

Thus, the square root d can be computed as:

d = pow(2, 0.5 * log2(N))
*/

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

    float ans = 0;

    ans = pow(2, 0.5 * log2(num));

    return ans;

}