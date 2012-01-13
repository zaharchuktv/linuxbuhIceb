/*$Id: buhrpznpw_r.c,v 1.14 2011-02-21 07:35:51 sasa Exp $*/
/*10.11.2009	01.03.2007	Белых А.И.	buhrpznpw_r.c
Расчёт распределения административных затрат на доходы
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
#include "buhrpznpw.h"

class buhrpznpw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class buhrpznpw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  buhrpznpw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhrpznpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhrpznpw_r_data *data);
gint buhrpznpw_r1(class buhrpznpw_r_data *data);
void  buhrpznpw_r_v_knopka(GtkWidget *widget,class buhrpznpw_r_data *data);
int buhrpznpw_r_nast(class iceb_u_str *sp_shet_d,class iceb_u_str *sp_shet_z,class iceb_u_str *naim_ras,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

int buhrpznpw_r(class buhrpznpw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhrpznpw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт распределения административних затрат на доходы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(buhrpznpw_r_key_press),&data);

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

repl.plus(gettext("Расчёт распределения административних затрат на доходы"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(buhrpznpw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)buhrpznpw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhrpznpw_r_v_knopka(GtkWidget *widget,class buhrpznpw_r_data *data)
{
//printf("buhrpznpw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhrpznpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhrpznpw_r_data *data)
{
// printf("buhrpznpw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}


/**********************************/
/*Проверка счёта*/
/*********************/
int buhrznp_prov(const char *sp_shet,const char *shet_prov,const char *shet_provk,char *shet_vib)
{
char shet_shetk[50];
char shet[40];
char shetk[40];

int kol=iceb_u_pole2(sp_shet,',');
for(int ii=0; ii < kol; ii++)
 {
  memset(shet,'\0',sizeof(shet));
  memset(shetk,'\0',sizeof(shetk));
  
  iceb_u_polen(sp_shet,shet_shetk,sizeof(shet_shetk),ii+1,',');
  if(shet_shetk[0] == '\0')
   continue;

  if(iceb_u_polen(shet_shetk,shet,sizeof(shet_shetk),1,'>') != 0)
   strcpy(shet,shet_shetk);
  else
   iceb_u_polen(shet_shetk,shetk,sizeof(shet_shetk),2,'>');

  if(iceb_u_proverka(shet,shet_prov,0,0) == 0)
   {
    if(shetk[0] != '\0')
     {
      if(iceb_u_proverka(shetk,shet_provk,0,0) == 0)
       {
        strcpy(shet_vib,shet_shetk);
        return(0);
       }
     }
    else
     {
      strcpy(shet_vib,shet_shetk);
      return(0);
     }
   }      
 }
return(1);
}

/******************************************/
/******************************************/

gint buhrpznpw_r1(class buhrpznpw_r_data *data)
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



class iceb_u_str doh_sheta;
class iceb_u_str zat_sheta;
class iceb_u_str naim_ras;

buhrpznpw_r_nast(&doh_sheta,&zat_sheta,&naim_ras,data->window);


