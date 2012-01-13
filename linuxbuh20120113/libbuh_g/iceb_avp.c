/*$Id: iceb_avp.c,v 1.13 2011-09-05 08:18:29 sasa Exp $*/
/*09.12.2010	14.04.2009	Белых А.И.	iceb_avp.c
Унивесальная подпрограмма выполнения проводок
*/
#include <errno.h>
#include <unistd.h>
#include "iceb_libbuh.h"
void iceb_avp_vp(class iceb_avp *avp,const char *snast,class iceb_u_spisok *sp_prov,class iceb_u_double *sum_prov_dk,FILE *ff_prot,GtkWidget*);

extern SQL_baza bd;

void iceb_avp(class iceb_avp *avp,const char *imaf_nast,
class iceb_u_spisok *sp_prov,
class iceb_u_double *sum_prov_dk,
FILE *ff_prot,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
class iceb_u_str kodstr("");
class iceb_u_str kodop("");
class iceb_u_str shetuh("");
class iceb_u_str shetsp("");
if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\niceb_avp-\n\
Сумма с НДС=%.2f\n\
Сумма без НДС=%.2f\n\
Сумма НДС=%.2f\n\
Сумма учета=%.2f\n\
Сумма НДС введённая вручную=%.2f\n\
Метка НДС=%d\n\
Счёт учёта=%s\n\
Счёт списания=%s\n",
  avp->suma_s_nds,
  avp->suma_bez_nds,
  avp->suma_nds,
  avp->suma_uheta,
  avp->suma_nds_vv,
  avp->metka_snds,
  avp->shetuh.ravno(),
  avp->shetsp.ravno());
 }

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nast);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки %s\n",imaf_nast);
  return;
 }

int metka_vp=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(strpbrk(row_alx[0],"|") == NULL) /*есть ли знак "-" в строке*/
   continue;

  if(metka_vp == 0)
   {
    if(iceb_u_polen(row_alx[0],&kodstr,1,'|') != 0)
     continue;

    if(iceb_u_SRAV("START",kodstr.ravno(),0) != 0)
      continue;

    iceb_u_polen(row_alx[0],&kodop,2,'|');
    iceb_u_polen(row_alx[0],&shetuh,3,'|');
    iceb_u_polen(row_alx[0],&shetsp,4,'|');

    if(iceb_u_SRAV(avp->kodop.ravno(),kodop.ravno(),0) != 0)
     continue;

    if(iceb_u_SRAV(avp->shetuh.ravno(),shetuh.ravno(),0) != 0)
     continue;

    if(avp->shetsp.getdlinna() > 1 && shetsp.getdlinna() > 1)
     if(iceb_u_SRAV(avp->shetsp.ravno(),shetsp.ravno(),0) != 0)
      {
       if(ff_prot != NULL)
        fprintf(ff_prot,"Не подходит по счёту списания %s != %s\n",avp->shetsp.ravno(),shetsp.ravno());
       continue;
      }
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s",row_alx[0]);
    metka_vp=1;
    continue;
   }
  
  if(iceb_u_SRAV("END",row_alx[0],1) == 0)
     break;
  if(iceb_u_SRAV("START",row_alx[0],1) == 0)
     break;

  if(ff_prot != NULL)
     fprintf(ff_prot,"%s",row_alx[0]);

  iceb_avp_vp(avp,row_alx[0],sp_prov,sum_prov_dk,ff_prot,wpredok);

 }
unlink(imaf_nast);

}

