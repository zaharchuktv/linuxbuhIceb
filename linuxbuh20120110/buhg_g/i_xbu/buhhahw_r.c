/*$Id: buhhahw_r.c,v 1.15 2011-02-21 07:35:51 sasa Exp $*/
/*09.11.2009	20.06.2006	Белых А.И.	buhhahw_r.c
Расчёт отчёта 
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
#include "buhhahw.h"

class buhhahw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class buhhahw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  buhhahw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhhahw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhhahw_r_data *data);
gint buhhahw_r1(class buhhahw_r_data *data);
void  buhhahw_r_v_knopka(GtkWidget *widget,class buhhahw_r_data *data);

int buhhah_rsh(class iceb_u_spisok *sheta,char *naim_oth);
void buhhah_sort(class iceb_u_spisok *sheta,GtkWidget*);
int buhhar_rs(short dn,short mn,short gn,short dk,short mk,short gk,int metka_ras,class SQLCURSOR *cur,int kolstr,class iceb_u_spisok *sheta,FILE *ff,FILE *ff1,GtkWidget *bar,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double   okrg1;
extern double	okrcn;

int buhhahw_r(class buhhahw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhhahw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Расчёт шахматки"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(buhhahw_r_key_press),&data);

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

repl.plus(gettext("Расчёт шахматки"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(buhhahw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)buhhahw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhhahw_r_v_knopka(GtkWidget *widget,class buhhahw_r_data *data)
{
//printf("buhhahw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhhahw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhhahw_r_data *data)
{
// printf("buhhahw_r_key_press\n");
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

gint buhhahw_r1(class buhhahw_r_data *data)
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




sprintf(strsql,"select sh,shk,deb from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and deb <> 0. order by datp asc",gn,mn,dn,gk,mk,dk);

SQLCURSOR cur;
int kolstr=0;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
//printw("Количество проводок:%d\n",kolstr);

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;

sprintf(strsql,"select str from Alx where fil='buhhah.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"buhhah.alx");
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char variant[512];

char imaf[32];
FILE *ff;

sprintf(imaf,"hax%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff);
fprintf(ff,"\x1B\x4D"); /*12-знаков*/
fprintf(ff,"\x0F");  /*Ужатый режим*/

char imaf1[30];
FILE *ff1;

sprintf(imaf1,"haxg%d.lst",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff1);
fprintf(ff1,"\x1B\x4D"); /*12-знаков*/
fprintf(ff1,"\x0F");  /*Ужатый режим*/

char naim_oth[512];
char spis_sh[2048];
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  

  if(iceb_u_polen(row_alx[0],variant,sizeof(variant),1,'|') != 0)
   continue;
  if(iceb_u_SRAV("Вариант расчёта",variant,0) != 0)
   continue;

  iceb_u_polen(row_alx[0],variant,sizeof(variant),2,'|');
  if(variant[0] == '\0')
   continue;

  if(iceb_u_proverka(data->rk->variant.ravno(),variant,0,0) != 0)
   continue;
  
  class iceb_u_spisok sheta;
  memset(naim_oth,'\0',sizeof(naim_oth));
  
  /*Читаем наименование отчёта*/
  cur_alx.read_cursor(&row_alx);
  iceb_u_polen(row_alx[0],naim_oth,sizeof(naim_oth),2,'|');
  
  /*Читаем список счетов*/
  cur_alx.read_cursor(&row_alx);
  iceb_u_polen(row_alx[0],spis_sh,sizeof(spis_sh),2,'|');
  iceb_fplus(0,spis_sh,&sheta,&cur_alx);
  
  iceb_u_zagolov(naim_oth,dn,mn,gn,dk,mk,gk,organ,ff);
  iceb_u_zagolov(naim_oth,dn,mn,gn,dk,mk,gk,organ,ff1);

  buhhar_rs(dn,mn,gn,dk,mk,gk,data->rk->metka_ras,&cur,kolstr,&sheta,ff,ff1,data->bar,data->window);

 }

