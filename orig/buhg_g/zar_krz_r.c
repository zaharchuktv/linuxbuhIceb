/*$Id: zar_krz_r.c,v 1.15 2011-04-14 16:09:36 sasa Exp $*/
/*12.02.2010	23.10.2006	Белых А.И.	zar_krz_r.c
Контроль расчёта зарплаты
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "kasothkcn.h"
#include "zarp1.h"

class zar_krz_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  
  short mr,gr;
  class iceb_u_spisok *imafr;
  class iceb_u_spisok *naimfr;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_krz_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_krz_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_krz_r_data *data);
gint zar_krz_r1(class zar_krz_r_data *data);
void  zar_krz_r_v_knopka(GtkWidget *widget,class zar_krz_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern double   okrg1;
extern double	okrcn;
extern struct ZAR zar;
extern short	*kodnvpen; /*Коды не входящие в расчет пенсионного отчисления*/
extern short	kodpenf; /*Код пенсионного фонда*/
extern short    kodpen;  /*Код пенсионных отчислений*/
extern char	shrpz[16]; /*Счет расчетов по зарплате*/
extern float    procpen[2]; /*Процент отчисления в пенсионный фонд*/
extern double   okrg; /*Округление*/

int zar_krz_r(short mr,short gr,class iceb_u_spisok *imafr,class iceb_u_spisok *naimfr,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_krz_r_data data;
data.mr=mr;
data.gr=gr;
data.imafr=imafr;
data.naimfr=naimfr;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Контроль расчёта зарплаты"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_krz_r_key_press),&data);

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

repl.plus(gettext("Контроль расчёта зарплаты"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_krz_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_krz_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_krz_r_v_knopka(GtkWidget *widget,class zar_krz_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_krz_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_krz_r_data *data)
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

gint zar_krz_r1(class zar_krz_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;


//читаем текущие настройки
zar_read_tnw(1,data->mr,data->gr,NULL,data->window);

int kolstr=0;

sprintf(strsql,"select tabn,fio,datk,sovm,datn,podr,kateg,shet,zvan,\
lgoti,datn,dolg from Kartb");
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

char imaf[30];
char imaf1[30];

sprintf(imaf,"nnz%d.lst",getpid());
sprintf(imaf1,"szr%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
FILE *f;

if((f = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,NULL);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char		knvp[100];
memset(knvp,'\0',sizeof(knvp));
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;
iceb_u_dpm(&dk,&data->mr,&data->gr,5);
iceb_u_zagolov(gettext("Зароботная плата"),1,data->mr,data->gr,dk,data->mr,data->gr,organ,f);
                                                                                    

fprintf(f,"\
------------------------------------------------------------------------------------------\n");
fprintf(f,gettext("Т.Н.|    Фамилия  Имя Отчество               | Сальдо   |Начислено |  Удержано |К выдаче\n"));

fprintf(f,"\
------------------------------------------------------------------------------------------\n");

iceb_u_zagolov(gettext("Контроль расчёта зарплаты"),1,data->mr,data->gr,dk,data->mr,data->gr,organ,ff);


SQLCURSOR curr;
int kol=0;
float kolstr1=0.;
double itdl=0.,itsl=0.,itsn=0.,itsu=0.;
int sovm=0;
int tabb=0;
char fio[512];
double sal=0.;
double sum=0.;
double sumn=0.;
double sumu=0.;
double saldb=0.;
char            pr[30]; /*Дата приема и увольнения с работы*/
int podr=0;
int kateg=0;    
short		zvan;
char		lgoti[50];
char shet[32];
short d,m,g;
double		nahh[5];
double nah=0.;
double		s1,s2;
double          suma_zarp_o; //Сумма чистой зарплаты общая
double          suma_zarp_b; //Сумма чистой зарплаты бюджетная
double   nmpo=0.; /*Вычисленная величина необлагаемой мат. пом.*/
double suma_boln_rm=0.;
  int kolkd1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  tabb=atoi(row[0]);
  memset(fio,'\0',sizeof(fio));
  strncpy(fio,row[1],sizeof(fio)-1);


  memset(pr,'\0',sizeof(pr));
  sprintf(pr,"%s/%s",row[4],row[2]);

  sovm=atoi(row[3]);
  podr=atoi(row[5]);
  kateg=atoi(row[6]);
  strncpy(shet,row[7],sizeof(shet));
  zvan=atoi(row[8]);
  strncpy(lgoti,row[9],sizeof(lgoti));

  sum=sumn=sumu=0.;

  /*Читаем сальдо*/
  sal=0.;
  sum=sal=zarsaldw(1,data->mr,data->gr,tabb,&saldb,data->window);
  
  if(fabs(sal) > 0.009)
   {
    /*Проверяем был ли вход в карточку*/
    sprintf(strsql,"select tabn from Zarn where tabn=%d and god=%d and \
mes=%d",tabb,data->gr,data->mr);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      iceb_u_rsdat(&dn,&mn,&gn,row[10],2);
      iceb_u_rsdat(&dk,&mk,&gk,row[2],2);
      zapzarnw(data->mr,data->gr,podr,tabb,kateg,sovm,zvan,shet,lgoti,dn,mn,gn,dk,mk,gk,0,row[11],data->window);
     }

    if(iceb_u_rsdat(&d,&m,&g,row[2],2) == 0)
     {
      sprintf(strsql,"%d %*.*s %s %d.%d.%d - %s %.2f\n",
      tabb,iceb_u_kolbait(20,fio),iceb_u_kolbait(20,fio),fio,gettext("Уволен"),d,m,g,
      gettext("не нулевое сальдо"),sal);
      
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

      fprintf(ff,"%d %s %s %d.%d.%d - %s %.2f\n",
      tabb,fio,gettext("Уволен"),d,m,g,
      gettext("не нулевое сальдо"),sal);
      continue;
     }
   }
  else
   {
    //Проверяем нет ли начислений или удержаний 
    sprintf(strsql,"select prn from Zarp where tabn=%d and datz >= '%04d-%2d-%2d' \
and datz <= '%04d-%2d-%2d' and suma <> 0.",tabb,data->gr,data->mr,1,data->gr,data->mr,31);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 0)
     {
      /*Проверяем был ли вход в карточку*/
      sprintf(strsql,"select tabn from Zarn where tabn=%d and god=%d and \
mes=%d",tabb,data->gr,data->mr);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        iceb_u_rsdat(&dn,&mn,&gn,row[10],2);
        iceb_u_rsdat(&dk,&mk,&gk,row[2],2);
        zapzarnw(data->mr,data->gr,podr,tabb,kateg,sovm,zvan,shet,lgoti,dn,mn,gn,dk,mk,gk,0,row[11],data->window);
       }
     }    
   }
   
  itsl+=sal;

  if(iceb_u_rsdat(&d,&m,&g,row[2],2) == 0)
    continue;

  double snvr=0.;
  double pens1=0.;
  double sumnprov=0.,sumuprov=0.;
  short metk=0;
  if((metk=zapmasw(tabb,data->mr,data->gr,data->window)) == 0)
   {
    for(int i=0;i<razm;i++)
     {
      if(zar.prnu[i] == 0)
        break;
      sum+=zar.sm[i];
      
      if(zar.prnu[i] == 1)
       {
        sumn+=zar.sm[i];
        if(provkodw(kodnvpen,zar.knu[i]) >= 0)
          snvr+=zar.sm[i];
       //Определяем нужно ли делать проводки
       sprintf(strsql,"select prov from Nash where kod=%d",zar.knu[i]);
       if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
        if(atoi(row1[0]) == 0)
          sumnprov+=zar.sm[i];
       }
      if(zar.prnu[i] == 2)
       {
        sumu+=zar.sm[i];
       //Определяем нужно ли делать проводки
       
       sprintf(strsql,"select prov from Uder where kod=%d",zar.knu[i]);
//       printw("%s\n",strsql);
       if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
        if(atoi(row1[0]) == 0)
          sumuprov+=zar.sm[i];
       }
      if(zar.prnu[i] == 2 && kodpen == zar.knu[i])   /*Удержания*/
       {
        pens1+=zar.sm[i];
       }
     }
    itsn+=sumn;
    itsu+=sumu;
    if(sal+sumn+sumu < 0.)
      itdl+=sal+sumn+sumu;
    double suma_nfnz,suma_nfnz_b;
    nahh[0]=nah=findnahmw(data->mr,1,NULL,NULL,&s1,&s2,&suma_nfnz,&suma_nfnz_b,&suma_zarp_o,&suma_zarp_b,&suma_boln_rm); /*Поиск всех начислений*/
    nahh[0]-=snvr;
    nah-=snvr;
    nah-=nmpo;    

    nahh[1]=nah;
    nahh[2]=nah;

    nah=pensw(tabb,data->mr,data->gr,nah,NULL,data->window)*-1;

    nahh[3]=nah;
    nah=iceb_u_okrug(nahh[3],okrg);
    nahh[4]=nah;
    if(fabs(nah - pens1) > 0.009)
     {
      /*
      printw("nah=%.2f - pens1=%.2f = %.2f\n",nah,pens1,nah-pens1);
      */
      sprintf(strsql,"%s %.2f (%.2f/%.2f) - ",
      gettext("Отчисления на пенсию не верны"),nahh[0],nah,pens1);

      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
      
      fprintf(ff,"%s %.2f(%.2f/%.2f) %.2f %.2f %.2f %.2f %.2f %.4f- ",
      gettext("Отчисления на пенсию не верны"),nahh[0],
      nah,pens1,nahh[1],nahh[2],snvr,nahh[3],nahh[4],okrg);
      
      sprintf(strsql,"%4d %-30s %s\n",tabb,fio,pr);

      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

      fprintf(ff,"%4d %-*s %s\n",tabb,iceb_u_kolbait(30,fio),fio,pr);
      kol++;
     }
   }

  if(metk == 1 || sum <=0. || sumn == 0. || sumu == 0.)
   {

    if(metk == 1)
     {
      sprintf(strsql,"%s ",gettext("Нет записей"));
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
      fprintf(ff,"%s ",gettext("Нет записей"));
     }
    else
     {
      if(sumn == 0.)
       {
        sprintf(strsql,"%s ",gettext("Нет начислений"));
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
        fprintf(ff,"%s ",gettext("Нет начислений"));
       }
      if(sumu == 0.)
       {
        sprintf(strsql,"%s ",gettext("Нет удержаний"));
        fprintf(ff,"%s ",gettext("Нет удержаний"));
       }

      if(sumn != 0. && sumu != 0.)
       {
        if(fabs(sum) <= 0.0001 )
         {
          sprintf(strsql,"%s ",gettext("Ноль"));
          iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
          fprintf(ff,"%s ",gettext("Ноль"));
         }
        else
         {
          sprintf(strsql,"%s ",gettext("Долг"));
          iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
          fprintf(ff,"%s ",gettext("Долг"));
         }
       }
     }
    sprintf(strsql,"%4d %-*s %s\n",tabb,iceb_u_kolbait(30,fio),fio,pr);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%4d %-*s %s\n",tabb,iceb_u_kolbait(30,fio),fio,pr);
    kol++;
   }

  /*Смотрим сумму выполненых проводок*/ 
  if(shrpz[0] != '\0')
   {
    double sumd=0.,sumk=0.;
    char sss1[30];
    sprintf(sss1,"%d-%d",data->mr,tabb);
    
    sumprzw(data->mr,data->gr,sss1,0,&sumd,&sumk,data->window);
    
    if(fabs(sumd-sumnprov) > 0.009 || fabs(sumuprov*(-1)-sumk) > 0.009)
     {
      sprintf(strsql,"%4d %-*s  %s (%f %f/%f %f)\n",
      tabb,iceb_u_kolbait(30,fio),fio,gettext("Не выполнены все проводки !"),
      sumd,sumnprov,sumuprov*-1,sumk);

      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

      fprintf(ff,"%4d %-*s  %s (%f %f/%f %f)\n",
      tabb,iceb_u_kolbait(30,fio),fio,gettext("Не выполнены все проводки !"),
      sumd,sumnprov,sumuprov*-1,sumk);
     }      
   }

  /*Смотрим введен ли табель и проверяем его*/
  int kolrd=0,kolkd=0;

  sprintf(strsql,"select dnei,kdnei from Ztab where god=%d and \
mes=%d and tabn=%d",data->gr,data->mr,tabb);
  SQLCURSOR cur2;
  int kolstr2=0;
  if((kolstr2=cur2.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
   {        
    sprintf(strsql,"%s - %d %s %s %d.%d%s\n",
    gettext("Табель не введен"),tabb,fio,
    gettext("за"),data->mr,data->gr,
    gettext("г."));
    
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%s - %d %s %s %d.%d%s\n",
    gettext("Табель не введен"),tabb,fio,
    gettext("за"),data->mr,data->gr,
    gettext("г."));
    goto vp;
   }

  while(cur2.read_cursor(&row1) != 0)
   {
    kolrd+=atoi(row1[0]);
    kolkd+=atoi(row1[1]);
   }  

  if(data->mr == 1 || data->mr == 3 || data->mr == 5 || data->mr == 7 || data->mr == 8 || data->mr == 10 || data->mr == 12)
    kolkd1=31;
  else
    kolkd1=30;

  if(data->mr == 2 )
   {
    if(kolkd > 29 || kolkd < 28)
     {
      sprintf(strsql,"%s %d %s\n",
      gettext("Не верно введено количество календарных дней"),
      tabb,fio);

      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

      fprintf(ff,"%s %d %s\n",
      gettext("Не верно введено количество календарных дней"),
      tabb,fio);
     }
   }
  else
   if(kolkd != kolkd1)
    {
     sprintf(strsql,"%s %d %s\n",
     gettext("Не верно введено количество календарных дней"),
     tabb,fio);


     iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

     fprintf(ff,"%s %d %s\n",
     gettext("Не верно введено количество календарных дней"),
     tabb,fio);
    }

  if(kolkd != 0 && kolrd > kolkd)
   {
     sprintf(strsql,"%s %d %s\n",
     gettext("Количество рабочих дней больше количества календарных дней"),
     tabb,fio);

     iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
     
     fprintf(ff,"%s %d %s\n",
     gettext("Количество рабочих дней больше количества календарных дней"),
     tabb,fio);
   }

 vp:;

   /*Проверяем был ли вход в карточку*/
   sprintf(strsql,"select tabn from Zarn where tabn=%d and god=%d and \
mes=%d",tabb,data->gr,data->mr);
   if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
    {
     sprintf(strsql,"%s %d %s\n",
     gettext("Не было входа в карточку для"),tabb,fio);

     iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

     fprintf(ff,"%s %d %s\n",
     gettext("Не было входа в карточку для"),tabb,fio);
    }
  fprintf(f,"%-4d %-40s %10.2f %10.2f %10.2f %10.2f\n",
  tabb,fio,sal,sumn,sumu,sal+sumn+sumu);

 }

fprintf(f,"\
------------------------------------------------------------------------------------------\n");
fprintf(f,"%-*s %10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(45,gettext("Итого")),gettext("Итого"),itsl,itsn,itsu,itsl+itsn+itsu-itdl);

fprintf(ff,"\
---------------------------------------------------------------\n\
%s %d\n",gettext("Количество работников"),kol);

sprintf(strsql,"%s %d\n",gettext("Количество работников"),kol);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fclose(f);
fclose(ff);

sprintf(strsql,"\n\
%-*s:%15.2f\n\
%-*s:%15.2f\n\
%-*s:%15.2f\n\
%-*s:%15.2f\n\
%-*s:%15.2f\n",
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsl,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),itsn,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),itsu,
iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),itdl,
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsl+itsn+itsu-itdl);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


data->imafr->plus(imaf);
data->imafr->plus(imaf1);
data->naimfr->plus(gettext("Контроль расчёта зарплаты"));
data->naimfr->plus(gettext("Зароботная плата"));


for(int nom=0; nom < data->imafr->kolih(); nom++)
 iceb_ustpeh(data->imafr->ravno(nom),3,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
