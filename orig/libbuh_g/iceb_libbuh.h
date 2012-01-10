/*$Id: iceb_libbuh.h,v 1.259 2011-09-05 08:18:29 sasa Exp $*/
/*01.09.2011	07.09.2003	Белых А.И.	iceb_libbuhg.h
*/
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <libsql.h>
#include <iceb_util.h>
#include <nl_types.h>
#include <iceb_d_lib.h>
#include "../iceBw.h"
#include "iceb_get_dev.h"
#include "iceb_gdite.h"
#include "iceb_razuz.h"
#include "iceb_fioruk.h"
#include "iceb_avp.h"
#include "iceb_tmptab.h"
#include "iceb_rnfxml.h"

#ifdef READ_CARD
#include        <torgdev_client.h>
extern int iceb_read_card(tdcon td_server,char *kod_kart,int razmer,int time_out,GtkWidget *wpredok);
#endif

#include "iceb_menu_vibknop.h"
#include "iceb_lock_tables.h"
#include "iceb_rsn.h"
#include "iceb_sql_flag.h"
#include "iceb_kassa.h"
#include "iceb_read_card_enter.h"
/********
#define   CONFIG_PATH "/etc/iceB"
*************/
//Располпжение окон на экране
//#define ICEB_POS_CENTER GTK_WIN_POS_CENTER
#define  ICEB_POS_CENTER GTK_WIN_POS_CENTER_ALWAYS
//#define  ICEB_POS_CENTER GTK_WIN_POS_CENTER_ON_PARENT

//Вид курсора
#define  ICEB_CURSOR       GDK_TOP_LEFT_ARROW //Обычный курсор
#define  ICEB_CURSOR_GDITE GDK_WATCH          //Курсор указывающий на то, что программа работает и надо подождать

//Клавиша нажимаемая совместно с F-клавишами
//#define  ICEB_REG_L      GDK_Control_L
//#define  ICEB_REG_R      GDK_Control_R

#define  ICEB_REG_L      GDK_Shift_L
#define  ICEB_REG_R      GDK_Shift_R

//Символ перед F клавишей при изменении регистра
#define  RFK  "S"

#define  SRIFT_RAVNOHIR "Fixed, 12"
#include "iceb_clock.h"

const short ICEB_PS_GK =  1; /*Код подсистемы "Главная книга"*/
const short ICEB_PS_MU =  2; /*Код подсистемы "Материальный учёт"*/
const short ICEB_PS_PD =  3; /*Код подсистемы "Платёжные документы"*/
const short ICEB_PS_ZP =  4; /*Код подсистемы "Заработная плата"*/
const short ICEB_PS_UOS=  5; /*Код подсистемы "Учёт основных средств"*/
const short ICEB_PS_UU =  6; /*Код подсистемы "Учёт услуг"*/
const short ICEB_PS_UKO=  7; /*Код подсистемы "Учёт кассовых ордеров"*/
const short ICEB_PS_UKR=  8; /*Код подсистемы "Учёт командировочных расходов"*/
const short ICEB_PS_UPL=  9; /*Код подсистемы "Учёт путевых листов"*/
const short ICEB_PS_RNN= 10; /*Код подсистемы "Реестр налоговых накладных"*/
const short ICEB_PS_UD = 11; /*Код подсистемы "Учёт доверенностей"*/
const short ICEB_PS_ABD = 12; /*Код подсистемы "Администрирование баз данных"*/



//Структура со всеми реквизитами счета
struct OPSHET
 {
  class iceb_u_str naim;//Наименование
  short tips;    // 0-активный 1-пассивный 2-активно-пассивный
  short vids;    // 0-счет 1-субсчет
  short saldo;   // 0-сальдо свернутое 3-сальдо развернутое
  short stat;    // 0-балансовый 1-не балансовый
  short val;     // 0-национальная валюта или номер валюты из справочника валют
 };

