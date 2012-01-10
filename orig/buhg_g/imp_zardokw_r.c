/*$Id: imp_zardokw_r.c,v 1.8 2011-02-21 07:35:52 sasa Exp $*/
/*05.11.2008	10.03.2006	Белых А.И.	imp_zardokw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include        <sys/stat.h>
#include <errno.h>
#include "buhg_g.h"
#include "imp_zardokw.h"

class imp_zardokw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class imp_zardokw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  imp_zardokw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   imp_zardokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_zardokw_r_data *data);
gint imp_zardokw_r1(class imp_zardokw_r_data *data);
void  imp_zardokw_r_v_knopka(GtkWidget *widget,class imp_zardokw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;

int imp_zardokw_r(class imp_zardokw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class imp_zardokw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка списка документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(imp_zardokw_r_key_press),&data);

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

repl.plus(gettext("Распечатка списка документов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(imp_zardokw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)imp_zardokw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_zardokw_r_v_knopka(GtkWidget *widget,class imp_zardokw_r_data *data)
{
printf("imp_zardokw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   imp_zardokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_zardokw_r_data *data)
{
 printf("imp_zardokw_r_key_press\n");
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

gint imp_zardokw_r1(class imp_zardokw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;



struct stat work;

if(stat(data->rk->imaf_imp.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Не найден файл"),data->rk->imaf_imp.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

FILE *ff=NULL;

if((ff = fopen(data->rk->imaf_imp.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->rk->imaf_imp.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }




char imafprom[50];
sprintf(imafprom,"prom%d.tmp",getpid());
FILE *ffprom;
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  iceb_er_op_fil(imafprom,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->rk->datadok.ravno(),1);
printf("%d.%d.%d\n",dd,md,gd);

char strok[1024];
int i=0;
int razmer=0;
int tabnom=0;
int knah=0;
double suma=0.;
char shet[50];
int podr=0;

struct ZARP     zp;
memset(&zp,'\0',sizeof(zp));
zp.prn=data->rk->prn;
zp.dz=0; /*Для того чтобы не удаляласть запись перед записью новой*/
zp.mz=md;
zp.gz=gd;
zp.godn=gd;
zp.mesn=md;

int nomzap=0;
int voz=0;
short dnr,mnr,gnr;
short dkr,mkr,gkr;
char shetkar[50];
char dolg[512];
int kateg=0;
int sovm=0;
char lgoti[30];
int zvan=0;
class iceb_u_str koment;

