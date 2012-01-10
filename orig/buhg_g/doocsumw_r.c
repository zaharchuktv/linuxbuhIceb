/*$Id: doocsumw_r.c,v 1.15 2011-02-21 07:35:51 sasa Exp $*/
/*29.09.2009	06.05.2005	Белых А.И. 	doocsumw_r.c
Расчёт сумм выполненых дооценок
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "doocsumw.h"

class doocsumw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class doocsumw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  doocsumw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   doocsumw_r_key_press(GtkWidget *widget,GdkEventKey *event,class doocsumw_r_data *data);
gint doocsumw_r1(class doocsumw_r_data *data);
void  doocsumw_r_v_knopka(GtkWidget *widget,class doocsumw_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

int doocsumw_r(class doocsumw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class doocsumw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать суммы выполненных автоматических дооценок"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(doocsumw_r_key_press),&data);

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

repl.plus(gettext("Распечатать суммы выполненных автоматических дооценок"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(doocsumw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)doocsumw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  doocsumw_r_v_knopka(GtkWidget *widget,class doocsumw_r_data *data)
{
printf("doocsumw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   doocsumw_r_key_press(GtkWidget *widget,GdkEventKey *event,class doocsumw_r_data *data)
{
 printf("doocsumw_r_key_press\n");
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

gint doocsumw_r1(class doocsumw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


char	kodopdooc[32];
memset(kodopdooc,'\0',sizeof(kodopdooc));

class iceb_u_str imafn("matnast.alx");
if(iceb_poldan("Код операции автоматической дооценки",kodopdooc,imafn.ravno(),data->window) != 0)
 {
kkkk:;
  iceb_menu_soob(gettext("В файле настройки не введён код операции автоматической дооценки !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kodopdooc[0] == '\0')
  goto kkkk;
//Проверяем есть ли код операции в списке приходов и расходов
sprintf(strsql,"select prov from Prihod where kod='%s'",kodopdooc);
if(sql_readkey(&bd,strsql) != 1)
 {
  sprintf(strsql,gettext("Не найден код %s в списке операций приходов !"),kodopdooc);
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }

sprintf(strsql,"select prov from Rashod where kod='%s'",kodopdooc);
if(sql_readkey(&bd,strsql) != 1)
 {
  sprintf(strsql,gettext("Не найден код %s в списке операций расходов !"),kodopdooc);
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



sprintf(strsql,"select datd,sklad,nomd,nomon,pn from Dokummat where \
datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' and kodop='%s' and \
tip=1",gn,mn,dn,gk,mk,dk,kodopdooc);

SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr;
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


int vt=0;
memset(strsql,'\0',sizeof(strsql));
iceb_poldan("Код группы возвратная тара",strsql,imafn.ravno(),data->window);
vt=atoi(strsql);
char imaf[30];
FILE *ff;

sprintf(imaf,"dooc%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт сумм выполненых дооценок"));
iceb_u_startfil(ff);
iceb_u_zagolov(gettext("Расчёт сумм выполненых дооценок"),dn,mn,gn,dk,mk,gk,organ,ff);

if(data->rk->sklad.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());

fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Склад|   Дата   |Номер док.|Номер док.|        Сумма        |  Сумма   |Основание\n"));
fprintf(ff,gettext("\
     |          | приход   |  расход  |  приход  |  расход  | дооценки |\n"));
fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
double	sumi[3];
memset(&sumi,'\0',sizeof(sumi));
SQL_str row;
SQL_str row1;
float kolstr1=0.;
short d,m,g;
int mnds;
char osnov[80];
double suma=0.,sumkor=0.,sumabn=0.;
double nds=0.,sum=0.,sum1=0.;
float pnds=0.;
double kolih=0.;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s %s\n",row[0],row[1],row[2]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  iceb_pbar(data->bar,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rk->sklad.ravno(),row[1],0,0) != 0)
    continue;
    
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  pnds=atof(row[4]);
  /*Узнаем НДС документа*/
  mnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=11",g,row[2],row[1]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    mnds=atoi(row1[0]);
   }  
  //Узнаем основание документа
  memset(osnov,'\0',sizeof(osnov));
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s \
and nomd='%s' and nomerz=3",g,row[1],row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(osnov,row1[0],sizeof(osnov)-1);
    
  sumzap1w(d,m,g,row[2],atoi(row[1]),&suma,&sumabn,mnds,vt,&sumkor,1,&kolih,data->window);

  if(mnds == 0)
    nds=(suma+sumkor)*pnds/100.;
  else
   nds=0.;

  sum=suma+sumkor+nds+sumabn;
  
  sumzap1w(d,m,g,row[3],atoi(row[1]),&suma,&sumabn,mnds,vt,&sumkor,2,&kolih,data->window);

  if(mnds == 0)
    nds=(suma+sumkor)*pnds/100.;
  else
   nds=0.;

  sum1=suma+sumkor+nds+sumabn;
  sumi[0]+=sum;
  sumi[1]+=sum1;
  sumi[2]+=sum-sum1;
  fprintf(ff,"%-5s %02d.%02d.%d %-10s %-*s %10.2f %10.2f %10.2f %s\n",
  row[1],
  d,m,g,
  row[2],
  iceb_u_kolbait(10,row[3]),row[3],
  sum,sum1,sum-sum1,osnov);
 }
fprintf(ff,"\
-------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f %10.2f\n",
iceb_u_kolbait(38,gettext("Итого")),gettext("Итого"),sumi[0],sumi[1],sumi[2]);

iceb_podpis(ff,data->window);
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
