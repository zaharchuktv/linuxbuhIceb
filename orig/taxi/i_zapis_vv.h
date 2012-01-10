/*$Id: i_zapis_vv.h,v 1.5 2011-01-13 13:50:09 sasa Exp $*/
/*14.11.2008	03.02.2006	Белых А.И.	i_zapis_vv.h
*/

//#include <iceb_util.h>
//#include <libsql.h>
#include <iceb_libbuh.h>
#define   NAME_SYSTEM "iceBw"
#define   CONFIG_PATH "/etc/iceB"

int zap_rest_dok(short d,short m,short g,iceb_u_str *nomdok,const char *nomstol,const char *kodkl,const char *famil,const char *koment,uid_t kod_operatora,
int podr,time_t vremz,float proc_sk);

int zap_vrest_dok(short dd,short md,short gd,const char *nomd,int skl,int metkaz,int kodm,const char *ei,double kolih,double cena,
const char *koment,uid_t kod_operatora,int podr,float ps);

void  vrshet_nomdok(short god,iceb_u_str *nomdok);

void t_msql_error(SQL_baza *bd,const char *zapros);
void i_vv_start(const char *putnansi,const char *imabaz);
