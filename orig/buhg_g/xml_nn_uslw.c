/*$Id: xml_nn_uslw.c,v 1.7 2011-08-29 07:13:44 sasa Exp $*/
/*23.07.2011	13.10.2006	Белых А.И.	xml_nn_uslw.c
Выгрузка в xml файл налоговой накладной из "Учёта услуг"
*/
#include <stdlib.h>
#include "buhg_g.h"

extern class iceb_rnfxml_data rek_zag_nn;
extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int xml_nn_uslw(int *pnd, //порядковый номер документа
int tipz, //1-приходный документ 2-расходный
short dd,short md,short gd, //Дата документа
const char *nomdok,                //Номер документа
const char *nom_nal_nak,           //Номер налоговой накладной
short mn,short gn,          //Дата начала поиска
short mk,
int period_type,
class iceb_u_str *imafil_xml,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
double procent_nds=0.;
char kontr[32];
memset(kontr,'\0',sizeof(kontr));
int podr=0;
class iceb_u_str kodop("");
//читаем шапку документа
sprintf(strsql,"select podr,kontr,nds,forop,datop,uslpr,sumkor,datdp,sumnds,pn,kodop from Usldokum where datd='%04d-%02d-%02d' and nomd='%s' and nomnn='%s' and tp=%d",
gd,md,dd,nomdok,nom_nal_nak,tipz);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  class iceb_u_str repl;
  repl.plus(gettext("Учёт услуг"));
  repl.ps_plus(gettext("Ненайден документ"));
  repl.plus("!");
  

  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%s",
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер"),
  nomdok,
  gettext("Налоговый номер"),
  nom_nal_nak);

  repl.ps_plus(strsql);
  
  iceb_menu_soob(&repl,wpredok);
  
  return(1);
 }

podr=atoi(row[0]);
strncpy(kontr,row[1],sizeof(kontr)-1);
int metka_nds=atoi(row[2]);

char kod_forop[512];
memset(kod_forop,'\0',sizeof(kod_forop));
strncpy(kod_forop,row[3],sizeof(kod_forop)-1);

char datop[64];
memset(datop,'\0',sizeof(datop));
short dop,mop,gop;
iceb_u_rsdat(&dop,&mop,&gop,row[4],2);
if(dop != 0)
 if(iceb_u_sravmydat(dop,mop,gop,dd,md,gd) < 0)
  sprintf(datop,"%02d%02d%d",dop,mop,gop);
if(datop[0] == '\0')
  sprintf(datop,"%02d%02d%d",dd,md,gd);

char uslov_prod[512];
memset(uslov_prod,'\0',sizeof(uslov_prod));
strncpy(uslov_prod,row[5],sizeof(uslov_prod)-1);

double sumkor=atof(row[6]);

short dnp,mnp,gnp;
iceb_u_rsdat(&dnp,&mnp,&gnp,row[7],2);
double suma_nds_v=0.;
if(tipz == 1)
  suma_nds_v=atof(row[8]);

procent_nds=atof(row[9]);
kodop.new_plus(row[10]);

char forma_rash[512];
memset(forma_rash,'\0',sizeof(forma_rash));

if(kod_forop[0] != '\0')
 {
  //читаем наименование формы оплаты
  sprintf(strsql,"select naik from Foroplat where kod='%s'",kod_forop);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(forma_rash,row[0],sizeof(forma_rash)-1);
 } 


FILE *ff_xml;
char imaf_xml[512];
char imaf[64]; 

*pnd+=1;

//открываем файл
if(open_fil_nn_xmlw(imaf_xml,*pnd,tipz,mn,gn,imaf,&rek_zag_nn,&ff_xml,wpredok) != 0)
 return(1);

imafil_xml->new_plus(imaf_xml);

//Заголовок файла
rnn_sap_xmlw(mn,gn,mk,"J12","010",3,*pnd,imaf,period_type,&rek_zag_nn,ff_xml);

//Заголовок документа
zag_nn_xmlw(tipz,nom_nal_nak,dnp,mnp,gnp,dd,md,gd,kontr,uslov_prod,forma_rash,ff_xml,wpredok);






int kolstr=0;
//читаем записи в документе
sprintf(strsql,"select metka,kodzap,kolih,cena,ei,dnaim from Usldokum1 where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
class iceb_u_str kod_ei_usl;
iceb_poldan("Код единицы измерения услуги",&kod_ei_usl,"uslnast.alx",wpredok);
int nomer=0;
char naim_mat[512];
double kolih;
double cena;
double suma;
double isuma_bnds=0.;
double suma_voz_tar=0.;
int kod_grup=0;
int metka_zap;
while(cur.read_cursor(&row) != 0)
 {
  nomer++;
  metka_zap=atoi(row[0]);

  kod_grup=0;
  memset(naim_mat,'\0',sizeof(naim_mat));
  if(metka_zap == 0)
   {
    sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[1]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     {
      kod_grup=atoi(row1[0]);
      strncpy(naim_mat,row1[1],sizeof(naim_mat)-1);
     }
   }

  if(metka_zap == 1)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[1]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
      strncpy(naim_mat,row1[0],sizeof(naim_mat)-1);
   }

  if(row[5][0] != '\0')
   {
    strcat(naim_mat," ");
    strcat(naim_mat,row[5]);
   }



  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,okrcn);

  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  suma=iceb_u_okrug(suma,okrg1);
  
  if(metka_zap == 0)
  if(kod_grup == rek_zag_nn.kod_gr_voz_tar)
   {
    suma_voz_tar+=suma;
    continue;
   }

  isuma_bnds+=suma;
    
  //вывод строки в файл
  if(iceb_u_proverka(kod_ei_usl.ravno(),row[4],0,1) == 0 && kolih == 1.)
    str_nn_xmlw(nomer,metka_nds,datop,gettext("Услуга"),cena,suma,gettext("грн."),naim_mat,ff_xml);
  else    
    str_nn_xmlw(nomer,metka_nds,datop,kolih,cena,suma,row[4],naim_mat,ff_xml);


}
//концовка файла
end_nn_xmlw(metka_nds,isuma_bnds,sumkor,suma_voz_tar,suma_nds_v,procent_nds,kodop.ravno(),"uslnast.alx",ff_xml);


fclose(ff_xml);

return(0);


}

