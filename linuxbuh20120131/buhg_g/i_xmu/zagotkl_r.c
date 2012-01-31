/*$Id: zagotkl_r.c,v 1.15 2011-02-21 07:35:59 sasa Exp $*/
/*12.03.2010	22.11.2004	Белых А.И.	zagotkl_r.c
Расчёт движения товарно-материальных ценностей форма 1
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "../headers/buhg_g.h"
#include "zagotkl.h"

class zagotkl_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zagotkl_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zagotkl_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zagotkl_r_key_press(GtkWidget *widget,GdkEventKey *event,class zagotkl_r_data *data);
gint zagotkl_r1(class zagotkl_r_data *data);
void  zagotkl_r_v_knopka(GtkWidget *widget,class zagotkl_r_data *data);

void itskl1(int skl,double kk1,double kk2,double kk3,double kk4,double ss1,double ss2,double ss3,double ss4,char str[],FILE *ff,FILE *ffkl2);
void itgrup_kl(int kodgr,double kk1,double kk2,double kk3,double kk4,double ss1,double ss2,double ss3,double ss4,short tr,FILE *ff);
void shdtmcf1k(short dn,short mn,short gn,int sli,FILE *ff,int skl,char nai[],FILE *ffkl2);
void itsh1(char sh[],double k1,double k2,double k3,double k4,double s1,double s2,double s3,double s4,char str[],FILE *ff,FILE *ffkl2);
void gsapp2(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl,char nai[],char str[],FILE *ff,FILE *ffkl2);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern double	okrg1;  /*Округление 1*/
extern double	okrcn;  /*Округление цены*/
extern int      kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;

int zagotkl_r(class zagotkl_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zagotkl_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать заготовку для отчёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zagotkl_r_key_press),&data);

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

repl.plus(gettext("Распечатать заготовку для отчёта"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zagotkl_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zagotkl_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zagotkl_r_v_knopka(GtkWidget *widget,class zagotkl_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zagotkl_r_key_press(GtkWidget *widget,GdkEventKey *event,class zagotkl_r_data *data)
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

gint zagotkl_r1(class zagotkl_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);




short ostkg=0;

iceb_poldan("Отчет в килограммах",strsql,"matnast.alx",data->window);
if(iceb_u_SRAV(strsql,"Вкл",1) == 0)
 ostkg=1;

sprintf(strsql,"select * from Kart");
SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str row;
SQL_str row1;

int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

char imaftmp[30];
FILE *ff1;

sprintf(imaftmp,"dv%d.tmp",getpid());
if((ff1 = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

short dk=0,mk=0,gk=0;

iceb_u_rsdat(&dk,&mk,&gk,data->rk->datao.ravno(),1);

short dn=1,mn=1,gn=gk;

sprintf(strsql,"%s\n%s %d.%d.%d %s %d.%d.%d\n",
gettext("Сортируем записи"),
gettext("Период с"),
dn,mn,gn,
gettext("по"),
dk,mk,gk);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int kgrm=0;
int kolstr2=0;
float kolstr1=0.;
char naim[512];

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[5],0,0) != 0)
   continue;

  kgrm=0;
  memset(naim,'\0',sizeof(naim));
  /*Читаем код группы материалла*/
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("%s %s !\n",gettext("Не найден код материалла"),row[2]);
    continue;
   }
  else
   {
    kgrm=atoi(row1[0]);
    strncpy(naim,row1[1],sizeof(naim)-1);
   }

  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[0],0,0) != 0)
   continue;

  if(provndsw(data->rk->nds.ravno(),row) != 0)
    continue;
/****************
  if(kodmz[0] != '\0')
   {
    if(kodmz[0] == '*' && maskmat != NULL)
     {
      if(iceb_u_proverka(maskmat,row[2],0,0) != 0)
         continue;
     }
    else
      if(iceb_u_proverka(kodmz,row[2],0,0) != 0)
         continue;
   }
*****************/
   if(iceb_u_proverka(data->rk->kodmat.ravno(),row[2],0,0) != 0)
         continue;

  kolstr2++;
  fprintf(ff1,"%s|%s|%d|%s|%s|%s|%s|%.10g|%.5g|%s|%s|\n",
  row[5],row[0],kgrm,naim,row[2],row[1],row[4],atof(row[6]),atof(row[9]),
  row[3],row[10]);

 }

