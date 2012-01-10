/*$Id: dvtmcf1_r.c,v 1.24 2011-02-21 07:35:51 sasa Exp $*/
/*11.01.2010	22.11.2004	Белых А.И.	dvtmcf1_r.c
Расчёт движения товарно-материальных ценностей форма 1
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "dvtmcf1.h"

class dvtmcf1_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class dvtmcf1_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  dvtmcf1_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   dvtmcf1_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf1_r_data *data);
gint dvtmcf1_r1(class dvtmcf1_r_data *data);
void  dvtmcf1_r_v_knopka(GtkWidget *widget,class dvtmcf1_r_data *data);

void itskl(int skl,double kk1,double kk2,double kk3,double kk4,double ss1,double ss2,double ss3,double ss4,FILE *ffuz,FILE *ff,FILE *ffitog,class dvtmcf1_r_data *data);
void gsapp(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl,char nai[],FILE *ffuz,FILE *ff);
void itsh(char sh[],double k1,double k2,double k3,double k4,double s1,double s2,double s3,double s4,FILE *ffuz,FILE *ff,FILE *ffitog,class dvtmcf1_r_data *data);
void itkg(char sh[],int skl,double k1,double k2,double k3,double k4,short met,FILE *ffuz,FILE *ff);
void shdtmcf1(short dn,short mn,short gn,short dk,short mk,short gk,int sli,FILE *ff,int skl,char nai[],FILE *ffuz);
void itgrup(int kodgr,double kk1,double kk2,double kk3,double kk4,double ss1,double ss2,double ss3,double ss4,FILE *ffuz,short tr,FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern double	okrg1;  /*Округление 1*/
extern double	okrcn;  /*Округление цены*/
extern int      kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;

int dvtmcf1_r(class dvtmcf1_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class dvtmcf1_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечетать движение товарно-материальных ценностей (форма 1)"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(dvtmcf1_r_key_press),&data);

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

repl.plus(gettext("Распечетать движение товарно-материальных ценностей (форма 1)"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(dvtmcf1_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)dvtmcf1_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  dvtmcf1_r_v_knopka(GtkWidget *widget,class dvtmcf1_r_data *data)
{
//printf("dvtmcf1_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dvtmcf1_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf1_r_data *data)
{
// printf("dvtmcf1_r_key_press\n");
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

gint dvtmcf1_r1(class dvtmcf1_r_data *data)
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
SQLCURSOR cur,curtmp;
SQLCURSOR cur1;
SQL_str row,rowtmp;
SQL_str row1;

int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

class iceb_tmptab tabtmp;
const char *imatmptab={"dvtmcf1"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(24) not null,\
skl int not null,\
kgm int not null,\
naim char(112) not null,\
km int not null,\
nk int not null,\
ei char(24) not null,\
cena double(15,6) not null,\
nds float(2) not null,\
mnds smallint not null,\
fas float(2) not null)",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  
/*********
char imaftmp[32];
FILE *ff1;

sprintf(imaftmp,"dv%d.tmp",getpid());
if((ff1 = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
**************/
short dn,mn,gn;
short dk,mk,gk;

iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);

sprintf(strsql,"%s\n%s %d.%d.%d %s %d.%d.%d\n",
gettext("Сортируем записи"),
gettext("Период с"),
dn,mn,gn,
gettext("по"),
dk,mk,gk);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int kgrm=0;
//int kolstr2=0;
float kolstr1=0.;
class iceb_u_str naim("");

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[5],0,0) != 0)
   continue;

  kgrm=0;
  naim.new_plus("");
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
    naim.new_plus(row1[1]);
   }

  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[0],0,0) != 0)
   continue;

  if(provndsw(data->rk->nds.ravno(),row) != 0)
    continue;

   if(iceb_u_proverka(data->rk->kodmat.ravno(),row[2],0,0) != 0)
         continue;

