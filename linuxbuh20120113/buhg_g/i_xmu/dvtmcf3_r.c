/*$Id: dvtmcf3_r.c,v 1.22 2011-02-21 07:35:51 sasa Exp $*/
/*09.06.2010	22.11.2004	Белых А.И.	dvtmcf3_r.c
Расчёт движения товарно-материальных ценностей 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "dvtmcf3_r.h"

#define         PL      45  /*Количество строк на первом листе*/
#define         VL      55  /*Количество строк на втором листе*/

gboolean   dvtmcf3_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf3_r_data *data);
gint dvtmcf3_r1(class dvtmcf3_r_data *data);
void  dvtmcf3_r_v_knopka(GtkWidget *widget,class dvtmcf3_r_data *data);

void	sozshgr(iceb_u_int *GRUP,iceb_u_spisok *SHET,int *kolgrup,int *kolshet,GtkWidget *wpredok);
void  sapitsh_f2(FILE *ff);
void	rasgshshk(short dn,short mn,short gn,short dk,short mk,short gk,
char *shetz,char *kodmz,char *grz,char *kodop,char *korz,char *sklz,
char *nomdokp,char *ndsc,char *imaf,iceb_u_spisok *GSHSHK,
iceb_u_double *SUMGSHSHK,GtkWidget *wpredok);

void	rasgrsh(char *imaf,short dn,short mn,short gn,short dk,short mk,short gk,
iceb_u_int *GRUP,iceb_u_spisok *SHET,int kolgrup,int kolshet,double *sumshgr,
char *shetz,char *kodmz,char *grz,char *kodop,char *korz,
char *sklz,char *nomdokp,char *ndsc,GtkWidget *wpredok);

void gsapp1(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,
int *kst,FILE *ff);

void rasitogo(int metka,double mas[2],int *klst,char kod[],FILE *ff,short metoth,class dvtmcf3_r_data *data);

void  dvtmcf3ot(char imafsort[],short dn,short mn,short gn,short dk,short mk,short gk,int kolstr2,short tipz,FILE *ff,
short metoth,class dvtmcf3_r_data *data);
void matsporw(class dvtmcf3_r_data *data,FILE *kaw,FILE *ff1);

int             matspow(class dvtmcf3_r_data *data);
int		matstatw(class dvtmcf3_r_data *data);
void            rasmothw(class dvtmcf3_r_data *data,int,FILE *ff,FILE *ff2);
short		matstatrw(class dvtmcf3_r_data *data,short dn,short mn,short gn,short dk,short mk,short gk,char imaf[]);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern double	okrg1;  /*Округление 1*/
extern double	okrcn;  /*Округление цены*/
extern int      kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;

