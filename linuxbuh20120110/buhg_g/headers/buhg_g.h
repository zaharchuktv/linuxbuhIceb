/*$Id: buhg_g.h,v 1.253 2011-09-05 08:18:27 sasa Exp $*/
/*30.08.2011	12.08.2003 	Белых А.И.	buhg_g.h
*/
//zs
//#include "../libbuh_g/iceb_libbuh.h"
#include "../../libbuh_g/iceb_libbuh.h"
//ze
#include "ostatok.h"
#include "rek_kartmat.h"
#include "zarp.h"
#include "poiinpdw.h"
#include "bsizw.h"
#include "ostuosw.h"
#include "bsiz_w.h"
#include "zar_read_tn1w.h"
#include "zarrnesvpw.h"

//zs
//#define   NAME_SYSTEM "iceBw"
#define   NAME_SYSTEM "linuxbuh.ru"
//ze
//#define   CONFIG_PATH "/etc/iceB"
const short ICEB_DNDESV=1,ICEB_MNDESV=1,ICEB_GNDESV=2011;


void   l_prov(class prov_poi_data*,GtkWidget*);
int     provvkp(const char *kto,GtkWidget *wpredok);
void    forzappoi(class prov_poi_data *data);
int     prov_prov_row(SQL_str row,class prov_poi_data *data);
void	sozkom(char *,int,char *,char *,char *);
void    rgpd(class go_rr *,short,short,short,short,short,short,iceb_u_spisok *,iceb_u_spisok *,double *,double *,FILE *,GtkWidget *);
void    rbd(int metka);
void	hrvnks(FILE *ff,int *kolstrok);
void    hdisk(int nomer,double itogoo,const char *imafkartf);
int rozraxw(short dn,short mn,short gn,short dk,short mk,short gk,short sb,class GLKTMP *PROM,GtkTextBuffer *buffer,GtkWidget *view,GtkWidget *bar,GtkWidget *wpredok);
void menuvnpw(int metka,int tipz,GtkWidget *wpredok);
void buhdenshetw(const char *zagol,class iceb_u_spisok *dni,class iceb_u_spisok *sheta,class iceb_u_double *den_shet_suma,FILE *ff);
int l_opldok(int metka_ps,short dd,short md,short gd,const char *nomdok,int podr,double suma_kopl,GtkWidget *wpredok);
int saldo_m(class saldo_m_rek *rek,GtkWidget *wpredok);


