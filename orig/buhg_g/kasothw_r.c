/*$Id: kasothw_r.c,v 1.37 2011-02-21 07:35:52 sasa Exp $*/
/*24.11.2010	10.03.2006	Белых А.И.	kasothw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "kasothw.h"
#define		KOLSTRHAP 9 //Количество строк в шапке

class kasothw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class kasothw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  kasothw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   kasothw_r_key_press(GtkWidget *widget,GdkEventKey *event,class kasothw_r_data *data);
gint kasothw_r1(class kasothw_r_data *data);
void  kasothw_r_v_knopka(GtkWidget *widget,class kasothw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;

int kasothw_r(class kasothw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class kasothw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка списка документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(kasothw_r_key_press),&data);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(kasothw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)kasothw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kasothw_r_v_knopka(GtkWidget *widget,class kasothw_r_data *data)
{
//printf("kasothw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kasothw_r_key_press(GtkWidget *widget,GdkEventKey *event,class kasothw_r_data *data)
{
// printf("kasothw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/*******/
/*Шапка*/
/*******/

void	sapkasord(short dd,short md,short gd,int nomlist,const char *zagolov,FILE *ffva)
{
char mesqc[32];
iceb_mesc(md,1,mesqc);

fprintf(ffva,"%s\n",zagolov);
fprintf(ffva,"%s \"%02d\" %s %d%s                     %s %d\n",
gettext("Касса за"),
dd,mesqc,gd,
gettext("г."),
gettext("Лист"),nomlist);

fprintf(ffva,"\
-------------------------------------------------------------------------\n");
fprintf(ffva,"\
 Номер  |  Від кого отримано чи кому   |  Номер   |  Надход- | Видаток  |\n\
докумен-|         видано               |кореспон- |  ження   |          |\n\
  та    |                              |дуючого   |          |          |\n\
        |                              |рахунку,  |          |          |\n\
        |                              |субрахунку|          |          |\n");
fprintf(ffva,"\
-------------------------------------------------------------------------\n");
/*
12345678 123456789012345678901234567890 1234567890 1234567890 1234567890
*/

}
/***************************/
/*Шапка журнала регистрации кассовых документов*/
/***********************************************/

void   hgrkd(FILE *ff)
{
fprintf(ff,"\
-------------------------------------------------------------------------------------\n\
|Прибутковий док.   |          |          |Видатковий док.    |          |          |\n\
---------------------  Сума    |Примітки  |--------------------  Сума    |Примітки  |\n\
|  дата    | номер  |          |          |  дата    | номер  |          |          |\n\
-------------------------------------------------------------------------------------\n");
}
/*************************/
/*Счётчик строк*/
/*******************/
void kasothr_ss(int metka, //0-счётчик применять 1-не применять
short dd,short md,short gd,int *nomlist,int *kolstr,double itpri,double itras,FILE *ffva,FILE *ffok,FILE *ff_prot)
{
if(metka == 0)
 {
  *kolstr+=1;
  if(*kolstr <= kol_strok_na_liste-1)
   return;
  *kolstr=1;
 }
else
 *kolstr=0;
 
*nomlist+=1;
*kolstr+=KOLSTRHAP;
if(ffva != NULL)
 {
  fprintf(ffva,"%*s:%10.2f %10.2f\n",
  iceb_u_kolbait(50,gettext("Перенос")),gettext("Перенос"),itpri,itras*-1);
  fprintf(ffva,"\f");
  sapkasord(dd,md,gd,*nomlist,gettext("Вкладной лист кассовой книги"),ffva);
 }
if(ffok != NULL)
 {
  fprintf(ffok,"%*s:%10.2f %10.2f\n",
  iceb_u_kolbait(50,gettext("Перенос")),gettext("Перенос"),itpri,itras*-1);
  fprintf(ffok,"\f");
  sapkasord(dd,md,gd,*nomlist,gettext("Отчет кассира"),ffok);
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"Вывели шапку kolstr=%d nomlist=%d\n",*kolstr,*nomlist); 
}

