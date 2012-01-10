/*$Id: kriostw_r.c,v 1.13 2011-02-21 07:35:52 sasa Exp $*/
/*11.11.2009	06.05.2005	Белых А.И. 	kriostw_r.c
Расчёт критических остатков по материаллам
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "kriostw.h"

class kriostw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class kriostw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  kriostw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   kriostw_r_key_press(GtkWidget *widget,GdkEventKey *event,class kriostw_r_data *data);
gint kriostw_r1(class kriostw_r_data *data);
void  kriostw_r_v_knopka(GtkWidget *widget,class kriostw_r_data *data);

void  krisap(int *kollist,int *kolstrlist,FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double	nds1;
extern int kol_strok_na_liste;

int kriostw_r(class kriostw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class kriostw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать материалы имеющие критический остаток"));//13
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kriostw_r_key_press),&data);

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

repl.plus(gettext("Распечатать материалы имеющие критический остаток"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(kriostw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)kriostw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kriostw_r_v_knopka(GtkWidget *widget,class kriostw_r_data *data)
{
//printf("kriostw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kriostw_r_key_press(GtkWidget *widget,GdkEventKey *event,class kriostw_r_data *data)
{
// printf("kriostw_r_key_press\n");
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

gint kriostw_r1(class kriostw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
short dos,mos,gos;

iceb_u_rsdat(&dos,&mos,&gos,data->rk->datao.ravno(),1);

sprintf(strsql,"%s:%d.%d.%d\n",gettext("Остатки вычислены на"),dos,mos,gos);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr=0;
SQL_str row,row1;
double kriost=0.;

sprintf(strsql,"select kodm,kodgr,naimat,kriost from Material");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
//  sss.clear_data();
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

FILE *ff;
char imaf[30];

int kolstr1=0;
int kolstr2=0;
double   ostmat=0.;
int kolstrlist=0;

sprintf(imaf,"kro%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff);
data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Распечатка материалов имеющих критические остатки"));

fprintf(ff,"\x1b\x6C%c",10); /*Установка левого поля*/

iceb_u_zagolov(gettext("Распечатка материалов имеющих критические остатки"),0,0,0,0,0,0,organ,ff);
kolstrlist=5;

fprintf(ff,"%s:%d.%d.%d\n",gettext("Остатки вычислены на"),dos,mos,gos);
kolstrlist++;

if(data->rk->sklad.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  kolstrlist++;
 }

if(data->rk->kodgrmat.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Группa"),data->rk->kodgrmat.ravno());
  kolstrlist++;
 }
if(data->rk->shet.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Счета учета"),data->rk->shet.ravno());
  kolstrlist++;
 }
if(data->rk->kodmat.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());
  kolstrlist++;
 }

int kollist=0;
krisap(&kollist,&kolstrlist,ff);
class ostatok ost;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rk->kodmat.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodgrmat.ravno(),row[1],0,0) != 0)
    continue;

  kriost=0.;
  if(row[3] != NULL)
    kriost=atof(row[3]);
  ostmat=0.;  
  sprintf(strsql,"select sklad,nomk,shetu from Kart where kodm=%s",row[0]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    break;
   }
  int metka=0;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(iceb_u_proverka(data->rk->sklad.ravno(),row1[0],0,0) != 0)
      continue;
    if(iceb_u_proverka(data->rk->shet.ravno(),row1[2],0,0) != 0)
      continue;

    ostkarw(1,1,gos,dos,mos,gos,atoi(row1[0]),atoi(row1[1]),&ost);
    metka=1;
    ostmat+=ost.ostg[3];    

   }
  if(metka == 0)
    continue;   

  if(ostmat > kriost)
   continue;  
  kolstrlist++;

  if(kolstrlist > kol_strok_na_liste)
   {
    fprintf(ff,"\f");
    kolstrlist=1; //Строка которая будет выведена
    krisap(&kollist,&kolstrlist,ff);
   }

  fprintf(ff,"%-5s|%-*.*s|%10.2f|%10.2f\n",
  row[0],
  iceb_u_kolbait(30,row[2]),iceb_u_kolbait(30,row[2]),row[2],
  kriost,ostmat);
 }

iceb_podpis(ff,data->window);

fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/
fclose(ff);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
/**********************************/
/*Шапка распечатки                */
/**********************************/

void  krisap(int *kollist,int *kolstrlist,FILE *ff)
{
*kollist+=1;
*kolstrlist+=4;
fprintf(ff,"%50s%s N%d\n","",gettext("Лист"),*kollist);
fprintf(ff,"\
-----------------------------------------------------------\n");
fprintf(ff,gettext("\
 Код |   Наименование               |Кр.остаток| Остаток  |\n"));
fprintf(ff,"\
-----------------------------------------------------------\n");

}