// Материальный учёт
int eks_ko_v(class zar_eks_ko_rek *rek,GtkWidget *wpredok);
int     matnastw(void); 
int  mater_pvu(int,const char *kod,GtkWidget *wpredok);
int     l_mater(int,iceb_u_str *,iceb_u_str *,int,int,GtkWidget *);
int     l_mater1(iceb_u_str *kod,iceb_u_str *naim,int skl_poi,const char *grup,int metka_skl,int metka_cen,GtkWidget *wpredok);
int l_mater_v_provkor(int nkod,int skod,GtkWidget *wpredok);
void mater_kkvt(int kods,int kodn,GtkWidget *wpredok);
double	ostdok1w(short dk,short mk,short gk,int skl,int kodm);
int     ostdokw(short dn,short mn,short gn,short dk,short mk,short gk,const char *skk,const char *nkk,class ostatok *data);
int     ostdokw(short dn,short mn,short gn,short dk,short mk,short gk,int skk,int nkk,class ostatok *data);
int     ostkarw(short dn,short mn,short gn,short dk,short mk,short gk,const char *skl,const char *nk,class ostatok *data);
int     ostkarw(short dn,short mn,short gn,short dk,short mk,short gk,int skl,int nk,class ostatok *data);
double ostkar1w(int kodmat,int sklad,short dost,short most,short gost,GtkWidget *wpredok);
int     l_zkartmat(const char *skl,const char *n_kart,GtkWidget *wpredok);
int     l_zkartmat(int skl,int  n_kart,GtkWidget *wpredok);
double	readkolkw(int skl,int nk,short d,short m,short g,const char *nn,GtkWidget *wpredok);
void    podvdokw(short d,short m,short g,const char *nn,int skll,GtkWidget *wpredok);
int     prosprw(short,int,short,short,short,const char *,int,int,int,const char*,const char *,iceb_u_str*,GtkWidget *);
int     zapkartm(int mz,class rek_kartmat *data,GtkWidget *wpredok);
int	prdppw(int skl,int nk,short *dp,short *mp,short *gp,GtkWidget *wpredok);
int     zapvkrw(short dp,short mp,short gp,const char *nn,int skl,int nk,short d,short m,short g,int tp,double kolih,double cena,GtkWidget*);
int     l_dokmat(iceb_u_str *datad,int tipz,iceb_u_str *sklad,iceb_u_str *nomdok,GtkWidget *wpredok);
int	sortnak(short dd,short mm,short gg,int skl,char *nomdok);
void	rasdokkr(double kol,double kolt,double kratz,FILE *ff);
int	nalndohw(void);
int	sortnakw(short dd,short mm,short gg,int skl,const char *nomdok,GtkWidget *wpredok);
void    rasdokw(short dg,short mg,short gg,int skl,const char *nomdok,const char *imaf,short lnds,short vr,FILE *f1,double ves,GtkWidget *wpredok);
int    rasnalnw(short dd,short md,short gd,int skl,const char *nomdok,const char *imaf,short tipnn,GtkWidget *wpredok);
int     udkarw(int skl,int nk,GtkWidget *wpredok);
void    matudw(const char *nomdok,short d,short m,short g,int skl,int tipz,GtkWidget *wpredok);
int     matuddw(short mu,short mprv,short mun,int tipz,short d,short m,short g,const char *nomdok,int skl,int nk,int kodm,GtkWidget *wpredok);
int 	provudvdokw(int sklad,short dd,short md,short gd,const char *nomd,int kodmat,int nomkart,GtkWidget *wpredok);
int     vbrpw(short dd,short md,short gd,int skl,const char *nomdok,int kodm,int tipz,GtkWidget*);
int     zapvdokw(short dd,short md,short gd,int skll,int nkk,int km,const char *nnn,double kol,double cen,const char *ei,double nds,int mnds,int voztar,int tipz,int nomkarp,const char *shet,const char *nomz,GtkWidget *wpredok);
short   zapkarw(int skll,int  nkk,int km,double ccn,const char *eii,const char *shu,double krt,double fas,int kodt,double nds,int mnds,double cenav,short denv,short mesv,short godv,const char *innom,const char *rnd,const char *nomz,short,short,short,int mv,GtkWidget *wpredok);
int     nomkrw(int sklad,GtkWidget *wpredok);
int     vdndw(int,iceb_u_str*,iceb_u_str *skl,iceb_u_str *nomdok,iceb_u_str *kodop,GtkWidget*);
int     zapdokw(class zapdokw_data *data,class iceb_lock_tables *lokt,GtkWidget *wpredok);
int     poi_kontr(iceb_u_str *kontr,iceb_u_str *naim_kontr,GtkWidget *wpredok);
void    sumdokw(SQL_str row,double *suma,double *sumabn,double *ves,int lnds,double *sumsnds,float);
int     sumdokwu(short d,short m,short g,int skl,const char *nn,double *suma,GtkWidget *wpredok);
double	makkorrw(double sum,iceb_u_double *maskor);
int     makkorw(iceb_u_spisok *MENU,short tr,iceb_u_str *naikor,iceb_u_double *maskor,const char *imaf);
void	cenniksw(char *imaf,GtkWidget *wpredok);
void	cennikw(short d,short m,short g,char *kodtv,char *naim,double cena,char *shrihkod,char *organ,FILE *ff);
void	sapkkivw(int *kolstr,int kollist,FILE *ff2);
void	hrasactw(int *kolstrok,int *kollist,FILE *ff);
void	sapkk1ivw(int *kolstrlist,int *kollist,FILE *ff1);
void    krasactw(char *shetz,char *naimshet,double kolihshet,double sumashet,double kolihi,double sumai,FILE *ff);
int     l_kartmatv(int,short,short,short,const char*,double,iceb_u_str*,int kodm,int skl,int *n_kart,double,int,float,GtkWidget *wpredok);
void    podtdok1w(short dd,short md,short gd,int skl,const char *nomdok,int tipz,short d1,short m1,short g1,int metkaeho,GtkWidget *wpredok);
void    rasoskrw(GtkWidget *wpredok);
int	provndsw(const char *nds,SQL_str row);
char    *prcnw(double cn);
void	sapvinw(const char *matot,const char *mesqc,short g,short gos,int *kolstriv,int *kollistiv,FILE *ff2);
void    koncinvw(char *mesqc,short gos,FILE *ff2);
int     import_m(iceb_u_str *imafz,char *zagolov,GtkWidget *wpredok);
int	findkarw(short dd,short md,short gd,int sklad,int kodmat,double kolih,iceb_u_int *nomkart,iceb_u_double *kolihp,int kodopuc,double cena,const char *shetu,GtkWidget *wpredok);
void    dvtmcf3_prp(class dvtmcf3_r_data *data,FILE *ff1);
int     vib_kodop_mu(iceb_u_str *,GtkWidget*);
void    sumzap1w(int d,int m,int g,const char *nn,int skl,double *suma,double *sumabn,int lnds,short vt,double *sumkor,int tipz,double *kolih,GtkWidget *wpredok);
void    sumzap1uw(int d,int m,int g,const char *nn,int podr,double *suma,double *sumabn,int lnds,int tp,GtkWidget *wpredok);
void    sumzapuosw(int d,int m,int g,const char *nn,double *suma,double *sumabn,int lnds,int tp,GtkWidget *wpredok);
int specprov(char *kodiz,iceb_u_str *naim,iceb_u_str *poisk,int metka_naim,GtkWidget *wpredok);
int   l_spec_v(const char *kod_izdel,iceb_u_str *kod_zap,int metka_zap,GtkWidget *wpredok);
int nomkmw(GtkWidget *wpredok);
int zap_s_mudokw(int tipz,short dd,short md,short gd,int sklad,const char *kontr,const char *nomdok,const char *nomnalnak,const char *kodop,int podt,int prov,const char *nomvstdok,short dpnn,short mpnn,short gpnn,int metka_opl,GtkWidget *wpredok);
void zap_prihodw(short dd,short md,short gd,const char *nomdok,int sklad,int kodmat,double kolih,double cena,const char *eiz,const char *shetu,double nds,int vtara,int mnds,const char *nomzak,const char *shet_sp,GtkWidget *wpredok);
void priv_k_kartw(short dk,short mk,short gk,const char *nomd,int sklad,int kodm,double kolih,double cena,const char *shetu,const char *ei,double ndss,int mnds,int voztar,const char *nomzak,int metka_priv,const char *shet_sp,GtkWidget *wpredok);
double buh_rhw(int metka_dkv,class iceb_u_spisok *sheta_deb,class iceb_u_spisok *sheta_kre,class iceb_u_double *sum_pr,FILE *ff);
void sapgorsw(short dn,short mn,short gn,short dk,short mk,short gk,int *nom_lista,int *kolstrok,FILE *ff);
void optimbazw(void);
void rpshet_rsw(const char *shet,short dn,short mn,short gn,short dk,short mk,short gk,double *deb_kre,GtkWidget *wpredok);
void rpshet_ssw(const char *shet,short dn,short mn,short gn,short dk,short mk,short gk,double *deb_kre, GtkWidget *wpredok);
void in_d12_itw(class iceb_u_spisok *sp_shet,class iceb_u_double *kol_shet,class iceb_u_double *sum_shet,FILE *ffipsh,GtkWidget *wpredok);
int in_d12_startw(FILE *ff);
double skoplmuw(short dd,short md,short gd,const char *nomdok,int sklad,GtkWidget *wpredok);
void rashapw(int tipz,int *kolstrok,const char *kodkontr00,const char *kontr,int ots,int kor,FILE *ffdok,GtkWidget *wpredok);


