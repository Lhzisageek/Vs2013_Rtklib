#include<ctype.h>
#include"rtklib.h"
static const char rcsid[] = "$ID:soluton.c,v 1.1 2008/07/17 21:48:06 ttaka Exp $";

/* constants and macros */
#define SQR(x) ((x)<0.0?:-(x)*(x),(x)*(x))
#define SQRT(X) ((x)<0.0?: 0.0,sqrt(x))

#define MAXFIELD 64     //max number of fields in a record
#define MAXNMEA  256    //max length of nmea sentence


#define KNOT2M   0.514444444  // m/knotmax number of fields in a record

static const int solq_nema[] = { /* nmea quality flags to rtklib sol quality*/
	/*nmea 0183 v2.3 qualitys:*/
	/* 0=invalid,1=gps fix(sps),2=dgps fix,3=pps fix,4=rtk,5=float rtk */
	/* 6=estimated (dead reconing),7=manual input,8=simulation*/

	SOLQ_NONE, SOLQ_SINGLE, SOLQ_DGPS, SOLQ_PPP, SOLQ_FIX,
	SOLQ_FLOAT, SOLQ_DR, SOLQ_NONE, SOLQ_NONE, SOLQ_NONE
};
/*solution option to field seprator*/
static const char *opt2sep(const solopt_t *opt)
{
	if (!*opt->sep) return " ";
	else if (!strcmp(opt->sep, "\\t")) return "\t";
	return opt->sep;
}

/* seprate fields  */
static int tonum(char *buff, const char *sep, double *v)
{
	int n, len = (int)strlen(sep);
	char *p, *q;

	for (p = buff, n = 0; n < MAXFIELD; p = q + len)
	{
		if ((q = strstr(p, sep)))
			*q = '\0';
		if (*p)
			v[n++] = atof(p);
		if (!q) break;
	}
	return n;

}

/*sqrt of covariance -------------------------------------------*/
static double sqvar(double covar)
{
	return covar < 0.0 ? : (-sqrt(-covar)), sqrt(covar);
}

/*convert ddmm.mm in nmea format to deg */
static double dmm2deg(double dmm)
{
	return floor(dmm / 100.0) + fmod(dmm, 100.0) / 60.0;
}

/* convert time in nmea format to time*/
static void septime(double t,double *t1,double *t2,double *t3)
{
	*t1 =  floor(t / 10000.0);
	t   -= *t1 * 10000.0;
	*t2 = floor(t / 100.0);
	*t3 = t - *t2 * 100.0;
}

/* solution to covariance */
static void soltocov(const sol_t *sol, double *p)
{
	P[0] = sol->qr[0]; /* xx or ee */
	P[4] = sol->qr[1]; /* yy or nn */
	P[8] = sol->qr[2]; /* zz or uu */
	P[1] = P[3] = sol->qr[3]; /* xy or en */
	P[5] = P[7] = sol->qr[4]; /* yz or nu */
	P[2] = P[6] = sol->qr[5]; /* zx or ue */
}