/******************************************/
/******************************************/

gint kasothw_r1(class kasothw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
char kod_edr[32];
SQL_str row,row1;
SQLCURSOR cur,cur1;

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

memset(kod_edr,'\0',sizeof(kod_edr));
//Читаем код
sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 strncpy(kod_edr,row[0],sizeof(kod_edr)-1);


char imaf[20];
FILE *ff;

sprintf(imaf,"kassa%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf1[56];
FILE *ffva;

sprintf(imaf1,"kasg%d.lst",getpid());
if((ffva = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafok[56];
FILE *ffok;

sprintf(imafok,"kasok%d.lst",getpid());
if((ffok = fopen(imafok,"w")) == NULL)
 {
  sss.clear_data();
  iceb_er_op_fil(imafok,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_prot[56];
FILE *ff_prot=NULL;
sprintf(imaf_prot,"kassapr%d.lst",getpid());

if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  sss.clear_data();
  iceb_er_op_fil(imafok,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


int kolstrok=1; //строка с наименованием отчёта
char imaf2[56];
FILE *ff2;

sprintf(imaf2,"kassf%d.lst",getpid());
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



if(iceb_u_sravmydat(dn,mn,gn,16,6,2009) < 0)
 {
  fprintf(ff2,"%s\n",organ);
  fprintf(ff2,"%55s Типова форма N КО-3а\n"," ");
  fprintf(ff2,"%55s ЗАТВЕРДЖЕНА\n"," ");
  fprintf(ff2,"%55s Наказом Мінстату України\n"," ");
  fprintf(ff2,"%55s від 15 лютого 1996р. N51\n"," ");

  fprintf(ff2,"%s %s\n",gettext("Идентификационный код по ЕГРПОУ"),kod_edr);
 }
else
 {
  fprintf(ff2,"\
                                     Додаток 4\n\
                                     до Положення про ведення касових операцій\n\
                                     у національній валюті в Україні\n\
                                     Типова форма N КО-3a\n");
  fprintf(ff2,"\n\
%-30s Ідентифікаційний код ЄДРПОУ %s\n",organ,kod_edr);  
 
 }
fprintf(ff2,"%30s %s\n","",gettext("ЖУРНАЛ"));
fprintf(ff2,"%7s %s\n","",gettext("регистрации приходных и расходных кассовых документов"));

iceb_u_zagolov("",dn,mn,gn,dk,mk,gk,"",ff2);
  

//Вычисляем сальдо на начало периода
double saldon=0.;

sprintf(strsql,"select kassa,tp,datd,nomd,suma from Kasord1 where \
datd >= '%04d-01-01' and datd < '%04d-%d-%d' and datp <> '0000-00-00' order by datd,nomd asc",
gn,gn,mn,dn);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

 
short d=0,m=0,g=0,dd=0,md=0,gd=0;
char nomdz[56];
memset(nomdz,'\0',sizeof(nomdz));
double kolih_prih_dok=0.;
double kolih_rash_dok=0.;

int kollist=1; 
kolstrok=KOLSTRHAP;
float kolstr1=0;
int koldok=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kassa.ravno(),row[0],0,0) != 0)
    continue;
  iceb_u_rsdat(&dd,&md,&gd,row[2],2);


  if(iceb_u_SRAV("000",row[3],0) == 0)
   {
    if(gd == gn)
     {
      saldon+=atof(row[4]);
     }
    continue;
   }
  if(iceb_u_sravmydat(d,m,g,dd,md,gd) != 0)
   {
    if(d != 0)
     {
      /*Вывод шапки так как поменялься день*/
      kasothr_ss(1,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,ff_prot);
      if(d != dd)
        kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,ff_prot);/*строка сальдо*/
     
     }
    else
      kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,ff_prot); /*остаток на начало дня*/
  
     
    d=dd; m=md; g=gd;
   }      

  if(data->rk->metka_pr != 0)
   {
    if(data->rk->metka_pr == 1 && row[1][0] == '2')
      continue;
    if(data->rk->metka_pr == 2 && row[1][0] == '1')
      continue;
   }

  if(iceb_u_SRAV(nomdz,row[3],0) != 0)
   {
    koldok++;
    kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,ff_prot);

    if(ff_prot != NULL)
     fprintf(ff_prot,"%s %s %s %s %s kolstrok=%d\n",row[0],row[1],row[2],row[3],row[4],kolstrok);

    strcpy(nomdz,row[3]);
   }    


  if(row[1][0] == '1')
   {
    saldon+=atof(row[4]);
   }
  else
   {
    saldon-=atof(row[4]);
   }
 }

if(ff_prot != NULL)
  fprintf(ff_prot,"===Количество строк=%d Номер листа =%d\n",kolstrok,kollist);

double saldoden=saldon;

sprintf(strsql,"%s %02d.%02d.%d :%.2f\n",
gettext("Сальдо на"),dn,mn,gn,saldon);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

sprintf(strsql,"select kassa,tp,datd,nomd,shetk,kodop,fio,osnov,dopol,nb \
from Kasord where datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' and podt=1 \
order by datd,tp,nomd asc",
gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  sss.clear_data();
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  fprintf(ff,"%s\n",gettext("Не найдено ни одной записи !"));
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


if(data->rk->metka_pr == 0)
  strcpy(strsql,gettext("Регистрация кассовых ордеров"));
if(data->rk->metka_pr == 1)
  strcpy(strsql,gettext("Регистрация приходних кассовых ордеров"));
if(data->rk->metka_pr == 2)
  strcpy(strsql,gettext("Регистрация расходных кассовых ордеров"));
  

iceb_u_zagolov(strsql,dn,mn,gn,dk,mk,gk,organ,ff);

hgrkd(ff2);


if(data->rk->kassa.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Касса"),data->rk->kassa.ravno());
if(data->rk->kodop.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
if(data->rk->shet.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счет"),data->rk->shet.ravno());

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Касса|+/-|Номер док.|Дата док. |Сумма док.|Номер бланка |            Фамилия           |Операция \n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");


d=m=g=dd=md=gd=0;
double itogp=0.;
double itogr=0.;
double suma_p=0.;
double suma_r=0.;
double sumd=0.;
double sumpod=0.;
char fio[512];
char naimop[512];
double itpri=0.,itras=0.,isumd=0.,isumpod=0.;

if(koldok > 0)
  kollist+=1;
int tipz=0;
short metka_sap=0;
kolstrok=KOLSTRHAP;
char vpr1[8];
kolstr1=0.;
int kollist_nah=kollist;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kassa.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodop.ravno(),row[5],0,0) != 0)
    continue;
  iceb_u_rsdat(&dd,&md,&gd,row[2],2);

  if(metka_sap == 0) /*В шапке должна быть дата первого документа*/
   {
    sapkasord(dd,md,gd,kollist,gettext("Вкладной лист кассовой книги"),ffva);
    sapkasord(dd,md,gd,kollist,gettext("Отчет кассира"),ffok);
    metka_sap++;
   }  

  if(data->rk->kod_kontr.getdlinna() > 1)
   {
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and god=%d and tp=%s and nomd='%s'",
    row[0],gd,row[1],row[3]);

    if(cur1.make_cursor(&bd,strsql) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    int metka_kontr=0;
    while(cur1.read_cursor(&row1) != 0)
     {
      if(iceb_u_proverka(data->rk->kod_kontr.ravno(),row1[0],0,0) == 0)
       {
        metka_kontr=1;
        break;
        
       } 
     }
    if(metka_kontr == 0)
     continue;
   }
  
  tipz=atoi(row[1]);
  
  if(iceb_u_sravmydat(d,m,g,dd,md,gd) != 0)
   {
    if(d != 0)
     {
      /*Выводим шапку так как поменялся день*/
      kasothr_ss(1,dd, md, gd,&kollist,&kolstrok,itpri,itras,ffva,ffok,ff_prot);
      if(d != dd)
       {
        kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,itpri,itras,ffva,ffok,ff_prot);
        fprintf(ffva,"%*s:%10.2f\n",
        iceb_u_kolbait(50,gettext("Сальдо")),gettext("Сальдо"),isumd+saldon);
        fprintf(ffok,"%*s:%10.2f\n",
        iceb_u_kolbait(50,gettext("Сальдо")),gettext("Сальдо"),isumd+saldon);
       }      
     }

    if(d == 0)    
     {
      if(iceb_u_SRAV("000",row[3],0) == 0)
       {
        sumkasorw(row[0],tipz,row[3],gd,&sumd,&sumpod,data->window);
        if(tipz == 2)
          sumd*=-1;

        saldoden+=sumd;
        saldon+=sumd;
       }

      kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,itpri,itras,ffva,ffok,ff_prot);
      sprintf(strsql,"%s",gettext("Остаток на начало дня"));
      fprintf(ffva,"%*s:%10.2f\n",iceb_u_kolbait(50,strsql),strsql,saldoden);
      fprintf(ffok,"%*s:%10.2f\n",iceb_u_kolbait(50,strsql),strsql,saldoden);
     }

    d=dd; m=md; g=gd;
    if(iceb_u_SRAV("000",row[3],0) == 0)
      continue;
   }      

  
  if(data->rk->metka_pr != 0)
   {
    if(data->rk->metka_pr == 1 && tipz == 2)
      continue;
    if(data->rk->metka_pr == 2 && tipz == 1)
      continue;
   }
  
  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
    continue;
 
 
  memset(vpr1,'\0',sizeof(vpr1));
  if(tipz == 1)
   vpr1[0]='+';
  if(tipz == 2)
   vpr1[0]='-';
  iceb_u_rsdat(&dd,&md,&gd,row[2],2);
  
  memset(fio,'\0',sizeof(fio));
  strncpy(fio,row[6],sizeof(fio)-1);
  if(fio[0] == '\0')
   {
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and \
god=%d and tp=%d and nomd='%s'",row[0],gd,tipz,row[3]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) >= 1)
     {
       /*Читаем наименование контрагента*/
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row1[0]);
      if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
        strncpy(fio,row1[0],sizeof(fio)-1);
     }
   }
  /*Читаем наименование операции*/
  memset(naimop,'\0',sizeof(naimop));
  if(tipz == 1)
   sprintf(strsql,"select naik from Kasop1 where kod='%s'",row[5]);       
  if(tipz == 2)
   sprintf(strsql,"select naik from Kasop2 where kod='%s'",row[5]);       
   if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    {
     strncpy(naimop,row1[0],sizeof(naimop)-1);
    }


  sumkasorw(row[0],tipz,row[3],gd,&sumd,&sumpod,data->window);

  if(tipz == 2)
   {
    suma_r+=sumd;

    fprintf(ff2,"%42s %02d.%02d.%d %-*s %10.2f %-*.*s\n",
    " ",dd,md,gd,
    iceb_u_kolbait(8,row[3]),row[3],
    sumd,
    iceb_u_kolbait(10,row[7]),iceb_u_kolbait(10,row[7]),row[7]);

    for(int ii=10; ii < iceb_u_strlen(row[7]); ii+=10)
     fprintf(ff2,"%73s %-*.*s\n",
     "",
     iceb_u_kolbait(10,iceb_u_adrsimv(ii,row[7])),
     iceb_u_kolbait(10,iceb_u_adrsimv(ii,row[7])),
     iceb_u_adrsimv(ii,row[7]));

    sumd*=-1;
    itras+=sumd;
    kolih_rash_dok++;
   } 
  else
   {
    suma_p+=sumd;

    fprintf(ff2," %02d.%02d.%d %-*s %10.2f %-*.*s\n",
    dd,md,gd,
    iceb_u_kolbait(8,row[3]),row[3],
    sumd,
    iceb_u_kolbait(10,row[7]),iceb_u_kolbait(10,row[7]),row[7]);

    for(unsigned int ii=10; ii < strlen(row[7]); ii+=10)
     fprintf(ff2,"%31s %-*.*s\n",
     "",
     iceb_u_kolbait(10,iceb_u_adrsimv(ii,row[7])),
     iceb_u_kolbait(10,iceb_u_adrsimv(ii,row[7])),
     iceb_u_adrsimv(ii,row[7]));

    itpri+=sumd;
    kolih_prih_dok++;
   }
  saldoden+=sumd;    
  isumd+=sumd;
  isumpod+=sumpod;  

  if(tipz == 1)
   itogp+=sumd;

  if(tipz == 2)
   itogr+=sumd;
   

  fprintf(ff,"%-5s %-3s %-*s %02d.%02d.%d %10.2f %-*s %-*.*s %s %s\n",
  row[0],
  vpr1,
  iceb_u_kolbait(10,row[3]),row[3],
  dd,md,gd,sumd,
  iceb_u_kolbait(13,row[9]),row[9],
  iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
  row[7],
  row[8]);


  kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,itpri,itras,ffva,ffok,ff_prot);
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s %s %s %s kolstrok=%d\n",row[0],row[1],row[2],row[3],row[4],row[5],kolstrok);
  if(tipz == 1)
   {
    fprintf(ffva,"%-*s %-*.*s %-10.10s %10.2f\n",
    iceb_u_kolbait(8,row[3]),row[3],
    iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
    row[4],sumd);

    fprintf(ffok,"%-*s %-*.*s %-*.*s %10.2f\n",
    iceb_u_kolbait(8,row[3]),row[3],
    iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
    iceb_u_kolbait(10,row[4]),iceb_u_kolbait(10,row[4]),row[4],
    sumd);
   }
  if(tipz == 2)
   {
    fprintf(ffva,"%-*s %-*.*s %-*.*s %10s %10.2f\n",
    iceb_u_kolbait(8,row[3]),row[3],
    iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
    iceb_u_kolbait(10,row[4]),iceb_u_kolbait(10,row[4]),row[4],
    " ",sumd*-1);

    fprintf(ffok,"%-*s %-*.*s %-*.*s %10s %10.2f\n",
    iceb_u_kolbait(8,row[3]),row[3],
    iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
    iceb_u_kolbait(10,row[4]),iceb_u_kolbait(10,row[4]),row[4],
    " ",sumd*-1);
   }
 } 

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(31,gettext("Итого приход")),gettext("Итого приход"),itogp);
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(31,gettext("Итого расход")),gettext("Итого расход"),itogr);