//  kolstr2++;
/************
  fprintf(ff1,"%s|%s|%d|%s|%s|%s|%s|%.10g|%.5g|%s|%s|\n",
  row[5],row[0],kgrm,naim.ravno(),row[2],row[1],row[4],atof(row[6]),atof(row[9]),
  row[3],row[10]);
*************/
  sprintf(strsql,"insert into %s values ('%s',%s,%d,'%s',%s,%s,'%s',%.10g,%.5g,%s,%s)",
  imatmptab,
  row[5],row[0],kgrm,naim.ravno_filtr(),row[2],row[1],row[4],atof(row[6]),atof(row[9]),
  row[3],row[10]);
  iceb_sql_zapis(strsql,1,0,data->window);    

 }

//fclose(ff1);


if(data->rk->metka_sort == 0) //Сортировать счет-склад-наименование материалла.
 {
//  sprintf(strsql,"sort -o %s -t\\| -k1,2 -k2,3n -k4,5 %s",imaftmp,imaftmp);
//  sprintf(strsql,"sort -o %s -t\\| +0 -1 +1n -2 +3 -4 %s",imaftmp,imaftmp);
  sprintf(strsql,"select * from %s order by sh asc,skl asc,naim asc",imatmptab);
 }
if(data->rk->metka_sort == 1) //Сортировать счет-склад-группа-наименование материалла.
 {
//  sprintf(strsql,"sort -o %s -t\\| -k1,2 -k2,3n -k3,4n -k4,5 %s",imaftmp,imaftmp);
//  sprintf(strsql,"sort -o %s -t\\| +0 -1 +1n -2 +2n -3 +3 -4 %s",imaftmp,imaftmp);
  sprintf(strsql,"select * from %s order by sh asc,skl asc,kgm asc,naim asc",imatmptab);
 }
//system(strsql); //Запуск сортировки
int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s.\n",gettext("Делаем отчет"));
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
/**************
if((ff1 = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
*************/
FILE *ff;
char imaf[32];

sprintf(imaf,"zag%d.lst",getpid());
data->rk->imaf.plus(imaf);

repl.new_plus(gettext("Оборотная ведомость движния товарно-материальных ценностей"));
repl.plus(" (");
repl.plus(gettext("формат"));
repl.plus(" A3)");

data->rk->naimf.plus(repl.ravno());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
FILE *ffuz;
char imafuz[32];

sprintf(imafuz,"zaguz%d.lst",getpid());
data->rk->imaf.plus(imafuz);

repl.new_plus(gettext("Оборотная ведомость движния товарно-материальных ценностей"));
repl.plus(" (");
repl.plus(gettext("формат"));
repl.plus(" A4)");

data->rk->naimf.plus(repl.ravno());

