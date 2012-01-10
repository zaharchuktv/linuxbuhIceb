/*$Id: impusldok_r.c,v 1.11 2011-02-21 07:35:52 sasa Exp $*/
/*28.11.2005	28.11.2005	Белых А.И.	impusldok_r.c
импорт документов из файла
*/
#include  <stdlib.h>
#include  <math.h>
#include  <errno.h>
#include  <unistd.h>
#include        <sys/stat.h>
#include "buhg_g.h"

class impusldok_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  iceb_u_str imafz;
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчет завершен 1-нет
  impusldok_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   impusldok_r_key_press(GtkWidget *widget,GdkEventKey *event,class impusldok_r_data *data);
gint impusldok_r1(class impusldok_r_data *data);
void  impusldok_r_v_knopka(GtkWidget *widget,class impusldok_r_data *data);
int impusldokt1(int metkaproh,class impusldok_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern double okrcn;
extern iceb_u_str shrift_ravnohir;

int impusldok_r(const char *imaf,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class impusldok_r_data data;

data.imafz.new_plus(imaf);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Импорт документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(impusldok_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт документов"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(impusldok_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)impusldok_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impusldok_r_v_knopka(GtkWidget *widget,class impusldok_r_data *data)
{
printf("impusldok_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   impusldok_r_key_press(GtkWidget *widget,GdkEventKey *event,class impusldok_r_data *data)
{
 printf("impusldok_r_key_press\n");
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

gint impusldok_r1(class impusldok_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
struct stat work;

if(stat(data->imafz.ravno(),&work) != 0)
 {
  iceb_u_str repl;
  repl.new_plus(gettext("Нет такого файла !"));
  repl.ps_plus(data->imafz.ravno());
  iceb_menu_soob(&repl,data->window);
  return(FALSE);
 }

int koloh=0;
if((koloh=impusldokt1(0,data)) == 0)
 impusldokt1(1,data);
else
 {
  iceb_u_str repl;
  
  sprintf(strsql,"%s %d !",gettext("Количество ошибок"),koloh);
  repl.plus(strsql);
  repl.ps_plus(gettext("Импорт документов осуществляется если нет ошибок !"));
  iceb_menu_soob(&repl,data->window);
 } 


data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);


data->voz=0;

return(FALSE);
}
/****************************************/
/*Проверка-загрузка документов из файла*/
/****************************************/
int impusldokt1(int metkaproh,class impusldok_r_data *data)
{
FILE *ff;
char strsql[512];
struct stat work;
time_t tmm;
SQLCURSOR curr;

stat(data->imafz.ravno(),&work);

if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  return(1);
 }


if(metkaproh == 0)
 sprintf(strsql,"%s\n",gettext("Проверяем записи в файле"));
if(metkaproh == 1)
 sprintf(strsql,"\n\n%s\n\n",gettext("Загружаем документы из файла"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

SQL_str row1;
SQLCURSOR cur;

float razmer=0.;
char nomdok[32];
char nomnalnak[20];
memset(nomdok,'\0',sizeof(nomdok));
char strok[1024];
int i=0;
int koloh=0;
int tipz=0;
short dd,md,gd;
int kodpod=0;
char kontragent[20];
char kodop[32];
short nds=0;
int kodus=0;
double kolih,cena;
char shet[32];
char eiz[32];
OPSHET rekshet;
char dop_naim[70];
time(&tmm);

while(fgets(strok,sizeof(strok)-1,ff) != NULL)
 {
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каректки меняем на пробел
   strok[i-2]=' ';

  iceb_printw(iceb_u_toutf(strok),data->buffer,data->view);
//  printw("%s",strok);
//  printw("nomdok=%s %d\n",nomdok,metkaproh);
//  strzag(LINES-1,0,work.st_size,razmer);
  iceb_pbar(data->bar,work.st_size,razmer);    

  if(iceb_u_SRAV(strok,"DOCU|",1) == 0)
   {
    
    kodpod=nds=dd=md=gd=0;
    tipz=0;
    memset(nomdok,'\0',sizeof(nomdok));
    memset(nomnalnak,'\0',sizeof(nomnalnak));
    memset(kodop,'\0',sizeof(kodop));
    memset(kontragent,'\0',sizeof(kontragent));
        
    iceb_u_polen(strok,strsql,sizeof(strsql),2,'|');

    if(strsql[0] == '+')
      tipz=1;
    if(strsql[0] == '-')
      tipz=2;

    if(tipz == 0)
     {
      iceb_menu_soob(gettext("Не определен вид документа (приход/расход) !"),data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,strsql,sizeof(strsql),3,'|');
    if(iceb_u_rsdat(&dd,&md,&gd,strsql,1) != 0)
     {
      iceb_menu_soob(gettext("Не верно введена дата!"),data->window);
      koloh++;
      koloh++;
      continue;
     }
    iceb_u_polen(strok,strsql,sizeof(strsql),4,'|');
    kodpod=atoi(strsql);
    sprintf(strsql,"select kod from Uslpodr where kod=%d",kodpod);
    if(sql_readkey(&bd,strsql) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),kodpod);
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,kontragent,sizeof(kontragent),5,'|');
    sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",
    kontragent);
    if(sql_readkey(&bd,strsql) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontragent);
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }


    iceb_u_polen(strok,nomnalnak,sizeof(nomnalnak),7,'|');

    if(tipz == 2 && nomnalnak[0] != '\0')
     {
      sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
      tipz,gd,gd,nomnalnak);
      if(sql_readkey(&bd,strsql,&row1,&cur) >= 1)
       {
        iceb_u_str repl;
        
        sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak);
        repl.plus(strsql);
        
        sprintf(strsql,"%s %s %s",gettext("Материальный учет"),row1[0],row1[1]);
        repl.ps_plus(strsql);

        iceb_menu_soob(&repl,data->window);
        koloh++;
        continue;
       }

      sprintf(strsql,"select datd,nomd from Usldokum where tp=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
      tipz,gd,gd,nomnalnak);
      if(sql_readkey(&bd,strsql,&row1,&curr) >= 1)
       {
        iceb_u_str repl;
        
        sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak);
        repl.plus(strsql);

        sprintf(strsql,"%s %s %s",gettext("В учете услуг"),row1[0],row1[1]);
        repl.ps_plus(strsql);

        iceb_menu_soob(&repl,data->window);

        koloh++;
        continue;
       }
     }

    iceb_u_polen(strok,kodop,sizeof(kodop),8,'|');
    if(tipz == 1)
      sprintf(strsql,"select kod from Usloper1 where kod='%s'",
      kodop);
    if(tipz == 2)
      sprintf(strsql,"select kod from Usloper2 where kod='%s'",
      kodop);

    if(sql_readkey(&bd,strsql) != 1)
     {
      iceb_u_str repl;
      repl.new_plus(gettext("Не найден код операции"));
      repl.plus(" ");
      repl.plus(kodop);
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    //Номер документа определяем последним так как он является индикатором
    //наличия шапки документа
    iceb_u_polen(strok,nomdok,sizeof(nomdok),6,'|');
//  printw("***nomdok=%s %d\n",nomdok,metkaproh);
 
    sprintf(strsql,"select nomd from Usldokum where tp=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and podr=%d and nomd='%s'",
    tipz,gd,gd,kodpod,nomdok);
    if(sql_readkey(&bd,strsql) >= 1)
     {
      sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdok);
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    //Запись в базу
    if(metkaproh != 0 && nomdok[0] != '\0')
     {
      sprintf(strsql,"insert into Usldokum \
(tp,datd,podr,kontr,nomd,nomnn,kodop,nds,ktoi,vrem) \
values (%d,'%04d-%02d-%02d',%d,'%s','%s','%s','%s',%d,%d,%ld)",
      tipz,gd,md,dd,kodpod,kontragent,nomdok,nomnalnak,kodop,nds,iceb_getuid(data->window),tmm);
//      printw("%s\n",strsql);
      iceb_sql_zapis(strsql,1,0,data->window);
     }        
    
   }

  if(iceb_u_SRAV(strok,"ZPU|",1) == 0)
   {

    iceb_u_polen(strok,strsql,sizeof(strsql),2,'|');
    kodus=atoi(strsql);    
    sprintf(strsql,"select kodus from Uslugi where kodus=%d",kodus);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodus);
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }
    iceb_u_polen(strok,strsql,sizeof(strsql),3,'|');
    kolih=atof(strsql);
    if(kolih == 0)
     {
      sprintf(strsql,"%s !",gettext("В записи нулевое количество"));
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,strsql,sizeof(strsql),4,'|');
    cena=atof(strsql);
    cena=iceb_u_okrug(cena,okrcn);

    iceb_u_polen(strok,eiz,sizeof(eiz),5,'|');
    sprintf(strsql,"select kod from Edizmer where kod='%s'",eiz);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      iceb_u_str repl;
      repl.new_plus(gettext("Не найдена единица измерения"));
      repl.plus(" ");
      repl.plus(eiz);
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,shet,sizeof(shet),6,'|'); 
    if(iceb_prsh1(shet,&rekshet,data->window) != 0)
     {
      koloh++;
      continue;
     }

    memset(dop_naim,'\0',sizeof(dop_naim));
    iceb_u_polen(strok,dop_naim,sizeof(dop_naim),7,'|');
    

    //Запись в базу
    if(metkaproh != 0 && nomdok[0] != '\0')
      zapuvdokw(tipz,dd,md,gd,nomdok,1,kodus,kolih,cena,eiz,shet,kodpod,"",dop_naim,data->window);

   }
   

 }

fclose(ff);

return(koloh);
}
