/*$Id: zar_spisr_r.c,v 1.15 2011-02-21 07:36:00 sasa Exp $*/
/*20.11.2009	22.11.2006	Белых А.И.	zar_spisr_r.c
Распечатка списка работников
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_spisr.h"

class zar_spisr_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_spisr_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_spisr_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_spisr_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_spisr_r_data *data);
gint zar_spisr_r1(class zar_spisr_r_data *data);
void  zar_spisr_r_v_knopka(GtkWidget *widget,class zar_spisr_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

int zar_spisr_r(class zar_spisr_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_spisr_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка списка работников"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_spisr_r_key_press),&data);

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

repl.plus(gettext("Распечатка списка работников"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_spisr_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_spisr_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_spisr_r_v_knopka(GtkWidget *widget,class zar_spisr_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_spisr_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_spisr_r_data *data)
{
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}
/***************************/
/*печать реквизитов поиска*/
/***************************/

void zar_spis_r_rp(class zar_spisr_rek *rk,FILE *ff)
{
if(rk->podr.getdlinna() <= 1)
 {
  fprintf(ff,"%s\n",gettext("Список по всем подразделениям"));
 }
else
 {
  fprintf(ff,"%s:%s\n",gettext("Код подразделения"),rk->podr.ravno()); 
 }
if(rk->tabnom.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),rk->tabnom.ravno());
if(rk->kod_zvan.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код звания"),rk->kod_zvan.ravno());
if(rk->kod_kateg.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код категории"),rk->kod_kateg.ravno());
if(rk->shetu.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),rk->shetu.ravno());
}

/******************************************/
/******************************************/

