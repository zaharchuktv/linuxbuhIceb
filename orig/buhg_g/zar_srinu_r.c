/*$Id: zar_srinu_r.c,v 1.11 2011-02-21 07:36:00 sasa Exp $*/
/*20.11.2009	28.11.2006	Белых А.И.	zar_srinu_r.c
Расчёт списка работников имеющих начисление или удержание
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_srinu.h"

class zar_srinu_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_srinu_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_srinu_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_srinu_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_srinu_r_data *data);
gint zar_srinu_r1(class zar_srinu_r_data *data);
void  zar_srinu_r_v_knopka(GtkWidget *widget,class zar_srinu_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_srinu_r(class zar_srinu_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_srinu_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать список работников имеющих начисление или удержание"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_srinu_r_key_press),&data);

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

repl.plus(gettext("Распечатать список работников имеющих начисление или удержание"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_srinu_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_srinu_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_srinu_r_v_knopka(GtkWidget *widget,class zar_srinu_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_srinu_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_srinu_r_data *data)
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

gint zar_srinu_r1(class zar_srinu_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);
short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short mnh=0,gnh=0;
iceb_u_rsdat1(&mnh,&gnh,data->rk->v_mes.ravno());

sprintf(strsql,"select tabn,prn,knah,suma,mesn,godn,shet,datz,kdn from Zarp where \
datz >= '%d-%d-%d' and datz <= '%d-%d-%d' and suma <> 0. \
order by tabn asc",gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
SQLCURSOR curr;
int kolstr=0;
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
sprintf(imaf,"spi%d.lst",getpid());
FILE *kaw;
if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_sum[30];
sprintf(imaf_sum,"spiis%d.lst",getpid());
FILE *ffsum;
if((ffsum = fopen(imaf_sum,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_sum,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }




iceb_u_zagolov(gettext("Список работников имеющих начисление/удержание"),dn,mn,gn,dk,mk,gk,organ,kaw);


iceb_u_zagolov(gettext("Список работников имеющих начисление/удержание"),dn,mn,gn,dk,mk,gk,organ,ffsum);
int klst=0;
if(data->rk->podr.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код подраздедения"),data->rk->podr.ravno());
  fprintf(ffsum,"%s:%s\n",gettext("Код подраздедения"),data->rk->podr.ravno());
  iceb_printcod(kaw,"Podr","kod","naik",0,data->rk->podr.ravno(),&klst);
  iceb_printcod(ffsum,"Podr","kod","naik",0,data->rk->podr.ravno(),&klst);
 }
else
 fprintf(kaw,"%s\n",gettext("По всем подразделениям"));

if(data->rk->kod_nah.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код начисления"),data->rk->kod_nah.ravno());
  iceb_printcod(kaw,"Nash","kod","naik",0,data->rk->kod_nah.ravno(),&klst);
  fprintf(ffsum,"%s:%s\n",gettext("Код начисления"),data->rk->kod_nah.ravno());
  iceb_printcod(ffsum,"Nash","kod","naik",0,data->rk->kod_nah.ravno(),&klst);
 }
if(data->rk->kod_ud.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код удержания"),data->rk->kod_ud.ravno());
  iceb_printcod(kaw,"Uder","kod","naik",0,data->rk->kod_ud.ravno(),&klst);
  fprintf(ffsum,"%s:%s\n",gettext("Код удержания"),data->rk->kod_ud.ravno());
  iceb_printcod(ffsum,"Uder","kod","naik",0,data->rk->kod_ud.ravno(),&klst);
 }
if(data->rk->tabnom.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());
  fprintf(ffsum,"%s:%s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());
 }
if(data->rk->shet.ravno()[0] != '\0')
 { 
  fprintf(kaw,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  fprintf(ffsum,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
 }
if(data->rk->metka_pr == 0)
 { 
  fprintf(kaw,"%s.\n",gettext("Начисления"));
  fprintf(ffsum,"%s.\n",gettext("Начисления"));

 }
if(data->rk->metka_pr == 1)
 {
  fprintf(kaw,"%s.\n",gettext("Удеражания"));
  fprintf(ffsum,"%s.\n",gettext("Удержания"));
 }
if(data->rk->kod_kat.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код категории"),data->rk->kod_kat.ravno());
  fprintf(ffsum,"%s:%s\n",gettext("Код категории"),data->rk->kod_kat.ravno());
 }
if(data->rk->v_mes.getdlinna() > 1)
 {
  fprintf(kaw,"%s:%s\n",gettext("В счёт какого месяца"),data->rk->v_mes.ravno());
  fprintf(ffsum,"%s:%s\n",gettext("В счёт какого месяца"),data->rk->v_mes.ravno());
 }
fprintf(kaw,"-----------------------------------------------------------------------\n");
fprintf(ffsum,"-----------------------------------------------------------------------\n");

/****************************************/