//Учёт услуг
int l_uslgrup(int metka,iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok);
int l_uslugi(int metka,iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok);
int l_uslopp(int metka,iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok);
int l_uslopr(int metka,iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok);
int l_uslpodr(int metka,iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok);
int l_usldok(short dd,short md,short gd,int tipz,int podr,const char *nomd,GtkWidget *wpredok);
double readkoluw(short d,short m,short g,int podr,const char *nomd,int tipz,int metka,int kodzap,int nomzap,GtkWidget *wpredok);
int vdnuw(short *dd,short *md,short *gd,int tipz,iceb_u_str *nomdok,int *podr,GtkWidget *wpredok);
int vuslw_v(short dd,short md,short gd,int tipz,int podr,const char *nomdok,const char *kod,int metka,int nom_zap,float pnds,GtkWidget*);
int prosprusw(short mr,int podr,short d,short m,short g,const char *nn,int tp,int lnds,const char *kodop,float pnds,class iceb_u_str *shet_suma,GtkWidget *wpredok);
int usldopnalw(int tipz,const char *kodop,float *procent,iceb_u_str *naim);
int zappoduw(int tp,short dd,short md,short gd,short dp,short mp,short gp,const char nomd[],int metka,int kodzap,double kolih,double cena,const char ei[],const char shetu[],int podr,int nomzap,GtkWidget *wpredok);
int vib_kodop_usl(iceb_u_str *kodop,GtkWidget *wpredok);
int uduvdokw(int tipz,short d,short m,short g,const char *nomd,int podr,int metka,int kodzap,int nomzap,GtkWidget *wpredok);
int zapuvdokw(int tp,short dd,short md,short gd,const char *nomd,int metka,int kodzap,double kolih,double cena,const char *ei,const char *shetu,int podr,const char *shet_sp,const char *dop_naim,GtkWidget *wpredok);
double skopluslw(short dd,short md,short gd,const char *nomdok,int podr,GtkWidget *wpredok);

