#include "ccl.h"
#include "ctest.h"
#include <stdio.h>
#include <math.h>

#define EMU_TOLERANCE 1.0E-2

CTEST_DATA(emu) {
  double N_nu_rel;
  double N_nu_mass;
  double mnu;
  double sigma_8[6];
  double Omega_c[6];
  double Omega_b[6];
  double n_s[6];
  double h[6];
  double w_0[6];
  double w_a[6];
};

CTEST_SETUP(emu) {
  
  data->N_nu_rel=3.04;
  data->N_nu_mass=0;
  data->mnu=0;

  double *sigma_8;
  double *Omega_c;
  double *Omega_b;
  double *n_s;
  double *h;
  double *w_0;
  double *w_a;
  int status=0;
  char fname[256],str[1024];
  char* rtn;
  FILE *f;
  int i;
  sigma_8=malloc(6*sizeof(double));
  Omega_c=malloc(6*sizeof(double));
  Omega_b=malloc(6*sizeof(double));
  n_s=malloc(6*sizeof(double));
  h=malloc(6*sizeof(double));
  w_0=malloc(6*sizeof(double));
  w_a=malloc(6*sizeof(double));
  
  sprintf(fname,"./tests/benchmark/emu_cosmologies.txt");
  f=fopen(fname,"r");
  if(f==NULL) {
    fprintf(stderr,"Error opening file %s\n",fname);
    exit(1);
  }
  
  double tmp;
  for(int i=0;i<6;i++) {
    
    status=fscanf(f,"%le %le %le %le %le %le %le\n",&Omega_c[i],&Omega_b[i],&h[i],&sigma_8[i],&n_s[i],&w_0[i],&w_a[i]);
    if(status!=7) {
      fprintf(stderr,"Error reading file %s, line %d\n",fname,i);
      exit(1);
    }
    data->w_0[i] = w_0[i];
    data->w_a[i] = w_a[i];
    data->h[i] = h[i];
    data->sigma_8[i] = sigma_8[i];
    data->Omega_c[i] = Omega_c[i];
    data->Omega_b[i] = Omega_b[i];
    data->n_s[i] = n_s[i];
  }
  close(f);
}

static int linecount(FILE *f)
{
  //////
  // Counts #lines from file
  int i0=0;
  char ch[1000];
  while((fgets(ch,sizeof(ch),f))!=NULL) {
    i0++;
  }
  return i0;
}

static void compare_emu(int i_model,struct emu_data * data)
{
  int nk,i,j;
  int status=0;
  char fname[256],str[1024];
  char* rtn;
  FILE *f;
  int i_model_vec[6]={1,3,5,6,8,10}; //The emulator cosmologies we can compare to
  
  ccl_configuration config = default_config;
  config.transfer_function_method = ccl_emulator;
  config.matter_power_spectrum_method = ccl_emu;
  
  //None of the current cosmologies being checked include neutrinos
  ccl_parameters params = ccl_parameters_create(data->Omega_c[i_model-1],data->Omega_b[i_model-1],0.0,data->N_nu_rel, data->N_nu_mass, data->mnu,data->w_0[i_model-1],data->w_a[i_model-1],data->h[i_model-1],data->sigma_8[i_model-1],data->n_s[i_model-1],-1,NULL,NULL, &status);
  params.Omega_g=0;
  params.sigma_8=data->sigma_8[i_model-1];
  ccl_cosmology * cosmo = ccl_cosmology_create(params, config);
  ASSERT_NOT_NULL(cosmo);
  
  sprintf(fname,"./tests/benchmark/emu_smooth_pk_M%d.txt",i_model_vec[i_model-1]);
  f=fopen(fname,"r");
  if(f==NULL) {
    fprintf(stderr,"Error opening file %s\n",fname);
    exit(1);
  }
  nk=linecount(f)-1; rewind(f);
  
  double k=0.,pk_bench=0.,pk_ccl,err;
  double z=0.; //Other redshift checks are possible but not currently implemented
  int stat=0;
  
  for(i=0;i<nk;i++) {      
    stat=fscanf(f,"%le %le\n",&k, &pk_bench);
    if(stat!=2) {
      fprintf(stderr,"Error reading file %s, line %d\n",fname,i);
      exit(1);
    }
    pk_ccl=ccl_nonlin_matter_power(cosmo,k,1./(1+z),&status);
    if (status) printf("%s\n",cosmo->status_message);
    err=fabs(pk_ccl/pk_bench-1);
    ASSERT_DBL_NEAR_TOL(err,0.,EMU_TOLERANCE);
  }

  fclose(f);

  ccl_cosmology_free(cosmo);
}

CTEST2(emu,model_1) {
  int model=1;
  compare_emu(model,data);
}

CTEST2(emu,model_2) {
  int model=2;
  compare_emu(model,data);
}

/*
//Additional tests are possible:
CTEST2(emu,model_3) {
  int model=3;
  compare_emu(model,data);
}

CTEST2(emu,model_4) {
  int model=4;
  compare_emu(model,data);
}

CTEST2(emu,model_5) {
  int model=5;
  compare_emu(model,data);
}

CTEST2(emu,model_6) {
  int model=6;
  compare_emu(model,data);
}
*/