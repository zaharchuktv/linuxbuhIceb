/*$Id: i_xud.c,v 1.15 2011-02-21 07:35:51 sasa Exp $*/
/*06.09.2010	07.04.2009	Белых А.И.	i_xud.c
Учёт доверенностей
*/

#define DVERSIQ "06.09.2010"

#include        "buhg_g.h"

void i_xud_rabdok();
int l_dovers(short dp,short mp,short gp,GtkWidget *wpredok);
void poixuddokw(GtkWidget *wpredok);

extern SQL_baza	bd;
extern char		*organ;
extern char		*host ;
const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern char		*parol;
extern iceb_u_str shrift_rek_raz;
extern iceb_u_str iceb_menu_mv_str1;
extern iceb_u_str iceb_menu_mv_str2;
extern int iceb_kod_podsystem; /*Объявлен в iceb_l_blok.c*/

int main(int argc,char **argv)
{
int		kom=0;
iceb_kod_podsystem=ICEB_PS_UD;
int nom_op=0;
class iceb_u_str fioop("");

iceb_start(argc,argv);

if(iceb_nastsys() != 0)
 iceb_exit(1);

if(iceb_perzap(1,NULL))
 iceb_exit(1);
nom_op=iceb_getuid(NULL);
fioop.new_plus(iceb_getfioop(NULL));

iceb_menu_mv_str1.new_plus(organ);
iceb_menu_mv_str2.new_plus(gettext("Учёт доверенностей"));

class iceb_u_spisok menu_str;
menu_str.plus(gettext("Работа с документами"));
menu_str.plus(gettext("Работа с файлом настройки"));
menu_str.plus(gettext("Работа с файлами распечаток"));
menu_str.plus(gettext("Выбор шрифта"));

while( kom >= 0 )   
 {

  kom=iceb_menu_start(kom,version,DVERSIQ,gettext("Учёт доверенностей"),fioop.ravno(),nom_op,&menu_str,"white");
  switch(kom)
   {
    case 0:
      if(iceb_smenabaz(1,NULL) == 0)
       {
        if(iceb_nastsys() != 0)
         iceb_exit(1);
       }
      break;

    case 1:
      i_xud_rabdok();
      break;

    case 2:
      iceb_f_redfil("nastud.alx",0,NULL);
      break;

    case 3:
      iceb_l_rsfr(NULL);
      break;

    case 4:
      iceb_font_selection(NULL);
      break;


  }
 }

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}



/****************************/
/*Работа с документами*/
/****************************/

void i_xud_rabdok()
{
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);


iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;
char strsql[512];
int nomer=0;
titl.plus(gettext("Работа с документами"));

//zagolovok.new_plus(organ);
zagolovok.plus(gettext("Работа с документами"));

punkt_m.plus(gettext("Ввод нового документа"));//0
punkt_m.plus(gettext("Поиск введённого документа"));//1
punkt_m.plus(gettext("Просмотр всех введённых документов"));//2

sprintf(strsql,"%s %02d.%02d.%d",gettext("Просмотр документов с"),dt,mt,gt);
punkt_m.plus(strsql); //3
punkt_m.plus(gettext("Устанавливать/снимать блокировку дат"));//4


while(nomer >= 0)
 {
  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

  iceb_u_str datad("");
  iceb_u_str nomdok("");
  switch (nomer)
   {
    case -1:
      break;

    case 0:
      if(udvhdw(&datad,&nomdok,NULL) == 0)
       l_dover(datad.ravno(),nomdok.ravno(),NULL);
      break;

    case 1:
      poixuddokw(NULL);
      break;

    case 2:
      l_dovers(0,0,0,NULL);
      break;

    case 3:
      l_dovers(dt,mt,gt,NULL);
      break;

    case 4:
//      iceb_f_redfil("dovblok.alx",0,NULL);
      iceb_l_blok(NULL);
      break;


   }
 }
}