/*Учёт основных средств*/
double uosgetlsw(int innom,short d,short m,short g,int metka_u,GtkWidget *wpredok);
int l_uosopp(int metka,iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok);
int l_uosopr(int metka,iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok);
int vib_kodop_uos(iceb_u_str *kodop,GtkWidget *wpredok);
int l_uospodr(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int l_uosgrnu(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int l_uosgrbu(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int l_uoshau(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int l_uoshz(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int l_uosmo(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int poiinw(int in,short d,short m,short g,int *podr,int *kodotl,GtkWidget *wpredok);
int poiinw(int in,const char *data,int *podr,int *kodotl,GtkWidget *wpredok);
int poiinpdw(int in,short m,short g,class poiinpdw_data*,GtkWidget *wpredok);
void uosgetnd(short g,short tipz,class iceb_u_str *nomdok,GtkWidget *wpredok);
int uosvhd(class iceb_u_str *datadok,class iceb_u_str *nomdok,class iceb_u_str *kod_op,int tipz,GtkWidget *wpredok);
int uosprovnd(short g,int tz,const char *ndk,GtkWidget*);
int uosprovdok(class iceb_u_str *datadok,const char *ndk,int tz,GtkWidget *wpredok);
int uosprhd(class uosvhd_data *data,GtkWidget *wpredok);
int uoszaphd(class uosvhd_data *data,GtkWidget *wpredok);
int l_uosdok(const char *datadok,const char *nomd,GtkWidget *wpredok);
int uosgetinn(class iceb_u_str *innom,int metka_v,GtkWidget *wpredok);
int bsizw(int in,int ceh,short dp,short mp,short gp,class bsizw_data*,FILE *ff,GtkWidget *wpredok);
void bsiz23w(const char *grp,short dp,short mp,short gp,double *bs,double *iz,double *iz1,FILE *ff_prot,GtkWidget *wpredok);
int uoskart2(int innom,short dp,short mp,short gp,GtkWidget *wpredok);
void i_xuos_vnsi(GtkWidget*);
int amortw(int in,short mr,short gr,short mra,short kvrt,const char *grupa,FILE *ff_prot,GtkWidget *wpredok);
void amort23w(int in,int podd,short kvrt,short gr,const char *hzt,FILE *ff_prot,GtkWidget *wpredok);
int amortbyw(long in,short mr,short gr,short mra,const char *grup_bu,FILE *ff_prot,GtkWidget *wpredok);
void uosrsdw(int in,GtkWidget *wpredok);
int puzapw(int in,int podd,short,short,short,const char *ndk,GtkWidget *wpredok);
void prpuosw(short,short,short,const char *ndk,int podd,class iceb_u_spisok *spsh,class iceb_u_double *sn,class iceb_u_double*,short mr,class iceb_u_str *shet_suma,GtkWidget *wpredok);
int l_uosin(int metka,class iceb_u_str *innom,class iceb_u_str *naimk,GtkWidget *wpredok);
void  ostuosw(int in,short dn,short mn,short gn,short dk,short mk,short gk,int,int,class ostuosw_data *uos_ost,GtkWidget *wpredok);
void strpod(short mn,short mk,short dlu,FILE *ff);
void strms(short mt,short mn,short mk,FILE *ff);
void rasmes(short mn,short mk,double *maot,double ostt,const char *hnaz,FILE *ff);
void bsiz23pw(char kodgr[],short dn,short mn,short gn,short dk,short mk,short gk,class bsiz_data *data,GtkWidget *wpredok);
int bsizpw(int in,int ceh,short dn,short mn,short gn,short dk,short mk,short gk,class bsiz_data *data,GtkWidget *wpredok);
int   l_uosdm(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int uosprovgrw(const char *kodgr,GtkWidget *wpredok);
int uosprovarw(const char *kodgr,int metkau,GtkWidget *wpredok);
double skopluosw(short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok);

/*Учёт кассовых ордеров*/
int l_kasopp(int metka,iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok);
int l_kasopr(int metka,iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok);
int l_spis_kas(int metka,iceb_u_str *kod,iceb_u_str *naimk,GtkWidget *wpredok);
int vkasord1(iceb_u_str *datad,iceb_u_str *nomdok,iceb_u_str *kassa,GtkWidget *wpredok);
int vkasord2(iceb_u_str *datad,iceb_u_str *nomdok,iceb_u_str *kassa,GtkWidget *wpredok);
int zaphkorw(int mkor,const char *kassa,int tipz,short d,short m,short g,const char *nomd,const char *shetk,const char *kodop,const char *osnov,const char *dopol,const char *fio,const char *dokum,const char *kassaz,const char *nomdz,short dz,short mz,short gz,const char*,const char *nomv,GtkWidget *wpredok);
int vib_kodop_kas(iceb_u_str *kodop,GtkWidget *wpredok);
void l_kasdok(const char *data_dok,int tipz,const char *nomdok,const char *kassa,GtkWidget *wpredok);
int zapzkorw(const char *kassa,int tipz,short dd,short md,short gd,const char *nomd,const char *kodkont,double suma,const char *kontrz,const char *koment,GtkWidget *wpredok);
int pbkasw(short md,short gd,GtkWidget *wpredok);
int udkasdokw(const char *kassa,const char *nomd,short dd,short md,short gd,short tipz,GtkWidget *wpredok);
int sumkasorw(const char kassa[],short tipz,const char nomd[],short god,double *sumd,double *sumpod,GtkWidget *wpredok);
void provprkasw(const char kassa[],short tipz,const char nomd[],short dd,short md,short gd,const char kodop[],GtkWidget *wpredok);
int   l_kaskcn(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
void	avtprkasw(const char kassa[],short tipz,const char nomd[],short dd,short md,short gd,const char shetk[],const char shetd[],const char kodop[],GtkWidget *wpredok);
void provpodkow(const char *kassa,const char *nomd,short dd,short md,short gd,short tipz,GtkWidget *wpredok);
int l_kasvedz_v_maxnd(short god,GtkWidget *wpredok);

/*Расчёт зарплаты*/
void zarsf1dfw(int kolih_str,int kolih_fiz_os,int kolih_list,int TYP,FILE *ff1,GtkWidget *wpredok);
void zarnvpw(GtkWidget *wpredok);
int  zaravpt_osw(int tbn,short mp,short gp,class iceb_u_spisok *data,class iceb_u_double *suma,GtkWidget *wpredok);
double findnahw(int tnn,short m,short g,GtkWidget *wpredok);
int l_zarsb(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int  zarstfn53w(char *naipodr,short mr,short gr,const char *shet,const char *nomerved,int *kolstr,FILE *ff);
void    sspp1w(FILE *kaw1,int list,int *kolstr);
void	sumprvedw(iceb_u_double *SUMPODR,iceb_u_int *KOLLIST,char *imaf);
void   	itogw(FILE *kaw1,double itogo,GtkWidget*);
int   l_gruppod(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_zarpodr(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_zarkateg(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_zarnah(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_zarud(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_zarvidtab(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_zarzvan(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_zargni(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_zarsocf(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int vkartz(class iceb_u_str *tabnom_p,GtkWidget *wpredok);
double okladw(int tabn,short mr,short gr,int *has,GtkWidget *wpredok);
double okladtrw(short mr,short gr,int roz,FILE *ff,GtkWidget *wpredok);
double zarsaldw(short dd,short mm,short gg,int tbn,double *saldob,GtkWidget *wpredok);
double prbperw(short mn,short gn,short mk,short gk,int tabb,double *sumab,int metka,GtkWidget *wpredok);
void nahudw(short g,short m,int tn,double *nah,double *nahprov,double *uder,double *uderprov,double *nahb,double *uderb,double *uderbprov,short ras,double sl,short mp,const char *fio,GtkWidget *wpredok);
void zapzarnw(short mz,short gz,int podr,int tabn,short kateg,short sovm,short zvan,const char shet[],const char lgot[],
short dn,short mn,short gn,short dk,short mk,short gk,short prov,const char dolg[],GtkWidget *wpredok);
int provblokzarpw(int tabn,short mp,short gp,short flagrk,int,GtkWidget *wpredok);
int provkodw(short *masiv,short kod);
int provkodw(short *masiv,char *kod);
void rudnfvw(double *suma_zar,double suma_ndfnz,FILE *ffp);
void zaravprw(int tbn,short mp,short gp,FILE *ff_prot,GtkWidget *wpredok);
void zarsocw(short mr,short gr,int tabn,FILE*,GtkWidget *wpredok);
void v_nsi(GtkWidget*);
void v_nsi_np(GtkWidget*);
void v_nsi_rs(GtkWidget*);
int l_nahud_t_v(int tabnomz,short dz,short mz,short gz,int prnz,int knahz,short mesv,short godv,int podrz,const char *shetz,int nom_zapz,GtkWidget *wpredok);
void zapzarn1w(int tnn,short tipn,short kd,short dn,short ms,short gd,const char *sh,GtkWidget *wpredok);
int zapzarpw(class ZARP *zp,double suma,short denz,short mv,short gv,short kdn,const char *shet,const char *koment,short nz,int podr,const char *nomdok,GtkWidget *wpredok);
int zapzarpvw(class ZARP *zp,double suma,short denz,short mv,short gv,short dd,short md,short gd,short kdn,const char *shet,const char *koment,short nz,int podr,const char *nomdok,GtkWidget *wpredok);
int zapzagotw(short mp,short gp,int tabn,int prn,int podr,GtkWidget *wpredok);
int zarudnuw(class ZARP *zp,GtkWidget *wpredok);
void redkalw(short m,short g,float *dnei,float *has,GtkWidget *wpredok);
int zapmasw(int tnn,short m,short g,GtkWidget *wpredok);
double zarpodprw(int kodpr,double *procb,GtkWidget *wpredok);
int dolhn1w(long tabn,double *oklad,float *has,float *kof,int *met,int *koddo,int *metka,int *metka1,char *shet,int*,GtkWidget *wpredok);
void fbezrw(int tabn,short mp,short gp,int podr,double *fbezr,double *fbezrb,int metka_zap,int metka_ras,double suma_boln,const char *uder_only,FILE *ff,GtkWidget *wpredok);
double fbezr_sumnalw(double suma);
double findnahmw(short mr,short mt,short *kodi,FILE *ff,double *sumanb,double *sumabu,double *suma_nfnz,double *suma_nfnz_b,double *suma_zarp_o,double *suma_zarp_b,double *suma_boln_rm);
double findnahmw(short mr,short gr,int tabn,short mt,short *kodi,FILE *ff,double *sumanb,double *sumabu,double *suma_nfnz,double *suma_nfnz_b,double *suma_zarp_o,double *suma_zarp_b,double *sumaf_boln_rm,GtkWidget *wpredok);
void ras_pensw(int tn,short mp,short gp,int podr,double *sumapo,double *sumapob,int metka_zap,int metka_ras,double suma_boln,const char *uder_only,FILE *ff,GtkWidget *wpredok);
void ras_socw(int tn,short mp,short gp,int podr,double *sumaos,double *sumaosb,int metka_zap,int metka_ras,double suma_boln,const char *uder_only,FILE *ff,GtkWidget *wpredok);
double ras_podohw(int  tn,short mp,short gp,int podr,double fbezrab,double fbezrabb,double sumapo,double sumapob,double sumaos,double sumaosb,int metka_zap,int metka_ras,double suma_boln,const char *uder_only,FILE *ff,GtkWidget *wpredok);
double ras_podoh1w(int tn,short mp,short gp,int podr,double suma_esv,double suma_esvb,int metka_zap,int metka_ras,double suma_boln,const char *uder_only,FILE *ff_prot,GtkWidget *wpredok);
double find_podohw(short mr,FILE *ff,double *suma_ndfv,double *sumanb,double *sumabu);
double find_podohw(short mr,short gr,int tabn,FILE *ff,double *suma_ndfv,double *sumanb,double *sumabu,GtkWidget *wpredok);

double matpom1w(int tbn,short mp,short gp,short kodp[],FILE *fff,int metka,double *matpomb,int metkar,double *matp,double *matpb,GtkWidget *wpredok);
float  zarlgotw(int tabn,short,short,int *kollgot,class iceb_u_str *kod_lgot,FILE *ffp,GtkWidget*);
double  podohrs1w(int tabn,short mr,short gr,double suma_zar,double suma_soc_ot,double suma_bsl,double suma_ndfv,FILE *ffp,GtkWidget*);
double podohrs2w(int tabn,short mr,short gr,double suma_zar,double suma_soc_ot,double suma_bsl,double suma_ndfv,int metka_zap,int metka_zap_lgot,FILE *ff_prot,GtkWidget *wpredok);
double zar_rsocw(double soc_suma,float prog_min_tek,FILE *ff);
double pensw(int,short,short god,double suma,FILE *ff,GtkWidget *wpredok);
void rasshetzw(int tabn,short mp,short gp,int kp,int prn,const char *nah_only,const char *uder_only,FILE *ff,GtkWidget *wpredok);
void strfilw(const char *a1,const char *a2,short mt,FILE *kaw);
void peh1w(const char *np,const char *zap,long tabb,const char *inn,short mr,short gr,const char *dolg,int kategor,FILE *kaw,FILE *kawk1,FILE *kawk2,FILE *ffkor2s,GtkWidget *wpredok);
void itogizw(double inah,double iu,double ius,double sal,double inahb,double iub,double iusb,double salb,FILE *kaw,FILE *kawk1,FILE *kawk2,FILE *ffkor2s);
int zaptabelw(int mz,short mp,short gp,int tabn,int tz,float dnei,float has,float kdnei,short dn,short mn,short gn,short dk,short mk,short gk,float kolrd,float kolrh,const char *koment,int nomz,int snomz,GtkWidget *wpredok);
int l_sptbn(int metka,class iceb_u_str *tabnom,class iceb_u_str *fio,int podr,GtkWidget *wpredok);
int l_sptbn1(class iceb_u_str *tabnom,class iceb_u_str *fio,int podr,GtkWidget *wpredok);
int zarudkarw(int tabn,GtkWidget *wpredok);
double sumprzw(short m,short g,char nn[],int podr,double *sumd,double *sumk,GtkWidget *wpredok);
int vvod_tabnom(class iceb_u_str *tabnom,GtkWidget *wpredok);
void provzzagw(short mp,short gp,int tabn,int prn,int podr,GtkWidget *wpredok);
void sapreestpw(short mr,short gr,int metka,FILE *ffpodr);
void pib_itlw(int *kolstrkartr,double *sumalist,int *kol_na_liste,const char *rukov,const char *glavbuh,FILE *ffkartr);
void pib_end_dokw(int kol_na_liste,double sumalist,double itogoo,int nomer,const char *rukov,const char *glavbuh,FILE *ffkartr);
void pib_strw(int nomer,const char *bankshet,double sum,const char *fio,const char *indkod,FILE *ffkartr);
void ukrsib_endw(char *imafdbf,const char *imaftmp,double sumai,int nomer,const char *rukov,const char *glavbuh,FILE *ffr,FILE *fftmp,GtkWidget*);
int ukrsib_startw(char *imafr,char *imaftmp,class iceb_u_str *operatorr,FILE **fftmp,FILE **ffr,GtkWidget *wpredok);
void ukrsib_strw(short dt,short mt,short gt,double suma,const char *kartshet,const char *fio,int *nomer,const char *tabnom,const char *ope_tor,FILE *ffr,FILE *fftmp);
void ukreksim_hw(const char *imaf,long kolz);
void ukreksim_endw(int nomer,const char *imaftmp,char *imafdbf,double sumai,const char *rukov,const char *glavbuh,FILE *fftmp,FILE *ffr,GtkWidget*);
int ukreksim_hapw(char *imafr,char *imaftmp,FILE **fftmp,FILE **ffr,GtkWidget *wpredok);
void ukreksim_zstrw(FILE *fftmp,FILE *ffr,int *nomer,double suma,char *fio,char *kartshet,char *tabn,short dt,short mt,short gt);
int l_nahud_prov_blok(short mp,short gp,int tabn,GtkWidget *wpredok);
void vztbw(int tn,short m,short g,short *dn,double *has,short *kdn,short met,FILE *ff,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
double	sosnsumw(char *tabn,short dn,short mn,short gn,short dk,short mk,short gk,GtkWidget *wpredok);
void rasnuptn1w(short mn,short gn,short dk,short mk,short gk,int tabn,char fio[],FILE *ff,GtkWidget *wpredok);
void ssf8dr(int *klst,int *kollist,FILE *ff);
void  zapf8dr(int npp1,short kvrt,short gn,const char *koded,short TYP,const char *inn,double suman,double sumav,double podohn,double podohv,const char *priz,const char *datn,const char *datk,short kodlg,short vidz,FILE *ff4);
void zagf1df(int kolrh,int kolrs,short kvrt,const char *kod,short TYP,int *klst,int vidd,short dn,short mn,short gn,short dk,short mk,short gk,int metka_oth,FILE *ff1,FILE *ff2,GtkWidget *wpredok);
void spkvd(int *klst,FILE *ff1);
void creatheaddbf(char imaf[],int kolz,GtkWidget *wpredok);
void koncf8dr(int kvrt,short gn,const char *koded,short TYP,double it1,double it1n,double it2,double it2n,double it11,double it11n,double it21,double it21n,int kolsh,int klls,int *npp1,int metkar,FILE *ff1,FILE *ff2,FILE *ff4,GtkWidget *wpredok);
int l_f1df_v(class iceb_u_str *nomdok,GtkWidget *wpredok);
void zar_read_tnw(short dn,short mn,short gn,FILE *ffprot,GtkWidget *wpredok);
void zar_read_tn1w(short dn,short mn,short gn,class zar_read_tn1h *nastr,FILE *ff_prot,GtkWidget *wpredok);

//int rnof_xmlw(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,const char *tip_dok,const char *pod_tip_dok,const char *nomer_versii_dok,int nomer_dok_v_pakete,int *period_type,FILE **ff_xml,GtkWidget *wpredok);
void rnn_sap_xmlw(short mn,short gn,short mk,const char *tip_dok,const char *podtip,int type_ver,int pnd,const char *imaf,int period_type,class iceb_rnfxml_data *rek_zag_nn,FILE *ff_xml);
void f1df_xml_konw(double isumadn,double isumad,double isumann,double isuman,int nomer_xml_str,int kolr,int klls,const char *naim_kontr,const char *telef_kontr,int TYP,FILE *ff_xml,GtkWidget *wpredok);
void f1df_xml_strw(int *nomer_xml_str,char *inn,double sumadn,double sumad,double sumann,double suman,const char *priz,const char *data11,const char *data21,const char *lgotach,const char *metka_zap,FILE *ff_xml);
void f1df_xml_zagw(int kvrt,int god,const char *kod,int TYP,int metka_oth,int kolih_v_htate,int kolih_po_sovm,const char *naim_kontr,char *imaf_xml,FILE **ff_xml,GtkWidget *wpredok);
int zarprtnw(short mr,short gr,int tabn,const char *imaf,FILE*,GtkWidget *wpredok);
int l_zar_dok_hap(class iceb_u_str *datad,class iceb_u_str *nomdok,short prn,GtkWidget *wpredok);
int l_zar_dok_prn(GtkWidget *wpredok);
int l_zar_dok_zap(short prn,const char *datad,const char *nomdok,GtkWidget *wpredok);
int zartarrozw(GtkWidget *wpredok);
float zartarrozw(int roz);
void zartarrozw(double min_zar,FILE *ff,GtkWidget *wpredok);
int zartarroz1w(GtkWidget *wpredok);
double zartarroz1w(int roz,int kof,FILE *ff);
int zartarroz1w(class iceb_u_str *imaf_ot,GtkWidget *wpredok);
void zarreadnw(short dr,short mr,short gr,int metka_r,FILE *ff,GtkWidget *wpredok);
void zarstvedw(int *nomer_vedom,int tabb,const char *fio,double suma,FILE *ff);
void zarstvedw(int *nomer_vedom,const char *tabb,const char *fio,double suma,FILE *ff);
double knnfndsw(int metka,int knu,const char *shet,double suma,double *sumab,FILE *ff_prot);
void zvb_ukrgaz_endw(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp,GtkWidget*);
int zvb_ukrgaz_startw(char *imafr,char *imaf_dbf,char *imaf_dbf_tmp,FILE **ff,FILE **ffr,GtkWidget*);
void zvb_ukrgaz_strw(int nomerpp,const char *fio,const char *nomersh,double suma,const char *inn,const char *tabnom,FILE *ff_dbf_tmp,FILE *ffras);
void zvb_promekonom_endw(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp,GtkWidget*);
int zvb_promekonom_startw(char *imafr,char *imaf_dbf,char *imaf_dbf_tmp,FILE **ff,FILE **ffr,GtkWidget*);
void zvb_promekonom_strw(int nomerpp,const char *fio,const char *nomersh,double suma,const char *inn,const char *tabnom,FILE *ff_dbf_tmp,FILE *ffras);
int zarbanksw(class iceb_u_str *kod00,int *kod_ban,GtkWidget*);
int zvb_ibank2ua_startw(char *imafr,char *imaf,class iceb_u_str *vidnahis,const char *kod00,FILE **ff,FILE **ffr,GtkWidget *wpredok);
void zvb_ibank2ua_endw(double sumai,const char *vid_nah,FILE *ffr,FILE *ff,GtkWidget *wpredok);
void zvb_ibank2ua_strw(int *nomer_zap,char *fio,char *shet_b,char *inn,double suma,FILE *ff,FILE *ffr);
int zvb_hreshatik_startw(char *imafr,char *imaf,double suma,short *d_pp,short *m_pp,short *g_pp,FILE **ff,FILE **ffr,GtkWidget *wpredok);
void zvb_hreshatik_strw(short d_pp,short m_pp,short g_pp,int *nomer_zap,char *fio,char *shet_b,char *inn,double suma,FILE *ff,FILE *ffr);
void zvb_hreshatik_endw(double sumai,int kolzap,FILE *ffr,FILE *ff,GtkWidget *wpredok);
void zarrnesvw(FILE *ff_prot,GtkWidget *wpredok);
void zarrnesvpw(short mr,short gr,class zarrnesvp_rek *all_proc,FILE *ff_prot,GtkWidget *wpredok);
double  zaresvw(int tabn,short mp,short gp,int podr,double *esv_s_lgot,double *esv_s_lgotb,int metka_zap,double suma_boln_for_per,const char *uder_only,FILE *ff_prot,GtkWidget *wpredok);
double zaresv_rs(short mp,short gp,int tabn,int podr,double suma_ob,double suma_bud,double suma_bol_ob,double suma_bol_bud,double suma_s_lgot,double suma_s_lgotb,double suma_s_lgot_bol,double suma_s_lgotb_bol,double suma_boln_for_per,double *esv_s_lgot,double *esv_s_lgotb,int metka_zap,const char *shetvud,FILE *ff_prot,GtkWidget *wpredok);
void  zarkvrt_zapvtabl(const char *imatmptab,const char *inn,int kod_doh,const char *kod_lgoti,double dohod_nah,double dohod_vip,double podoh_nah,double podoh_vip,const char *datanr,const char *datakr,const char *fio,const char *tab_kontr,FILE *ff_prot,GtkWidget *wpredok);

/*Платёжные документы*/

int l_xdkop(int metka,iceb_u_str *kod,iceb_u_str *naimk,const char *tablica,GtkWidget *wpredok);
int l_xdkop_v(iceb_u_str *kodzap,char *tablica,GtkWidget *wpredok);
int vzrkbzw(const char *kod_kontr,GtkWidget *wpredok);
void vzrkfw(GtkWidget *wpredok);
int readpdokw(const char *tabl,short god,const char *npp,GtkWidget *wpredok);
double sumpzpdw(const char *tabl,short *dp,short *mp,short *gp,double *sumpp,GtkWidget *wpredok);
double sumprpdw(short d,short m,short g,const char *nn,int pod,const char *kto,const char *shet,GtkWidget *wpredok);
int xdkdok(const char *tablica,GtkWidget *wpredok);
void provpdw(char tabl[],GtkWidget *wpredok);
int  nomdokw(short god,const char *tabl,GtkWidget *wpredok);
void	readkomw(const char *tabl,short dd,short md,short gd,const char *nomdk,class iceb_u_str *koment,GtkWidget *wpredok);
GtkWidget *xkdvnp(const char *naz_pl,GtkTextBuffer **buffer);
void xdkvnp_rt(class iceb_u_str *text,GtkTextBuffer *buffer);

/*Учёт командировочных расходов*/
int   l_ukrgrup(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_ukrzat(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int l_ukrvk(int metka_rr,class iceb_u_str *kod,class iceb_u_str *naim,GtkWidget *wpredok);
int ukrvhd(class iceb_u_str *datadok,class iceb_u_str *nomdok,GtkWidget *wpredok);
void l_ukrspdok(short ds,short ms,short gs,GtkWidget *wpredok);
int l_ukrdok(const char *data_dok,const char *nomd,GtkWidget *wpredok);
int l_ukrdok_pb(const char *datadok,GtkWidget *wpredok);
int l_ukrdok_pb(short,short,GtkWidget *wpredok);
int l_ukrdok_v(short dd,short md,short gd,const char *nomdok,const char *shet_zap,int kod,int nomzap,GtkWidget *wpredok);
void prosprkrw(short dd,short md,short gd,const char *nomdok,int metkareg,class iceb_u_str *shet_sum,GtkWidget *wpredok);


/*Учёт путевых листов*/
int   l_uplpod(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_uplmt(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_uplout(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,int metka_pok,GtkWidget *wpredok);
int   l_uplavt(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_uplgo(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_uplost(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
void  l_uplsal(GtkWidget *wpredok);
void  l_uplscnt(GtkWidget *wpredok);
int l_upldok_pb(short m,short g,GtkWidget *wpredok);
int l_upldok_pb(const char *datadok,GtkWidget *wpredok);
int l_upldok(const char *data_dok,const char *nomd,const char *kod_pod,GtkWidget *wpredok);
int l_upldok_v(class iceb_u_str *data_dok,class iceb_u_str *kod_pod,class iceb_u_str *nomdok,GtkWidget *wpredok);
double	ostvaw(short dos,short mos,short gos,const char *kodvod,const char *kodavt,int nom_zap,iceb_u_spisok *KTC,iceb_u_double *OST,GtkWidget *wpredok);
double	ostvaw(const char *data_ost,const char *kodvod,const char *kodavt,int nom_zap,iceb_u_spisok *KTC,iceb_u_double *OST,GtkWidget *wpredok);
int upl_zapvdokw(short dd,short md,short gd,int kp,const char *nomd,int ka,int kv,int kodsp,int tz,const char *kodtp,double kolih,const char *kom,int kap,int nz,GtkWidget *wpredok);
void provpstw(short dd,short md,short gd,const char *nomd,int podr,GtkWidget *wpredok);
void spst(short dd,short md,short gd,const char *nomd,int podr,class iceb_u_spisok *KOT,class iceb_u_double *KOL,GtkWidget *wpredok);

/*Реестр налоговых накладных*/
int   l_xrnngpn(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int   l_xrnngvn(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
char *filtr_xmlw(const char *stroka);
int xrnn_kopd(int metka,class iceb_u_str *kodop,GtkWidget *wpredok);
int xrnn_kord(int metka,class iceb_u_str *kodop,GtkWidget *wpredok);
int zapvree1w(short dd,short md,short gd,const char *nnn,double sumd1,double snds1,double sumd2,double snds2,double sumd3,double snds3,double sumd4,double snds4,const char *inn,const char *nkontr,int mi,const char *datdi,const char *nomdi,const char *koment,const char *kodgr,const char *kodop,short dv,short mv,short gv,const char *viddok,GtkWidget *wpredok);
int zapvreew(short dd,short md,short gd,const char *nnn,double sumd,double snds,int mt,const char *inn,const char *nkontr,int mi,const char *datdi,const char *nomdi,const char *koment,const char *kodgr,const char *kodop,const char *viddok,const char *datv,GtkWidget *wpredok);
int rpvnn_psw(short dn,short mn,short gn,short dk,short mk,short gk,FILE *ff,GtkWidget*);
void rpvnn_ps_pw(FILE *ff_prot);
int xml_nn_matw(int *pnd,int tipz,short dd,short md,short gd,const char *nomdok,const char *nom_nal_nak,short mn,short gn,short mk,int period_type,class iceb_u_str *imafil_xml,GtkWidget *wpredok);
int xml_nn_uslw(int *pnd,int tipz,short dd,short md,short gd,const char *nomdok,const char *nom_nal_nak,short mn,short gn,short mk,int period_type,class iceb_u_str *imafil_xml,GtkWidget *wpredok);
int xml_nn_uosw(int *pnd,int tipz,short dd,short md,short gd,const char *nomdok,const char *nom_nal_nak,short mn,short gn,short mk,int period_type,class iceb_u_str *imafil_xml,GtkWidget *wpredok);
int xml_nn_ukrw(int *pnd,short dd,short md,short gd,const char *nomdok,const char *nom_nal_nak,short mn,short gn,short mk,int period_type,class iceb_u_str *imafil_xml,GtkWidget *wpredok);
int open_fil_nn_xmlw(char *imaf_xml,int pnd,int tipz,short mn,short gn,char *imaf,class iceb_rnfxml_data *rek_xml,FILE **ff_xml,GtkWidget *wpredok);
void zag_nn_xmlw(int tipz,const char *nomdok,short dnp,short mnp,short gnp,short dd,short md,short gd,const char *kontr,const char *uslov_prod,const char *forma_rash,FILE *ff_xml,GtkWidget*);
void str_nn_xmlw(int nomer,int metka_nds,const char *datop,double kolih,double cena,double suma,const char *ei,const char *naim_mat,FILE *ff_xml);
void str_nn_xmlw(int nomer,int metka_nds,const char *datop,char *kolih,double cena,double suma,const char *ei,const char *naim_mat,FILE *ff_xml);
void end_nn_xmlw(int metka_nds,double isuma_bnds,double sumkor,double suma_voz_tar,double suma_nds_v,double procent_nds,const char *kodop,const char *imaf_nastr,FILE *ff_xml);
int xrnn_rasp_r(class xrnn_poiw *datark,GtkWidget *wpredok);
int rnn_imp(int metka,GtkWidget *wpredok);
int rnnvvd(class iceb_u_str *viddok,GtkWidget *wpredok);
int rnnovdw(int metka,class rnnovd_rek *nvd,GtkWidget *wpredok);
void rnn_imp_ovd(class iceb_u_str *viddok,const char *kodop,class iceb_u_spisok *spvd,class iceb_u_spisok *spko);
int rnnvdw(int tipz,const char *viddok);

/*****Учёт доверенностей********************/

int udvhdw(class iceb_u_str *datadov,class iceb_u_str *nomerdov,GtkWidget *wpredok);
int l_dover(const char *datadov,const char *nomerdov,GtkWidget *wpredok);

/*****************Администрирование************************/
int prov_uduserw(const char *user,const char *imabazp,int metka_soob,GtkWidget *wpredok);
int admin_bases(int metka_r,class iceb_u_str *vimabaz,GtkWidget *wpredok);
int admin_provbaz(const char *imapb,GtkWidget *wpredok);