/*********************/
/*Проверка счёта и кода контрагента*/
/*****************/
int iceb_avp_provsh(const char *shet,const char *kontr_v_dok,class iceb_u_str *shetv,class iceb_u_str *kontrv,FILE *ff_prot,
GtkWidget *wpredok)
{
class iceb_u_str kontrp("");
class iceb_u_str shetp("");


if(ff_prot != NULL)
 fprintf(ff_prot,"Проверка счёта и кода контрагента=%s\n",shet);

kontrv->new_plus("");
shetv->new_plus("");

if(iceb_u_polen(shet,&shetp,1,':') != 0)
 shetp.new_plus(shet);

if(iceb_u_polen(shet,&kontrp,2,':') == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Задан код контрагента=%s\n",kontrp.ravno());
 }

class OPSHET shet_op;

if(kontrp.ravno()[0] == '\0')
 kontrp.new_plus(kontr_v_dok);
if(ff_prot != NULL)
 fprintf(ff_prot,"Проверяем Счёт=%s Контрагент=%s\n",shetp.ravno(),kontrp.ravno());

if(iceb_provsh(&kontrp,shetp.ravno(),&shet_op,2,0,wpredok) != 0)
 {
  return(1);
 }

kontrv->new_plus(kontrp.ravno());
shetv->new_plus(shetp.ravno());
             
if(ff_prot != NULL)
 fprintf(ff_prot,"Счёт=%s Контрагент=%s\n",shetv->ravno(),kontrv->ravno());

return(0);
}
/******************************/
/*Разборка списка счетов*/
/**************************/

int iceb_avp_rsh(const char *shet,const char *kontr_v_dok,class iceb_u_str *shetv,class iceb_u_str *kontr,
class iceb_avp *avp,
FILE *ff_prot,
GtkWidget *wpredok)
{
kontr->new_plus("");
shetv->new_plus("");

int kolp=iceb_u_pole2(shet,',');

if(kolp == 0)
 {
  return(iceb_avp_provsh(shet,kontr_v_dok,shetv,kontr,ff_prot,wpredok));
  
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"Составляем список счетов=%s\n",shet);
 
char bros[512];
class iceb_u_spisok spsh;
class iceb_u_spisok naimsh;
for(int ii=0; ii < kolp; ii++)
 {
  if(iceb_u_polen(shet,bros,sizeof(bros),ii+1,',') != 0)
   break;
  if(bros[0] == '\0')
   break;
  spsh.plus(bros);
  naimsh.plus("");
 }
class iceb_u_str shet_menu("");
class iceb_u_str kontr_menu("");
int nomer=0;

if((nomer=avp->vib_shets.find(shet)) >= 0)
 {

  if(iceb_u_polen(avp->vib_shet.ravno(nomer),&shet_menu,1,':') != 0)
   shet_menu.new_plus(avp->vib_shet.ravno(nomer));
  else
   iceb_u_polen(avp->vib_shet.ravno(nomer),&kontr_menu,2,':');

 }
else
 {

  char zagolov[512];
  class iceb_u_spisok naim_kol;
  class iceb_u_str naim_menu;

  sprintf(zagolov,"%s:%s %s",
  gettext("Счёт учёта"),
  avp->shetuh.ravno(),
  gettext("Выберите нужный счёт"));

  naim_kol.plus(gettext("Счёт"));
  naim_kol.plus(gettext("Наименование"));
  naim_menu.plus(gettext("Виберіть рахунок"));
  //int voz=dirmasiv(&spsh, -1,-1, 0,zagolov,1,&kod_kl);
  int voz=0;
  if((voz=iceb_l_spisokm(&spsh,&naimsh,&naim_kol,&naim_menu,0,wpredok)) < 0)
   return(-1);



  if(iceb_u_polen(spsh.ravno(voz),&shet_menu,1,':') != 0)
   shet_menu.new_plus(spsh.ravno(voz));
  else
   iceb_u_polen(spsh.ravno(voz),&kontr_menu,2,':');
 }
 
if(kontr_menu.getdlinna() <= 1)
 kontr_menu.new_plus(kontr_v_dok);

return(iceb_avp_provsh(shet_menu.ravno(),kontr_menu.ravno(),shetv,kontr,ff_prot,wpredok));

}
/****************************/
/*               */
/***************************/
void iceb_avp_vp(class iceb_avp *avp,const char *snast,
class iceb_u_spisok *sp_prov,
class iceb_u_double *sum_prov_dk,
FILE *ff_prot,
GtkWidget *wpredok)
{
class iceb_u_str shet("");
class iceb_u_str shetkor("");
class iceb_u_str sumaprov("");
class iceb_u_str kekv("");
class iceb_u_str koment("");
class iceb_u_str kontr_shet("");
class iceb_u_str kontr_shetkor("");
class iceb_u_str nast_shet("");
class iceb_u_str nast_shetkor("");

if(ff_prot != NULL)
 fprintf(ff_prot,"Разбираем строку=%s\n",snast);
double deb=0.;
double kre=0.;

if(iceb_u_polen(snast,&nast_shet,1,'|') != 0)
 return;

if(iceb_u_polen(snast,&nast_shetkor,2,'|') != 0)
 return;


if(iceb_u_polen(snast,&sumaprov,3,'|') != 0)
 return;


iceb_u_polen(snast,&kekv,4,'|');

iceb_u_polen(snast,&koment,5,'|');
if(iceb_u_strstrm(koment.ravno(),"NOMER_DOK") == 1) /*есть ли в строке*/
 {
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"Подстваляем в комментарий номер документа=%s\n",avp->nomer_dok.ravno());
    if(avp->nomer_dok.getdlinna() <= 1)
     fprintf(ff_prot,"В шапку документа не введено номер документа поставщика!!!!\n");
   }
  iceb_u_zvstr(&koment,"NOMER_DOK",avp->nomer_dok.ravno());
 }
