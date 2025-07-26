
/*
Recursion in C Programming
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -

    (1)What is Recursion?
     ->Recursion is a process in which a function calls itself directly or indirectly to solve a problem.
     It is often used to solve problems that can be broken down into smaller sub - problems of the same type.


    (1.1)Formal Definition:
    ->A recursive function is a function that solves a problem by calling itself on a smaller input, and combines the result to form the solution to the original problem.

    (1.2) Fundamentals of Recursion:
        Every recursive function must have :

        1. Base Case – A condition that stops the recursion.
        2. Recursive Case – The part where the function calls itself to break down the problem.



   (1.3) Types of Recursion:

    [i]. Direct Recursion : The function calls itself directly.

            void fun()
        {
            // some code
            fun(); // direct recursive call
        }

    [ii]. Indirect Recursion : Function A calls function B, and B calls function A.

        void A()
        {
            B();
        }
        void B()
        {
            A();
        }

    [iii]. Tail Recursion : The recursive call is the last operation in the function.

        int tailFact(int n, int result)
        {
            if (n == 0)
                return result;
            print("%d",n);

            return tailFact(n - 1, n * result);
        }

    [iv]. Head Recursion : The recursive call happens before any operation in the function.

          void headRec(int n)
            {
                if (n == 0)
                    return;
                headRec(n - 1); // recursive call first
                printf("%d ", n);
            }

    [v]. Tree Recursion : A function calls itself more than once.

            int fib(int n)
            {
                if (n <= 1)
                    return n;
                return fib(n - 1) + fib(n - 2); // two recursive calls
            }



(1.4)How Recursion Works Internally (Call Stack):
---------------------------------------------------
    Each recursive call is stored in the call stack with its own copy of variables.
    As base case is reached, the stack starts unwinding, returning values back to previous calls.

** Without a base case, recursion will continue infinitely and cause stack overflow.



(*)When to Use Recursion:
--------------------------------
- Problems with repetitive sub-structure
- Divide and conquer algorithms (e.g., Merge Sort, Quick Sort)
- Tree and graph traversal
- Backtracking problems (e.g., Sudoku, N-Queens)
- Mathematical series (Factorial, Fibonacci, GCD, etc.)



(*) Drawbacks of Recursion:
-------------------------------
- High memory usage
- May cause stack overflow
- Usually slower than loops for simple problems

(*)Recursive vs Iterative:
----------------------------

| Feature         | Recursion           | Iteration            |
|-----------------|---------------------|----------------------|
| Uses stack      | Yes                 | No                   |
| Memory usage    | Higher              | Lower                |
| Speed           | Slower              | Faster               |
| Code clarity    | Better for complex  | Better for simple    |

*/

// sum of N natural number : 1+2_3+4+........+n= ?
/*
#include <stdio.h>
#include <stdlib.h>

int sum_itr(int n)
{
    int sum = 0;
    while (n != 0)
    {
        sum = sum + n;
        n--;
    }
    return sum;
}

int sum_formulla(int n)
{
    return (n * (n + 1)) / 2;
}

int sum_rec(int n)
{
    if (n == 0)
    {
        return 0;
    }
    else
        return sum_rec(n - 1) + n;
}

int main()
{
    int n = 5;
    printf("using formulla ,sum = %d O(1)", sum_formulla(n));
    printf("\nusing iteration ,sum = %d O(n)", sum_itr(n));
    printf("\nmusing recursion ,sum = %d O(n+1)", sum_rec(n));
}
*/

/*
// Factorials :

#include <stdio.h>
#include <stdlib.h>

int fact_itr(int n) // O(n)
{
    int result = 1;
    while (n != 0)
    {
        result = result * n;
        n--;
    }
    return result;
}

int fact_rec(int n) // O(n+1)
{
    if (n == 0)
    {
        return 1;
    }
    else
        return fact_rec(n - 1) * n;
}

int main()
{
    int n = 5;
    printf("\nusing iteration ,result = %d ", fact_itr(n));
    printf("\nmusing recursion ,result = %d ", fact_rec(n));
}
*/

/*
// expontent (m)^n

#include <stdio.h>

int exponent(int base, int powwer)
{

    if (powwer == 0)
    {
        return 1;
    }
    else
        return exponent(base, powwer - 1) * base;
}

int main()
{
    printf("result= %d ", exponent(10, 5));
}





// Tailor series expansions e^x

#include <stdio.h>
#include <stdlib.h>

float power(float base, float head)
{
    if (head == 0)
    {
        return 1;
    }
    return base * power(base, head - 1);
}

int fact(int n)
{
    if (n == 0)
    {
        return 1;
    }
    return n * fact(n - 1);
}

float T_ex(float x, int order)
{

    if (order == 0)
    {
        return 1;
    }
    float term = power(x, order) / fact(order);

    return term + T_ex(x, order - 1);
}
int main()
{
    for (int i = 0; i <= 10; i++)
    {
        float res = T_ex(i, 12);
        printf("\ne^%d = %f", i, res);
    }
}




#include <stdio.h>

double e(int x, int order)
{
    static double p = 1, f = 1;
    double r;

    if (order == 0)
    {
        return 1;
    }

    r = e(x, order - 1);
    p = p * x;
    f = f * order;
    return r + p / f;
}

int main()
{

    double res = e(1, 100);
    printf("e^4 = %lf\n", res);

    return 0;
}



// reduce order o(n):

#include <stdio.h>

double e(float x, int order)
{
    double s = 1;

    for (int i = order; i >= 1; i--)
    {
        s = 1 + (x / i) * s;
    }
    return s;
}
int main()
{
    double res;

    res = e(15, 100);
    printf("%lf", res);
}



// Tower of Honoi

#include <stdio.h>

int TOH(int num_disk, char A, char B, char C)
{
    static int move_count = 0;
    if (num_disk == 1)
    {
        printf("Move disk 1 from %c to %c\n", A, C);
        move_count = move_count + 1;
        return move_count;
    }

    TOH(num_disk - 1, A, C, B);                             // Move n-1 disks from A to B
    printf("Move disk %d from %c to %c\n", num_disk, A, C); // Move nth disk from A to C
    TOH(num_disk - 1, B, A, C);                             // Move n-1 disks from B to C
}

int main()
{
    int n = 50; // You can change this to any number of disks
    int steps = TOH(n, 'A', 'B', 'C');
    printf("%d", steps);

    return 0;
}

*/