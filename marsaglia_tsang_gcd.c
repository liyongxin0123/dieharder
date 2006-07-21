/*
 * $Id: diehard_craps.c 191 2006-07-13 08:23:50Z rgb $
 *
 * See copyright in copyright.h and the accompanying file COPYING
 *
 */

/*
 *========================================================================
 *                    Marsaglia and Tsang GCD Test
 * 
 * 10^7 tsamples (default) of uint rands u, v are generated and two
 * statistics are generated: their greatest common divisor (GCD) (w)
 * and the number of steps of Euclid's Method required to find it
 * (k).  Two tables of frequencies are thus generated -- one for the
 * number of times each value for k in the range 3 to 35 (with counts
 * greater than or less than this range lumped in with the endpoints).
 * The other table is the frequency of occurrence of each GCD w.
 * k should be distributed (approximately) binomially -- a chisq test
 * is performed on a scaled table provided by Marsaglia and Tsang that
 * is a slight improvement on this.  w is distributed like
 * (6/pi^2) 1/w^2 (no table is needed) and a chisq test converts
 * this as well to a p-value.  In the dieharder implementation p_k and
 * p_w are accumulated separately psamples (100) times and a final
 * KS test is performed to generate overall p-values for both aspects
 * of the test.
 *
 * Actually, at the moment I don't have the table so I'll use the
 * binomial distribution P(k) = (50 k).376^k .624^{50-k) as the target
 * of the chisq until I can do better.  I suspect there is a prime factor
 * theorem that would give the exact distribution but need time to verify
 * this (or to solicit MT's table or to make my own empirical table)
 *========================================================================
 */

#include "dieharder.h"
/*
 * Test specific data
 */
#include "marsaglia_tsang_gcd.h"

double marsaglia_tsang_gcd()
{

 double pks;
 uint ps_save,ts_save;

 /*
  * Do a standard test if -a(ll) is selected.
  * ALSO use standard values if tsamples or psamples are 0
  */
 if(all == YES){
   ts_save = tsamples;
   tsamples = dtest->tsamples_std;
   ps_save = psamples;
   psamples = dtest->psamples_std;
 }
 if(tsamples == 0){
   tsamples = dtest->tsamples_std;
 }
 if(psamples == 0){
   psamples = dtest->psamples_std;
 }
 
 /*
  * Allocate memory for THIS test's ks_pvalues, etc.  Make sure that
  * any missed prior allocations are freed.  THIS test generates TWO
  * statistics (like the craps test) so we need to allocate two
  * tables for p-values.
  */
 if(ks_pvalue) nullfree(ks_pvalue);
 ks_pvalue  = (double *)malloc((size_t) psamples*sizeof(double));
 if(ks_pvalue2) nullfree(ks_pvalue2);
 ks_pvalue2  = (double *)malloc((size_t) psamples*sizeof(double));

 /*
  * Reseed FILE random number generators once per individual test.
  * This correctly resets the rewind counter per test.  Also reset
  * the RNG if Seed was specified, as only that will give us a
  * standard test result independent of test order in an -all run.
  */
 if(strncmp("file_input",gsl_rng_name(rng),10) == 0){
   gsl_rng_set(rng,1);
 } else if(Seed){
   gsl_rng_set(rng,Seed);
 }

 test_header(dtest);
 /*
  * Any custom test header output lines go here.  They should be
  * used VERY sparingly.
  */

 /*
  * This is the standard test call.
  */
 kspi = 0;  /* Always zero first */
 pks = sample((void *)marsaglia_tsang_gcd_test);

 /*
  * Test Results, standard form.
  */
 test_footer(dtest,pks,ks_pvalue,"Lagged Sum Test");

 /*
  * the GCD test generates two statistics, not one, so we need an extra
  * KS test on the second vector of p-values.
  */
 pks = kstest_kuiper(ks_pvalue2,kspi);
 test_footer(dtest,pks,ks_pvalue2,"Marsaglia and Tsang GCD Test");

 /*
  * Put back tsamples
  */
 if(all == YES){
   tsamples = ts_save;
   psamples = ps_save;
 }

 if(ks_pvalue) nullfree(ks_pvalue);
 if(ks_pvalue2) nullfree(ks_pvalue2);

 return(pks);

}

void marsaglia_tsang_gcd_test()
{

 uint t,i,k,u,v,w;
 uint gcd[101],ktbl[36];
 Btest btest_k,btest_w;


 if(verbose == D_MARSAGLIA_TSANG_GCD || verbose == D_ALL){
   printf("# user_marsaglia_tsang_gcd(): Beginning gcd test\n");
 }

 for(t=0;t<tsamples;t++){
   /* Initialize counter for this sample */
   k = 0;
   /* Get nonzero u,v */
   do{
    u = gsl_rng_get(rng);
   } while(u == 0);
   do{
    v = gsl_rng_get(rng);
   } while(v == 0);

   do{
     w = u%v;
     u = v;
     v = w;
     k++;
   } while(v>0);
   /* lump gcd's greater than 100 in with 100, increment table */
   w = (w>100)?100:w;
   gcd[w]++;
   /* Lump k<3 in with k, k>35 in with 35, increment table */
   k = (k<3)?3:k;
   k = (k>35)?35:k;
   ktbl[k]++;

 }

 /*
  * Evaluate test statistics for this run
  */

 Btest_eval(btest_k);
 ks_pvalue[kspi] = btest_k.pvalue;

 Btest_eval(btest_w);
 ks_pvalue2[kspi] = btest_w.pvalue;

 if(verbose == D_USER_TEMPLATE || verbose == D_ALL){
   printf("# user_marsaglia_tsang_gcd(): ks_pvalue_k[%u] = %10.5f  ks_pvalue_w[%u] = %10.5f\n",kspi,ks_pvalue[kspi],ks_pvalue2[kspi]);
 }

 kspi++;

}

void help_marsaglia_tsang_gcd()
{

  printf("%s",dtest->description);

}