fprintf(ffva,"\
-------------------------------------------------------------------------\n");
fprintf(ffva,"%*s|%10.2f|%10.2f|\n",iceb_u_kolbait(50,gettext("Итого за день")),gettext("Итого за день"),itpri,itras*-1);
fprintf(ffva,"%50s|---------------------|\n"," ");
fprintf(ffva,"%*s|%10.2f|\n",iceb_u_kolbait(50,gettext("Остаток на конец дня")),gettext("Остаток на конец дня"),isumd+saldon);
fprintf(ffva,"%50s|----------|\n"," ");
fprintf(ffva,"%*s|%10s|\n",iceb_u_kolbait(50,gettext("В том числе на зарплату")),gettext("В том числе на зарплату")," ");
fprintf(ffva,"%50s|----------|\n"," ");

fprintf(ffok,"\
-------------------------------------------------------------------------\n");
fprintf(ffok,"%*s|%10.2f|%10.2f|\n",iceb_u_kolbait(50,gettext("Итого за день")),gettext("Итого за день"),itpri,itras*-1);
fprintf(ffok,"%50s|---------------------|\n"," ");
fprintf(ffok,"%*s|%10.2f|\n",iceb_u_kolbait(50,gettext("Остаток на конец дня")),gettext("Остаток на конец дня"),isumd+saldon);
fprintf(ffok,"%50s|----------|\n"," ");
fprintf(ffok,"%*s|%10s|\n",iceb_u_kolbait(50,gettext("В том числе на зарплату")),gettext("В том числе на зарплату")," ");
fprintf(ffok,"%50s|----------|\n"," ");