if((ffuz = fopen(imafuz,"w")) == NULL)
 {
  iceb_er_op_fil(imafuz,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

/***************
if(tipras == 1)
 {
  sprintf(imafkl2,"zagg%d.lst",getpid());

  if((ffkl2 = fopen(imafkl2,"w")) == NULL)
   ERROR1(gettext("Ошибка открытия файла"),errno,imafkl2);
  startfil(ffkl2);
 }
else
 {
*****************/
char imafitog[32];
sprintf(imafitog,"zai%d.lst",getpid());
data->rk->imaf.plus(imafitog);

data->rk->naimf.plus(gettext("Свод движения материальных ценностей"));
FILE *ffitog;
if((ffitog = fopen(imafitog,"w")) == NULL)
 {
  iceb_er_op_fil(imafitog,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Свод движения материальных ценностей"),dn,mn,gn,dk,mk,gk,organ,ffitog);


iceb_u_zagolov(gettext("Оборотная ведомость движния товарно-материальных ценностей"),dn,mn,gn,dk,mk,gk,organ,ff);
iceb_u_zagolov(gettext("Оборотная ведомость движния товарно-материальных ценностей"),dn,mn,gn,dk,mk,gk,organ,ffuz);


int kst=4;

if(data->rk->shet.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  kst++;
 }
if(data->rk->sklad.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  kst++;
 }
if(data->rk->grupa.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Група"),data->rk->grupa.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("Група"),data->rk->grupa.ravno());
  kst++;
 }

if(data->rk->kodmat.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());
  kst++;
 }
if(data->rk->nds.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  kst++;
 }
char shbm[32];
char naiskl[112];

memset(shbm,'\0',sizeof(shbm));
memset(naiskl,'\0',sizeof(naiskl));
int skl=0;
int sli=1;
double itg1=0.,itg2=0.,itg3=0.,itg4=0.,itg5=0.,itg6=0.,itg7=0.,itg8=0.;
double k1=0.,s1=0.,k2=0.,s2=0.,k3=0.,s3=0.,k4=0.,s4=0.;
double kk1=0.,ss1=0.,kk2=0.,ss2=0.,kk3=0.,ss3=0.,kk4=0.,ss4=0.;
double kg0=0.,kg1=0.,kg2=0.,kg3=0.;
double kgs0=0.,kgs1=0.,kgs2=0.,kgs3=0.;
int kgrmzp=0;
char str[512];
memset(str,'\0',sizeof(str));
memset(str,'.',233);
int mvsh=0;
kolstr1=0;
//char str1[1000];
class ostatok ost;
char shb[20];
int skl1;
int kodm;
int nk;
char ei[32];
char		mnds='\0';
double cena,nds;
char naimshet[512];
char nomn[112];

//while(fgets(str1,sizeof(str1),ff1) != NULL)
// {
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  iceb_pbar(data->bar,kolstrtmp,++kolstr1);    
 
/********
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
**************/
  strncpy(shb,rowtmp[0],sizeof(shb)-1);
  skl1=atoi(rowtmp[1]);
  kgrm=atoi(rowtmp[2]);
  naim.new_plus(rowtmp[3]);
  kodm=atoi(rowtmp[4]);
  nk=atoi(rowtmp[5]);

  /*Остаток по карточкам*/
  ostkarw(dn,mn,gn,dk,mk,gk,skl1,nk,&ost);

  if(ost.ostm[0] == 0. && ost.ostm[1] == 0. && ost.ostm[3] == 0. && fabs(ost.ostmc[3]) < 0.009)
     continue;
/************  
  iceb_u_pole(str1,ei,7,'|');
  iceb_u_pole(str1,strsql,8,'|');
  cena=atof(strsql);
  iceb_u_pole(str1,strsql,9,'|');
  nds=atof(strsql);

  iceb_u_pole(str1,strsql,10,'|');
***********/
  strncpy(ei,rowtmp[6],sizeof(ei)-1);
  cena=atof(rowtmp[7]);
  nds=atof(rowtmp[8]);
  strncpy(strsql,rowtmp[9],sizeof(strsql)-1);

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
        itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,0,ff);
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
        if(data->rk->metka_sort == 1)
        if(itg1 != 0. || itg2 != 0. || itg3 != 0. || itg4 != 0.)
         {
          itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,0,ff);
          itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
          kst+=2;
         }
	itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ffuz,ff,ffitog,data);
        kst+=2;
        if(ostkg == 1 && fabs((kk1+kk2+kk3+kk4) - (kg0+kg1+kg2+kg3)) > 0.0001 &&
        kg0+kg1+kg2+kg3 > 0.0001)
         { 
          itkg(shbm,skl,kg0,kg1,kg2,kg3,1,ffuz,ff);
          kst+=2;
          kg0=kg1=kg2=kg3=0.;
         }
	itsh(shbm,k1,k2,k3,k4,s1,s2,s3,s4,ffuz,ff,ffitog,data);
        kst+=2;
        if(ostkg == 1 && fabs((k1+k2+k3+k4) - (kgs0+kgs1+kgs2+kgs3)) > 0.0001 &&
         kgs0+kgs1+kgs2+kgs3 > 0.0001)
         { 
          itkg(shbm,skl,kgs0,kgs1,kgs2,kgs3,0,ffuz,ff);
          kst+=2;
          kgs0=kgs1=kgs2=kgs3=0.;
         }
	skl=0;