while(fgets(strok,sizeof(strok),ff) != NULL)
 {
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каретки меняем на пробел
   strok[i-2]=' ';

  sprintf(strsql,"%s",strok);
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
  
//   strzag(LINES-1,0,work.st_size,razmer);
  iceb_pbar(data->bar,work.st_size,razmer);    
  
  if(iceb_u_polen(strok,&tabnom,1,'|') != 0)
   continue;
  
  if(tabnom == 0)
   {
    fprintf(ffprom,"%s\n%s\n",strok,"Табельный номер равен нолю");
    continue;
   }
  
  /*Проверяем табельный номер и узнаём код подразделения,счёт в карточке*/
  sprintf(strsql,"select dolg,zvan,podr,kateg,datn,datk,shet,sovm,lgoti from Kartb where tabn=%d",tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
   
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabnom);
    iceb_menu_soob(strsql,data->window);
    
    fprintf(ffprom,"%s\n%s\n",strok,gettext("Не найден табельный номер"));
    continue;
    
   }
  memset(shetkar,'\0',sizeof(shetkar));
  memset(shet,'\0',sizeof(shet));
  memset(dolg,'\0',sizeof(dolg));
  memset(lgoti,'\0',sizeof(lgoti));
  
  strncpy(shet,row[6],sizeof(shet)-1);
  strncpy(shetkar,row[6],sizeof(shetkar)-1);
  podr=atoi(row[2]);
  kateg=atoi(row[3]);
  sovm=atoi(row[7]);
  strncpy(lgoti,row[8],sizeof(lgoti)-1);
  strncpy(dolg,row[0],sizeof(dolg)-1);
  iceb_u_rsdat(&dnr,&mnr,&gnr,row[4],2);
  iceb_u_rsdat(&dkr,&mkr,&gkr,row[5],2);
  zvan=atoi(row[1]);
      
  iceb_u_polen(strok,&knah,2,'|');
  if(knah == 0)
   {
    if(data->rk->prn == 1)
     {
      iceb_menu_soob("Код начисления равен нолю!",data->window);
      fprintf(ffprom,"%s\nКод начисления равен нолю!\n",strok);
     }
    if(data->rk->prn == 2)
     {
      iceb_menu_soob("Код удержания равен нолю!",data->window);
      fprintf(ffprom,"%s\nКод удержания равен нолю!\n",strok);
     }
    continue;
   }
  iceb_u_polen(strok,&koment,4,'|');
   
  /*Проверяем код начисления*/
  if(data->rk->prn == 1)
    sprintf(strsql,"select kod from Nash where kod=%d",knah);
  if(data->rk->prn == 2)
    sprintf(strsql,"select kod from Uder where kod=%d",knah);
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    if(data->rk->prn == 1)
     {
      sprintf(strsql,"Не найден код начисления %d !\n",knah);
      iceb_menu_soob(strsql,data->window);
      fprintf(ffprom,"%s\nНе найден код начисления %d!\n",strok,knah);
     }
    if(data->rk->prn == 2)
     {
      sprintf(strsql,"Не найден код удержания %d !\n",knah);
      iceb_menu_soob(strsql,data->window);
      fprintf(ffprom,"%s\nНе найден код удержания %d!\n",strok,knah);
     }
    continue;
   }

  iceb_u_polen(strok,&suma,3,'|');
  if(suma == 0.)
   {
    iceb_menu_soob("Сумма равняется нолю!",data->window);
    fprintf(ffprom,"%s\nСумма равняется нолю!\n",strok);
    continue;
   }

  /*Смотрим счёт в списке начислений на работника*/
  sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='%d' and knah=%d",tabnom,data->rk->prn,knah);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    memset(shet,'\0',sizeof(shet));
    strncpy(shet,row[0],sizeof(shet)-1);
   }
  else
   {
    if(data->rk->prn == 1) 
      sprintf(strsql,"select shet from Nash where kod=%d",knah);
    if(data->rk->prn == 2) 
      sprintf(strsql,"select shet from Uder where kod=%d",knah);
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      if(row[0][0] != '\0')
       {
        memset(shet,'\0',sizeof(shet));
        strncpy(shet,row[0],sizeof(shet)-1);
       }
     }
         
   }
  nomzap=0;
  zp.tabnom=tabnom;
  zp.knu=knah;
zapis:;  
  if(zapzarpvw(&zp,suma,dd,md,gd,0,0,0,0,shet,koment.ravno(),nomzap,podr,data->rk->nomdok.ravno(),data->window) != 0)
   {
    if(voz == 1)
     {
      nomzap++;
      goto zapis;
     }     
    fprintf(ffprom,"%s",strok);
    continue;
   }   

  zarsocw(md,gd,tabnom,NULL,data->window);
  zaravprw(tabnom,md,gd,NULL,data->window);
  
  //Проверяем есть ли запись карточки по этой дате и если нет делаем запись
  sprintf(strsql,"select god from Zarn where tabn=%d and god=%d and mes=%d",tabnom,gd,md);
  if(iceb_sql_readkey(strsql,data->window) == 0)  
    zapzarnw(md,gd,podr,tabnom,kateg,sovm,zvan,shetkar,lgoti,dnr,mnr,gnr,dkr,mkr,gkr,0,dolg,data->window);
 }

fclose(ff);
fclose(ffprom);
unlink(data->rk->imaf_imp.ravno());

rename(imafprom,data->rk->imaf_imp.ravno());




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}