int dvtmcf3_r(class dvtmcf3_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class dvtmcf3_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечетать движение товарно-материальных ценностей (форма 3)"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(dvtmcf3_r_key_press),&data);

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

repl.plus(gettext("Распечетать движение товарно-материальных ценностей (форма 3)"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(dvtmcf3_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)dvtmcf3_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  dvtmcf3_r_v_knopka(GtkWidget *widget,class dvtmcf3_r_data *data)
{
//printf("dvtmcf3_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dvtmcf3_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf3_r_data *data)
{
// printf("dvtmcf3_r_key_press\n");
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

gint dvtmcf3_r1(class dvtmcf3_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);



short dn,mn,gn;
short dk,mk,gk;
iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);


/*Создаем массивы операций-материалов*/
matspow(data);

matstatw(data);

sprintf(strsql,"%s\n",gettext("Массивы созданы"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);



sprintf(strsql,"select sklad,nomk,nomd,datdp,datd,tipz,kolih,cena \
from Zkart where \
datdp >= '%d-%02d-%02d' and datdp <= '%d-%02d-%02d'",
gn,mn,dn,gk,mk,dk);
/*printw("strsql=%s\n",strsql);*/
SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  repl.new_plus(gettext("Не найдено ни одной записи !"));
  iceb_menu_soob(&repl,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf3[30];
FILE *ff2;
sprintf(imaf3,"mot%d.tmp",getpid());
if((ff2 = fopen(imaf3,"w")) == NULL)
 {
  iceb_er_op_fil(imaf3,gettext("Аварийное завершение работы программы !"),errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    
char shu[32];
int kodm;
double cenak;
double cenad;
double cenaz=-1.;
short d,m,g;
char kor[32];
char kop[32];
int tipzz=0;
int kgrm;
char naimat[512];
char ei[32];
SQL_str row;
SQL_str row1;
double fas;

float kolstr1=0;
int kolih_strok=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(LINES-1,0,kolstr,++kolstr1);
/*
  printw("%s %s %s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
  refresh();
*/
  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
    continue;

   /*Читаем реквизиты карточки материалла*/
   sprintf(strsql,"select shetu,kodm,cena,ei,fas from Kart where \
sklad=%s and nomk=%s",
   row[0],row[1]);
   shu[0]='\0';
   kodm=0;   
   if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
    {
     repl.new_plus(gettext("Не нашли карточку"));
     repl.plus(" ");
     repl.plus(row[1]);
     repl.plus(" ");
     repl.plus(gettext("Склад"));
     repl.plus(" ");
     repl.plus(row[0]);
     iceb_menu_soob(&repl,data->window);     
     continue;
    }

  memset(shu,'\0',sizeof(shu));
  strncpy(shu,row1[0],sizeof(shu)-1);
  kodm=atoi(row1[1]);

  cenak=atof(row1[2]);
  cenak=iceb_u_okrug(cenak,okrcn);
  if(data->rk->metka_ceni == 2)
   cenad=cenak;
  else
   {
    cenad=atof(row[7]);
    cenad=iceb_u_okrug(cenad,okrcn);
   }

  memset(ei,'\0',sizeof(ei));
  strncpy(ei,row1[3],sizeof(ei)-1);
  fas=atof(row1[4]);
  
  if(iceb_u_proverka(data->rk->kodmat.ravno(),kodm,0,0) != 0)
      continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),shu,0,0) != 0)
    continue;

  /*Читаем шапку документа*/
  iceb_u_rsdat(&d,&m,&g,row[4],2);
  sprintf(strsql,"select kontr,kodop from Dokummat \
  where datd='%d-%02d-%02d' and sklad=%s and nomd='%s'",
  g,m,d,row[0],row[2]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {
    repl.new_plus(gettext("Не найден документ"));
    repl.plus(" ");
    repl.plus(row[2]);
    repl.ps_plus(gettext("Склад"));
    repl.plus(" ");
    repl.plus(row[0]);
    repl.ps_plus(gettext("Номер карточки"));
    repl.plus(" ");
    repl.plus(row[1]);
    iceb_menu_soob(&repl,data->window);        
    continue;
   }

  strncpy(kor,row1[0],sizeof(kor)-1);
  strncpy(kop,row1[1],sizeof(kop)-1);
    
  if(iceb_u_proverka(data->rk->kontr.ravno(),kor,0,0) != 0)
    continue;

  if(data->rk->grupa_kontr.getdlinna() > 1)
   {
    iceb_u_str kontr;
    iceb_00_kontr(kor,&kontr);

    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",kontr.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      if(iceb_u_proverka(data->rk->grupa_kontr.ravno(),row1[0],0,0) != 0)
        continue;
     }
    else
     {
      repl.new_plus(gettext("Не найден код контрагента"));
      repl.plus(" ");
      repl.plus(kontr.ravno());
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);
      
     }
   }


  if(tipzz != 0 && tipzz != atoi(row[5]))
   continue;

  if(cenaz != -1. && cenaz != cenad)
   continue;

  if(iceb_u_proverka(data->rk->kodop.ravno(),kop,0,0) != 0)
        continue;

  /*Читаем наименование материалла*/
  memset(naimat,'\0',sizeof(naimat));
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%d",kodm);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    repl.new_plus(gettext("Не найден код материалла"));
    repl.plus(" ");
    repl.plus(kodm);
    repl.plus(" !");
    iceb_menu_soob(&repl,data->window);
    continue;      
   }
 
  kgrm=atoi(row1[0]);
  strncpy(naimat,row1[1],sizeof(naimat)-1);

  if(iceb_u_proverka(data->rk->grupa_mat.ravno(),row1[0],0,0) != 0)
        continue;
  iceb_u_rsdat(&d,&m,&g,row[3],2);
  fprintf(ff2,"%s|%s|%s|%d|%.10g|%s|%s|%s|%.10g|%s|%s|%d.%d.%d|%d|%.10g\n",
  row[5],row[0],kop,kodm,cenad,kor,row[6],naimat,cenak,ei,row[2],
  d,m,g,kgrm,fas);
  kolih_strok++;

 }

fclose(ff2);

sprintf(strsql,"%s\n",gettext("Сортируем записи"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

//sprintf(strsql,"sort -o %s -t\\| +1n +2n %s",imaf3,imaf3);
sprintf(strsql,"sort -o %s -t\\| -k2,3n -k3,4n %s",imaf3,imaf3);
system(strsql);

sprintf(strsql,"%s\n",gettext("Конец сортировки"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if((ff2 = fopen(imaf3,"r")) == NULL)
 {
  iceb_er_op_fil(imaf3,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    

char imaf[30];
char imaf1[30];

sprintf(imaf,"ko%d.lst",getpid());
sprintf(imaf1,"kog%d.lst",getpid());


FILE *ff;
FILE *ff1;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    

fprintf(ff,"%s.\n",gettext("Движение материалов"));
fprintf(ff1,"%s.\n",gettext("Движение по группам материалов"));

fprintf(ff,"%s.\n",gettext("Отчет по карточкам материалов"));
fprintf(ff1,"%s.\n",gettext("Отчет по карточкам материалов"));
if(data->rk->metka_ras == 2)
 {
  fprintf(ff,"%s.\n",gettext("Отчет по доходу (цена реализации - учетная цена)"));
  fprintf(ff1,"%s.\n",gettext("Отчет по доходу (цена реализации - учетная цена)"));
 }
else
 {
  if(data->rk->metka_ceni == 2)
   {
    fprintf(ff,"%s.\n",gettext("Отчет по учетным ценам"));
    fprintf(ff1,"%s.\n",gettext("Отчет по учетным ценам"));
   }

  if(data->rk->metka_ceni == 1)
   { 
    fprintf(ff,"%s.\n",gettext("Отчет по ценам реализации"));
    fprintf(ff1,"%s.\n",gettext("Отчет по ценам реализации"));
   }
 }

iceb_u_zagolov(gettext("Движение товарно-материалных ценностей"),dn,mn,gn,dk,mk,gk,organ,ff1);

dvtmcf3_prp(data,ff1);



//rasmoth(&VV,tr,sklz,kodmz,kodoz,dn,mn,gn,dk,mk,gk,tipzz,cenaz,kopz,kk1,kk2,kk3,kk4,data->rk->metka_ras,ff,ff2);
rasmothw(data,kolih_strok,ff,ff2);


/*Распечатываем и закрываем массивы операций-материалов*/

//matspor(kk1,kk2,kk3,kk4,ff,ff1,data->rk->metka_ras);
matsporw(data,ff,ff1);

//matstatr(&VV,dn,mn,gn,dk,mk,gk,imaf2);
char imaf2[50];

matstatrw(data,dn,mn,gn,dk,mk,gk,imaf2);

iceb_podpis(ff,data->window);
iceb_podpis(ff1,data->window);


fclose(ff);
fclose(ff1);
fclose(ff2);
unlink(imaf3);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Движение материалов"));

data->rk->imaf.plus(imaf1);
data->rk->naimf.plus(gettext("Движение по группам материалов"));
data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Распечака динамики реализации"));

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
/*********************************/
/*Печать реквизитов поиска*/
/******************************/

void  dvtmcf3_prp(class dvtmcf3_r_data *data,FILE *ff1)
{
if(data->rk->sklad.ravno()[0] != 0)
  fprintf(ff1,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
if(data->rk->kontr.ravno()[0] != '\0')
  fprintf(ff1,"%s:%s\n",gettext("Контрагент"),data->rk->kontr.ravno());
if(data->rk->kodop.ravno()[0] != '\0')
  fprintf(ff1,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
if(data->rk->shet.ravno()[0] != '\0')
  fprintf(ff1,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());

if(data->rk->pr.getdlinna() > 1)
  fprintf(ff1,"%s:%s\n",gettext("Приход/расход"),data->rk->pr.ravno());



if(data->rk->vcena.getdlinna() > 1)
  fprintf(ff1,"%s:%s\n",gettext("Отпускная цена"),data->rk->vcena.ravno());


if(data->rk->grupa_mat.getdlinna() > 1)
  fprintf(ff1,"%s:%s\n",gettext("Код группы материалла"),data->rk->grupa_mat.ravno());


if(data->rk->kodmat.ravno()[0] != 0)
  fprintf(ff1,"%s:%s\n",gettext("Материал"),data->rk->kodmat.ravno());

if(data->rk->grupa_kontr.ravno()[0] != 0)
 {
  fprintf(ff1,"%s:%s\n",gettext("Группа контрагента"),data->rk->grupa_kontr.ravno());
  int klst=0;
  iceb_printcod(ff1,"Gkont","kod","naik",0,data->rk->grupa_kontr.ravno(),&klst);
 }
}