class akt_sverki //Для расчёта акта сверки по контрагентам
 {
  public:
   class iceb_u_spisok data_nomd; //Дата|номер документа
   class iceb_u_spisok koment;    //коментарий
   class iceb_u_double suma_deb; 
   class iceb_u_double suma_kre; 
   double start_saldo_deb;
   double start_saldo_kre;
   char shet[56];  
   short dn,mn,gn;
     
  akt_sverki()
   {
    start_saldo_deb=start_saldo_kre=0.;
    memset(shet,'\0',sizeof(shet));
    dn=mn=gn=0;
   }
  void clear()
   {
    start_saldo_deb=start_saldo_kre=0.;
    memset(shet,'\0',sizeof(shet));
    suma_deb.free_class();
    suma_kre.free_class();
    data_nomd.free_class();
    koment.free_class();    
    dn=mn=gn=0;
   }
 };



extern void      iceb_msql_error(SQL_baza*,const char*,const char*,GtkWidget*);
extern void      iceb_menu_soob(iceb_u_spisok*,GtkWidget*);
extern void      iceb_menu_soob(iceb_u_str*,GtkWidget*);
extern void      iceb_menu_soob(const char*,GtkWidget*);
extern void      iceb_vparol(char**,char**);
extern void      iceb_eropbaz(const char*,int,const char*,int);
extern void      iceb_infosys(const char*,const char*,const char*,short,short,short,const char*,iceb_u_str*,int);
extern void      iceb_readkey(int,char**,char**,char**,char**,int*);
extern void      iceb_get_text(GtkWidget*,char*);
extern void      iceb_get_text_str(GtkWidget*,iceb_u_str*);
extern int       iceb_menu_danet(iceb_u_spisok*,int,GtkWidget*);
extern int       iceb_menu_danet(iceb_u_str*,int,GtkWidget*);
extern int       iceb_menu_danet(const char*,int,GtkWidget*);
extern gboolean  iceb_vihod(GtkWidget*,GdkEventKey*,int*);
extern void      iceb_podpis(FILE*,GtkWidget*);
extern void      iceb_podpis(uid_t,FILE*,GtkWidget*);
extern int       iceb_mydoscopy(const char*,GtkWidget*);
extern gboolean  iceb_key_release(GtkWidget *,GdkEventKey *,int *);
extern void      iceb_rabfil(iceb_u_spisok*,iceb_u_spisok*,const char*,int,GtkWidget*);
extern void      iceb_ponp(const char*,const char*,GtkWidget*);
extern int       iceb_get_new_kod(const char*,int,GtkWidget*);
extern int       iceb_get_new_kod(const char*,const char*,int,GtkWidget*);
extern int       iceb_menu_vvod1(const char*,iceb_u_str*,int,GtkWidget*);
extern int       iceb_menu_vvod1(iceb_u_spisok*,iceb_u_str*,int,GtkWidget*);
extern int       iceb_menu_vvod1(const char*,char*,int,GtkWidget*);
extern int       iceb_menu_vvod1(iceb_u_str*,iceb_u_str*,int,GtkWidget*);
extern int       iceb_menu_vvod1(iceb_u_str*,char*,int,GtkWidget*);
extern int       iceb_parol(int,GtkWidget*);
extern int       iceb_parol(int,class iceb_u_spisok*,GtkWidget*);
extern int       iceb_parol(int metkap,class iceb_u_spisok *text_menu,class iceb_u_str *parol_voz,GtkWidget *wpredok);
extern int       iceb_parol(class iceb_u_str *parol,GtkWidget *wpredok);
extern int	 iceb_sql_zapis(const char *,int,int,GtkWidget*);
extern void      iceb_er_op_fil(const char *,const char *,int,GtkWidget*);
extern void      iceb_pbar(GtkWidget *,int,gfloat);
extern int       iceb_dikont(const char,const char *,const char *,const char *);
int iceb_vizred(const char *,GtkWidget*);
int iceb_vizred(const char *imaf,const char *editor,GtkWidget *wpredok);
extern void      iceb_spks(const char*,const char*,GtkWidget*);

extern int       iceb_rsdatp(short *,short *,short *,const char *,short *,short *,short *,const char *,GtkWidget*);
extern int       iceb_rsdatp(const char *datn,const char *datk,GtkWidget*);
extern int       iceb_rsdatp(const char *datn,class iceb_u_str *datk,GtkWidget*);

