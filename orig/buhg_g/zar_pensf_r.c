/*$Id: zar_pensf_r.c,v 1.16 2011-02-21 07:35:59 sasa Exp $*/
/*05.12.2010	15.01.2007	Белых А.И.	zar_pensf_r.c
Расчёт данных для перегруза в программу Пенсионного фонда Украины
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_pensf.h"

class zar_pensf_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_pensf_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_pensf_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_pensf_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_pensf_r_data *data);
gint zar_pensf_r1(class zar_pensf_r_data *data);
void  zar_pensf_r_v_knopka(GtkWidget *widget,class zar_pensf_r_data *data);

int zar_pensf_vib();
int masinvw(class iceb_u_int *tni,const char *imaf,GtkWidget *wpredok);
void	zapstr1(char str[],FILE *ff1,double xsumno,double xsumn,double xpens,double boln,short kold);
void zardod13w(const char *inn,const char *fio,char kodor[],char naimor[],char rukov[],int *kol,int *nomst,short dd,short md,short gd,short dr,short mr,short gr,short pol_slug,FILE *ff13r,FILE *ff13w,FILE *ffvv);

void zardod91(char fio[],char inn[],char sovm[],double sumano[],double suman[],double sumapen[],double boln[],short kold[],
short kolds[],double sumapzg,short kodts,short godr,char rukov[],char glavb[],char mpr[],short dd,short md,short gd,char kodor[],
char datn[],char datk[],FILE *ff1,FILE *ff2,FILE *ffv);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern iceb_u_str shrift_ravnohir;
extern short	kodpenf; /*Код пенсионного фонда*/
extern char *imabaz;
extern short    kodpen;  /*Код пенсионных отчислений*/
extern short    *kodbl;  /*Код начисления больничного*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_pensf_r(class zar_pensf_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_pensf_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать свод начислений и удержаний по категориям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_pensf_r_key_press),&data);

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

repl.plus(gettext("Распечатать свод начислений и удержаний по категориям"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_pensf_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_pensf_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_pensf_r_v_knopka(GtkWidget *widget,class zar_pensf_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_pensf_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_pensf_r_data *data)
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

gint zar_pensf_r1(class zar_pensf_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];




int metka_bd=0;

if(shetb != NULL)
  metka_bd=zar_pensf_vib();
if(metka_bd < 0 || metka_bd > 2)
 {
  data->kon_ras=0;

  gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  gtk_widget_show_all(data->window);

  iceb_printw_vr(vremn,data->buffer,data->view);

  data->voz=0;
  return(FALSE);
 }
 

class iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR curr; 
/*Читаем наименование организации*/
char		kodor[20];
memset(kodor,'\0',sizeof(kodor));
sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не нашли код контрагента 00 !"),data->window);
 }
else
 strncpy(kodor,row[0],sizeof(kodor)-1);

short mn=data->rk->mesn.ravno_atoi();
short mk=data->rk->mesk.ravno_atoi();
short godr=data->rk->god.ravno_atoi();
short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->rk->datad.ravno(),1);

sprintf(strsql,"Період расчёта %d > %d Рік %d\n",mn,mk,godr);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

class iceb_u_str imaf_nast("zarnast.alx");


char kodnvdp[512];
memset(kodnvdp,'\0',sizeof(kodnvdp));
iceb_poldan("Коды не входящие в расчет для начисления пенсии",kodnvdp,imaf_nast.ravno(),data->window);
sprintf(strsql,"Коды не входящие в расчет для начисления пенсии:%s\n",kodnvdp);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

char		knvp[100]; /*Коды не входящие в расчет пенсионного отчисления*/
memset(knvp,'\0',sizeof(knvp));
sprintf(strsql,"select kodn from Zarsoc where kod=%d",kodpenf);
if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
 {  
  strncpy(knvp,row[0],sizeof(knvp)-1);
 }

sprintf(strsql,"Коды не входящие в расчет отчисления в пенсионный фонд:%s\n",knvp);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


char bros[512];
memset(bros,'\0',sizeof(bros));
iceb_poldan("Табельный номер руководителя",bros,imaf_nast.ravno(),data->window);

