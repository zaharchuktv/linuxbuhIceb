/*$Id: iceb_util.h,v 1.110 2011-08-29 07:13:49 sasa Exp $*/
/*12.08.2011	03.09.2003	Белых А.И.	iceb_util.h
*/
#include <libintl.h>
#include <stdio.h>
#include <nl_types.h>
#include <time.h>

#include "iceb_u_utf.h"
#include "iceb_u_int.h"
#include "iceb_u_double.h"
#include "iceb_u_spisok.h"
#include "iceb_u_str.h"
#include "iceb_u_file.h"

extern int      iceb_u_pole3(const char *D,char *K,int N,char R);
extern int      iceb_u_pole2(const char *str,char raz);
extern short    iceb_u_pole1(const char*,const char*,char,short,int*);
extern int      iceb_u_pole(const char*,char*,int,char);
extern int      iceb_u_pole(const char*,class iceb_u_str*,int,char);
extern int      iceb_u_polen(const char*,char*,int,int,char);
extern int      iceb_u_polen(const char*,double*,int,char);
extern int      iceb_u_polen(const char*,float*,int,char);
extern int      iceb_u_polen(const char*,int*,int,char);
extern int      iceb_u_polen(const char*,short*,int,char);
extern int      iceb_u_polen(const char*,class iceb_u_str*,int,char);
extern int      iceb_u_polen(const char*,class iceb_u_spisok*,int,char);
extern int 	iceb_u_SRAV(const char*,const char*,int);
extern int      iceb_u_proverka(const char*,const char*,int,int);
extern int      iceb_u_proverka(const char*,int,int,int);
extern char     *iceb_u_catgets(nl_catd,int,int,const char*);
extern int      iceb_u_strstrm(const char*,const char*);
extern const char *iceb_u_vremzap(char*);
extern const char *iceb_u_vremzap(time_t);
extern const char *iceb_u_kszap(char *kto,int metka_kod);
extern const char *iceb_u_kszap(int,int metka_kod);
extern void	iceb_u_zagolov(const char*,short,short,short,short,short,short,const char*,FILE*);
extern void	iceb_u_zagolov(const char *naim,const char *datan,const char *datak,const char *organ,FILE *ff);
extern void     iceb_u_startfil(FILE*);
extern int      iceb_u_poldan(const char*,char*,const char*);
extern int      iceb_u_poldan(const char*,class iceb_u_str*,const char*);
extern int      iceb_u_ps_minus(char*);
extern int      iceb_u_rsdat(short*,short*,short*,char*,int);
extern int      iceb_u_rsdat(short*,short*,short*,const char*,int);
extern int	iceb_u_sravmydat(short,short,short,short,short,short);
extern int	iceb_u_sravmydat(const char *data1,const char *data2);
extern int      iceb_u_rstime(short *,short *,short *,const char *);
extern const char *iceb_u_datzap(const char*);
extern double   iceb_u_atof(const char*);
extern void     iceb_u_dpm(short *d,short *m,short *g,short mt);
extern char     *iceb_u_prnbr(double);
extern char     *iceb_u_sqldata(char *data);
extern double   iceb_u_okrug(double sh,double mt);
extern int iceb_u_print(const char *imaf,int kolkop,const char *progp,const char *name_printer,const char *kluh_peh,const char *kluh_name_pr,const char *kluh_kol_kop);
extern int      iceb_u_pstrz(char *stroka,int razmer,const char *stroka_plus);
extern int      iceb_u_period(short,short,short,short,short,short,int);
extern void     iceb_u_preobr(double,char *,short);
extern int      iceb_u_rsdat1(short *m,short *g,char *str);
extern int      iceb_u_rsdat1(short *m,short *g,const char *str);
//extern char     *iceb_u_kikz(char *kto,char *vremzap,nl_catd fils,int metka_kod);
extern char     *iceb_u_kikz(int kto,time_t vremzap,nl_catd fils,int metka_kod);
extern time_t   iceb_u_vrem(iceb_u_str *data,iceb_u_str *vrem,int metka);
extern void     iceb_u_bstab(char *STI,const char *STB,int K1,int K2,int hag);
extern void     iceb_u_bstab(char *STI,short,int K1,int K2,int hag);
extern void     iceb_u_poltekdat(short*,short*,short*);
extern int      iceb_u_fgets(char *stroka,int razmer,FILE *ff);
extern int 	iceb_u_vstavfil(const char *imafil,FILE *ff);
extern char     *iceb_u_vrem_tek(void);
extern time_t iceb_u_dat_to_sec(const char *data);
extern time_t iceb_u_dat_to_sec(short);
extern time_t iceb_u_dat_to_sec(short,short,short);
extern time_t iceb_u_dat_to_sec_end(const char *data);
extern time_t iceb_u_dat_to_sec_end(short);
extern time_t iceb_u_dat_to_sec_end(short,short,short);
extern void iceb_u_ud_lp(char *stroka);
extern int iceb_u_sravtime(short has1,short min1,short sek1,short has2,short min2,short sek2);
extern void    iceb_u_prnb(double nb,char *str);
extern void iceb_u_ud_simv(char *stroka,const char *simvol);
extern void iceb_u_prnb1(double nb,char *nb1);
extern void iceb_u_ekrs(char *stroka,int razmer,char ekran,const char *simv);
extern void iceb_u_sozmas(short **kod,char *st1);
extern void iceb_u_sozmas(short **kod,char *st1,FILE*);
extern void iceb_u_denrog(short *d,short *m,short *g,int kold);
extern void iceb_u_slstr(char *st,short dl,char *str);
extern time_t iceb_u_datetime_sec(const char *datetime);
extern time_t iceb_u_datetime_sec(short d,short m,short g,short has,short min,short sek);
extern const char *iceb_u_datetime_pr(char *datatime);
extern void iceb_u_tolower_str(char *stroka);
extern void iceb_u_toupper_str(char *stroka);
extern int iceb_u_srav_r(const char *stroka1,const char *stroka2,int metka);
extern void iceb_u_fplus(const char *stroka,class iceb_u_str *stroka1,FILE *ff);
extern void iceb_u_fplus(int metka_sp,const char *stroka,class iceb_u_spisok *nastr,FILE *ff);
extern void iceb_u_fio(const char *fio,class iceb_tu_str *fio_in);
extern int iceb_u_kol_str(int max_kol_std_na_liste,int ras_kol_str);
extern int iceb_u_kol_str_l(int max_kol_std_na_liste,int ras_kol_str);
extern int iceb_u_zvstr(class iceb_u_str *stroka,const char *obr,const char *zamena);
extern void iceb_u_cpbstr(char *strin,int dlstrin,const char *strout,const char *udstr);
extern void iceb_u_cpbstr(class iceb_u_str *strin,const char *strout,const char *udstr);
extern void iceb_u_cpbstr(class iceb_u_spisok *strin,const char *strout,const char *udstr);
extern int iceb_u_poltekgod();
extern int iceb_u_ustpeh(int dlinna_stroki,int orient,FILE *ff);
extern int iceb_u_strlen(const char *stroka);
extern const char *iceb_u_adrsimv(int nomer_simv,const char *str);
extern int iceb_u_kolbait(int kolih_simv,const char *str);
extern const char *iceb_u_dattosql(const char *data);
extern int iceb_u_strncpy(char *tocop,const char *fromcop,int kolsimv);
extern const char *iceb_u_getlogin();
const char *iceb_u_sqlfiltr(const char *stroka);
const char *iceb_u_filtr_xml(const char *stroka);
const char *iceb_u_propis(double hs,int metka_1s);
const char *iceb_u_nofwords(double number);