extern int       iceb_rsdatp_str(iceb_u_str *datan,iceb_u_str *datak,GtkWidget *wpredok);
extern void      iceb_zagacts(short dn,short mn,short gn,short dk,short mk,short gk,const char *kodkontr,const char *naimkont,FILE *ff);
extern void	 iceb_konact(FILE *ff,GtkWidget*);
extern void	 iceb_zagsh(const char *shet,FILE *ff,GtkWidget*);
extern void      iceb_salorksl(double dd, double kk,double ddn,double kkn,double debm,double krem,FILE *ff1,FILE *ffact);
extern void      iceb_printw(const char *stroka,GtkTextBuffer *buffer,GtkWidget *view);
extern void      iceb_printw_vr(time_t vrem_n,GtkTextBuffer *buffer,GtkWidget *view);
extern int       iceb_prsh(const char*,OPSHET *,GtkWidget*);
extern int       iceb_prsh1(const char *shet,OPSHET *shetv,GtkWidget *wpredok);
extern int       iceb_l_kontr(int,iceb_u_str *,iceb_u_str *,GtkWidget  *);
extern int       iceb_l_kontrsh(int,const char*,iceb_u_str *,iceb_u_str *,GtkWidget *);
int iceb_f_redfil(const char *imafil,int,GtkWidget *wpredok);
int iceb_f_redfil(const char *imafil,int,const char *editor,GtkWidget *wpredok);
extern int       iceb_l_gkontr(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
extern int       iceb_l_plansh(int,iceb_u_str *,iceb_u_str *,GtkWidget  *);
extern int iceb_prblm(short m,short g,const char *imaf,const char *soobstr,GtkWidget *wpredok);
extern int       iceb_zapprov(int,int,int,int,const char *,const char *,const char *,const char *,const char *,const char *,const char *,double,double,const char *,short,int,time_t,short,short,short,int,int,GtkWidget *);
extern void      iceb_menu_vibknop(GtkWidget *widget,struct vibknop_data *data);
extern int       iceb_menu_mv(iceb_u_str *titl,iceb_u_str *zagolovok,iceb_u_spisok *punkt_menu,int,GtkWidget *wpredok);
extern int       iceb_vibrek(int,const char *tablica,iceb_u_str *kod,GtkWidget *wpredok);
extern int       iceb_vibrek(int,const char *tablica,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
extern int       iceb_vibrek(int,const char *tablica,char *kod,int,GtkWidget *wpredok);
extern int       iceb_udprov(int,short,short,short,const char *,const char *,time_t,double,double,const char *,short,GtkWidget *);
extern int       iceb_udprov(int,const char*,const char *,const char *,time_t,double,double,const char *,short,GtkWidget *);
extern int       iceb_klient_pvu(const char *kodkl,GtkWidget *wpedok);
extern void      iceb_klient_kor(const char *skod,const char *nkod);
extern int       iceb_sql_readkey(const char *zapros,SQL_str *row,SQLCURSOR *cur,GtkWidget *wpredok);
extern int       iceb_sql_readkey(const char *zapros,GtkWidget *wpredok);
extern void      iceb_printcod(FILE *,const char *,const char *,const char *,int,const char *,int *);
extern int       iceb_rsn(class SYSTEM_NAST *data,GtkWidget *wpredok);
extern void      iceb_l_rsfr(GtkWidget *wpredok);
extern int       iceb_rmenuf(char *imafil,iceb_u_spisok *kol1,iceb_u_spisok *kol2,GtkWidget *wpredok);
extern int       iceb_l_spisokm(iceb_u_spisok *kol1,iceb_u_spisok *kol2,iceb_u_spisok *naim_kol,iceb_u_str*,int nom_str,GtkWidget *wpredok);
int   iceb_smenabaz(int mopen,GtkWidget *wpredok);
int   iceb_smenabaz_m(iceb_u_str *imabaz,iceb_u_str *host);
extern void      iceb_l_printcap(GtkWidget *wpredok);
extern int       iceb_runsystem(char *komanda,char*,GtkWidget *wpredok);
extern int       iceb_l_grupmat(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
extern int       iceb_l_ei(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
extern int       iceb_l_forop(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
extern int       iceb_l_sklad(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
extern int       iceb_l_opmup(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
extern int       iceb_l_opmur(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);

extern int       iceb_print(const char *imaf,int kolkop,const char *name_print,const char *kluh_peh,GtkWidget *wpredok);
extern int       iceb_print(const char *imaf,GtkWidget *wpredok);

extern void      iceb_imafn(const char *imaf,iceb_u_str *pput);
extern void      iceb_str_poisk(iceb_u_str *stroka,const char *data,const char *naim);
extern void      iceb_refresh(void);
extern void      iceb_kodkon(char *kodkon,char *str);
extern int       iceb_rstimep_str(iceb_u_str *vremn,iceb_u_str *vremk,GtkWidget*);
extern void      iceb_mesc(short mes,short skl,char *naz);
extern char      *iceb_prcn(double cn);
extern int       iceb_udprgr(const char *kto,short dd,short md,short gd,const char *nn,int pod,int tipz,GtkWidget *wpredok);
extern int       iceb_provinnom(const char *innom,GtkWidget *wpredok);
extern int       iceb_invnomer(GtkWidget *wpredok);
extern void      iceb_nomnak(short g,const char *skll,iceb_u_str *ndk,int tz,int metka,int,GtkWidget *wpredok);
extern void      iceb_nomnak(const char*,const char *skll,iceb_u_str *ndk,int tz,int metka,int,GtkWidget *wpredok);
extern int       iceb_00_skl(const char *kontr);
extern void      iceb_00_kontr(const char *kontr_old,iceb_u_str *kontr_new);
extern void      iceb_get_menu(GtkWidget *widget,int *data);

extern int iceb_vprov(const char *metkasys,const char *shet_d,const char *shet_k,const char *suma,const char *data_prov,const char *koment,
  const char *nomdok,const char *kodop,int sklad,short dd,short md,short gd,const char *kontr,time_t vremz,int ktozap,const char *mtsh,
  int pods,int tipz,int val,int kekv,GtkWidget *wpredok);

extern int iceb_provsh(class iceb_u_str *kor,const char *shp4,struct OPSHET *shetv,int metka,int metkanbs,GtkWidget *wpredok);
extern double iceb_sumvpr(const char *sh,const char *shk,short d,short m,short g,const char *nn,int skk,const char *kom,time_t vrem,int metkaprov,const char *kto);
extern void iceb_zapmpr(short g,short m,short d,const char *sh,const char *shk,const char *kor1,const char *kor2,double deb,double kre,const char *kom,int kolpr,int kekv,class iceb_u_spisok*,class iceb_u_double*,FILE*);
extern void iceb_zapmpr1(const char *nn,const char *kop,int skk,time_t vrem,const char *kto,short dd,short md,short gd,int,class iceb_u_spisok*,class iceb_u_double*,FILE*,GtkWidget *wpredok);
extern void	iceb_raspprov(const char *zapros,short dd,short md,short gd,const char *nomd,int pods,GtkWidget *wpredok);
extern void	iceb_pehf(const char *imaf,short kk,GtkWidget *wpredok);
extern void	iceb_kasord1(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,const char*,GtkWidget *wpredok);

extern void	iceb_kasord2(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,
  double suma,const char *fio,const char *osnov,const char *dopol,const char *dokum,const char *hcn,FILE *ff,GtkWidget *wpredok);

extern int iceb_nalndoh(GtkWidget *wpredok);
extern int iceb_razuz(class iceb_razuz_data *data,GtkWidget*);
extern int iceb_razuz1(class iceb_razuz_data *data,GtkWidget*);
extern int iceb_razuz_kod(class iceb_razuz_data *data,GtkWidget*);
extern int	iceb_getnkas(int autoid,KASSA *kasr);
extern int	iceb_errorkas(int errn,GtkWidget *wpredok);
extern int	iceb_errorkas(KASSA *kasr, char *str,GtkWidget *wpredok);
extern int      iceb_cmd(KASSA *kasr, char *cmd,GtkWidget *wpredok);
extern int      iceb_p_kol_strok_list(GtkWidget*);
extern int  iceb_file_selection(iceb_u_str *imaf_v,GtkWidget *wpredok);
extern int      iceb_nastsys(void);
extern int  iceb_kolstrok(int);
extern int  iceb_mous_klav(char *nadpis,iceb_u_str *stroka_data,int max_dlinna_str,int metka_r,int metka_parol,int,GtkWidget *wpredok);
extern int iceb_exit(int);
extern void    iceb_rab_kas(struct KASSA *kasr,int);
extern int iceb_mariq_vs(int metka,struct KASSA *kasr);
extern int iceb_mariq_zvs(int metka,double suma,int vidnal,struct KASSA *kasr,GtkWidget *wpredok);
extern int	iceb_mariq_slvi(struct KASSA *kasr,double *suma,int metka);
extern int iceb_calendar(short *dv,short *mv,short *gv,GtkWidget *wpredok);
extern int iceb_calendar1(class iceb_u_str *mes_god,GtkWidget *wpredok);
extern int iceb_calendar(iceb_u_str*,GtkWidget *wpredok);
extern void iceb_start(int argc,char **argv);
extern void iceb_read_card_enter(class iceb_read_card_enter_data *data);
extern int iceb_connect_dserver(const char*,const char*);
extern void iceb_close_dserver(void);
extern int iceb_perzap(int metka,GtkWidget *wpredok);
extern int iceb_get_dev(const char *imafil);
extern int   	iceb_nomnalnak(short mes,short god,GtkWidget*);
extern int	iceb_provnomnnak(short mes,short god,const char *nomnalnak,GtkWidget *wpredok);
extern void iceb_rasnaln1(const char *imaf,int *nomer_str,int *kor,short dd,short md,short gd,short dnn,short mnn,short gnn,const char *nomnn,const char *osnov,const char *uspr,const char *frop,const char *kpos,int tipz,FILE **f1,FILE **f2,GtkWidget *wpredok);
extern void iceb_rasnaln2(int nomer_str,int kor,short tipnn,double itogo,double sumkor,const char *naimnal,float procent,float pnds,const char *imaf_nastr,const char *kodop,double okrg1,double suma_voz_tar,FILE *f1,FILE *f2,GtkWidget*);
extern void iceb_snanomer(int kolzap,int *snanomer,GtkWidget *treeview);
extern void iceb_gdite(class iceb_gdite_data *data,int,GtkWidget *wpredok);
extern void iceb_prosf(const char *imaf,GtkWidget*);
extern int iceb_menu_mes_god(class iceb_u_str *mes_god,const char *nadpis,const char *imaf_nast,GtkWidget *wpredok);
extern int iceb_spis_komp(const char *imaf,GtkWidget *wpredok);
extern int   iceb_l_subbal(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
extern int iceb_menu_import(iceb_u_str *imafz,const char *zagolov,const char *imaf_nast,GtkWidget *wpredok);
extern void iceb_start_rf();

void iceb_pm_nds(GtkWidget **opt,int *nds);
//void iceb_pm_nds(GtkWidget **opt,int *nds,float pnds);
void iceb_pm_nds(const char *data,GtkWidget **opt,int *nds);
void iceb_pm_nds(short d,short m,short g,GtkWidget **opt,int *nds);

extern void iceb_pm_vibor(class iceb_u_spisok *strmenu,GtkWidget **opt,int *nomer_str);
extern int iceb_prov_iz(int kod,GtkWidget *wpredok);
extern int iceb_prospp(const char *imaf,GtkWidget*);
extern void iceb_salorok(const char *kontr,GtkWidget *wpredok);
extern void iceb_akt_sv(class akt_sverki *akt_svr,FILE *ff,FILE *ff_bi,GtkWidget*);
extern void iceb_hasv(char *shet,FILE *ff,GtkWidget *wpredok);
extern int iceb_pbp(int metka_ps,short dd,short md,short gd,const char *nomdok,int pod,int tipz,const char *soob,GtkWidget *wpredok);
extern int iceb_pbp(const char *kto,short dd,short md,short gd,const char *nomdok,int pod,int tipz,const char *soob,GtkWidget *wpredok);
extern int iceb_pbpds(short mes,short god,GtkWidget *wpredok);
extern int iceb_pbpds(short mes,short god,int podsystem,GtkWidget *wpredok);
extern int iceb_pbpds(const char *data,GtkWidget *wpredok);
extern int iceb_pbsh(short mes,short god,const char *shet,const char *shetkor,const char *repl,GtkWidget *wpredok);
extern int iceb_delfil(const char *katalog,const char *rashir,GtkWidget *wpredok);
extern int iceb_fioruk(int metka,class iceb_fioruk_rk *rek,GtkWidget *wpredok);
extern void iceb_mpods(int metka_ps,char *kto);
extern int iceb_pvkdd(int metka_ps,short dd,short md,short gd,short mk,short gk,int pod,const char *nomdok,int tipz,GtkWidget *wpredok);
extern void iceb_uionp(GtkWidget *wpredok);
extern int iceb_pvglkni(short mes,short god,GtkWidget *wpredok);
extern int iceb_pvglkni(const char *mesgod,GtkWidget *wpredok);
extern int iceb_make_kat(class iceb_u_str *putnakat,const char *podkat,GtkWidget *wpredok);
extern void iceb_fsystem(const char *imaf_nast,GtkWidget *wpredok);
extern void iceb_avp(class iceb_avp *avp,const char *imaf_nast,class iceb_u_spisok *sp_prov,class iceb_u_double *sum_prov_dk,FILE *ff_prot,GtkWidget *wpredok);
extern int  iceb_poi_kontr(class iceb_u_str *kontr,class iceb_u_str *naim_kontr,GtkWidget *wpredok);
extern void iceb_ustpeh(const char *imaf,int orient,GtkWidget *wpredok);
extern int iceb_sortkk(int metkasort,class iceb_u_spisok *skontr,class iceb_u_spisok *skontr_sort,GtkWidget *wpredok);
extern int iceb_cp(const char *imaf_out,const char *imaf_in,int metka,GtkWidget *wpredok);
extern int iceb_cat(const char *imaf1,const char *imaf2,GtkWidget *wpredok);
int iceb_poldan(const char *strpoi,char *find_dat,const char *imaf,GtkWidget *wpredok);
int iceb_poldan(const char *strpoi,class iceb_u_str *find_dat,const char *imaf,GtkWidget *wpredok);
int iceb_poldan(const char *strpoi,int *kk,const char *imaf,GtkWidget *wpredok);
int iceb_poldan_vkl(const char *strpoi,const char *imaf,GtkWidget *wpredok);
extern int iceb_perecod(int metka_kod,const char *imaf,GtkWidget*);
extern int iceb_pnnnp(const char *datann,const char *datavnn,const char *kodkontr,const char *nomnn,const char *datadok,const char *nomdok,int metka_ps,GtkWidget *wpredok);
extern int iceb_vperecod(const char *imaf,GtkWidget *wpredok);
extern const char *iceb_tokoi8u(const char *fromstr);
extern int iceb_dir_select(class iceb_u_str *imadir,const char *titl,GtkWidget *wpredok);
extern int iceb_menu_start(int nomstr_menu,const char *version,const char *dataver,const char *naim_pods,const char *fioop,int nom_op,class iceb_u_spisok *menustr,const char *cvet_fona);
extern void iceb_about();
extern int   iceb_l_smenabaz(class iceb_u_str *imabazv,int metka,GtkWidget *wpredok);
extern void iceb_font_selection(GtkWidget *wpredok);
extern void iceb_pm_pr(GtkWidget **opt,int *metka_pr);
extern void iceb_l_blok(GtkWidget *wpredok);
extern const char *iceb_getnps();
extern int iceb_getuid(GtkWidget *wpredok);
extern int iceb_prr(GtkWidget *wpredok);
extern int iceb_prr(int podsystem,GtkWidget *wpredok);
extern int iceb_l_blok_prov(GtkWidget *wpredok);
extern int iceb_pblok(short mes,short god,int kodps,GtkWidget *wpredok);
extern const char *iceb_getfioop(GtkWidget *wpredok);
extern void iceb_alxin(const char *imaf,GtkWidget *wpredok);
extern int iceb_alxout(const char *imaf,GtkWidget *wpredok);
extern int iceb_prn(int podsystem,GtkWidget *wpredok);
extern int iceb_prn(GtkWidget *wpredok);
extern const char *iceb_kszap(int kodop,int metka,GtkWidget *wpredok);
extern const char *iceb_kszap(const char *kodop,int metka,GtkWidget *wpredok);
extern int iceb_print_operation(GtkPrintOperationAction operation_action,const char *filname,GtkWidget *wpredok);
extern int iceb_insfil(const char *imaf,FILE *ff,GtkWidget *wpredok);
extern double iceb_pnds(short d,short m,short g,GtkWidget *wpredok);
extern double iceb_pnds(const char *data,GtkWidget *predok);
extern double iceb_pnds(GtkWidget *wpredok);
extern int iceb_alxzag(const char *put_alx,int metka,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok) ;
char *iceb_kikz(char *kto,char *vremzap,GtkWidget *wpredok);
char *iceb_kikz(int kto,time_t vremzap,GtkWidget *wpredok);
void iceb_fplus(const char *stroka,class iceb_u_str *stroka1,class SQLCURSOR *cur);
void iceb_fplus(int metka_sp,const char *stroka,class iceb_u_spisok *nastr,class SQLCURSOR *cur);
void iceb_sozmas(short **kod,char *st1,class SQLCURSOR *cur);
int iceb_vixod(short d,short m,short g,GtkWidget *wpredok);
int iceb_t_nalnaks(const char *imaf,int *nomer_st,short dd,short md,short gd,short dnn,short mnn,short gnn,const char *nomnn,const char *osnov,const char *uspr,const char *frop,const char *kpos,int tipz,FILE **ff,GtkWidget *wpredok);
void iceb_nalnake(int tipz,short tipnn,double itogo,double sumkor,const char *naimnal,float procent,float pnds,const char *imaf_nastr,const char *kodop,double okrg1,double suma_voz_tar,FILE *ff,GtkWidget *wpredok);
void iceb_nalnak_str(int metka_str,int tipnn,const char *razdel,const char *datop,const char *naim,const char *ei,double kolih,double cena,double suma,FILE *ff);
int iceb_nalnaks(const char *imaf,int *nomer_st,short dd,short md,short gd,short dnn,short mnn,short gnn,const char *nomnn,const char *osnov,const char *uspr,const char *frop,const char *kpos,int tipz,int metka_kop,FILE **ff,GtkWidget *wpredok);
void iceb_vkk00(GtkWidget *wpredok);
void       kontr_korkod(const char *kods,const char *kodn,GtkWidget *wpredok);
int        kontr_prov_row(SQL_str row,class kontr_data *data);
int iceb_getuslp(const char *kodkon,class iceb_u_str *uslprod,GtkWidget *wpredok);
int iceb_getuslp(int un_nom_zap,class iceb_u_str *uslprod,GtkWidget *wpredok);
int iceb_getuslp_m(const char *kodkon,class iceb_u_str *usl_prod,GtkWidget *wpredok);
int   iceb_l_kontdog(int metka_rr,const char *kodkon,GtkWidget *wpredok);
void iceb_rnfxml(class iceb_rnfxml_data *rek_zag_nn,GtkWidget *wpredok);
int iceb_openxml(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,const char *tip_dok,const char *pod_tip_dok,const char *nomer_versii_dok,int nomer_dok_v_pakete,int *period_type,class iceb_rnfxml_data *rek_zag_nn,FILE **ff_xml,GtkWidget *wpredok);
int iceb_menu_vibkat(class iceb_u_spisok *zapros,class iceb_u_str *stroka,GtkWidget *wpredok);
int iceb_menu_vibkat(const char *zapros,class iceb_u_str *stroka,GtkWidget *wpredok);
const char *iceb_get_pnk(const char *kodkontr,int metka,GtkWidget *wpredok);