int kolp=0;
double sum=0.;
int tabn=0,tabnz=0;
float kolstr1=0;
class iceb_u_spisok sp_tab;
class iceb_u_double sum_tab;
int nomer;
short d,m,g;
char kateg_z[20];
memset(kateg_z,'\0',sizeof(kateg_z));
char fio[512];
SQL_str row,row2;
char bros[512];
short mz,gz;
double sym=0.;
char kodpodr[30];
int prn;

while(cur.read_cursor(&row) != 0)
 {

//  kolstr1++;
//  strzag(LINES-1,0,kolstr,kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[6],0,0) != 0)
    continue;

  tabn=atol(row[0]);    
  if(tabn != tabnz)
   {
    memset(fio,'\0',sizeof(fio));
    memset(kateg_z,'\0',sizeof(kateg_z));
    
    sprintf(strsql,"select fio,podr,kateg from Kartb where tabn=%s",row[0]);
    if(iceb_sql_readkey(strsql,&row2,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %s !\n",gettext("Не найден табельный номер"),row[0]);
      iceb_menu_soob(strsql,data->window);
     }
    else
     {
      strncpy(fio,row2[0],sizeof(fio)-1);
      strncpy(kodpodr,row2[1],sizeof(kodpodr)-1);
      strncpy(kateg_z,row2[2],sizeof(kateg_z)-1);
     }
    tabnz=tabn;
   
   }

  if(iceb_u_proverka(data->rk->podr.ravno(),kodpodr,0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->kod_kat.ravno(),kateg_z,0,0) != 0)
    continue;

  prn=atoi(row[1]);

  if(prn == 1 && data->rk->metka_pr == 1)
     continue;
  if(prn == 2 && data->rk->metka_pr == 0)
     continue;
  
  if(prn == 1)
   if(iceb_u_proverka(data->rk->kod_nah.ravno(),row[2],0,0) != 0)
     continue;

     
  if(prn == 2)
   if(iceb_u_proverka(data->rk->kod_ud.ravno(),row[2],0,0) != 0)
     continue;

  sym=atof(row[3]);
  mz=atoi(row[4]);
  gz=atoi(row[5]);

  if(mnh != 0 && (mnh != mz || gnh != gz))
    continue;
    
  if(prn == 2 && data->rk->kod_nah.ravno()[0] == '\0')
    sym*=-1;
  iceb_u_rsdat(&d,&m,&g,row[7],2);
  memset(bros,'\0',sizeof(bros));
  iceb_u_prnb(sym,bros);
  
  sprintf(strsql,"%-7s %-*.*s %9s %d.%d %02d.%02d.%d\n",
  row[0],iceb_u_kolbait(25,fio),iceb_u_kolbait(25,fio),fio,bros,mz,gz,d,m,g);

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  fprintf(kaw,"%-7s %-*.*s %9s %02d.%d %-6s %02d.%02d.%d %s %s\n",
  row[0],iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,bros,mz,gz,row[6],d,m,g,row[8],row[2]);

  sum+=sym;
  kolp++;
  
  if((nomer=sp_tab.find(row[0])) == -1)
   sp_tab.plus(row[0]);
  sum_tab.plus(sym,nomer);
  
 }

fprintf(kaw,"-----------------------------------------------------------------------\n");
fprintf(kaw,"%s - %.2f\n%s - %d",
gettext("Итого"),sum,
gettext("Количество работников"),kolp);


sprintf(strsql,"----------------------------------------------\n");

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"%s - %.2f\n%s - %d\n",
gettext("Итого"),sum,
gettext("Количество работников"),kolp);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

iceb_podpis(kaw,data->window); 
fclose(kaw);


sum=0.;
int kolzap=sp_tab.kolih();
for(int i=0; i < kolzap; i++)
 {
  memset(fio,'\0',sizeof(fio));
  sprintf(strsql,"select fio from Kartb where tabn=%s",sp_tab.ravno(i));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   strncpy(fio,row[0],sizeof(fio)-1);
   
  fprintf(ffsum,"%-7s %-*.*s %10.2f\n",
  sp_tab.ravno(i),iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,sum_tab.ravno(i));
  sum+=sum_tab.ravno(i);
 }
fprintf(ffsum,"-----------------------------------------------------------------------\n");
fprintf(ffsum,"%*s:%10.2f",iceb_u_kolbait(38,gettext("Итого")),gettext("Итого"),sum);

iceb_podpis(ffsum,data->window); 
fclose(ffsum);


data->rk->imaf.plus(imaf);
data->rk->imaf.plus(imaf_sum);
data->rk->naimf.plus(gettext("Список работников имеющих начисление/удержание"));
data->rk->naimf.plus(gettext("Список работников имеющих начисление/удержание (свёрнутый)"));


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);




data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
