/*$Id: imp_nu_r.c,v 1.11 2011-02-21 07:35:52 sasa Exp $*/
/*10.03.2009	09.10.2006	Белых А.И.	imp_nu_r.c
импорт начислений/удержаний из файла
*/
#include  <stdlib.h>
#include  <math.h>
#include  <errno.h>
#include  <unistd.h>
#include        <sys/stat.h>
#include "buhg_g.h"

class imp_nu_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  class iceb_u_str imafz;
  short mz,gz;
  int prn;
  int kod_nah_ud;
        
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчет завершен 1-нет
  imp_nu_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_nu_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_nu_r_data *data);
gint imp_nu_r1(class imp_nu_r_data *data);
void  imp_nu_r_v_knopka(GtkWidget *widget,class imp_nu_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern double okrcn;
extern iceb_u_str shrift_ravnohir;

int imp_nu_r(const char *imaf,
short mz,short gz,
int prn,
int kod_nah_ud,
GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class imp_nu_r_data data;

data.imafz.new_plus(imaf);
data.mz=mz;
data.gz=gz;
data.prn=prn;
data.kod_nah_ud=kod_nah_ud;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
if(data.prn == 1)
  sprintf(strsql,"%s %s",name_system,gettext("Импорт начислений"));
if(data.prn == 2)
  sprintf(strsql,"%s %s",name_system,gettext("Импорт удержаний"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(imp_nu_r_key_press),&data);

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

GtkWidget *label=NULL;
if(data.prn == 1)
  label=gtk_label_new(gettext("Импорт начислений"));
if(data.prn == 2)
  label=gtk_label_new(gettext("Импорт удержаний"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(imp_nu_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)imp_nu_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_nu_r_v_knopka(GtkWidget *widget,class imp_nu_r_data *data)
{
printf("imp_nu_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   imp_nu_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_nu_r_data *data)
{
 printf("imp_nu_r_key_press\n");
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

gint imp_nu_r1(class imp_nu_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_clock sss(data->window);
struct stat work;
class ZARP     zp;





if(stat(data->imafz.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Нет найден файл"),data->imafz.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

FILE *ff=NULL;

if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafprom[50];
FILE *ffprom=NULL;

sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  iceb_er_op_fil(imafprom,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
SQL_str row;
class SQLCURSOR cur;

short den=0;
char  shet[40];
short mvr,gvr;
int   podr=0;
short nomzap;
double suma=0.;
char koment[512];
short denp=0;
class OPSHET	shetv;
iceb_u_dpm(&denp,&data->mz,&data->gz,5);
float razmer=0.;
short dn,mn,gn;
short du,mu,gu;
char	shetkar[20];
int podrkar=0;
int kateg;
int sovm;
int zvan;
char lgot[40];
char dolg[60];
char strok[1024];
int i;
short metkadat=0;
short dzz=1,mzz=data->mz,gzz=data->gz;
int tabnom=0;

while(fgets(strok,sizeof(strok),ff) != NULL)
 {
//  printf("%s",strok);
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каретки меняем на пробел
   strok[i-2]=' ';
  
  
  iceb_printw(iceb_u_toutf(strok),data->buffer,data->view);

  iceb_pbar(data->bar,work.st_size,razmer);    
  
//  strzag(LINES-1,0,work.st_size,razmer);
   
  if(iceb_u_polen(strok,strsql,sizeof(strsql),1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

 
  if(iceb_u_SRAV(strsql,"Дата",0) == 0)
   {
    fprintf(ffprom,"%s",strok); //Всегда записываем
    metkadat=0;
    iceb_u_polen(strok,strsql,sizeof(strsql),2,'|');    
    dzz=1;
    if(iceb_u_rsdat1(&mzz,&gzz,strsql) != 0)
     {
      iceb_printw(gettext("Не верно введена дата !"),data->buffer,data->view);
      iceb_printw("\n",data->buffer,data->view);
      
      iceb_menu_soob(gettext("Не верно введена дата !"),data->window);
      metkadat=1;      
      continue;
     }
    denp=1;
    iceb_u_dpm(&denp,&mzz,&gzz,5);
    continue;
   }

  if(metkadat == 1)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  
  if((tabnom=atoi(strsql)) == 0)
   {
    sprintf(strsql,"%s%s\n",strok,"Табельный номер равен нолю");
    fprintf(ffprom,"%s",strsql);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    continue;
   }

  //Проверяем есть ли такой табельный номер  
  sprintf(strsql,"select datn,datk,shet,podr,kodg,kateg,\
sovm,zvan,lgoti,dolg from Kartb where tabn=%d",tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabnom);
    
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    iceb_printw("\n",data->buffer,data->view);
    
    iceb_menu_soob(strsql,data->window);
    fprintf(ffprom,"%s",strok);
    continue;
   }

  iceb_u_rsdat(&dn,&mn,&gn,row[0],2);
  iceb_u_rsdat(&du,&mu,&gu,row[1],2);
  strncpy(shetkar,row[2],sizeof(shetkar)-1);
  podrkar=atoi(row[3]);
  kateg=atoi(row[5]);
  sovm=atoi(row[6]);
  zvan=atoi(row[7]);
  memset(lgot,'\0',sizeof(lgot));
  strncpy(lgot,row[8],sizeof(lgot)-1);
  memset(dolg,'\0',sizeof(dolg));
  strncpy(dolg,row[9],sizeof(dolg)-1);

  if(iceb_u_polen(strok,strsql,sizeof(strsql),3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  den=atoi(strsql);

  if(den < 1 && den > denp)
   {
    
    iceb_printw(gettext("Неправильно введён день начисления/удержания !"),data->buffer,data->view);
    iceb_printw("\n",data->buffer,data->view);

    iceb_menu_soob(gettext("Неправильно введён день начисления/удержания !"),data->window);
    fprintf(ffprom,"%s",strok);
    continue;
   }
  if(iceb_u_polen(strok,shet,sizeof(shet),4,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

   
  //Проверить есть ли счет в плане счетов
  if(iceb_prsh1(shet,&shetv,data->window) != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  //Дата в счет какого месяца
  if(iceb_u_polen(strok,strsql,sizeof(strsql),5,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  if(iceb_u_rsdat1(&mvr,&gvr,strsql) != 0)
   {
    iceb_menu_soob(gettext("Неправильно введено в счёт какого месяца !"),data->window);
    fprintf(ffprom,"%s",strok);
    continue;
   }
  
  //Подразделение     
  if(iceb_u_polen(strok,strsql,sizeof(strsql),6,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  podr=atoi(strsql);
  //Проверяем код подразделения
  sprintf(strsql,"select naik from Podr where kod=%d",podr);

  if(iceb_sql_readkey(strsql,data->window) != 1)  
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr);

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    iceb_printw("\n",data->buffer,data->view);

    iceb_menu_soob(strsql,data->window);
    fprintf(ffprom,"%s",strok);
    continue;
   }

  //Номер записи
  if(iceb_u_polen(strok,strsql,sizeof(strsql),7,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  nomzap=atoi(strsql);

  //Сумма
  if(iceb_u_polen(strok,strsql,sizeof(strsql),8,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
  suma=iceb_u_atof(strsql);
  if(suma == 0.)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }
   
  if(data->prn == 2)
   suma*=-1;
   
  //Коментарий
  if(iceb_u_polen(strok,koment,sizeof(koment),9,'|') != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }

  zp.tabnom=tabnom;
  zp.prn=data->prn;
  zp.knu=data->kod_nah_ud;
  zp.dz=den;
  zp.mz=data->mz;
  zp.gz=data->gz;
  zp.mesn=mvr; zp.godn=gvr;
  zp.nomz=0;
  zp.podr=podr;
  strcpy(zp.shet,shet);
      
  if(zapzarpvw(&zp,suma,den,mvr,gvr,0,0,0,0,shet,koment,nomzap,podr,"",data->window) != 0)
   {
    fprintf(ffprom,"%s",strok);
    continue;
   }   

  if(data->prn == 1)
   zarsocw(mzz,gzz,tabnom,NULL,data->window);
  zaravprw(tabnom,mzz,gzz,NULL,data->window);
  int prov=0;
  
  //Проверяем есть ли запись карточки по этой дате и если нет делаем запись
  sprintf(strsql,"select god from Zarn where tabn=%d and god=%d and mes=%d",tabnom,gzz,mzz);
  if(iceb_sql_readkey(strsql,data->window) == 0)  
    zapzarnw(mzz,gzz,podrkar,tabnom,kateg,sovm,zvan,shetkar,lgot,dn,mn,gn,du,mu,gu,prov,dolg,data->window);

 }

fclose(ff);
fclose(ffprom);
unlink(data->imafz.ravno());

rename(imafprom,data->imafz.ravno());




data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);



//repl.new_plus(gettext("Загрузка завершена"));
//repl.ps_plus(gettext("Количество документов"));
//repl.plus(":");
//repl.plus(koldok);

//iceb_menu_soob(&repl,data->window);


data->voz=0;

return(FALSE);
}
