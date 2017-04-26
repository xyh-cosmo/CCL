#pragma once

int ccl_tracer_corr(ccl_cosmology *cosmo, int n_theta, double **theta, CCL_ClTracer *ct1, CCL_ClTracer *ct2, int i_bessel, double **corr_func);

int ccl_tracer_corr2(ccl_cosmology *cosmo, int n_theta, double **theta,
		     CCL_ClTracer *ct1, CCL_ClTracer *ct2, int i_bessel,double **corr_func,
		     double (*angular_cl)(ccl_cosmology *cosmo,int l,CCL_ClTracer *clt1,
					  CCL_ClTracer *clt2, int * status) );
