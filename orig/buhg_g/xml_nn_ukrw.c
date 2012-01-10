/*$Id: xml_nn_ukrw.c,v 1.6 2011-08-29 07:13:44 sasa Exp $*/
/*22.07.2011	16.10.2006	Белых А.И.	xml_nn_ukrw.c
Выгрузка налоговых накладных для подсистемы "Учёт командировочных расходов".
*/
#include <stdlib.h>
#include "buhg_g.h"

extern class iceb_rnfxml_data rek_zag_nn;
extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int xml_nn_ukrw(int *pnd, //порядковый номер документа
short dd,short md,short gd, //Дата документа
const char *nomdok,                //Номер документа
const char *nom_nal_nak,           //Номер налоговой накладной
short mn,short gn,          //Дата начала поиска
short mk,
int period_type,
class iceb_u_str *imafil_xml,
GtkWidget *wpredok)
{
int tipz=1; //1-приходный документ 2-расходный

char strsql[512];
SQL_str row;
class SQLCURSOR cur,cur1;
double procent_nds=0.;
char kontr[32];
memset(kontr,'\0',sizeof(kontr));
//может быть в документе несколько записей с одинаковым номером налоговой накладной - используем цену для однозначного определения
//Читаем запись в документе
sprintf(strsql,"select kodr,kolih,cena,ediz,snds,kdrnn from Ukrdok1 where datd='%04d-%02d-%02d' and \
ndrnn='%s' and nomd='%s'",
gd,md,dd,nom_nal_nak,nomdok);

//читаем шапку документа
//sprintf(strsql,"select kontr from Ukrdok where datd='%04d-%02d-%02d' and nomd='%s'",gd,md,dd,nomdok);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {

  class iceb_u_str repl;
  repl.plus(gettext("Учёт командировочных расходов"));
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

strncpy(kontr,row[5],sizeof(kontr)-1);
double suma_nds_v=atof(row[4]);
int metka_nds=0;
if(suma_nds_v == 0.)
 metka_nds=3;
 
char eiz[32];
strncpy(eiz,row[3],sizeof(eiz)-1);

double kolih=atof(row[1]);
double cena=atof(row[2]);
double suma=cena;
double isuma_bnds=suma;

char datop[64];
memset(datop,'\0',sizeof(datop));
sprintf(datop,"%02d%02d%04d",dd,md,gd);

char uslov_prod[512];
memset(uslov_prod,'\0',sizeof(uslov_prod));

double sumkor=0.;

short dnp=dd,mnp=md,gnp=gd;


procent_nds=0.;

char forma_rash[512];
memset(forma_rash,'\0',sizeof(forma_rash));

//читаем наименование
char naim_mat[200];
sprintf(strsql,"select naim from Ukrkras where kod=%s",row[0]);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 strncpy(naim_mat,row[0],sizeof(naim_mat)-1);


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






int nomer=1;
double suma_voz_tar=0.;

//вывод строки в файл
str_nn_xmlw(nomer,metka_nds,datop,kolih,cena,suma,eiz,naim_mat,ff_xml);


//концовка файла
end_nn_xmlw(metka_nds,isuma_bnds,sumkor,suma_voz_tar,suma_nds_v,procent_nds,"","",ff_xml);


fclose(ff_xml);

return(0);
}