/****************
      if(tipras == 1)
       {
	itskl1(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,str,ff,ffkl2);
	skl=0;
	itsh1(shbm,k1,k2,k3,k4,s1,s2,s3,s4,str,ff,ffkl2);
	kst+=4;
       }
*******************/
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
      fprintf(ffuz,"\f");
//      if(ffkl2 != NULL)
//         fprintf(ffkl2,"\f");
      kst=0;
      sli++;
     }


    fprintf(ff,"\n%s %s \"%s\"\n",gettext("Счет"),shb,naimshet);
    fprintf(ffuz,"\n%s %s \"%s\"\n",gettext("Счет"),shb,naimshet);
    if(ffitog != NULL)
      fprintf(ffitog,"\n%s %s \"%s\"\n",gettext("Счет"),shb,naimshet);
    kst+=2; 
//    if(tipras == 0)
     gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ffuz,ff);
//    else
//     gsapp2(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,str,ff,ffkl2);

    mvsh=1;

    strcpy(shbm,shb);
   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
        if(data->rk->metka_sort == 1)
        if(itg1 != 0. || itg2 != 0. || itg3 != 0. || itg4 != 0.)
         {
          itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,0,ff);
          itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
          kst+=2;
         }

	itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ffuz,ff,ffitog,data);
        kst+=2;
        if(ostkg == 1 && fabs((kk1+kk2+kk3+kk4) - (kg0+kg1+kg2+kg3)) > 0.0001 &&
        kg0+kg1+kg2+kg3 > 0.0001)
         { 
          itkg(shbm,skl,kg0,kg1,kg2,kg3,1,ffuz,ff);
          kst+=2;
          kg0=kg1=kg2=kg3=0.;
         }

//      if(tipras == 1)
//       {
//	itskl1(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,str,ff,ffkl2);
//        kst+=2;
//       }
     }
     kk1=ss1=kk2=ss2=kk3=ss3=kk4=ss4=0.;

    /*Читаем наименование склада*/
    memset(naiskl,'\0',sizeof(naiskl));
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
      fprintf(ffuz,"\f");
//      if(ffkl2 != NULL)
//         fprintf(ffkl2,"\f");
      kst=0;
      sli++;
     }
    mvsh=1;
    skl=skl1;

   }
  if(mvsh == 1)
   {
//    if(tipras == 0)
//     {
      shdtmcf1(dn,mn,gn,dk,mk,gk,sli,ff,skl,naiskl,ffuz);
      kst+=6; 
//     }
/***************
    if(tipras == 1)
     {
      shdtmcf1k(dn,mn,gn,sli,ff,skl,naiskl,ffkl2);
      kst+=6;
     }
***************/
    mvsh=0;
   }

  
  /*Остаток по карточкам*/
//  ostkar(dn,mn,gn,dk,mk,gk,skl1,nk,ostg,ostm,ostgc,ostmc);

//  sprintf(nomn,"%d.%s.%d.%d",skl1,shb,kgrm,nk);
  sprintf(nomn,"%s.%d.%d",shb,kgrm,nk);


