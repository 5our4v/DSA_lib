#include <stdio.h>
#include <stdlib.h>
#include <Windows.h> // Only for system("color") on Windows
#include <conio.h>   // For getch()

#define N 40

int main()
{
    int A[N][N], B[N][N], product[N][N];
    int i, j, k;
    int arows, acolumns, brows, bcolumns;
    int sum = 0;

    system("color 70"); // Light background and dark text

start:
    // Input Matrix A
    printf("Enter the number of rows of MATRIX A: ");
    scanf("%d", &arows);
    printf("Enter the number of columns of MATRIX A: ");
    scanf("%d", &acolumns);
    printf("\nInput elements for MATRIX A:\n");
    for (i = 0; i < arows; i++)
    {
        for (j = 0; j < acolumns; j++)
        {
            printf("A[%d][%d] = ", i + 1, j + 1);
            scanf("%d", &A[i][j]);
        }
    }

    // Input Matrix B
    printf("\nEnter the number of rows of MATRIX B: ");
    scanf("%d", &brows);
    printf("Enter the number of columns of MATRIX B: ");
    scanf("%d", &bcolumns);
    printf("\nInput elements for MATRIX B:\n");
    for (i = 0; i < brows; i++)
    {
        for (j = 0; j < bcolumns; j++)
        {
            printf("B[%d][%d] = ", i + 1, j + 1);
            scanf("%d", &B[i][j]);
        }
    }

    // Matrix multiplication rule check
    if (acolumns != brows)
    {
        printf("\n Matrix multiplication is not possible.\n");
        printf("Reason: Columns of MATRIX A must equal Rows of MATRIX B.\n");
    }
    else
    {
        // Matrix multiplication logic
        for (i = 0; i < arows; i++)
        {
            for (j = 0; j < bcolumns; j++)
            {
                sum = 0;
                for (k = 0; k < acolumns; k++)
                {
                    sum += A[i][k] * B[k][j];
                }
                product[i][j] = sum;
            }
        }

        // Print matrix multiplication steps
        printf("\n\n Resultant MATRIX A x MATRIX B (with computation steps):\n");
        printf("------------------------------------------------------------\n");

        for (i = 0; i < arows; i++)
        {
            for (j = 0; j < bcolumns; j++)
            {
                printf("C[%d][%d] = ", i + 1, j + 1);
                for (k = 0; k < acolumns; k++)
                {
                    printf("A[%d][%d]*B[%d][%d](%d*%d)",
                           i + 1, k + 1, k + 1, j + 1,
                           A[i][k], B[k][j]);

                    if (k != acolumns - 1)
                        printf(" + ");
                }
                printf(" = %d\n", product[i][j]);
            }
            printf("\n");
        }

        // Print final matrix only (optional)
        printf("\n Final Resultant Matrix (A x B):\n");
        for (i = 0; i < arows; i++)
        {
            for (j = 0; j < bcolumns; j++)
            {
                printf("(%3d) ", product[i][j]);
            }
            printf("\n");
        }
    }

    printf("\nPress any key to perform another multiplication or Ctrl+C to exit...\n");
    getch();
    printf("\n\n------------------------------------------------------------\n\n");
    goto start;
}
