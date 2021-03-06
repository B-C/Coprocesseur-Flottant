#include <stdio.h>

/*******************************************************************************
 * appel de la fonction 'user' en asm pour acceder au coprocesseur
 * float data1,data2,resultat ;
 * data1    = (float) 1.0f ;
 * data2    = (float) -16.0f ;
 * resultat = (float) 0.0 ;
 * asm volatile("user %[dest],%[src1],%[src2],0x02":[dest] "=r" (resultat)
 *                                                 :[src1] "r" (data1),
 *                                                  [src2] "r" (data2)) ;
 *
 ******************************************************************************/

// nombres de cycles depuis le d�marage du lm32
static inline unsigned int get_cc(void)
{
  int tmp;
  asm volatile (
		"rcsr %0, CC" :"=r"(tmp)
		);
  return tmp;
}

// CODE A MODIFIER
static inline float fmult(float x, float y)
{
  float resultat;
  asm volatile("user %[dest],%[src1],%[src2],0x02":[dest] "=r" (resultat)
		:[src1] "r" (x),
		[src2] "r" (y)) ;

  return  resultat ;
}

static inline float fsub(float x, float y)
{
  float resultat;
  asm volatile("user %[dest],%[src1],%[src2],0x01":[dest] "=r" (resultat)
		:[src1] "r" (x),
		[src2] "r" (y)) ;

  return  resultat ;
}

static inline float fadd(float x, float y)
{
  float resultat;
  asm volatile("user %[dest],%[src1],%[src2],0x00":[dest] "=r" (resultat)
		:[src1] "r" (x),
		[src2] "r" (y)) ;

  return  resultat ;
}

static inline float fdiv(float x, float y)
{
  float resultat;
  asm volatile("user %[dest],%[src1],%[src2],0x03":[dest] "=r" (resultat)
		:[src1] "r" (x),
		[src2] "r" (y)) ;

  return  resultat ;
}
// FIN DU CODE A MODIFIER

// fonction calculant pi en utilisant le copro
float dev_lim_atn_copro ( float x , int n ) {
  float res ,x2, mem_puiss ;
  int i ;
  float  signe;
  signe  =1.0;
  res =0.0 ;
  mem_puiss = x ;
  x2 = fmult(x,x)  ;
  for ( i =1; i <= n ; i +=2) {
	 res = fadd(res,fdiv(fmult(signe , mem_puiss),(float) i))  ;
	 signe = fmult(-1.0,signe) ;
	 mem_puiss = fmult(mem_puiss , x2) ;
  }
  return res ;
}


// fonction de reference
float dev_lim_atn_ref ( float x , int n ) {
  float res ,x2, mem_puiss ;
  int i ;
  float  signe;

  signe  =1.0;
  res =0.0 ;
  x2 = x*x;
  mem_puiss = x ;

  for ( i =1; i <= n ; i +=2) {
	 res += signe * mem_puiss / (float)i  ;
	 signe = -1.0 * signe  ;
	 mem_puiss = mem_puiss * x2 ;
  }
  return res ;
}


#define FREQ 100.0f // in MHz

int main()
{
  float x = 1.2562356;
  float y = 2.3235321;
  float resultat;
  //unsigned int t0, t1;
  //float pi ;
  //int i ;
  // sans le copro
  /*for (i=1;i<100000;i*=10) {
	 //printf("degug............................");
	 t0 = get_cc();
	 pi = 4.0*dev_lim_atn_ref(1.0,i) ;
	 t1 = get_cc();
	 printf("Duration 1    %f ms\n\r", (t1-t0)/1000./FREQ);
	 printf("Iterations :%10d Pi : %10.9f\n",i,pi) ;
  }
  // utilisant le copro
  for (i=1;i<100000;i*=10) {
	 t0 = get_cc();
	 pi = fmult(4.0,dev_lim_atn_copro(1.0,i)) ;
	 t1 = get_cc();
	 printf("Duration 1    %f ms\n\r", (t1-t0)/1000./FREQ);
	 printf("Iterations :%10d Pi : %10.9f\n",i,pi) ;
  }*/
  asm volatile("user %[dest],%[src1],%[src2],0x03":[dest] "=r" (resultat)
		:[src1] "r" (x),
		[src2] "r" (y)) ;

  printf("resultat : %e\n",resultat);
  printf("-------------------\n");
  printf("x = %e ------------\n",x);
  printf("y = %e ------------\n",y);
  printf("resultat x/y        function : %e\n",fdiv(x, y));
  printf("ref                          : %e\n",(float)(x/y));
  printf("resultat y/x        function : %e\n",fdiv(y, x));
  printf("ref                          : %e\n",(float)(y/x));
  printf("resultat x*y        function : %e\n",fmult(x, y));
  printf("ref                          : %e\n",(float)(y*x));
  printf("resultat x+y        function : %e\n",fadd(x, y));
  printf("ref                          : %e\n",(float)(y+x));


  return 0;
}