//  if(tipras == 0)
//   {
    gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ffuz,ff);

    fprintf(ff,"\
%4d %-*.*s %-*s %-*s %c%2.2g %14.10g %15.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n",
    kodm,
    iceb_u_kolbait(40,naim.ravno()),iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(13,nomn),nomn,
    iceb_u_kolbait(10,ei),ei,
    mnds,nds,cena,
    ost.ostm[0],ost.ostmc[0],ost.ostm[1],ost.ostmc[1],ost.ostm[2],ost.ostmc[2],ost.ostm[3],ost.ostmc[3]);

    fprintf(ffuz,"\
%4d %-*.*s %-*s %-*s %c%2.2g %7s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",
    kodm,
    iceb_u_kolbait(23,naim.ravno()),iceb_u_kolbait(23,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(13,nomn),nomn,
    iceb_u_kolbait(3,ei),ei,
    mnds,nds,iceb_prcn(cena),
    ost.ostm[0],ost.ostmc[0],ost.ostm[1],ost.ostmc[1],ost.ostm[2],ost.ostmc[2],ost.ostm[3],ost.ostmc[3]);


     if(ostkg == 1)
      {
       double ost1=0.,ost2=0.,ost3=0.,ost4=0.;
       double fas=0.;
//       iceb_u_pole(str1,strsql,11,'|');
//       fas=atof(strsql);
       fas=atof(rowtmp[10]);
       if(fas > 0.0000001)
        { 
         ost1=ost.ostm[0]*fas;
         ost2=ost.ostm[1]*fas;
         ost3=ost.ostm[2]*fas;
         ost4=ost.ostm[3]*fas;
         kg0+=ost1;
         kg1+=ost2;
         kg2+=ost3;
         kg3+=ost4;
         kgs0+=ost1;
         kgs1+=ost2;
         kgs2+=ost3;
         kgs3+=ost4;

         gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl,ffuz,ff);

         fprintf(ff,"\
%4s %-40.40s %-13s %-10s %3s %14s %15.10g %15s %15.10g %15s %15.10g %15s \
%15.10g %15s\n"," "," "," "," "," "," ",
ost1," ",ost2," ",ost3," ",ost4," ");

         fprintf(ffuz,"\
%4s %-23.23s %-13s %-3s %3s %7s %8.8g %8s %8.8g %8s %8.8g %8s \
%8.8g %8s\n"," "," "," "," "," "," ",
ost1," ",ost2," ",ost3," ",ost4," ");

        } 
       else
         if(iceb_u_SRAV(ei,"Кг",0) == 0 || iceb_u_SRAV(ei,"кг",0) == 0 ||
          iceb_u_SRAV(ei,"КГ",0) == 0)
          {
           kg0+=ost.ostm[0];
           kg1+=ost.ostm[1];
           kg2+=ost.ostm[2];
           kg3+=ost.ostm[3];
           kgs0+=ost.ostm[0];
           kgs1+=ost.ostm[1];
           kgs2+=ost.ostm[2];
           kgs3+=ost.ostm[3];
          }             
      }     

  k1+=ost.ostm[0];   kk1+=ost.ostm[0]; itg1+=ost.ostm[0];
  s1+=ost.ostmc[0];  ss1+=ost.ostmc[0]; itg5+=ost.ostmc[0];

  k2+=ost.ostm[1];   kk2+=ost.ostm[1];  itg2+=ost.ostm[1];
  s2+=ost.ostmc[1];  ss2+=ost.ostmc[1]; itg6+=ost.ostmc[1];

  k3+=ost.ostm[2];   kk3+=ost.ostm[2];    itg3+=ost.ostm[2];
  s3+=ost.ostmc[2];  ss3+=ost.ostmc[2];   itg7+=ost.ostmc[2];

  k4+=ost.ostm[3];   kk4+=ost.ostm[3];    itg4+=ost.ostm[3];
  s4+=ost.ostmc[3];  ss4+=ost.ostmc[3];   itg8+=ost.ostmc[3];
 }

if(data->rk->metka_sort == 1)
    itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,0,ff);

itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ffuz,ff,ffitog,data);

if(ostkg == 1 && fabs((kk1+kk2+kk3+kk4) - (kg0+kg1+kg2+kg3)) > 0.0001 &&  kg0+kg1+kg2+kg3 > 0.0001)
  itkg(shbm,skl,kg0,kg1,kg2,kg3,1,ffuz,ff);

itsh(shbm,k1,k2,k3,k4,s1,s2,s3,s4,ffuz,ff,ffitog,data);

if(ostkg == 1 && fabs((k1+k2+k3+k4) - (kgs0+kgs1+kgs2+kgs3)) > 0.0001 &&  kgs0+kgs1+kgs2+kgs3 > 0.0001)
   itkg(shbm,skl,kgs0,kgs1,kgs2,kgs3,0,ffuz,ff);

iceb_podpis(ffitog,data->window);
fprintf(ffitog,"\x1b\x6C%c",1); /*Установка левого поля*/
fclose(ffitog);


