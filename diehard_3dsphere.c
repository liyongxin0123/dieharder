/*
 * $Id$
 *
 * See copyright in copyright.h and the accompanying file COPYING
 */

/*
 *========================================================================
 * This is the Diehard 3d spheres test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 * Basically, we choose 4000 points in a cube of side 1000.  Compute the
 * smallest nearest neighbor distance (radius R of the smallest sphere
 * that doesn't overlap any neighboring point). R^3 is exponentially
 * distributed with an empirical exponential distribution with mean 30.
 * Thus p = 1.0 - exp(-R^3/30.0) should be a uniform distribution.  Run
 * a KS test on a vector of independent samples of this entire test to
 * find out.
 *========================================================================
 */


#include "rand_rate.h"
#define POINTS 4000
#define DIM 3

typedef struct {
  double x[3];
} C3;
 
int diehard_3dsphere()
{

 int i,j,k,l,m;
 C3 *c3;
 double r1,r2,r3,rmin,r3min;
 double *pvalue;

 /*
  * This one should be pretty straightforward.  Generate a vector
  * of three random coordinates in the range 0-1000 (check the
  * diehard code to see what "in" a 1000^3 cube means, but I'm assuming
  * real number coordinates greater than 0 and less than 1000).  Do
  * a simple double loop through to float the smallest separation out.
  * Generate p, save in a sample vector.  Apply KS test.
  */
 pvalue = (double *)malloc(samples*sizeof(double));
 c3 = (C3 *)malloc(POINTS*sizeof(C3));

 for(i=0;i<samples;i++){
  rmin = 2000.0;
  for(m=0;m<POINTS;m++){

   /* Pick 1000 points in the cube */
   for(j=0;j<POINTS;j++){
     /* This is a portable way to generate a 3 vector */
     for(k=0;k<DIM;k++) c3[j].x[k] = 1000.0*gsl_rng_uniform_pos(rng);
     if(j<0){
       printf("%d: (%8.2f,%8.2f,%8.2f)\n",j,c3[j].x[0],c3[j].x[1],c3[j].x[2]);
     }
   }
   
   for(j=0;j<POINTS-1;j++){
     for(k=j+1;k<POINTS;k++){
       r2 = 0.0;
       for(l=0;l<DIM;l++) r2 += c3[j].x[l]*c3[k].x[l];
       r1 = sqrt(r2);
       r3 = r2*r1;
       /* printf("rmin = %f, r1 = %f\n",rmin,r1); */
       if(r1<rmin) {
         rmin = r1;
	 r3min = r3;
       }
     }
   }
  }

  printf("Found rmin = %f  (r^3 = %f)\n",rmin,r3min);
  pvalue[i] = 1.0 - exp(-r3min/30.0);
  printf("p-value[%d] = %f\n",i,pvalue[i]);

 }

 if(!quiet){
   printf("#==================================================================\n");
   printf("#              Diehard \"3d Sphere\" test (modified).\n");
   printf("# Generate 4000 points in a 1000^3 integer cube.  Determine the\n");
   printf("# the shortest nearest neighbor distance R.  This should generate\n");
   printf("# p = 1.0 - exp(-R^3/30).  Repeat for lots of samples, apply a\n");
   printf("# KS test to see if p is uniform.\n");
   printf("#==================================================================\n");
   printf("# Random number generator tested: %s\n",gsl_rng_name(rng));
   printf("# size of vector tested = %u\n",size);
 }

}