fclose(ff1);


if(data->rk->metka_sort == 0) //Сортировать счет-склад-наименование материалла.
  sprintf(strsql,"sort -o %s -t\\| -k1,2 -k2,3n -k4,5 %s",imaftmp,imaftmp);
//  sprintf(strsql,"sort -o %s -t\\| +0 -1 +1n -2 +3 -4 %s",imaftmp,imaftmp);

if(data->rk->metka_sort == 1) //Сортировать счет-склад-группа-наименование материалла.
  sprintf(strsql,"sort -o %s -t\\| -k1,2 -k2,3n -k3,4n -k4,5 %s",imaftmp,imaftmp);
//  sprintf(strsql,"sort -o %s -t\\| +0 -1 +1n -2 +2n -3 +3 -4 %s",imaftmp,imaftmp);

system(strsql); //Запуск сортировки

sprintf(strsql,"%s.\n",gettext("Делаем отчет"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

if((ff1 = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
FILE *ff;
char imaf[32];

sprintf(imaf,"zag%d.lst",getpid());
data->rk->imaf.plus(imaf);

data->rk->naimf.plus(gettext("Пустографка для отчёта материально-ответственного лица"));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafkl2[32];

sprintf(imafkl2,"zagg%d.lst",getpid());
data->rk->imaf.plus(imafkl2);

data->rk->naimf.plus(gettext("Пустографка для отчёта материально-ответственного лица с приходами"));
FILE *ffkl2;
if((ffkl2 = fopen(imafkl2,"w")) == NULL)
 {
  iceb_er_op_fil(imafkl2,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Оборотная ведомость движния товарно-материальных ценностей"),dn,mn,gn,dk,mk,gk,organ,ff);

SQL_str row_alx;
class SQLCURSOR cur_alx;
const char *imaf_alx={"matzag.alx"};
char zagol[4][512];
memset(zagol,'\0',sizeof(zagol));

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
 }

if(kolstr > 0)
 {

  int i1=0;
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;
    strncpy(zagol[i1++],row_alx[0],sizeof(zagol[i1++])-1);
    zagol[i1-1][strlen(zagol[i1-1])-1]='\0';
    if(i1 == 4)
       break;
   }  
 }
  struct tm *bf;
  bf=localtime(&vremn);
  fprintf(ffkl2,"\x0F");

  fprintf(ffkl2,"%-*s %s\n%135s%s\n\
  				%s %s %d.%d.%d%s %s %d.%d.%d%s %10s%s\n\
  					       %s %d.%d.%d%s  %s - %d:%d%54s%s\n",
  iceb_u_kolbait(133,organ),organ,zagol[0]," ",zagol[1],
  gettext("Оборотная ведомость движения товарно-материальных ценностей"),
  gettext("За период с"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."),
  " ",
  zagol[2],
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min," ",zagol[3]);

int kst=4;

if(data->rk->shet.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  kst++;
 }
if(data->rk->sklad.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  kst++;
 }
if(data->rk->grupa.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Група"),data->rk->grupa.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("Група"),data->rk->grupa.ravno());
  kst++;
 }

if(data->rk->kodmat.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());
  kst++;
 }
if(data->rk->nds.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  kst++;
 }
char shbm[30];
char naiskl[100];

memset(shbm,'\0',sizeof(shbm));
memset(naiskl,'\0',sizeof(naiskl));
int skl=0;
int sli=1;
double itg1=0.,itg2=0.,itg3=0.,itg4=0.,itg5=0.,itg6=0.,itg7=0.,itg8=0.;
double k1=0.,s1=0.,k2=0.,s2=0.,k3=0.,s3=0.,k4=0.,s4=0.;
double kk1=0.,ss1=0.,kk2=0.,ss2=0.,kk3=0.,ss3=0.,kk4=0.,ss4=0.;
int kgrmzp=0;
char str[1024];
memset(str,'\0',sizeof(str));
memset(str,'.',233);
int mvsh=0;
kolstr1=0;
char str1[1000];
class ostatok ost;
char shb[20];
int skl1;
int kodm;
int nk;
char ei[32];
char		mnds='\0';
double cena,nds;
char naimshet[512];
char nomn[100];
double ost1=0.,ost2=0.;
while(fgets(str1,sizeof(str1),ff1) != NULL)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    

  iceb_u_pole(str1,shb,1,'|');
  iceb_u_pole(str1,strsql,2,'|');
  skl1=atoi(strsql);
  iceb_u_pole(str1,strsql,3,'|');
  kgrm=atoi(strsql);

  iceb_u_pole(str1,naim,4,'|');
  iceb_u_pole(str1,strsql,5,'|');
  kodm=atoi(strsql);
  iceb_u_pole(str1,strsql,6,'|');
  nk=atoi(strsql);

  /*Остаток по карточкам*/
  ostkarw(dn,mn,gn,dk,mk,gk,skl1,nk,&ost);

  
  if(ost.ostm[0] == 0. && ost.ostm[1] == 0.)
    continue;
    
  iceb_u_pole(str1,ei,7,'|');
  iceb_u_pole(str1,strsql,8,'|');
  cena=atof(strsql);
  iceb_u_pole(str1,strsql,9,'|');
  nds=atof(strsql);

  iceb_u_pole(str1,strsql,10,'|');
  if(atoi(strsql) == 0)
   mnds=' ';
  if(atoi(strsql) == 1)
   mnds='+';
      
  if(kgrmzp != kgrm)
   {
    if(kgrmzp != 0)
     {
      if(data->rk->metka_sort == 1)
      if(itg1 != 0. || itg2 != 0. || itg3 != 0. || itg4 != 0.)
       {
        itgrup_kl(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,0,ff);
        kst+=2;
       }
     }
    itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
    kgrmzp=kgrm;
   }

  if(iceb_u_SRAV(shb,shbm,1) != 0)
   {
    if(shbm[0] != '\0')
     {
	itskl1(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,str,ff,ffkl2);
	skl=0;
	itsh1(shbm,k1,k2,k3,k4,s1,s2,s3,s4,str,ff,ffkl2);
	kst+=4;
     }
    k1=s1=k2=s2=k3=s3=k4=s4=0.;

    /*Читаем наименование счета*/
    memset(naimshet,'\0',sizeof(naimshet));
    sprintf(strsql,"select nais from Plansh where ns='%s'",shb);
    if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
     {
      printf("Не найден счет %s в плане счетов !\n",shb);
      fprintf(ff,"%s %s !\n",gettext("Не найден счет"),shb);
     }
    else
      strncpy(naimshet,row[0],sizeof(naimshet)-1);       

    sprintf(strsql,"\n%s %s \"%s\"\n",gettext("Счет"),shb,naimshet);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    if(kst > kol_strok_na_liste - 10)
     {
      fprintf(ff,"\f");
      kst=0;
      sli++;
     }


    fprintf(ff,"\n%s %s \"%s\"\n",gettext("Счет"),shb,naimshet);
    kst+=2; 
    gsapp2(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,str,ff,ffkl2);

    mvsh=1;

    strcpy(shbm,shb);
   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      itskl1(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,str,ff,ffkl2);
      kst+=2;
     }
     kk1=ss1=kk2=ss2=kk3=ss3=kk4=ss4=0.;

    /*Читаем наименование склада*/
    naiskl[0]='\0';
    sprintf(strsql,"select naik from Sklad where kod='%d'",skl1);
    if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
     {
      printf("Не найден склад %d в списке складов\n",skl1);
      fprintf(ff,"%s %d !\n",gettext("Не найден склад"),skl1);
     }
    strncpy(naiskl,row[0],sizeof(naiskl));

    sprintf(strsql,"%s: %d %s\n",gettext("Склад"),skl1,naiskl);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    if(kst != 0)
    if(kst > kol_strok_na_liste - 10)
     {
      fprintf(ff,"\f");
      kst=0;
      sli++;
     }
    mvsh=1;
    skl=skl1;

   }
  if(mvsh == 1)
   {
      shdtmcf1k(dn,mn,gn,sli,ff,skl,naiskl,ffkl2);
      kst+=6;
    mvsh=0;
   }

  
  sprintf(nomn,"%s.%d.%d",shb,kgrm,nk);

  
    ost1=ost.ostm[0];
    ost2=ost.ostmc[0];
    char ost3[40],ost4[40];
    memset(ost3,'\0',sizeof(ost3));
    memset(ost4,'\0',sizeof(ost4));
    if(ost.ostm[1] != 0.)
       sprintf(ost3,"%10.10g",ost.ostm[1]);
    if(ost.ostmc[1] != 0.)
       sprintf(ost4,"%12.2f",ost.ostmc[1]);

    //Остаток по документам
     
    ostdokw(dn,mn,gn,dk,mk,gk,skl1,nk,&ost);

    fprintf(ff,"\
%4d %-*.*s %-*s %-*s %c%2.2g %7.7g %7.7g %15.2f|               |               |               |               |               |               |               |             |\n%s\n",
    kodm,
    iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
    iceb_u_kolbait(13,nomn),nomn,
    iceb_u_kolbait(10,ei),ei,
    mnds,
    nds,ost1,ost.ostm[0],ost2,str);

    gsapp2(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,str,ff,ffkl2);

    fprintf(ffkl2,"\
%4d %-*.*s %-*s %-*s %c%2.2g %10.10g %10.10g %12.2f %10s %12s|       |       |       |       |       |       |       |       |       |       |         |        |\n%s\n",
    kodm,
    iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
    iceb_u_kolbait(13,nomn),nomn,
    iceb_u_kolbait(10,ei),ei,
    mnds,nds,cena,ost1,ost2,ost3,ost4,str);
     
  k1+=ost.ostm[0];   kk1+=ost.ostm[0]; itg1+=ost.ostm[0];
  s1+=ost.ostmc[0];  ss1+=ost.ostmc[0]; itg5+=ost.ostmc[0];

  k2+=ost.ostm[1];   kk2+=ost.ostm[1];  itg2+=ost.ostm[1];
  s2+=ost.ostmc[1];  ss2+=ost.ostmc[1]; itg6+=ost.ostmc[1];

  k3+=ost.ostm[2];   kk3+=ost.ostm[2];    itg3+=ost.ostm[2];
  s3+=ost.ostmc[2];  ss3+=ost.ostmc[2];   itg7+=ost.ostmc[2];

  k4+=ost.ostm[3];   kk4+=ost.ostm[3];    itg4+=ost.ostm[3];
  s4+=ost.ostmc[3];  ss4+=ost.ostmc[3];   itg8+=ost.ostmc[3];
 }


itskl1(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,str,ff,ffkl2);
itsh1(shbm,k1,k2,k3,k4,s1,s2,s3,s4,str,ff,ffkl2);


iceb_podpis(ffkl2,data->window);

fclose(ffkl2);



iceb_podpis(ff,data->window);
fclose(ff);

 
fclose(ff1);
unlink(imaftmp);

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
/*****************/
/*Итого по группе*/
/*****************/
void            itgrup_kl(int kodgr,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,
short tr,
FILE *ff)
{
char		bros[512];
char		naigr[40];
SQL_str         row;
SQLCURSOR       cur;
if(tr == 1)
  return;

/*Читаем наименование группы*/
memset(naigr,'\0',sizeof(naigr));
sprintf(bros,"select naik from Grup where kod=%d",kodgr);
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  printf("%s %d !\n","Не нашли группу",kodgr);
 }
else
 strncpy(naigr,row[0],sizeof(naigr)-1);

sprintf(bros,"%s %d %s:",gettext("Итого по группе"),kodgr,naigr);


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%*s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n",iceb_u_kolbait(90,bros),bros,
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

}
/*******/
/*Шапка для кладовщика*/
/*******/
void		shdtmcf1k(short dn,short mn,short gn,int sli,FILE *ff,
int skl,char nai[],FILE *ffkl2)
{
char		str[300];
char		bros[512];

memset(str,'\0',sizeof(str));

memset(str,'-',233);
sprintf(bros,"%s: %d %s",gettext("Склад"),skl,nai);

fprintf(ff,"%-*s %s N%d\n%s\n",iceb_u_kolbait(196,bros),bros,gettext("Лист"),sli,str);

fprintf(ff,gettext("    |             Наименование               |  Номенкла-  | Единица  |   | Остаток на  %02d.%02d.%4dг.      |               |               |               |               |               |               |               |             |\n"),
dn,mn,gn),
fprintf(ff,gettext("Код |        товарно-материальных            |турный номер | измерения|НДС|-------------------------------|               |               |               |               |               |               |               |             |\n"));
fprintf(ff,gettext("    |             ценностей                  |             |          |   |   Количество  |    Сумма      |               |               |               |               |               |               |               |             |\n"));
fprintf(ff,"%s\n",str);

memset(str,'\0',sizeof(str));
memset(str,'-',233);

fprintf(ffkl2,"%-*s %s N%d\n%s\n",iceb_u_kolbait(196,bros),bros,gettext("Лист"),sli,str);

fprintf(ffkl2,gettext("\
    |             Наименование               |  Номенкла-  | Единица  |   |          |Остаток на %02d.%02d.%4dг.|       Приход          |               |               |               |               |               |                  |\n"),
dn,mn,gn),
fprintf(ffkl2,gettext("\
Код |        товарно-материальных            |турный номер | измерения|НДС|   Цена   |-----------------------|-----------------------|---------------|---------------|---------------|---------------|---------------|------------------|\n"));
fprintf(ffkl2,gettext("\
    |             ценностей                  |             |          |   |          |Количество|   Сумма    |Количество|   Сумма    |       |       |       |       |       |       |       |       |       |       |         |        |\n"));

fprintf(ffkl2,"%s\n",str);

}
/****************/
/*Итого по счету для кладовщика*/
/****************/
void            itsh1(char sh[],double k1,double k2,double k3,double k4,
double s1,double s2,double s3,double s4,char str[],FILE *ff,FILE *ffkl2)
{
char		bros[108];

sprintf(bros,"%s %s",gettext("Итого по счету"),sh);

memset(str,'\0',sizeof(str));

memset(str,'-',233);
fprintf(ff,"%-*s %15.2f\n%s\n",iceb_u_kolbait(90,bros),bros,s1,str);
fprintf(ffkl2,"%-*s %10.10g %12.2f %10.10g %12.2f\n%s\n",
iceb_u_kolbait(85,bros),bros,k1,s1,k2,s2,str);

memset(str,'\0',sizeof(str));
memset(str,'.',233);
}

/*****************/
/*Итого по складу для кладовщика*/
/*****************/
void            itskl1(int skl,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,char str[],
FILE *ff,FILE *ffkl2)
{
char		bros[512];

sprintf(bros,"%s %d",gettext("Итого по складу"),skl);

memset(str,'\0',sizeof(str));
memset(str,'-',233);

fprintf(ff,"%-*s %15.2f\n%s\n",iceb_u_kolbait(90,bros),bros,ss1,str);
fprintf(ffkl2,"%-*s %10.10g %12.2f %10.10g %12.2f\n%s\n",
iceb_u_kolbait(85,bros),bros,kk1,ss1,kk2,ss2,str);
/*********
printw("%s %10.10g %12.2f %10.10g %12.2f\n",
bros,kk1,ss1,kk2,ss2);
OSTANOV();
**********/
memset(str,'.',233);
}
/*******/
/*Счетчик для кладовщика*/
/*******/
void            gsapp2(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl,  //Счетчик листов
int *kst, //Счетчик строк
int skl,char nai[],
char str[],
FILE *ff,FILE *ffkl2)
{

*kst+=2;

if(*kst >= kol_strok_na_liste-1)
 {
  fprintf(ff,"\f");
  fprintf(ffkl2,"\f");

  *sl+=1;
  shdtmcf1k(dn,mn,gn,*sl,ff,skl,nai,ffkl2);
  *kst=6;

  memset(str,'.',233);

 }

}
