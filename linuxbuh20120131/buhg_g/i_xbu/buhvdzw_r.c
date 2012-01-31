/*$Id: buhvdzw_r.c,v 1.19 2011-02-21 07:35:51 sasa Exp $*/
/*10.11.2009	02.03.2007	Белых А.И.	buhvdzw_r.c
Расчёт валовых доходов и затрат
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhvdzw.h"

class buhvdzw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class buhvdzw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  buhvdzw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhvdzw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhvdzw_r_data *data);
gint buhvdzw_r1(class buhvdzw_r_data *data);
void  buhvdzw_r_v_knopka(GtkWidget *widget,class buhvdzw_r_data *data);

void buhvdz_fn1(int metka_ras,short dn,short mn,short gn,short dk,short mk,short gk,class iceb_u_spisok *sp_sheta_deb,class iceb_u_spisok *sp_sheta_kre,\
int kolstr,class SQLCURSOR *cur,FILE *ff_dv,FILE *ff_it,class buhvdzw_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

int buhvdzw_r(class buhvdzw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhvdzw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт валовых доходов и затрат"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(buhvdzw_r_key_press),&data);

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

repl.plus(gettext("Расчёт валовых доходов и затрат"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),imabaz,organ);
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno_toutf());

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(buhvdzw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)buhvdzw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhvdzw_r_v_knopka(GtkWidget *widget,class buhvdzw_r_data *data)
{
//printf("buhvdzw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhvdzw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhvdzw_r_data *data)
{
// printf("buhvdzw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}


/******************************************/
/******************************************/

gint buhvdzw_r1(class buhvdzw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);




int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

