#ifndef _NNROK_H
#define _NNROK_H

#include<gsl/gsl_math.h>
#include<gsl/gsl_matrix.h>
#include<gsl/gsl_matrix_float.h>
#include<gsl/gsl_vector.h>


#define THETA2_FNAME "theta2"
#define THETA1_FNAME "theta1"
gsl_matrix_float *read_theta(char *filename, int s1, int s2);

int predict(gsl_matrix_float *theta1, gsl_matrix_float *theta2, gsl_vector *inVector, float *certain);

#endif