fprintf(ff,"\x12");
fprintf(ffuz,"\x12");

iceb_podpis(ff,data->window);
fclose(ff);

iceb_podpis(ffuz,data->window);
fclose(ffuz);
//if(tipras == 1)
// unlink(imafuz);
 
//fclose(ff1);
//unlink(imaftmp);
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
/*****************/
/*Итого по группе*/
/*****************/
void            itgrup(int kodgr,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,
FILE *ffuz,  //узкая распечтака
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
%15.10g %15.2f\n",
iceb_u_kolbait(90,bros),bros,
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

fprintf(ffuz,"\
----------------------------------------------------------------------------------------------------------------------------------\n\
%*s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_u_kolbait(58,bros),bros,
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);
}
/*******/
/*Шапка*/
/*******/
void		shdtmcf1(short dn,short mn,short gn,short dk,short mk,
short gk,int sli,FILE *ff,int skl,char nai[],
FILE *ffuz) //узкая распечтака*/
{
char		bros[512];

  sprintf(bros,"%s: %d %s",gettext("Склад"),skl,nai);
  fprintf(ff,"%-*s %s N%d\n",iceb_u_kolbait(196,bros),bros,gettext("Лист"),sli);
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("    |             Наименование               |  Номенкла-  | Единица  |   |              | Остаток на  %02d.%02d.%4dг.      |     Приход   за период        |    Расход  за период          |  Остаток на %02d.%02d.%4dг.      |\n"),
  dn,mn,gn,dk,mk,gk);
  fprintf(ff,gettext("Код |        товарно-материальных            |турний номер | измерения|НДС|   Ц е н а    |-------------------------------|------------------------------------------------------------------------------------------------\n"));
  fprintf(ff,gettext("    |             ценностей                  |             |          |   |              |   Количество  |  Сумма        |    Количество |     Сумма     |    Количество |     Сумма     |   Количество  |    Сумма      |\n"));


  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  sprintf(bros,"%s: %d %s %s %02d.%02d.%04d%s => %02d.%02d.%04d%s",
  gettext("Склад"),skl,nai,
  gettext("Период с"),
  dn,mn,gn,
  gettext("г."),
  dk,mk,gk,
  gettext("г."));
  
  fprintf(ffuz,"%-*s %s N%d\n",iceb_u_kolbait(120,bros),bros,gettext("Лист"),sli);

  fprintf(ffuz,"\
-----------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ffuz,gettext("\
    |     Наименование      |  Номенкла-  |Ед.|   |       |     Остаток     |      Приход     |      Расход     |     Остаток     |\n\
Код |товарно-материальных   |турный номер |изм|НДС|Ц е н а|------------------------------------------------------------------------\n\
    |     ценностей         |             |   |   |       |Колич.  |  Сумма |Колич.  | Сумма  |Колич.  | Сумма  |Колич.  | Сумма  |\n"));

  fprintf(ffuz,"\
-----------------------------------------------------------------------------------------------------------------------------------\n");
}
/****************/
/*Итого в килограммах по счету и по складу*/
/****************/
void            itkg(char sh[],int skl,double k1,double k2,double k3,
double k4,
short met, //0-по счету 1-по складу*/
FILE *ffuz,  //узкая распечатка
FILE *ff)
{
char		bros[512];
if(met == 1)
  sprintf(bros,"%s %d %s",gettext("Итого по складу"),
  skl,
  gettext("Кг."));

if(met == 0)
  sprintf(bros,"%s %s %s",
  gettext("Итого по счету"),
  sh,
  gettext("Кг."));

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%4s %-*.*s %-13s %-10s %4s %14s %15.10g %15s %15.10g %15s %15.10g %15s \
%15.10g %15s\n"," ",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros," "," "," "," ",
k1," ",k2," ",k3," ",k4," ");

