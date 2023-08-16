#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int solve_qeq(double a, double b, double c, double *roots)
{
    printf("I am in solve_qeq\n");
    if (roots == NULL) {
        fprintf(stderr, "Memory for the roots array was not provided\n");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "root is memory\n");
    }
    fflush(stdout);
    if (a == 0.0) {
        roots[0] = -c / b;
        roots[1] = -c / b;
    } else {
        double D = pow(b, 2.0) - 4 * a* c;
        if (b > 0) {
            roots[0] = (-b - sqrt(D)) / (2 * a);
            roots[1] = c / (a * roots[0]);
        } else {
            roots[0] = (-b + sqrt(D)) / (2 * a);
            roots[1] = c / (a * roots[0]);
        }
    }

    return 0;
}
