/*$Id: i_rest.h,v 1.70 2011-01-13 13:50:09 sasa Exp $*/
/*14.11.2008	19.02.2004	Белых А.И.	i_rest.h
*/
#include <iceb_libbuh.h>
#define NAME_SYSTEM "iceBw"
#define CONFIG_PATH "/etc/iceB"
#define SLEEP_STR 90000 //Задержка в микросекундах

int    vrshet_v(short *god,iceb_u_str *nomdok,GtkWidget*);
int    vip_shet(short god,iceb_u_str *nomdok,GtkWidget *wpredok);
int    l_restdok(short god,iceb_u_str *nomdok,GtkWidget *wpredok);
int    l_restdok2(short god,iceb_u_str *nomdok,GtkWidget *wpredok);
void   l_srestdok(GtkWidget *wpredok);
int    l_spisok(int,int,iceb_u_str *,int,GtkWidget *);
int    l_spisok_v(int*,int*,int,int,GtkWidget *);
int    l_klientr(int,iceb_u_str *kodkl,GtkWidget *wpredok);
int    rpod_l(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int    l_grkl(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
double sumapsh(short gd,const char *nomd,const char*,GtkWidget *wpredok);
void   restdok_rshet(short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok);
double otm_opl_doks(const char *kodkl,double *suma,double*,GtkWidget *wpredok);
int    otm_opl_dok(short god,const char *nomdok,double suma_dok,GtkWidget *wpredok);
double ras_sal_kl(const char *kodkl,double*,double*,GtkWidget *wpredok);
int    ras_sheks(iceb_u_int *gods,iceb_u_spisok *nomdoks,GtkWidget *wpredok);
int    zapvrkas(char *kodkl,double suma,int metka,GtkWidget *wpredok);
int    rest_flag_kl(iceb_sql_flag*,const char *kodkl,GtkWidget *wpredok);
int    restdok_uddok(short dd,short md,short gd,const char *nomdok,GtkWidget*);
void   read_card(int time_out,char *cod,GtkWidget *wpredok);
double rest_sal_kl(const char *kodkl,GtkWidget *wpredok);
void   vrshet_nomdok(short god,iceb_u_str *nomdok,GtkWidget *wpredok);
void   l_restdok_spis_skl(class iceb_u_str *spis_skl,int podr);
int    zap_vrest_dok(short dd,short md,short gd,const char *nomd,int skl,int metkaz,int kodm,const char *ei,double kolih,double cena,const char *koment,uid_t kod_operatora,int podr,float ps);
int    zap_rest_dok(short d,short m,short g,iceb_u_str *nomdok,const char *nomstol,const char *kodkl,const char *famil,const char *koment,uid_t kod_operatora,int podr,time_t vremz,float proc_sk);
int    zap_v_kas(int nomkas,const char *kodkl,double suma,int kodz,const char *nomdok,short dd,short md,short gd,int podr,uid_t  kod_operatora,const char *koment);
void   restdok_rskl(short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok);
int    l_restdok_v(short dd,short md,short gd,const char *nomd,int,int,int,int,int,int,float,const char*,GtkWidget *wpredok);
int kod_podr_v(iceb_u_str *podr,GtkWidget *wpredok);
int  l_tk_saldo(GtkWidget *wpredok);
double osttov(int kodmat,int sklad,const char *ei_iz,const char *imaf,GtkWidget *wpredok);
int vid_kar_kl(const char*);