char		rukov[512]; /*Фамилия руководителя*/
memset(rukov,'\0',sizeof(rukov));
if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не нашли табельный номер руководителя"),bros);
    iceb_menu_soob(strsql,data->window);
   }
  else
   {
    strncpy(rukov,row[0],sizeof(rukov)-1);
   }
 } 

iceb_poldan("Табельный номер бухгалтера",bros,imaf_nast.ravno(),data->window);

char		glavb[40]; /*Фамилия главного бухгалтера*/
memset(glavb,'\0',sizeof(glavb));

if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не нашли табельный номер главного бухгалтера"),bros);
    iceb_menu_soob(strsql,data->window);
   }
  else
   {
    strncpy(glavb,row[0],sizeof(glavb)-1);
   }
 }

class iceb_u_int inval; /*Массив с табельными номерами инвалидов*/
class iceb_u_int dogp;  /*Массив работающих по договору подряда*/

sprintf(strsql,"Создаем массив табельных номеров инвалидов.\n");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int koltbn=masinvw(&inval,"zarinv.alx",data->window);

sprintf(strsql,"Создаем массив работающих по договорам подряда.\n");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int koldp=masinvw(&dogp,"zardog.alx",data->window);

sprintf(strsql,"Массивы созданы.\n");
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


char		mpr[2]; /*Метка периода*/
memset(mpr,'\0',sizeof(mpr));
mpr[0]='0'; 
if(mn == 1 && mk == 3)
 mpr[0]='1';
if(mn == 4 && mk == 6)
 mpr[0]='2';
if(mn == 7 && mk == 9)
 mpr[0]='3';
if(mn == 10 && mk == 12)
 mpr[0]='4';
if(mn == 1 && mk == 6)
 mpr[0]='5';
if(mn == 7 && mk == 12)
 mpr[0]='6';
if(mn == 1 && mk == 9)
 mpr[0]='7';

int kolstr=0;

sprintf(strsql,"select tabn,fio,datn,datk,sovm,inn,denrog,pl from Kartb");
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни один табельный номер !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
sprintf(strsql,"Количество табельных номеров - %d\n",kolstr);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