iceb_podpis(ff,data->window);
fclose(ff);
iceb_podpis(ff1,data->window);
fclose(ff1);


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Отчёт с дебетами по вертикали"));
data->rk->imaf.plus(imaf1);
data->rk->naimf.plus(gettext("Отчёт с кредитами по вертикали"));



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/************************************************/
/*Расчёт по варианту*/
/*************************************************/

int buhhar_rs(short dn,short mn,short gn,
short dk,short mk,short gk,
int metka_ras,
class SQLCURSOR *cur,
int kolstr,
class iceb_u_spisok *sheta,
FILE *ff,
FILE *ff1,
GtkWidget *bar,
GtkWidget *wpredok)
{
SQL_str row;
char strsql[512];


char imaf_tmp[30];
FILE *ff_tmp;

sprintf(imaf_tmp,"hax%d.tmp",getpid());
if((ff_tmp = fopen(imaf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tmp,"",errno,wpredok);
  return(1);
 }

/*********
for(int i=0; i < sheta->kolih(); i++)
 printw("%s ",sheta->ravno(i));
printw("\n");
OSTANOV();
***************/
class iceb_u_spisok sheta_k;

float kolstr1=0;
int kol_zap=0;
//Создаём список счетов корреспондентов
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(bar,kolstr,++kolstr1);    

  if(metka_ras == 1)
   if(sheta->find(row[1]) >= 0)
    continue;
    
  if(sheta->find(row[0],1) < 0)
   continue;
  
  if(sheta_k.find(row[1],1) < 0)
   sheta_k.plus(row[1]);       
  
  fprintf(ff_tmp,"%s|%s|%s|\n",row[0],row[1],row[2]);
  kol_zap++;
 }

fclose(ff_tmp);


if(kol_zap == 0)
 {
  unlink(imaf_tmp);
  iceb_menu_soob(gettext("Не найдено ни одной проводки со счётом из файла настройки !"),wpredok);
  return(1);
 }

buhhah_sort(&sheta_k,wpredok); //сортируем список счетов
 
if((ff_tmp = fopen(imaf_tmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaf_tmp,"",errno,wpredok);
  return(1);
 }

unlink(imaf_tmp);



class iceb_u_double suma_d; //Массив сумм
suma_d.make_class(sheta->kolih()*sheta_k.kolih());

kolstr1=0;
char stroka[1024];
//Заполняем массив сумм по счетам
char shet[32];
char shet_k[20];
double suma;
int nomer_sh;
int nomer_shk;
int kol_sh_k=sheta_k.kolih();

while(fgets(stroka,sizeof(stroka),ff_tmp) != 0)
 {
//  strzag(LINES-1,0,kol_zap,++kolstr1);
  iceb_pbar(bar,kol_zap,++kolstr1);    
  iceb_u_polen(stroka,shet,sizeof(shet),1,'|');
  iceb_u_polen(stroka,shet_k,sizeof(shet_k),2,'|');
  iceb_u_polen(stroka,strsql,sizeof(strsql),3,'|');
  suma=atof(strsql);
  nomer_sh=sheta->find(shet,1);
  nomer_shk=sheta_k.find(shet_k,1);
  suma_d.plus(suma,nomer_sh*kol_sh_k+nomer_shk);
 }

fclose(ff_tmp);


buh_rhw(0,sheta,&sheta_k,&suma_d,ff); //распечатываем массив
buh_rhw(1,sheta,&sheta_k,&suma_d,ff1); //распечатываем массив



return(0);
}

/********************************/
/*Сортировка массива счетов*/
/*****************************/
void buhhah_sort(class iceb_u_spisok *sheta,GtkWidget *wpredok)
{

FILE *ff;
char imaf[56];
sprintf(imaf,"iceb_sort%d.tmp",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

for(int i=0; i < sheta->kolih(); i++)
 fprintf(ff,"%s\n",sheta->ravno(i));

fclose(ff);

char strsql[512];

sprintf(strsql,"sort -o %s %s",imaf,imaf);

system(strsql);

if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
unlink(imaf);

sheta->free_class();

while(fgets(strsql,sizeof(strsql),ff) != NULL)
 {
  iceb_u_ud_simv(strsql,"\n");
  sheta->plus(strsql);
 }
fclose(ff);

}