//Создаём список дебетовых счетов
sprintf(strsql,"select distinct sh from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by sh asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok sp_sheta_deb;

while(cur.read_cursor(&row) != 0)
  sp_sheta_deb.plus(row[0]);

//Создаём список кредитовых счетов
sprintf(strsql,"select distinct shk from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by shk asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok sp_sheta_kre;

while(cur.read_cursor(&row) != 0)
  sp_sheta_kre.plus(row[0]);




sprintf(strsql,"select sh,shk,deb from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and deb <> 0.",gn,mn,dn,gk,mk,dk);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



char imaf_dv[50];
sprintf(imaf_dv,"buhvdz%d.lst",getpid());
FILE *ff_dv;

if((ff_dv=fopen(imaf_dv,"w")) == NULL) //Открываем файл с распечаткой
 {
  iceb_er_op_fil(imaf_dv,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char imaf_it[50];
sprintf(imaf_it,"buhvdzi%d.lst",getpid());
FILE *ff_it;

if((ff_it=fopen(imaf_it,"w")) == NULL) //Открываем файл с распечаткой
 {
  iceb_er_op_fil(imaf_it,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


if(data->rk->var_ras == 0 || data->rk->var_ras == 1)
  buhvdz_fn1(0,dn,mn,gn,dk,mk,gk,&sp_sheta_deb,&sp_sheta_kre,kolstr,&cur,ff_dv,ff_it,data);

if(data->rk->var_ras == 0 || data->rk->var_ras == 2)
 {
  if(data->rk->var_ras == 0)
   {
    fprintf(ff_dv,"\f");
    fprintf(ff_it,"\f");
   }
  buhvdz_fn1(1,dn,mn,gn,dk,mk,gk,&sp_sheta_deb,&sp_sheta_kre,kolstr,&cur,ff_dv,ff_it,data);
 }


iceb_podpis(ff_dv,data->window);
fclose(ff_dv);



iceb_podpis(ff_it,data->window);
fclose(ff_it);
iceb_ustpeh(imaf_it,0,data->window);
iceb_ustpeh(imaf_dv,0,data->window);

data->rk->imaf.plus(imaf_it);
data->rk->imaf.plus(imaf_dv);
data->rk->naimf.plus(gettext("Расчёт валовых доходов и затрат"));
data->rk->naimf.plus(gettext("Расшифровка расчёта"));




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}

/********************************/
/*Получить из строки настройки список счетов*/
/*****************************************/
void buhvdz_spshet(const char *str_nast,class iceb_u_spisok *deb,class iceb_u_spisok *kre,class iceb_u_spisok *kontr)
{
if(str_nast[0] == '\0')
 {
  deb->plus("");
  kre->plus("");
 }
int koltz=iceb_u_pole2(str_nast,';');
char nastr[1000];
if(koltz == 0)
 {
  if(iceb_u_pole2(str_nast,':') == 0)
   {
    deb->plus(str_nast);
    kre->plus("");
    kontr->plus("");
   }
  else
   {
    iceb_u_polen(str_nast,deb,1,':');
    iceb_u_polen(str_nast,kre,2,':');
    iceb_u_polen(str_nast,kontr,3,':');
   }     
  return;
 }

for(int ii=0; ii < koltz; ii++)
 {
  memset(nastr,'\0',sizeof(nastr));
  iceb_u_polen(str_nast,nastr,sizeof(nastr),ii+1,';');
  if(nastr[0] == '\0')
   break;
  if(iceb_u_pole2(nastr,':') == 0)
   {
    deb->plus(nastr);
    kre->plus("");
    kontr->plus("");
   }
  else
   {
    iceb_u_polen(nastr,deb,1,':');
    iceb_u_polen(nastr,kre,2,':');
    iceb_u_polen(nastr,kontr,3,':');
   }     
  
 }

}



/*************************************/
/*Расчёт однопроходный*/
/***************************/

void buhvdz_fn1(int metka_ras, //0-расчёт валовых доходов 1-расчёт валовых затрат
short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_spisok *sp_sheta_deb,
class iceb_u_spisok *sp_sheta_kre,
int kolstr,
class SQLCURSOR *cur,
FILE *ff_dv,FILE *ff_it,
class buhvdzw_r_data *data)
{
class iceb_u_str imaf;
if(metka_ras == 0)
  imaf.plus("buhvdz_d.alx");
if(metka_ras == 1)
  imaf.plus("buhvdz_z.alx");

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr2=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf.ravno());
if((kolstr2=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr2 == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf.ravno());
  iceb_menu_soob(strsql,data->window);
  return;
 }


//Вычисляем количество настроек
int kolih_nast=0;
char stroka1[1024];
char kod_dz[512];
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],stroka1,sizeof(stroka1),1,'|') != 0 )
    continue; 
  if(iceb_u_SRAV(stroka1,"Код",0) != 0 )
   continue;

  iceb_u_polen(row_alx[0],kod_dz,sizeof(kod_dz),2,'|');
  if(kod_dz[0] == '\0')
   continue;

  kolih_nast++;    
 }

class iceb_u_spisok sheta_deb[kolih_nast];
class iceb_u_spisok sheta_deb_p[kolih_nast]; //Персональные дебетовые счета для кредитового счета
class iceb_u_spisok spkontr_deb_p[kolih_nast]; /*Персональный список контрагентов*/

class iceb_u_spisok sheta_kre[kolih_nast];
class iceb_u_spisok sheta_kre_p[kolih_nast]; //персональные кредитовые счета для дебетового счёта
class iceb_u_spisok spkontr_kre_p[kolih_nast]; 

class iceb_u_double sum_pr[kolih_nast]; //Массив сумм проводок

char naim_r[512];
class iceb_u_spisok kod_dz_s;
class iceb_u_spisok naim_r_s;
int nomer_dz=0;
cur_alx.poz_cursor(0);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],stroka1,sizeof(stroka1),1,'|') != 0 )
    continue; 
  if(iceb_u_SRAV(stroka1,"Код",0) != 0 )
   continue;
  else
   {

    iceb_u_polen(row_alx[0],kod_dz,sizeof(kod_dz),2,'|');
    if(kod_dz[0] == '\0')
     continue;
    
    kod_dz_s.plus(kod_dz);    
    
    cur_alx.read_cursor(&row_alx);
    iceb_u_polen(row_alx[0],naim_r,sizeof(naim_r),2,'|');
    naim_r_s.plus(naim_r);
    
    class iceb_u_str shet_d;
    class iceb_u_str shet_k;
    
    cur_alx.read_cursor(&row_alx);
    iceb_u_polen(row_alx[0],stroka1,sizeof(stroka1),2,'|');
    iceb_fplus(stroka1,&shet_d,&cur_alx);
        
    cur_alx.read_cursor(&row_alx);
    iceb_u_polen(row_alx[0],stroka1,sizeof(stroka1),2,'|');
    iceb_fplus(stroka1,&shet_k,&cur_alx);
    

    buhvdz_spshet(shet_d.ravno(),&sheta_deb[nomer_dz],&sheta_kre_p[nomer_dz],&spkontr_kre_p[nomer_dz]);
    buhvdz_spshet(shet_k.ravno(),&sheta_kre[nomer_dz],&sheta_deb_p[nomer_dz],&spkontr_deb_p[nomer_dz]);
    
    nomer_dz++;  
   }
    
 }

