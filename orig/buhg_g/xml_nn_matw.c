/*$Id: xml_nn_matw.c,v 1.7 2011-08-29 07:13:44 sasa Exp $*/
/*23.07.2011	12.10.2006	Белых А.И.	xml_nn_matw.c
Выгруз в файл налоговой накладной в xml формате из подсистемы "Материальный учёт".
Если вернули 0- документ найден
             1- не найден
*/
#include <stdlib.h>
#include "buhg_g.h"

extern class iceb_rnfxml_data rek_zag_nn;

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int xml_nn_matw(int *pnd, //порядковый номер документа
int tipz, //1-приходный документ 2-расходный
short dd,short md,short gd, //Дата документа
const char *nomdok,                //Номер документа
const char *nom_nal_nak,           //Номер налоговой накладной
short mn,short gn,          //Дата начала поиска
short mk,                   //Месяц конца периода
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
int sklad=0;
class iceb_u_str kodop("");
//читаем шапку документа
sprintf(strsql,"select sklad,kontr,pn,kodop from Dokummat where datd='%04d-%02d-%02d' and nomd='%s' and nomnn='%s' and tip=%d",
gd,md,dd,nomdok,nom_nal_nak,tipz);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  class iceb_u_str repl;
  repl.plus(gettext("Материальный учёт"));
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

sklad=atoi(row[0]);
strncpy(kontr,row[1],sizeof(kontr)-1);
procent_nds=atof(row[2]);
kodop.new_plus(row[3]);

char uslov_prod[512];
char forma_rash[512];
memset(uslov_prod,'\0',sizeof(uslov_prod));
memset(forma_rash,'\0',sizeof(forma_rash));


char kod_form_opl[30];
memset(kod_form_opl,'\0',sizeof(kod_form_opl));

int metka_nds=0;
double sumkor=0.;

char		datop[64];
memset(datop,'\0',sizeof(datop));

int kolstr=0;

sprintf(strsql,"select nomerz,sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d",
gd,nomdok,sklad);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
double suma_nds_v=0.;//Сумма ндс введённая вручную для приходных документов
int nomer=0;
short dnp=0,mnp=0,gnp=0; //Дата накладной поставщика - для приходных документов
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  nomer=atoi(row[0]);

  if(nomer == 6)
   if(tipz == 1)
    suma_nds_v=atof(row[1]);
  if(nomer == 7)
     strncpy(uslov_prod,row[1],sizeof(uslov_prod)-1);
  if(nomer == 8)
    strncpy(kod_form_opl,row[1],sizeof(kod_form_opl)-1);
  if(nomer == 9)
   {
    short d,m,g;
    iceb_u_rsdat(&d,&m,&g,row[1],0);
    if(d != 0 && iceb_u_sravmydat(d,m,g,dd,md,gd) < 0)
      sprintf(datop,"%02d%02d%04d",d,m,g);
   }
  if(nomer == 11)
    metka_nds=atoi(row[1]);

  if(nomer == 13)
   sumkor=atof(row[1]);  
  if(nomer == 14)
    iceb_u_rsdat(&dnp,&mnp,&gnp,row[1],2);
 }

if(datop[0] == '\0')
  sprintf(datop,"%02d%02d%04d",dd,md,gd);
 
if(kod_form_opl[0] != '\0')
 {
  //читаем наименование формы оплаты
  sprintf(strsql,"select naik from Foroplat where kod='%s'",kod_form_opl);
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


//читаем записи в документе
sprintf(strsql,"select kodm,kolih,cena,ei,dnaim,voztar from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,sklad,nomdok);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
nomer=0;
char naim_mat[512];
double kolih;
double cena;
double suma;
double isuma_bnds=0.;
double suma_voz_tar=0.;
int kod_grup=0;
while(cur.read_cursor(&row) != 0)
 {
  nomer++;


  kod_grup=0;
  memset(naim_mat,'\0',sizeof(naim_mat));
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    kod_grup=atoi(row1[0]);
    strncpy(naim_mat,row1[1],sizeof(naim_mat)-1);
   }

  if(row[4][0] != '\0')
   {
    strcat(naim_mat," ");
    strcat(naim_mat,row[4]);
   }



  kolih=atof(row[1]);
  cena=atof(row[2]);
  cena=iceb_u_okrug(cena,okrcn);

  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  suma=iceb_u_okrug(suma,okrg1);

  if(kod_grup == rek_zag_nn.kod_gr_voz_tar || atoi(row[5]) == 1)
   {
    suma_voz_tar+=suma;
    continue;
   }

  isuma_bnds+=suma;

  //вывод строки в файл
  str_nn_xmlw(nomer,metka_nds,datop,kolih,cena,suma,row[3],naim_mat,ff_xml);
    

 }


sprintf(strsql,"select ei,kolih,cena,naimu,ku from Dokummat3 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,sklad,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

while(cur.read_cursor(&row) != 0)
 {
  nomer++;
  kolih=atof(row[1]);
  cena=atof(row[2]);
  cena=iceb_u_okrug(cena,okrcn);

  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  suma=iceb_u_okrug(suma,okrg1);

  isuma_bnds+=suma;

  memset(naim_mat,'\0',sizeof(naim_mat));
  if(atoi(row[4]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[4]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     strncpy(naim_mat,row1[0],sizeof(naim_mat)-1);
   }
  if(row[3][0] != '\0')
   {
    if(naim_mat[0] != '\0')
     strcat(naim_mat," ");
    strcat(naim_mat,row[4]);
   }
  //вывод строки в файл
  str_nn_xmlw(nomer,metka_nds,datop,kolih,cena,suma,row[0],naim_mat,ff_xml);

 }

//концовка файла
end_nn_xmlw(metka_nds,isuma_bnds,sumkor,suma_voz_tar,suma_nds_v,procent_nds,kodop.ravno(),"matnast.alx",ff_xml);


fclose(ff_xml);

return(0);

}

