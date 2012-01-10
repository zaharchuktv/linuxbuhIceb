/*$Id: xml_nn_uosw.c,v 1.7 2011-08-29 07:13:44 sasa Exp $*/
/*23.07.2011	13.10.2006	Белых А.И.	xml_nn_uosw.c
Выгрузка в xml файл налоговой накладной из "Учёта основных средств"
*/
#include <stdlib.h>
#include "buhg_g.h"

extern class iceb_rnfxml_data rek_zag_nn;
extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int xml_nn_uosw(int *pnd, //порядковый номер документа
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
sprintf(strsql,"select kontr,podr,forop,datpnn,datop,nds,pn,kodop from Uosdok where datd='%04d-%02d-%02d' and nomd='%s' and nomnn='%s' and tipz=%d",
gd,md,dd,nomdok,nom_nal_nak,tipz);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {

  class iceb_u_str repl;
  repl.plus(gettext("Учёт основных средств"));
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

podr=atoi(row[1]);
strncpy(kontr,row[0],sizeof(kontr)-1);
int metka_nds=atoi(row[5]);

char kod_forop[512];
memset(kod_forop,'\0',sizeof(kod_forop));
strncpy(kod_forop,row[2],sizeof(kod_forop)-1);

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
//strncpy(uslov_prod,row[5],sizeof(uslov_prod)-1);

double sumkor=0.;

short dnp,mnp,gnp;
iceb_u_rsdat(&dnp,&mnp,&gnp,row[3],2);

double suma_nds_v=0.;

procent_nds=atof(row[6]);
kodop.new_plus(row[7]);

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
sprintf(strsql,"select innom,bs,bsby,cena from Uosdok1 where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s'",gd,md,dd,podr,nomdok);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

int nomer=0;
char naim_mat[512];
double kolih;
double cena;
double suma;
double isuma_bnds=0.;
double suma_voz_tar=0.;
int metka_zap;
double bs=0.;
double bsby=0.;

while(cur.read_cursor(&row) != 0)
 {
  nomer++;
  metka_zap=atoi(row[0]);

  memset(naim_mat,'\0',sizeof(naim_mat));
  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
      strncpy(naim_mat,row1[0],sizeof(naim_mat)-1);

  

  bs=atof(row[1]);
  bsby=atof(row[2]);
  kolih=1.;

  if(tipz == 2)
    cena=atof(row[3]);
  else
   {
    if(bs != 0.)
     cena=bs;
    else
     cena=bsby;
   }

  cena=iceb_u_okrug(cena,okrcn);

  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  suma=iceb_u_okrug(suma,okrg1);


  isuma_bnds+=suma;
    
  //вывод строки в файл
  str_nn_xmlw(nomer,metka_nds,datop,kolih,cena,suma,"Шт",naim_mat,ff_xml);


}
//концовка файла
end_nn_xmlw(metka_nds,isuma_bnds,sumkor,suma_voz_tar,suma_nds_v,procent_nds,kodop.ravno(),"uosnast.alx",ff_xml);


fclose(ff_xml);

return(0);


 }