if(zat_sheta.getdlinna() <= 1)
 {
  iceb_menu_soob("Не введены \"Затратные счета\" !",data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(doh_sheta.getdlinna() <= 1)
 {
  iceb_menu_soob("Не введены \"Доходные счета\" !",data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kolstr;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select sh,shk,deb from Prov where datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by sh asc",gn,mn,dn,gk,mk,dk);

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
class iceb_u_spisok sp_doh;
class iceb_u_spisok sp_zat;
class iceb_u_double sum_doh;
class iceb_u_double sum_zat;
int nomer=0;
double suma=0.;
char shet_vib[50];

while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  memset(shet_vib,'\0',sizeof(shet_vib));
    
  if(buhrznp_prov(doh_sheta.ravno(),row[0],row[1],shet_vib) == 0)
   {
    if((nomer=sp_doh.find(shet_vib)) < 0)
     sp_doh.plus(shet_vib);
    sum_doh.plus(suma,nomer);
   
    continue;
   }
  
  if(buhrznp_prov(zat_sheta.ravno(),row[0],row[1],shet_vib) == 0)
   {
    if((nomer=sp_zat.find(shet_vib)) < 0)
     sp_zat.plus(shet_vib);
    sum_zat.plus(suma,nomer);
   
    continue;
   }
 } 


char imaf[56];
sprintf(imaf,"rznp%d.lst",getpid());
FILE *ff;
if((ff=fopen(imaf,"w")) == NULL) 
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(naim_ras.getdlinna() <= 1)
 iceb_u_zagolov(gettext("Расчёт распределения административных затрат на доходы"),dn,mn,gn,dk,mk,gk,organ,ff);
else
 iceb_u_zagolov(naim_ras.ravno(),dn,mn,gn,dk,mk,gk,organ,ff);

fprintf(ff,"%s:%s\n",gettext("Доходные счета"),doh_sheta.ravno());
fprintf(ff,"%s:%s\n",gettext("Затратные счета"),zat_sheta.ravno());

double itogo_doh=sum_doh.suma();

//создаём массив для коэффициентов

double koef[sp_zat.kolih()];
fprintf(ff,"\n%s\n",gettext("Расчёт коэффициентов"));

for(int ii=0; ii < sp_zat.kolih(); ii++)
 {
  if(itogo_doh != 0.)
    koef[ii]=sum_zat.ravno(ii)/itogo_doh;
  else
    koef[ii]=0.;
  
  fprintf(ff,"%-*s %.2f/%.2f=%f\n",
  iceb_u_kolbait(7,sp_zat.ravno(ii)),sp_zat.ravno(ii),sum_zat.ravno(ii),itogo_doh,koef[ii]);
 }

fprintf(ff,"\n");

class iceb_u_str liniq("----------------------");
for(int ii=0; ii < sp_zat.kolih()+1; ii++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());

 
fprintf(ff,"%s",gettext("Счёт дохо.|Сумма дох.|"));
for(int ii=0; ii < sp_zat.kolih(); ii++)
 fprintf(ff,"%-*.*s|",iceb_u_kolbait(10,sp_zat.ravno(ii)),iceb_u_kolbait(10,sp_zat.ravno(ii)),sp_zat.ravno(ii));
fprintf(ff,"%-*.*s|\n",iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",liniq.ravno());
double itogo=0.;
for(int ii=0; ii < sp_doh.kolih() ; ii++)
 {
  fprintf(ff,"%-*.*s|%10.2f|",
  iceb_u_kolbait(10,sp_doh.ravno(ii)),iceb_u_kolbait(10,sp_doh.ravno(ii)),sp_doh.ravno(ii),sum_doh.ravno(ii));
  itogo=0.;
  for(int kk=0; kk < sp_zat.kolih() ; kk++)
   {
    suma=sum_doh.ravno(ii)*koef[kk];
    suma=iceb_u_okrug(suma,0.01);
    fprintf(ff,"%10.2f|",suma);
    itogo+=suma;
   }
  fprintf(ff,"%10.2f|\n",itogo);
 }

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%*.*s|%10.2f|",
iceb_u_kolbait(10,gettext("Итого")),
iceb_u_kolbait(10,gettext("Итого")),
gettext("Итого"),

itogo_doh);
//Вычисляем итоги по колонкам
double itogo_ob=0.;

for(int ii=0; ii < sp_zat.kolih(); ii++)
 {
  itogo=0.;
  for(int kk=0; kk < sp_doh.kolih(); kk++)
   {
    suma=sum_doh.ravno(kk)*koef[ii];
    suma=iceb_u_okrug(suma,0.01);
    itogo+=suma;
   } 
  fprintf(ff,"%10.2f|",itogo);
  itogo_ob+=itogo;
 }
fprintf(ff,"%10.2f|\n",itogo_ob);

iceb_podpis(ff,data->window);
fclose(ff);

iceb_ustpeh(imaf,0,data->window);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт распределения административних затрат на доходы"));



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}


/*******************************/
/*Чтение файла настроек*/
/****************************/
int buhrpznpw_r_nast(class iceb_u_str *sp_shet_d,
class iceb_u_str *sp_shet_z,
class iceb_u_str *naim_ras,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
//char stroka[10240];
char stroka1[10240];

sprintf(strsql,"select str from Alx where fil='buhrznp.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"buhrznp.alx");
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;


  if(iceb_u_polen(row_alx[0],stroka1,sizeof(stroka1),2,'|') != 0)
   continue;
 
  if(iceb_u_SRAV("Доходные счета",row_alx[0],1) == 0)
   {
    iceb_fplus(stroka1,sp_shet_d,&cur_alx);
    continue;
   }
  if(iceb_u_SRAV("Затратные счета",row_alx[0],1) == 0)
   {
    iceb_fplus(stroka1,sp_shet_z,&cur_alx);
    continue;
   }

  if(iceb_u_SRAV("Наименование расчёта",row_alx[0],1) == 0)
   {
    naim_ras->new_plus(stroka1);
    continue;
   }

 }
return(0); 

}