gint zar_spisr_r1(class zar_spisr_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
int kolstr=0;
class iceb_clock sss(data->window);



char bros[512];
sprintf(strsql,"select * from Kartb");

if(data->rk->metka_sort == 0)
  sprintf(bros," order by tabn asc");
else
  sprintf(bros," order by fio asc");
strcat(strsql,bros);

class SQLCURSOR cur;
class SQLCURSOR curr;
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
char imaf[56];
sprintf(imaf,"sr%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf1[56];

sprintf(imaf1,"sr1%d.lst",getpid());
FILE *ff1;
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf2[56];
sprintf(imaf2,"sr2%d.lst",getpid());
FILE *ff2;
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_ks[40];
sprintf(imaf_ks,"sr_ks%d.lst",getpid());
FILE *ff_ks;
if((ff_ks = fopen(imaf_ks,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_ks,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->rk->metka_ras == 0)
 strcpy(strsql,gettext("Распечатка работающих"));
if(data->rk->metka_ras == 1)
 strcpy(strsql,gettext("Распечатка уволенных"));
if(data->rk->metka_ras == 2)
 strcpy(strsql,gettext("Распечатка работающих совместителей"));
 
iceb_u_zagolov(strsql,0,0,0,0,0,0,organ,ff);
iceb_u_zagolov(strsql,0,0,0,0,0,0,organ,ff1);
iceb_u_zagolov(strsql,0,0,0,0,0,0,organ,ff2);
iceb_u_zagolov(strsql,0,0,0,0,0,0,organ,ff_ks);

if(data->rk->podr.getdlinna() <= 1)
 {
  sprintf(strsql,"%s\n",gettext("Список по всем подразделениям"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    
 }
else
 {
  sprintf(strsql,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno()); 
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


 }
 

zar_spis_r_rp(data->rk,ff);
zar_spis_r_rp(data->rk,ff1);
zar_spis_r_rp(data->rk,ff2);
zar_spis_r_rp(data->rk,ff_ks);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("Т/н |               Ф. И. О.                 |Инд. номер|Счет |Дата приём./увольнения |  Должность |Категория|Подразделение|Льготы| Звание |\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff1,"\
--------------------------------------------------------------------\n");
fprintf(ff1,gettext("\
Т/н |               Ф. И. О.                 |          |          |\n"));
fprintf(ff1,"\
--------------------------------------------------------------------\n");

fprintf(ff2,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff2,gettext("\
Т/н |               Ф. И. О.                 |Инд. номер|Дата рожд.|                 Адрес                            | N паспорта    |               Кем выдан                          |Дата в/п  | Телефон\n"));
fprintf(ff2,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_ks,"\
-------------------------------------------------------------------------------\n");
fprintf(ff_ks,gettext("\
 Т/н |               Ф. И. О.                 |Инд. номер|     Карт-счёт      |\n"));
fprintf(ff_ks,"\
-------------------------------------------------------------------------------\n");
/*
12345 1234567890123456789012345678901234567890 12345678901234567890
*/
short dv,mv,gv;
short dr,mr,gr;

int kolr=0;
float kolstr1=0;
SQL_str row,row1;
char podr[512];
char kateg[512];
char zvan[512];
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->podr.ravno(),row[4],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->kod_zvan.ravno(),row[3],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->kod_kateg.ravno(),row[5],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->shetu.ravno(),row[8],0,0) != 0)
   continue;
    
  if(data->rk->metka_ras == 0 && row[7][0] != '0')
     continue;
  if(data->rk->metka_ras  == 1 && row[7][0] == '0')
    continue;
  if(data->rk->metka_ras  == 2 && (row[9][0] == '0' || row[7][0] != '0' ))
    continue;

  kolr++;

  sprintf(strsql,"%-4s %-*s %*s %s\n",row[0],iceb_u_kolbait(30,row[1]),row[1],iceb_u_kolbait(4,row[8]),row[8],row[5]);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  /*Определяем наименование подразделения*/
  memset(podr,'\0',sizeof(podr));
  sprintf(strsql,"select naik from Podr where kod=%s",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
   strncpy(podr,row1[0],sizeof(podr)-1);

  /*Определяем наименование категории*/
  memset(kateg,'\0',sizeof(kateg));
  sprintf(strsql,"select naik from Kateg where kod=%s",row[5]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
   strncpy(kateg,row1[0],sizeof(kateg)-1);

  /*Определяем наименование звания*/
  memset(zvan,'\0',sizeof(zvan));
  sprintf(strsql,"select naik from Zvan where kod=%s",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
   strncpy(zvan,row1[0],sizeof(zvan)-1);

  fprintf(ff,"%-4s %-*s %-10s %-*s %-11s %-11s %-*.*s %-*.*s %-*.*s %-*.*s %s\n",
  row[0],iceb_u_kolbait(40,row[1]),row[1],
  row[10],
  iceb_u_kolbait(5,row[8]),row[8],
  row[6],
  row[7],
  iceb_u_kolbait(12,row[2]),
  iceb_u_kolbait(12,row[2]),
  row[2],
  iceb_u_kolbait(9,kateg),
  iceb_u_kolbait(9,kateg),
  kateg,
  iceb_u_kolbait(13,podr),
  iceb_u_kolbait(13,podr),
  podr,
  iceb_u_kolbait(6,row[17]),
  iceb_u_kolbait(6,row[17]),
  row[17],
  zvan);

  fprintf(ff1,"%-4s %-*s|%10s|%10s|\n\
....................................................................\n",
  row[0],iceb_u_kolbait(40,row[1]),row[1]," "," ");

  dr=mr=gr=0;

  iceb_u_rsdat(&dr,&mr,&gr,row[21],2);
  iceb_u_rsdat(&dv,&mv,&gv,row[19],2);
    
  fprintf(ff2,"%-4s %-*s %-10s %02d.%02d.%04d %-*.*s %-*s %-*.*s %02d.%02d.%04d %s\n",
  row[0],iceb_u_kolbait(40,row[1]),row[1],row[10],dr,mr,gr,
  iceb_u_kolbait(50,row[11]),iceb_u_kolbait(50,row[11]),row[11],
  iceb_u_kolbait(15,row[12]),row[12],
  iceb_u_kolbait(50,row[13]),iceb_u_kolbait(50,row[13]),row[13],
  dv,mv,gv,row[14]);
  
  fprintf(ff_ks,"%-5s %-*s %*s %-*s\n",row[0],iceb_u_kolbait(40,row[1]),row[1],
  iceb_u_kolbait(20,row[10]),row[10],
  iceb_u_kolbait(20,row[18]),row[18]);
  
 }

fprintf(ff1,"\
--------------------------------------------------------------------\n");

fprintf(ff_ks,"\
-------------------------------------------------------------------------------\n");

fprintf(ff,"\n%s:%d\n\n",
gettext("Количество работников"),kolr);

iceb_podpis(ff,data->window);
iceb_podpis(ff1,data->window);
iceb_podpis(ff2,data->window);
iceb_podpis(ff_ks,data->window);



fclose(ff);
fclose(ff1);
fclose(ff2);
fclose(ff_ks);

sprintf(strsql,"\n%s: %d\n",
gettext("Количество человек"),kolr);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);



data->rk->imaf.plus(imaf);
data->rk->imaf.plus(imaf1);
data->rk->imaf.plus(imaf2);
data->rk->imaf.plus(imaf_ks);

data->rk->naimf.plus(gettext("Распечатка с реквизитами карточки работника"));
data->rk->naimf.plus(gettext("Распечатка пустографки"));
data->rk->naimf.plus(gettext("Распечатка паспортных данных"));
data->rk->naimf.plus(gettext("Распечатка карт-счетов"));


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);


data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