if(iceb_u_strstrm(koment.ravno(),"KOD_KONTRAGENTA") == 1) /*есть ли в строке*/
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Подстваляем в комментарий номер документа=%s\n",avp->kontr_v_dok.ravno());
  iceb_u_zvstr(&koment,"KOD_KONTRAGENTA",avp->kontr_v_dok.ravno());
 }

int znak=1;
int metka_is=0; /*Метка идентефикатора суммы*/
if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_S_NDS") == 1)
 {
  deb=avp->suma_s_nds;
  metka_is=1;
 }

if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_BEZ_NDS") == 1)
 {
  deb=avp->suma_bez_nds;
  metka_is=1;
 }

if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_IZNOSA") == 1)
 {
  deb=avp->suma_bez_nds;
  metka_is=1;
 }

if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_NDS") == 1)
 {
  if(avp->suma_nds_vv != 0.)
   {
    if(avp->metka_snds == 0)
     deb=avp->suma_nds_vv;
    avp->metka_snds=1;
   }
  else
   deb=avp->suma_nds;
  metka_is=1;
 }
if(iceb_u_strstrm(sumaprov.ravno(),"SUMA_UHETA") == 1)
 {
  deb=avp->suma_uheta;
  metka_is=1;
 }

if(metka_is == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Неправильный идентефикатор суммы! %s\n",sumaprov.ravno());
 }

if(strpbrk(sumaprov.ravno(),"-") != NULL) /*есть ли знак "-" в строке*/
  znak=-1;

deb*=znak;

if(iceb_avp_rsh(nast_shet.ravno(),avp->kontr_v_dok.ravno(),&shet,&kontr_shet,avp,ff_prot,wpredok) != 0)
 return;
if(iceb_avp_rsh(nast_shetkor.ravno(),avp->kontr_v_dok.ravno(),&shetkor,&kontr_shetkor,avp,ff_prot,wpredok) != 0)
 return;


iceb_zapmpr(avp->gp,avp->mp,avp->dp,shet.ravno(),shetkor.ravno(),
kontr_shet.ravno(),kontr_shetkor.ravno(),deb,kre,koment.ravno(),2,kekv.ravno_atoi(),sp_prov,sum_prov_dk,ff_prot);

}
