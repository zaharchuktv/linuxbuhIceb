/*$Id: ost_skl_shetw_r.c,v 1.12 2011-02-21 07:35:55 sasa Exp $*/
/*14.11.2009	17.11.2008	Белых А.И.	ost_skl_shetw_r.c
Расчет отчета остатка 
*/
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "ost_skl_shetw.h"

class ost_skl_shetw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  class ost_skl_shetw_data *rk;
  
  int voz; //0-все в порядке расчёт сделан 1-нет
  ost_skl_shetw_r_data()
   {
    voz=1;
   }
 };

gboolean   ost_skl_shetw_r_key_press(GtkWidget *widget,GdkEventKey *event,class ost_skl_shetw_r_data *data);
gint ost_skl_shetw_r1(class ost_skl_shetw_r_data *data);
void  ost_skl_shetw_r_v_knopka(GtkWidget *widget,class ost_skl_shetw_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern short	vtara; /*Код группы возвратная тара*/
extern char     *organ; 
extern int      kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;

int ost_skl_shetw_r(class ost_skl_shetw_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class ost_skl_shetw_r_data data;
data.rk=rek_ras;



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт остатков по складам и счетам учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(ost_skl_shetw_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

GtkWidget *label=gtk_label_new(gettext("Расчёт остатков по складам и счетам учёта"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),450,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
//PangoFontDescription *font_pango=pango_font_description_from_string("Nimbus Mono L, Bold 12");
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_modify_font(GTK_WIDGET(data.view),font_pango);
pango_font_description_free(font_pango);

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(ost_skl_shetw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)ost_skl_shetw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ost_skl_shetw_r_v_knopka(GtkWidget *widget,class ost_skl_shetw_r_data *data)
{
//printf("ost_skl_shetw_r_v_knopka\n");
if(data->voz == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ost_skl_shetw_r_key_press(GtkWidget *widget,GdkEventKey *event,class ost_skl_shetw_r_data *data)
{
// printf("ost_skl_shetw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/********************************/
/*Строка подчёркивания*/
/********************************/
void ost_skl_shet_str(int kol,int metka,FILE *ff)
{
//Полка над наименованием контрагента
if(metka == 0)
 fprintf(ff,"---------------------------");
if(metka == 1)
 fprintf(ff,"-----------------------------");

for(int ii=0; ii < kol+1; ii++)
 fprintf(ff,"---------------------");
 //          123456789|1234567890|
fprintf(ff,"\n");
}


/**********************************/
/*Проверка на условия поиска*/
/*********************************/

int ost_skl_shet_prov(SQL_str row,
const char *sklad,
const char *grup_mat,
const char *kodmat,
const char *shetz,
GtkWidget *wpredok)
{

if(iceb_u_proverka(sklad,row[0],0,0) != 0)
  return(1);
if(iceb_u_proverka(kodmat,row[2],0,0) != 0)
  return(1);
if(iceb_u_proverka(shetz,row[3],0,0) != 0)
  return(1);
if(grup_mat[0] != '\0') //Узнаём группу материалла
 {
  SQL_str row1;
  SQLCURSOR cur1;
  char strsql[512];
  sprintf(strsql,"select kodgr from Material where kodm=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row[2]);
    iceb_menu_soob(strsql,wpredok);
    return(1);

   }
  if(iceb_u_proverka(grup_mat,row1[0],0,0) != 0)
   return(1);
 }     

return(0);
}

/*****************************/
/*Распечатка массивов по складам*/
/*****************************/
void ost_skl_shet_rm_skl(class iceb_u_int *sp_skl,
class iceb_u_spisok *sp_shetu,
class iceb_u_double *m_kolih,
class iceb_u_double *m_suma,
FILE *ff,
GtkWidget *wpredok)
{
char strsql[512];
char naim[512];
SQL_str row;
SQLCURSOR cur;

int kolih_shet=sp_shetu->kolih();
int kolih_skl=sp_skl->kolih();

ost_skl_shet_str(kolih_shet,0,ff);


fprintf(ff,"%-*.*s|%-*.*s|",
iceb_u_kolbait(5,gettext("Код")),
iceb_u_kolbait(5,gettext("Код")),
gettext("Код"),
iceb_u_kolbait(20,gettext("Склад")),
iceb_u_kolbait(20,gettext("Склад")),
gettext("Склад"));
//          12345 12345678901234567890
//Пишем первую строку со счетами
for(int ii=0; ii < kolih_shet; ii++)
 {
  memset(naim,'\0',sizeof(naim));
  //узнаём наименование 

  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shetu->ravno(ii));  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);  

  sprintf(strsql,"%s %s",sp_shetu->ravno(ii),row[0]);

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,strsql),iceb_u_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,gettext("Итого")),iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"     |                    |");

//строка разделения

for(int ii=0; ii < kolih_shet+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"     |                    |");


for(int ii=0; ii < kolih_shet+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_u_kolbait(9,gettext("Количество")),
  iceb_u_kolbait(9,gettext("Количество")),
  gettext("Количество"),
  iceb_u_kolbait(10,gettext("Сумма")),
  iceb_u_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }
fprintf(ff,"\n");

ost_skl_shet_str(kolih_shet,0,ff);

//Распечатываем массив
for(int s_skl=0; s_skl < kolih_skl; s_skl++)
 {
  memset(naim,'\0',sizeof(naim));
  //узнаём наименование 

  sprintf(strsql,"select naik from Sklad where kod=%d",sp_skl->ravno(s_skl));  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);  

  fprintf(ff,"%-5d|%-*.*s|",
  sp_skl->ravno(s_skl),
  iceb_u_kolbait(20,row[0]),iceb_u_kolbait(20,row[0]),row[0]);

  double itog_str_k=0.;
  double itog_str_s=0.;
  for(int s_shet=0; s_shet < kolih_shet; s_shet++)
   {
    double kolih=m_kolih->ravno(s_skl*kolih_shet+s_shet);
    double suma=m_suma->ravno(s_skl*kolih_shet+s_shet);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9.9g|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9.9g|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
   
 }

ost_skl_shet_str(kolih_shet,0,ff);

fprintf(ff,"%*s|",iceb_u_kolbait(26,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int ii=0; ii < kolih_shet; ii++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < kolih_skl; skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(skontr*kolih_shet+ii);
    itogo_po_kol_suma+=m_suma->ravno(skontr*kolih_shet+ii);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");

}

/*****************************/
/*Распечатка массивов по счетам*/
/*****************************/
void ost_skl_shet_rm_shet(class iceb_u_int *sp_skl,
class iceb_u_spisok *sp_shetu,
class iceb_u_double *m_kolih,
class iceb_u_double *m_suma,
FILE *ff,
GtkWidget *wpredok)
{
char strsql[512];
char naim[512];
SQL_str row;
SQLCURSOR cur;

int kolih_shet=sp_shetu->kolih();
int kolih_skl=sp_skl->kolih();

ost_skl_shet_str(kolih_skl,1,ff);


fprintf(ff,"%-*.*s|%-*.*s|",
iceb_u_kolbait(7,gettext("Счёт")),
iceb_u_kolbait(7,gettext("Счёт")),
gettext("Счёт"),
iceb_u_kolbait(20,gettext("Наименование")),
iceb_u_kolbait(20,gettext("Наименование")),
gettext("Наименование"));

//Пишем первую строку со счетами
for(int ii=0; ii < kolih_skl; ii++)
 {
  memset(naim,'\0',sizeof(naim));
  //узнаём наименование 
  sprintf(strsql,"select naik from Sklad where kod=%d",sp_skl->ravno(ii));  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);  

  sprintf(strsql,"%d %s",sp_skl->ravno(ii),row[0]);

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,strsql),iceb_u_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,gettext("Итого")),iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"       |                    |");

//строка разделения

for(int ii=0; ii < kolih_skl+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"       |                    |");


for(int ii=0; ii < kolih_skl+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_u_kolbait(9,gettext("Количество")),
  iceb_u_kolbait(9,gettext("Количество")),
  gettext("Количество"),
  iceb_u_kolbait(10,gettext("Сумма")),
  iceb_u_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }
fprintf(ff,"\n");

ost_skl_shet_str(kolih_skl,1,ff);

//Распечатываем массив
for(int s_shet=0; s_shet < kolih_shet; s_shet++)
 {
  memset(naim,'\0',sizeof(naim));
  //узнаём наименование 
  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shetu->ravno(s_shet));  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strncpy(naim,row[0],sizeof(naim)-1);  

  fprintf(ff,"%-*s|%-*.*s|",
  iceb_u_kolbait(7,sp_shetu->ravno(s_shet)),sp_shetu->ravno(s_shet),
  iceb_u_kolbait(20,row[0]),iceb_u_kolbait(20,row[0]),row[0]);

  double itog_str_k=0.;
  double itog_str_s=0.;

  for(int s_skl=0; s_skl < kolih_skl; s_skl++)
   {
    double kolih=m_kolih->ravno(s_skl*kolih_shet+s_shet);
    double suma=m_suma->ravno(s_skl*kolih_shet+s_shet);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9.9g|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9.9g|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
   
 }

ost_skl_shet_str(kolih_skl,1,ff);

fprintf(ff,"%*s|",iceb_u_kolbait(28,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int s_skl=0; s_skl < kolih_skl; s_skl++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int s_shet=0; s_shet < kolih_shet; s_shet++)
   {
                                     
    itogo_po_kol_kolih+=m_kolih->ravno(s_skl*kolih_shet+s_shet);
    itogo_po_kol_suma+=m_suma->ravno(s_skl*kolih_shet+s_shet);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");
}


/*******************************/
/*Печать реквизитов поиска*/
/****************************/
void ost_skl_shet_rekpoi(const char *sklad,
const char *grup_mat,
const char *kodmat,
const char *shetz,
FILE *ff)
{
if(sklad[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Склад"),sklad);
if(grup_mat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Группа"),grup_mat);
if(kodmat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код материалла"),kodmat);
if(shetz[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shetz);

}


/******************************************/
/******************************************/

gint ost_skl_shetw_r1(class ost_skl_shetw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_u_str repl;
class iceb_clock sss(data->window);


short dk,mk,gk;

if(iceb_u_rsdat(&dk,&mk,&gk,data->rk->data_ost.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


SQL_str row;
SQLCURSOR cur;
int kolstr=0;
float kolstr1=0;


sprintf(strsql,"select sklad,nomk,kodm,shetu,cena from Kart");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
FILE *fftmp;
char imaftmp[50];
sprintf(imaftmp,"ost_skl_shet%d.tmp",getpid());
if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


//Создаём нужные списки
class iceb_u_int sp_skl;
class iceb_u_spisok sp_shetu;
int kolstr2=0;
kolstr1=0;
class ostatok ost;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(ost_skl_shet_prov(row,data->rk->sklad.ravno(),data->rk->grupa.ravno(),data->rk->kodmat.ravno(),data->rk->shet.ravno(),data->window) != 0)
   continue;

  ostkarw(1,1,gk,dk,mk,gk,atoi(row[0]),atoi(row[1]),&ost);
  if(ost.ostg[3] == 0.)
    continue;  

  if(sp_shetu.find(row[3]) < 0)
   sp_shetu.plus(row[3]);

  if(sp_skl.find(row[0]) < 0)
   sp_skl.plus(row[0]);

  fprintf(fftmp,"%s|%s|%s|%10.10g|\n",row[0],row[3],row[4],ost.ostg[3]);
  kolstr2++;  
 }
fclose(fftmp);

class iceb_u_double m_kolih;
class iceb_u_double m_suma;

m_kolih.make_class(sp_shetu.kolih()*sp_skl.kolih());
m_suma.make_class(sp_shetu.kolih()*sp_skl.kolih());

int nom_shet=0;
int nom_skl=0;
double suma=0.;
int kol_shet=sp_shetu.kolih();
cur.poz_cursor(0);
kolstr1=0;
if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
unlink(imaftmp);

char stroka[1024];
char sklads[30];
char shet[32];
char cena[30];
char kolih[30];
kolstr1=0;
while(fgets(stroka,sizeof(stroka)-1,fftmp) != NULL)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    
  iceb_u_polen(stroka,sklads,sizeof(sklads),1,'|');
  iceb_u_polen(stroka,shet,sizeof(shet),2,'|');
  iceb_u_polen(stroka,cena,sizeof(cena),3,'|');
  iceb_u_polen(stroka,kolih,sizeof(kolih),4,'|');

  nom_skl=sp_skl.find(sklads);
  nom_shet=sp_shetu.find(shet);

  suma=atof(kolih)*atof(cena);
  m_kolih.plus(atof(kolih),nom_skl*kol_shet+nom_shet);
  m_suma.plus(suma,nom_skl*kol_shet+nom_shet);
 }
fclose(fftmp);


FILE *ff;

char imaf[56];
sprintf(imaf,"ostskl%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_startfil(ff);

fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/
fprintf(ff,"\x1B\x4D"); /*12-знаков*/
fprintf(ff,"\x0F");  /*Ужатый режим*/

iceb_u_zagolov(gettext("Расчёт остатков по складам и счетам учёта"),0,0,0,dk,mk,gk,organ,ff);

ost_skl_shet_rekpoi(data->rk->sklad.ravno(),data->rk->grupa.ravno(),data->rk->kodmat.ravno(),data->rk->shet.ravno(),ff);

ost_skl_shet_rm_skl(&sp_skl,&sp_shetu,&m_kolih,&m_suma,ff,data->window);

iceb_podpis(ff,data->window);

fclose(ff);

char imaf2[50];
sprintf(imaf2,"ostshet%d.lst",getpid());

if((ff = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_startfil(ff);

fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/
fprintf(ff,"\x1B\x4D"); /*12-знаков*/
fprintf(ff,"\x0F");  /*Ужатый режим*/

iceb_u_zagolov(gettext("Расчёт остатков по складам и счетам учёта"),0,0,0,dk,mk,gk,organ,ff);

ost_skl_shet_rekpoi(data->rk->sklad.ravno(),data->rk->grupa.ravno(),data->rk->kodmat.ravno(),data->rk->shet.ravno(),ff);

ost_skl_shet_rm_shet(&sp_skl,&sp_shetu,&m_kolih,&m_suma,ff,data->window);

iceb_podpis(ff,data->window);

fclose(ff);



data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт остатков по складам и счетам учёта"));
data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Расчёт остатков по складам и счетам учёта"));





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);
}