fprintf(ffuz,"\
----------------------------------------------------------------------------------------------------------------------------------\n\
%*s %-13s %-3s %3s %7s %8.8g %8s %8.8g %8s %8.8g %8s \
%8.8g %8s\n",iceb_u_kolbait(58,bros),bros," "," "," "," ",
k1," ",k2," ",k3," ",k4," ");

}
/****************/
/*Итого по счету*/
/****************/
void            itsh(char sh[],double k1,double k2,double k3,double k4,
double s1,double s2,double s3,double s4,
FILE *ffuz, //узкая распечтака
FILE *ff,FILE *ffitog,
class dvtmcf1_r_data *data)
{
char		bros[1024];

sprintf(bros,"%s %s",gettext("Итого по счету"),sh);


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%4s %-*.*s %-13s %-10s %14s %-4s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n"," ",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros," "," "," "," ",
k1,s1,k2,s2,k3,s3,k4,s4);

fprintf(ffuz,"\
----------------------------------------------------------------------------------------------------------------------------------\n\
%*s %-13s %-3s %3s %7s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_u_kolbait(28,bros),bros," "," "," "," ",
k1,s1,k2,s2,k3,s3,k4,s4);


sprintf(bros,"%s %s:\n\
%-10s %15.10g %15.2f\n\
%-10s %15.10g %15.2f\n\
%-10s %15.10g %15.2f\n\
%-10s %15.10g %15.2f\n\n",
gettext("Итого по счету"),
sh,
gettext("Остаток"),
k1,s1,
gettext("Приход"),
k2,s2,
gettext("Расход"),
k3,s3,
gettext("Остаток"),
k4,s4);

iceb_printw(iceb_u_toutf(bros),data->buffer,data->view);

if(ffitog != NULL)
  fprintf(ffitog,"%s %s:\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\n",
gettext("Итого по счету"),
sh,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
k1,s1,
iceb_u_kolbait(10,gettext("Приход")),gettext("Приход"),
k2,s2,
iceb_u_kolbait(10,gettext("Расход")),gettext("Расход"),
k3,s3,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
k4,s4);


}
/*******/
/*Счетчик*/
/*******/
void            gsapp(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl, //Счетчик листов
int *kst, //Счетчик строк
int skl,char nai[],
FILE *ffuz, //узкая распечтака
FILE *ff)
{

*kst+=1;
/*
printw("gsapp kst=%d sl=%d\n",*kst,*sl);
refresh();
*/
if(*kst >= kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  fprintf(ffuz,"\f");

  *sl+=1;
  shdtmcf1(dn,mn,gn,dk,mk,gk,*sl,ff,skl,nai,ffuz);
  *kst=6;  
 }

}

/*****************/
/*Итого по складу*/
/*****************/
void            itskl(int skl,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,
FILE *ffuz, //узкая распечтака
FILE *ff,FILE *ffitog,
class dvtmcf1_r_data *data)
{
char		bros[1024];

sprintf(bros,"%s %d",gettext("Итого по складу"),skl);


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%4s %-*.*s %-13s %-10s %14s %-4s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n"," ",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros," "," "," "," ",
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

fprintf(ffuz,"\
----------------------------------------------------------------------------------------------------------------------------------\n\
%*s %-13s %-3s %3s %7s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_u_kolbait(28,bros),bros," "," "," "," ",
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

sprintf(bros,"%s %d:\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\n",
gettext("Итого по складу"),
skl,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
kk1,ss1,
iceb_u_kolbait(10,gettext("Приход")),gettext("Приход"),
kk2,ss2,
iceb_u_kolbait(10,gettext("Расход")),gettext("Расход"),
kk3,ss3,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
kk4,ss4);

iceb_printw(iceb_u_toutf(bros),data->buffer,data->view);

if(ffitog != NULL)
  fprintf(ffitog,"%s %d:\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\n",
gettext("Итого по складу"),
skl,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
kk1,ss1,
iceb_u_kolbait(10,gettext("Приход")),gettext("Приход"),
kk2,ss2,
iceb_u_kolbait(10,gettext("Расход")),gettext("Расход"),
kk3,ss3,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
kk4,ss4);

}
