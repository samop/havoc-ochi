#include<math.h>
#include<gsl/gsl_math.h>
#include<gsl/gsl_matrix.h>
#include<gsl/gsl_matrix_float.h>
#include<gsl/gsl_vector.h>
#include<gsl/gsl_blas.h>
#include<leptonica/allheaders.h>

/* function reads data from files generated from octave with save -ascii filename variablename */
gsl_matrix_float *read_theta(char *filename, int s1, int s2){
	int i,j;
	float tmp;
	FILE *fd=fopen(filename,"r");
	gsl_matrix_float *m=gsl_matrix_float_alloc(s1,s2);
	for(i=0;i<s1;i++){
		for(j=0;j<s2;j++){
			fscanf(fd,"%f ",&tmp);
			gsl_matrix_float_set(m,i,j,tmp);
		}
	}
	
	fclose(fd);
	return m;
}

/* does sigmoid function over all elements in matrix inline  */
void sigmoid(gsl_matrix_float *z){
	int i,j;
	for(i=0;i<z->size1;i++)
		for(j=0;j<z->size2;j++)
			gsl_matrix_float_set(z,i,j,1.0/(1.0+exp(-gsl_matrix_float_get(z,i,j))));
}

/* does prediction of numeral presented as vector of pixels */
int predict(gsl_matrix_float *theta1, gsl_matrix_float *theta2, gsl_vector *inVector, float *certain){
	int i;
	long unsigned int result, useless;
	gsl_matrix_float *inMatrix=gsl_matrix_float_alloc(1,inVector->size+1);
	//build [ones(m, 1) X], where m = size(X,1)
	gsl_matrix_float_set(inMatrix,0,0,1.0);
	for(i=1;i<inMatrix->size2;i++){
		gsl_matrix_float_set(inMatrix,0,i,gsl_vector_get(inVector,i-1));	
	}
	gsl_matrix_float *h1=gsl_matrix_float_alloc(1,theta1->size1);
	gsl_matrix_float *h1p=gsl_matrix_float_alloc(1,theta1->size1+1);
	gsl_matrix_float *h2=gsl_matrix_float_alloc(1,theta2->size1);
	gsl_blas_sgemm(CblasNoTrans, CblasTrans,1.0,inMatrix,theta1,0.0,h1);
	sigmoid(h1);
	gsl_matrix_float_set(h1p,0,0,1.0);
	for(i=1;i<h1p->size2;i++){
		gsl_matrix_float_set(h1p,0,i,gsl_matrix_float_get(h1,0,i-1));	
	}
	gsl_blas_sgemm(CblasNoTrans, CblasTrans,1.0,h1p,theta2,0.0,h2);
	sigmoid(h2);
	for(i=0;i<h2->size2;i++){
		fprintf(stderr,"%f ",gsl_matrix_float_get(h2,0,i));
	}
	gsl_matrix_float_max_index(h2,&useless,&result);
	gsl_matrix_float_free(h1);
	gsl_matrix_float_free(h2);
	gsl_matrix_float_free(h1p);
	gsl_matrix_float_free(inMatrix);
	*certain=gsl_matrix_float_max(h2);
	return result;
}

