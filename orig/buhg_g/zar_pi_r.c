/*$Id: zar_pi_r.c,v 1.12 2011-02-21 07:35:59 sasa Exp $*/
/*08.12.2010	29.01.2007	Белых А.И.	zar_pi_r.c
Отчёт по пенсионерам и инвалидам
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"

class zar_pi_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  const char *data_d;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
    
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_pi_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_pi_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_pi_r_data *data);
gint zar_pi_r1(class zar_pi_r_data *data);
void  zar_pi_r_v_knopka(GtkWidget *widget,class zar_pi_r_data *data);

void  zar_sdpf_ps(class iceb_u_int *sppi,int *kolih_inv,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;


int zar_pi_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_pi_r_data data;

data.data_d=data_d;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать отчёт по пенсионерам и инвалидам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_pi_r_key_press),&data);

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

repl.plus(gettext("Распечатать отчёт по пенсионерам и инвалидам"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_pi_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_pi_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_pi_r_v_knopka(GtkWidget *widget,class zar_pi_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_pi_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_pi_r_data *data)
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

/******************************************/
/******************************************/

gint zar_pi_r1(class zar_pi_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mr,gr;
iceb_u_rsdat1(&mr,&gr,data->data_d);
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

FILE *ff;
char imaf[32];
sprintf(imaf,"sdpf%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kolih_inv=0;
class iceb_u_int sppi;
zar_sdpf_ps(&sppi,&kolih_inv,data->window);

int kolih=sppi.kolih();

fprintf(ff,"\
                               Список працюючих пенсионерів та інвалідів по\n\
                               %s\n\
                                        Станом на %d.%d.%d\n",organ,dt,mt,gt);
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 N  |  Призвище, ім'я по батькові  |     Домашня адреса           |   ІПН    |Дата наро.|Дата прий.|З/п за %02d.%d\n",
 mr,gr);
/*
1234 123456789012345678901234567890 123456789012345678901234567890 1234567890 1234567890 1234567890
*/
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
int nomer=0;
char fio[512];
double suma=0.;
short drg,mrg,grg;
short dp,mp,gp;
char inn[32];
SQL_str row;
SQLCURSOR cur;
char adres[512];
int kolstr=0;
double itogo=0.;
float kolstr1=0.;
double itogo_inv_pen=0;
for(int ii=0; ii < kolih; ii++)
 {
  iceb_pbar(data->bar,kolih,++kolstr1);    

  if(ii == 0 && kolih_inv > 0)
   fprintf(ff,"%s\n",gettext("Инвалиды"));

  if(ii == kolih_inv)
   {
    fprintf(ff,"%*s:%10.2f\n",
    iceb_u_kolbait(99,gettext("Итого по инвалидам")),gettext("Итого по инвалидам"),itogo_inv_pen);
    fprintf(ff,"%s\n",gettext("Пенсионеры"));
    itogo_inv_pen=0;
   }  

  sprintf(strsql,"select fio,datn,inn,adres,denrog from Kartb where tabn=%d",sppi.ravno(ii));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),sppi.ravno(ii));
    iceb_menu_soob(strsql,data->window);
    continue;
   }  

  strncpy(fio,row[0],sizeof(fio)-1);
  iceb_u_rsdat(&dp,&mp,&gp,row[1],2);
  strncpy(inn,row[2],sizeof(inn)-1);
  strncpy(adres,row[3],sizeof(adres)-1);
  iceb_u_rsdat(&drg,&mrg,&grg,row[4],2);
  //Узнаём сумму начисленную
  sprintf(strsql,"select suma from Zarp where datz >= '%04d-%02d-%02d' \
and datz <= '%04d-%02d-%02d' and tabn=%d and prn='1'",
  gr,mr,1,gr,mr,31,sppi.ravno(ii));

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  suma=0.;  
  while(cur.read_cursor(&row) != 0)
   {
    suma+=atof(row[0]);    
   }

  sprintf(strsql,"%-5d %-*.*s %8.2f\n",sppi.ravno(ii),iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,suma);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  if(suma == 0.)
   continue;
  itogo+=suma;
  itogo_inv_pen+=suma;  
  
  fprintf(ff,"%-4d %-*.*s %-*.*s %-10s %02d.%02d.%04d %02d.%02d.%04d %10.2f\n",
  ++nomer,
  iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
  iceb_u_kolbait(30,adres),iceb_u_kolbait(30,adres),adres,
  inn,drg,mrg,grg,dp,mp,gp,suma);

  if(iceb_u_strlen(adres) > 30)
    fprintf(ff,"%4s %30s %s\n","","",iceb_u_adrsimv(30,adres));
  
 }

fprintf(ff,"%*s:%10.2f\n",
 iceb_u_kolbait(99,gettext("Итого по пенсионерам")),gettext("Итого по пенсионерам"),itogo_inv_pen);

char bros[512];
memset(bros,'\0',sizeof(bros));
class iceb_u_str imaf_nast("zarnast.alx");

iceb_poldan("Табельный номер руководителя",bros,imaf_nast.ravno(),data->window);

char fioruk[512];
memset(fioruk,'\0',sizeof(fioruk));
if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"Не знайдено табельного номера %s керівника !",bros);
    iceb_menu_soob(strsql,data->window);
   }
  else
   {
    strncpy(fioruk,row[0],sizeof(fioruk)-1);
   }
 } 

memset(bros,'\0',sizeof(bros));
iceb_poldan("Табельный номер бухгалтера",bros,imaf_nast.ravno(),data->window);
char fiobuh[512];
memset(fiobuh,'\0',sizeof(fiobuh));

if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"Не знайдено табельного номера %s головного бухгалтера!",bros);
    iceb_menu_soob(strsql,data->window);
   }
  else
   {
    strncpy(fiobuh,row[0],sizeof(fiobuh)-1);

   }
 }


fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(99,gettext("Итого")),gettext("Итого"),itogo);
fprintf(ff,"\n\
                      %*s__________________%s\n\n",iceb_u_kolbait(30,gettext("Руководитель")),gettext("Руководитель"),fioruk);
fprintf(ff,"\
                      %*s__________________%s\n\n",iceb_u_kolbait(30,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),fiobuh);
                         
iceb_podpis(ff,data->window);
fclose(ff);




data->imaf->plus(imaf);
data->naimf->plus(gettext("Список работающих пенсионеров и инвалидов"));

iceb_ustpeh(imaf,1,data->window);

data->kon_ras=0;
data->voz=0;
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
/*******************************/
/*получение списка пенсионеров и инвалидов*/
/*******************************************/

void  zar_sdpf_ps(class iceb_u_int *sppi,int *kolih_inv,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
*kolih_inv=0;
//FILE *ff;
class iceb_u_str imaf;

for(int ii=0; ii < 2; ii++)
 {
  if(ii == 0)
    imaf.new_plus("zarinv.alx");
  if(ii == 1)
    imaf.new_plus("zarpens.alx");
  
  sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf.ravno());
  if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }

  if(kolstr == 0)
   {
    return;
   }

  
  
  
  char v_pole[108];
  int tabn;

  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;
    if(iceb_u_polen(row_alx[0],v_pole,sizeof(v_pole),1,'|') != 0)
     continue;
    tabn=atoi(v_pole);
    if(tabn == 0)
     continue;
    sppi->plus(tabn,-1);
    if(ii == 0)
     *kolih_inv+=1;         
   }
 }



return;
}