for(int ii=0; ii < kolih_nast; ii++)
  sum_pr[ii].make_class(sp_sheta_deb->kolih()*sp_sheta_kre->kolih());

int nom_sd=0;
int nom_sk=0;
float kolstr1=0;
double suma=0.;
cur->poz_cursor(0);
int kolih_k=sp_sheta_kre->kolih();
SQL_str row;

while(cur->read_cursor(&row) != 0)
 {
//  sprintf(strsql,"%-6s %-6s %s\n",row[0],row[1],row[2]);
//  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  for(int ii=0; ii < kolih_nast; ii++)
   {
    if((nom_sd=sheta_deb[ii].find(row[0],1)) < 0)
      continue;
    
    if(strlen(sheta_kre_p[ii].ravno(nom_sd)) > 0)
     {
      if(iceb_u_proverka(sheta_kre_p[ii].ravno(nom_sd),row[1],1,0) != 0)
        continue;
     }   
    else
     {
      
      if((nom_sk=sheta_kre[ii].find(row[1],1)) < 0)
       continue;

      if(strlen(sheta_deb_p[ii].ravno(nom_sk)) > 0)
       {
        if(iceb_u_proverka(sheta_deb_p[ii].ravno(nom_sk),row[0],1,0) != 0)
          continue;
       }   
     }  
    
    suma=atof(row[2]);
    

    if((nom_sd=sp_sheta_deb->find(row[0],1)) < 0)
     continue;

    if((nom_sk=sp_sheta_kre->find(row[1],1)) < 0)
     continue;

//    sprintf(strsql,"***%-6s %-6s %s\n",row[0],row[1],row[2]);
//    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    
    sum_pr[ii].plus(suma,nom_sd*kolih_k+nom_sk);
  }  
 }

char zag_ras[200];
memset(zag_ras,'\0',sizeof(zag_ras));
iceb_poldan("Заголовок распечатки",zag_ras,imaf.ravno(),data->window);

sprintf(strsql,"\n%s\n",zag_ras);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

iceb_u_zagolov(zag_ras,dn,mn,gn,dk,mk,gk,organ,ff_dv);


iceb_u_zagolov(zag_ras,dn,mn,gn,dk,mk,gk,organ,ff_it);

fprintf(ff_it,"\
------------------------------------------------\n");
if(metka_ras == 0)
 fprintf(ff_it,gettext("\
 Код |Наименование статей доходов   |  Сумма   |\n"));
if(metka_ras == 1)
 fprintf(ff_it,gettext("\
 Код |Наименование статей затрат    |  Сумма   |\n"));

fprintf(ff_it,"\
------------------------------------------------\n");

/****** Распечатываем ******************/
double it=0;
for(int ii=0 ; ii < kolih_nast; ii++)
 {

  fprintf(ff_dv,"\n%s %s\n",kod_dz_s.ravno(ii),naim_r_s.ravno(ii));  

  //Распечатываем дебет по вертикали
  suma=buh_rhw(0,sp_sheta_deb,sp_sheta_kre,&sum_pr[ii],ff_dv); 
  it+=suma;

  fprintf(ff_it,"%*s %-*.*s %10.2f\n",
  iceb_u_kolbait(5,kod_dz_s.ravno(ii)),kod_dz_s.ravno(ii),
  iceb_u_kolbait(30,naim_r_s.ravno(ii)),iceb_u_kolbait(30,naim_r_s.ravno(ii)),naim_r_s.ravno(ii),
  suma);
  
  for(int nom=30; nom < iceb_u_strlen(naim_r_s.ravno(ii)); nom+=30)
   fprintf(ff_it,"%5s %-*.*s\n",
   "",
   iceb_u_kolbait(30,iceb_u_adrsimv(nom,naim_r_s.ravno(ii))),
   iceb_u_kolbait(30,iceb_u_adrsimv(nom,naim_r_s.ravno(ii))),
   iceb_u_adrsimv(nom,naim_r_s.ravno(ii)));
  
  if(strlen(naim_r_s.ravno(ii)) > 60)
   fprintf(ff_it,"%5s %s\n","",&naim_r_s.ravno(ii)[60]);

  sprintf(strsql,"%2s %-30.30s %10.2f\n",kod_dz_s.ravno(ii),naim_r_s.ravno(ii),suma);  
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  fprintf(ff_dv,"\n\n\n\n");  
  
 }
sprintf(strsql,"%33s:%10.2f\n",gettext("Итого"),it);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff_it,"\
------------------------------------------------\n");
fprintf(ff_it,"%36s %10.2f\n",gettext("Итого"),it);
 
}