char imafdod9[50];
FILE *ff1;
sprintf(imafdod9,"dod9_%d.lst",getpid());
if((ff1=fopen(imafdod9,"w")) == NULL)
 {
  iceb_er_op_fil(imafdod9,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


FILE *ff2;
if(iceb_alxout("zardod9.alx",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
if((ff2=fopen("zardod9.alx","r")) == NULL)
 {
  iceb_er_op_fil("zardod9.alx","",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafdod13[50];
sprintf(imafdod13,"dod13_%d.lst",getpid());
FILE *ff13w;
if((ff13w=fopen(imafdod13,"w")) == NULL)
 {
  iceb_er_op_fil(imafdod13,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff13r;
if(iceb_alxout("zardod13.alx",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if((ff13r=fopen("zardod13.alx","r")) == NULL)
 {
  iceb_er_op_fil("zardod13.alx","",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafv[50];
sprintf(imafv,"apm_i%d.txt",godr);
FILE *ffv;
if((ffv=fopen(imafv,"w")) == NULL)
 {
  iceb_er_op_fil(imafv,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafvv[50];
sprintf(imafvv,"apm_a%d.txt",godr);
FILE *ffvv;
if((ffvv=fopen(imafvv,"w")) == NULL)
 {
  iceb_er_op_fil(imafvv,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kol=0,mff=0;
float kolstr1=0;
int nomst=1;
char inn[20];
short dr,mr,gr;
short pol_slug=0;
int tbn=0;
short		kodts; /*Код типа ставки страхового сбора
                        1-на общих основаниях для юридических и физических особ
                        2-для инвалидов
                        3- для особ, которые работают по договорам гражданского правового характера
                        4-для особ, которые делают добровольные взносы
                        5-для владельцев патента
                        */
double		sumano[12]; /*Начисленная сумма*/
double		suman[12]; /*Сумма с которой взяты пенсионные*/
double          sumanvdp[12]; //Сумма не входящая в расчет для пенсии
double		sumapen[12]; /*Сумма пенсионного*/
double		sumapzg=0.;     //Сумма перечисленная за год
double		boln[12]; /*Сумма больничных*/
short		kold[12]; /*Количество дней больничных*/
short		kolds[12]; /*Количество дней стажа*/
int kolstr2=0;
SQL_str row1;
short prn=0;
short d,m,g;
short		mesn,godn;
int knah;
double bb;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);

  tbn=atol(row[0]);


  /*Смотрим былили начисления */

  sprintf(strsql,"select datz,prn,knah,suma,kdn,godn,mesn,shet from Zarp where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and tabn=%d and suma <> 0.",
  godr,mn,godr,mk,tbn);

/*
  printw("%s\n",strsql);
  refresh();
*/
  class SQLCURSOR cur1;
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
   {
    //если не уволен грузить
    if(row[3][0] != '0')
      continue;        
   }
  memset(&sumano,'\0',sizeof(sumano));
  memset(&sumanvdp,'\0',sizeof(sumanvdp));
  memset(&sumapen,'\0',sizeof(sumapen));
  memset(&kolds,'\0',sizeof(kolds));
  memset(&kold,'\0',sizeof(kold));
  memset(&boln,'\0',sizeof(boln));
  while(cur1.read_cursor(&row1) != 0)
   {
    prn=atoi(row1[1]);
    if(metka_bd == 1) //Бюджет
     {    
      if(prn == 1)
        if(iceb_u_proverka(shetb,row1[7],0,1) != 0)
         continue;
      if(prn == 2)
        if(iceb_u_proverka(shetbu,row1[7],0,1) != 0)
         continue;
     }
    if(metka_bd == 2) //Хозрасчёт
     {    
      if(prn == 1)
        if(iceb_u_proverka(shetb,row1[7],0,1) == 0)
         continue;
      if(prn == 2)
        if(iceb_u_proverka(shetbu,row1[7],0,1) == 0)
         continue;
     }

    iceb_u_rsdat(&d,&m,&g,row1[0],2);
    godn=atoi(row1[5]);
    mesn=atoi(row1[6]);
    
    knah=atoi(row1[2]);
    bb=atof(row1[3]);
    if(prn == 1)
     {
      if(godr == godn && provkodw(kodbl,knah) >= 0)
       {
        boln[mesn-1]+=bb;
        kold[mesn-1]+=atoi(row1[4]);
        sumano[mesn-1]+=bb;
        continue;
       }

      if(iceb_u_proverka(knvp,row1[2],0,1) != 0)
       sumano[m-1]+=bb;
/************
      if(iceb_u_proverka(kodkzno,row1[2],0,1) == 0)
        kzno[m-1]+=bb;
******************/
      if(iceb_u_proverka(kodnvdp,row1[2],0,1) == 0)
        sumanvdp[m-1]+=bb;
     }
    if(prn == 2)
     {
      if(godr == godn && knah == kodpen)
        sumapen[mesn-1]+=bb*(-1);
     }
   }
  //Смотрим отчисления в соц-фонд

  memset(&suman,'\0',sizeof(suman));
  sumapzg=0.;
  sprintf(strsql,"select datz,sumas,sumap,sumapb,sumasb from Zarsocz where tabn=%d \
and datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and kodz=%d",
  tbn,godr,mn,godr,mk,kodpenf);

//  printw("%s\n",strsql);
  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  
  if(kolstr2 == 0)
   {
    //У предприятий на едином налоге нет начисления на фонд зарплаты
//    printw("%d Нет соц. отчислений !\n",tbn);
//    continue;        
   }
  while(cur1.read_cursor(&row1) != 0)
   {
//    printw("%s %s %s\n",row1[0],row1[1],row1[2]);
//  OSTANOV();
    iceb_u_rsdat(&d,&m,&g,row1[0],2);

    if(metka_bd == 0) //Всё
     {
      
//      suman[m-1]+=atof(row1[1])-kzno[m-1]-sumanvdp[m-1];
      suman[m-1]+=atof(row1[1])-sumanvdp[m-1];

      if(fabs(sumanvdp[m-1]) > 0.009)
       {
        sumapen[m-1]=pensw(tbn,m,g,suman[m-1],NULL,data->window);
//        bb=atof(row1[1])-boln[m-1]-kzno[m-1]-sumanvdp[m-1];
        bb=atof(row1[1])-boln[m-1]-sumanvdp[m-1];
        bb=bb*.32;
        bb=iceb_u_okrug(bb,0.01);
        sumapzg+=bb;
       }
      else    
        sumapzg+=atof(row1[2]);
     }
    if(metka_bd == 1) //Бюджет
     {
      
//      suman[m-1]+=atof(row1[4])-kzno[m-1]-sumanvdp[m-1];
      suman[m-1]+=atof(row1[4])-sumanvdp[m-1];

      if(fabs(sumanvdp[m-1]) > 0.009)
       {
        sumapen[m-1]=pensw(tbn,m,g,suman[m-1],NULL,data->window);
//        bb=atof(row1[4])-boln[m-1]-kzno[m-1]-sumanvdp[m-1];
        bb=atof(row1[4])-boln[m-1]-sumanvdp[m-1];
        bb=bb*.32;
        bb=iceb_u_okrug(bb,0.01);
        sumapzg+=bb;
       }
      else    
        sumapzg+=atof(row1[3]);
     }
    if(metka_bd == 2) //Хозрасчёт
     {
      
//      suman[m-1]+=(atof(row1[1])-atof(row1[4]))-kzno[m-1]-sumanvdp[m-1];
      suman[m-1]+=(atof(row1[1])-atof(row1[4]))-sumanvdp[m-1];

      if(fabs(sumanvdp[m-1]) > 0.009)
       {
        sumapen[m-1]=pensw(tbn,m,g,suman[m-1],NULL,data->window);
//        bb=(atof(row1[1])-atof(row1[4]))-boln[m-1]-kzno[m-1]-sumanvdp[m-1];
        bb=(atof(row1[1])-atof(row1[4]))-boln[m-1]-sumanvdp[m-1];
        bb=bb*.32;
        bb=iceb_u_okrug(bb,0.01);
        sumapzg+=bb;
       }
      else    
        sumapzg+=atof(row1[2])-atof(row1[3]);
     }
   }
//  printw("sumapzg=%f\n",sumapzg);
//  OSTANOV();
  /*Определяем количество дней стажа*/
  
  sprintf(strsql,"select mes,kdnei from Ztab where \
god = %d and mes >= %d and mes <= %d and tabn=%d order by mes asc",
  godr,mn,mk,tbn);

/*
  printw("%s\n",strsql);
  refresh();
*/
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

//  short mz=0;
  if(kolstr2 != 0)
  while(cur1.read_cursor(&row1) != 0)
   {
    m=atoi(row1[0]);
    short i=atoi(row1[1]);
/********************** 
//если не вводили календарніе дни
    if(mz != m)
     {
      if( i == 0 )
       {
        d=1;
        dpm(&d,&m,&godr,5);
        i=d;
       }
      mz=m;
     }
********************/
    kolds[m-1]+=i;
   }

  kodts=1;
  if(koltbn != 0)
   if(inval.find(tbn) >= 0)
     kodts=2;
  if(koldp != 0)
   if(dogp.find(tbn) >= 0 )
    kodts=3;
  
  iceb_u_rsdat(&dr,&mr,&gr,row[6],2);
  pol_slug=atoi(row[7]);
  
  mff++;
  memset(inn,'\0',sizeof(inn));
  strncpy(inn,row[5],sizeof(inn)-1);
  if(inn[0] == '\0')
   sprintf(inn,"%010d",tbn);

  if(mff > 0)
    fprintf(ff1,"\f");
  
  zardod91(row[1],inn,row[4],sumano,suman,sumapen,boln,kold,kolds,sumapzg,\
  kodts,godr,rukov,glavb,mpr,dd,md,gd,kodor,row[2],row[3],ff1,ff2,ffv);

  zardod13w(inn,row[1],kodor,organ,rukov,&kol,&nomst,dd,md,gd,dr,mr,gr,pol_slug,ff13r,ff13w,ffvv);

 }
zardod13w("","",kodor,organ,rukov,&kol,&nomst,dd,md,gd,0,0,0,0,ff13r,ff13w,ffvv);

fclose(ff1);
fclose(ff2);
fclose(ff13r);
fclose(ff13w);
fclose(ffv);
fclose(ffvv);
unlink("zardod9.alx");
unlink("zardod13.alx");


data->rk->imaf.plus(imafvv);

data->rk->imaf.plus(imafdod9);
data->rk->naimf.plus(gettext("Приложение 9"));

data->rk->imaf.plus(imafdod13);
data->rk->naimf.plus(gettext("Приложение 13"));

for(int nom=0; data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),1,data->window);
 
data->rk->imaf.plus(imafv);
data->rk->naimf.plus(gettext("Приложение 9 для записи на дискету"));

data->rk->imaf.plus(imafvv);
data->rk->naimf.plus(gettext("Приложение 13 для записи на дискету"));




data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/*******************/
/*выбрать вид отчета*/
/*********************/

int zar_pensf_vib()
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Хозрасчёт и бюджет"));//0
punkt_m.plus(gettext("Бюджет"));//1
punkt_m.plus(gettext("Хозрасчёт"));//2


int nomer=0;
return(iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL));

}

/**************************/
/*Распечатка приложения N9*/
/**************************/

void zardod91(char fio[], //Фамилия имя отчество
char inn[], //Индетификационный номер
char sovm[], //0-нет 1-метка совместителя
double sumano[], //Начисленная сумма
double suman[], //Сумма с которой взяты пенсионные
double sumapen[], //Сумма пенсионного
double boln[],    //Сумма больничных
short kold[],   //Количество дней больничных
short kolds[],  //Количество дней стажа
double sumapzg,
short kodts, //Код типа ставки страхового збора
short godr,  //Год расчета
char rukov[],
char glavb[],
char mpr[],  //Метка периода
short dd,short md,short gd,
char kodor[], //Код роботодавця
char datn[], //Дата приема на работу
char datk[], //Дата увольнения
FILE *ff1,FILE *ff2,FILE *ffv)
{
short		nomstr;
char		bros[512];
char		str[300];
double		drob,cel,isumno,isumn,ipens,iboln;
int		ikolds;
int		i;
double		sumsb=0.; /*Сумма страхового сбора уплаченного заработника*/
short		dn,mn,gn;
short		dk,mk,gk;
short		kolpm,kolpd;
short		d,m,g;

iceb_u_rsdat(&dn,&mn,&gn,datn,2);
if(gn < godr)
 {
  gn=godr;
  dn=mn=1;
 }

iceb_u_rsdat(&dk,&mk,&gk,datk,2);
if(gk == 0 || gk > godr) 
 {
  gk=godr;
  dk=31;
  mk=12;
 } 

ipens=isumn=isumno=0.;
ikolds=0;
iboln=0.;
for(i=0; i < 12 ; i++)
 {
  iboln+=boln[i];
  isumno+=sumano[i];
  ikolds+=kolds[i];
  isumn+=suman[i];
  ipens+=sumapen[i];
 }

d=dn;
m=mn;
g=gn;
iceb_u_dpm(&d,&m,&g,5);
kolpd=0;
kolpm=mk-mn+1;
if(dn != 1 )
 {
  kolpm--;
  kolpd=d-dn+1;
 }

if(mn != mk)
 {
  d=dk;
  m=mk;
  g=gk;


  iceb_u_dpm(&d,&m,&g,5);
  if(dk < d)
   {
    kolpm--;
    kolpd+=dk;
   }
 }

if(mn == mk && gn == gk)
 kolpd=dk -dn+1; 
for(nomstr=1; fgets(str,sizeof(str),ff2) != NULL ; nomstr++)
 {
  switch(nomstr)
   {
    case 12: 
      iceb_u_bstab(str,inn,25,46,2);
      fprintf(ff1,"%s",str);
      break;

  /*Фамилия*/
    case 14: 
      iceb_u_pole(fio,bros,1,' ');
      iceb_u_bstab(str,bros,14,54,1);
      fprintf(ff1,"%s",str);
      break;

  /*Имя*/
    case 15: 
      iceb_u_pole(fio,bros,2,' ');
      iceb_u_bstab(str,bros,14,54,1);
      fprintf(ff1,"%s",str);
      break;

  /*Отчество*/    
    case 16: 
      iceb_u_pole(fio,bros,3,' ');
      iceb_u_bstab(str,bros,14,54,1);
      fprintf(ff1,"%s",str);
      break;

    case 18: 
      sprintf(bros,"%d",godr);
      iceb_u_bstab(str,bros,37,46,2);
      iceb_u_bstab(str,mpr,64,65,1);
      fprintf(ff1,"%s",str);
      break;

    case 22: 
      sprintf(bros,"%d",godr);
      iceb_u_bstab(str,bros,40,48,2);
      fprintf(ff1,"%s",str);
      break;

    case 24: 
      iceb_u_bstab(str,organ,19,65,1);
      fprintf(ff1,"%s",str);
      break;

    case 25: 
      memset(bros,'\0',sizeof(bros));
      if(sovm[0] == '0')
       bros[0]='X';
      iceb_u_bstab(str,bros,68,69,1);
      fprintf(ff1,"%s",str);
      break;

    case 27: /*Код типа ставки страхового сбора*/
      sprintf(bros,"%d",kodts);
      iceb_u_bstab(str,bros,34,36,1);
      fprintf(ff1,"%s",str);
      break;

    /*Сумма страхового сбора уплаченных за работника*/
    case 30: 
             
//      sumsb=(isumn-iboln)*tarif/100.;
      sumsb=sumapzg;
      drob=modf(sumsb,&cel);

      sprintf(bros,"%5.f",cel);
      iceb_u_bstab(str,bros,51,61,2);
      sprintf(bros,"%02.f",drob*100.);
      iceb_u_bstab(str,bros,66,70,2);
      fprintf(ff1,"%s",str);
      break;

    /*Сумма страхового сбора из зароботка работника*/
    case 32: 
      drob=modf(ipens,&cel);

      sprintf(bros,"%5.f",cel);
      iceb_u_bstab(str,bros,51,61,2);
      sprintf(bros,"%02.f",drob*100.);
      iceb_u_bstab(str,bros,66,70,2);
      fprintf(ff1,"%s",str);
      break;
  
    /*1 Январь*/
    case 42: 
      sprintf(bros,"%02d",dn);
      iceb_u_bstab(str,bros,82,86,2);
      zapstr1(str,ff1,sumano[0],suman[0],sumapen[0],boln[0],kolds[0]);
      
      break;
  
  /*2 Февраль*/
    case 43: 
      zapstr1(str,ff1,sumano[1],suman[1],sumapen[1],boln[1],kolds[1]);
      break;

    /*3 Март*/
    case 44: 
      sprintf(bros,"%02d",mn);
      iceb_u_bstab(str,bros,82,86,2);
      zapstr1(str,ff1,sumano[2],suman[2],sumapen[2],boln[2],kolds[2]);
      break;

    /*4 Апрель*/
    case 45: 
      zapstr1(str,ff1,sumano[3],suman[3],sumapen[3],boln[3],kolds[3]);
      break;

    /*5 Май*/
    case 46: 
      if(gn < 2000)
       sprintf(bros,"%02d",gn-1900);
      else
       sprintf(bros,"%02d",gn-2000);
      iceb_u_bstab(str,bros,82,86,2);
      zapstr1(str,ff1,sumano[4],suman[4],sumapen[4],boln[4],kolds[4]);
      break;

    /*6 Июнь*/
    case 47: 
      zapstr1(str,ff1,sumano[5],suman[5],sumapen[5],boln[5],kolds[5]);
      break;

    /*7 Июль*/
    case 48: 
      zapstr1(str,ff1,sumano[6],suman[6],sumapen[6],boln[6],kolds[6]);
      break;

    /*8 Август*/
    case 49: 
      zapstr1(str,ff1,sumano[7],suman[7],sumapen[7],boln[7],kolds[7]);
      break;

    /*9 Сентябрь*/
    case 50: 
      sprintf(bros,"%02d",dk);
      iceb_u_bstab(str,bros,82,86,2);
      zapstr1(str,ff1,sumano[8],suman[8],sumapen[8],boln[8],kolds[8]);
      break;

    /*10 Октябрь*/
    case 51: 
      zapstr1(str,ff1,sumano[9],suman[9],sumapen[9],boln[9],kolds[9]);
      break;

    /*11 Ноябрь*/
    case 52: 
      sprintf(bros,"%02d",mk);
      iceb_u_bstab(str,bros,82,86,2);
      zapstr1(str,ff1,sumano[10],suman[10],sumapen[10],boln[10],kolds[10]);
      break;

    /*12 Декабрь*/
    case 53: 
      zapstr1(str,ff1,sumano[11],suman[11],sumapen[11],boln[11],kolds[11]);
      break;

    case 54: 
      if(gn < 2000)
       sprintf(bros,"%02d",gn-1900);
      else
       sprintf(bros,"%02d",gn-2000);
      iceb_u_bstab(str,bros,82,86,2);
      fprintf(ff1,"%s",str);
      break;
      
    /*Итого*/
    case 55: 
      zapstr1(str,ff1,isumno,isumn,ipens,iboln,ikolds);
      break;

    /*Количество полных дней и месяцев*/
    case 59: 
      sprintf(bros,"%02d",kolpm);
      iceb_u_bstab(str,bros,64,68,2);
      sprintf(bros,"%02d",kolpd);
      iceb_u_bstab(str,bros,77,81,2);
      fprintf(ff1,"%s",str);
      break;
    
    /*Подпись руководителя*/
    case 76: 
      sprintf(bros,"/%s/",rukov);
      iceb_u_bstab(str,bros,70,100,1);
      fprintf(ff1,"%s\n",str);
      break;

    /*Подпись главбуха*/
    case 78: 
      sprintf(bros,"/%s/",glavb);
      iceb_u_bstab(str,bros,70,100,1);
      fprintf(ff1,"%s\n",str);
      break;
    /*Дата документа*/
    case 80:
      sprintf(bros,"%02d",dd);
      iceb_u_bstab(str,bros,6,10,2);
      sprintf(bros,"%02d",md);
      iceb_u_bstab(str,bros,13,17,2);
      sprintf(bros,"%d",gd);
      iceb_u_bstab(str,bros,20,27,2);
      fprintf(ff1,"%s",str);
      break;

    default :
      fprintf(ff1,"%s",str);
      break;
   }
  memset(str,'\0',sizeof(str));
}
rewind(ff2);

memset(bros,'\0',sizeof(bros));

if(sovm[0] == '0')
 bros[0]='1';
if(sovm[0] == '1')
 bros[0]='0';
 
fprintf(ffv,"\"%s\",\"%s\",%d,\"%s\",%d,\"%s\",\"%s\",\"%d\",\
%.2f,%.2f",
"1",inn,godr,mpr,godr,kodor,bros,kodts,sumsb,ipens);

for(i=0 ; i < 12; i++)
  fprintf(ffv,",%.2f,%.2f,%.2f,%.2f,%d",sumano[i],suman[i],boln[i],sumapen[i],kolds[i]);


 
fprintf(ffv,",%.2f,%.2f,%.2f,%.2f,%d,%d%02d%02d,%d%02d%02d,%d,%d",
isumno,isumn,iboln,ipens,ikolds,gn,mn,dn,gk,mk,dk,kolpm,kolpd);

for(i=0; i < 4 ; i++)
 fprintf(ffv,",\"%s\",%d,\"%s\",%d,%d,%d,%d,%d,%d,%d,\"%s\"",
 "",0,"0",0,0,0,0,0,0,0,"0");
 
fprintf(ffv,",%d,%d%02d%02d,%d,%d%02d%02d,\"%s\",\"%s\",\"\",\"\"\n",
0,gd,md,dd,0,gd,md,dd,"","");

}

/******************************/
/*******************************/
void	zapstr1(char str[],FILE *ff1,double xsumno,double xsumn,
double xpens,double boln,short kold)
{
double		drob,cel;
char		bros[512];

if(xsumn > 0.009 || xsumno > 0.009)
 {
  drob=modf(xsumno,&cel);
  sprintf(bros,"%5.f%02.f",cel,drob*100);
  iceb_u_bstab(str,bros,4,18,2);
  drob=modf(xsumn,&cel);
  sprintf(bros,"%5.f%02.f",cel,drob*100);
  iceb_u_bstab(str,bros,18,32,2);
  if(boln > 0.009)
   {
    drob=modf(boln,&cel);
    sprintf(bros,"%4.f%02.f",cel,drob*100);
    iceb_u_bstab(str,bros,32,44,2);
   }
  drob=modf(xpens,&cel);
  sprintf(bros,"%5.f%02.f",cel,drob*100);
  iceb_u_bstab(str,bros,44,58,2);
  sprintf(bros,"%03d",kold);
  iceb_u_bstab(str,bros,58,64,2);
 }

fprintf(ff1,"%s",str);

}
