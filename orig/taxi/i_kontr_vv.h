/*$Id: i_kontr_vv.h,v 1.6 2011-01-13 13:50:09 sasa Exp $*/
/*14.11.2008	03.02.2006	Белых А.И.	i_kontr_vv.h
*/
#include <iceb_libbuh.h>
//#include <iceb_util.h>
//#include <libsql.h>

#define   NAME_SYSTEM "iceBw"
#define   CONFIG_PATH "/etc/iceB"

double ras_sal_kl_t(char *kodkl,double *ost_na_saldo);
double  sumapsh_t(short gd,const char *nomd);
int otm_opl_dok_t(short god,const char *nomdok,double suma_dok);
double otm_opl_doks_t(const char *kodkl,double *ost_na_saldo);
void t_msql_error(SQL_baza *bd,const char *zapros);
int zap_v_kas(int nomkas,const char *kodkl,double suma,int kodz,const char *nomdok,short dd,short md,short gd,int podr,uid_t  kod_operatora,const char *koment);
void i_vv_start(const char *putnansi,const char *imabaz);