int koldn=iceb_u_period(dn,mn,gn,dk,mk,gk,0);
if(koldn == 1)
 {
  fprintf(ffva,"%s:%d\n",gettext("Количество листов"),kollist-kollist_nah+1);
  fprintf(ffok,"%s:%d\n",gettext("Количество листов"),kollist-kollist_nah+1);
 }
else
 {
  fprintf(ffva,"%s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
  gettext("С"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."));

  fprintf(ffva,"%s %d %s.\n",
  gettext("распечатано"),
  kollist,
  gettext("листов кассовой книги"));

  fprintf(ffok,"%s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
  gettext("С"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."));

  fprintf(ffok,"%s %d %s.\n",
  gettext("распечатано"),
  kollist,
  gettext("листов кассовой книги"));
 }  

char propis_prih[50];
char propis_rash[50];
memset(propis_prih,'\0',sizeof(propis_prih));
memset(propis_rash,'\0',sizeof(propis_rash));

iceb_u_preobr(kolih_prih_dok,propis_prih,2);
iceb_u_preobr(kolih_rash_dok,propis_rash,2);

fprintf(ffva,"\n%s_________________\n\n",gettext("Кассир"));

fprintf(ffva,"%s\n",gettext("Записи в касовой книге проверил, и документы в количестве"));
fprintf(ffva,"%s %s\n",propis_prih,gettext("приходных и"));
fprintf(ffva,"%s %s\n",propis_rash,gettext("расходных получил"));


fprintf(ffva,"\n%s_______________\n",gettext("Бухгалтер"));

fprintf(ffok,"\n%s_________________\n\n",gettext("Кассир"));

fprintf(ffok,"%-*.*s\n",iceb_u_kolbait(66,strsql),iceb_u_kolbait(66,strsql),strsql);
if(iceb_u_strlen(strsql) > 66)
 fprintf(ffok,"%s",iceb_u_adrsimv(66,strsql));


fprintf(ffok,"\n%s_______________\n",gettext("Бухгалтер"));

fprintf(ff2,"\
-------------------------------------------------------------------------------\n");
fprintf(ff2,"%*s:%10.2f %29s %10.2f\n",iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"),suma_p," ",suma_r);

iceb_podpis(ff,data->window);
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля в ноль*/
fprintf(ff,"\x1B\x50"); /*10-знаков*/
fprintf(ff,"\x12");

iceb_podpis(ffva,data->window);
iceb_podpis(ffok,data->window);

fprintf(ffva,"\x1B\x50"); /*10-знаков*/
fprintf(ffva,"\x12");
fprintf(ffva,"\x1b\x6C%c",1); /*Установка левого поля в ноль*/

fprintf(ffok,"\x1B\x50"); /*10-знаков*/
fprintf(ffok,"\x12");
fprintf(ffok,"\x1b\x6C%c",1); /*Установка левого поля в ноль*/

iceb_podpis(ff2,data->window);

fprintf(ff2,"\x1B\x50"); /*10-знаков*/
fprintf(ff2,"\x1b\x6C%c",1); /*Установка левого поля*/

fclose(ff);
fclose(ffva);
fclose(ffok);
fclose(ff2);
if(ff_prot != NULL)
 fclose(ff_prot);

data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Cписок кассовых ордеров"));

data->rk->imaf.plus(imaf1);
data->rk->naimf.plus(gettext("Вкладной лист кассовой книги"));

data->rk->imaf.plus(imafok);
data->rk->naimf.plus(gettext("Отчёт кассира"));

data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Типовая форма КО-3а"));

if(ff_prot != NULL)
 {
  data->rk->imaf.plus(imaf_prot);
  data->rk->naimf.plus(gettext("Протокол хода растчёта"));
 }

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),3,data->window);





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
