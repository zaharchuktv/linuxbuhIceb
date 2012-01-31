/*$Id: dvtmcf2_r.c,v 1.17 2011-02-21 07:35:51 sasa Exp $*/
/*19.06.2010	22.11.2004	Белых А.И.	dvtmcf2_r.c
Расчёт движения товарно-материальных ценностей форма 1
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "dvtmcf2.h"

class dvtmcf2_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class dvtmcf2_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  dvtmcf2_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   dvtmcf2_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf2_r_data *data);
gint dvtmcf2_r1(class dvtmcf2_r_data *data);
void  dvtmcf2_r_v_knopka(GtkWidget *widget,class dvtmcf2_r_data *data);

void sozshgr(iceb_u_int *GRUP,iceb_u_spisok *SHET,int *kolgrup,int *kolshet,GtkWidget *wpredok);
void sapitsh_f2(FILE *ff);

void gsapp1(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,FILE *ff);

void rasitogo(int metka,double mas[2],int *klst,char kod[],FILE *ff,short metoth,class dvtmcf2_r_data *data,short,short,short,short,short,short,int*);

void  dvtmcf2ot(char imafsort[],short dn,short mn,short gn,short dk,short mk,short gk,int kolstr2,short tipz,FILE *ff,
short metoth,class dvtmcf2_r_data *data);

void dvtmcf2_r_sapka(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern double	okrg1;  /*Округление 1*/
extern double	okrcn;  /*Округление цены*/
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;
extern iceb_u_str shrift_ravnohir;

int dvtmcf2_r(class dvtmcf2_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class dvtmcf2_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечетать движение товарно-материальных ценностей (форма 1)"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(dvtmcf2_r_key_press),&data);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(dvtmcf2_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)dvtmcf2_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  dvtmcf2_r_v_knopka(GtkWidget *widget,class dvtmcf2_r_data *data)
{
//printf("dvtmcf2_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dvtmcf2_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf2_r_data *data)
{
// printf("dvtmcf2_r_key_press\n");
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
/*Распечатка массива группа-счет          */
/******************************************/

void rasgrsh(const char *imaf,
short dn,short mn,short gn,
short dk,short mk,short gk,
iceb_u_int *GRUP, //Массив количества групп
iceb_u_spisok *SHET, //Список счетов
int kolgrup, //Количество групп
int kolshet, //Количество счетов
double *sumshgr,
const char *shetz,
const char *kodmz,const char *grz,const char *kodop,const char *korz,
const char *sklz,const char *nomdokp,const char *ndsc,
GtkWidget *wpredok)
{
FILE	*ff;
int	i,i1,i2;
int	nomgrup;
char	shet[32];
char	strsql[512];
SQL_str	row;
SQLCURSOR cur;
char	naimgrup[80];
char	stroka[1024];
double	itogkol=0.;
double  itogo=0.;


if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }    
iceb_u_startfil(ff);

fprintf(ff,"\x0F");

iceb_u_zagolov(gettext("Движение товарно-материалных ценностей"),dn,mn,gn,dk,mk,gk,organ,ff);

if(sklz[0] != 0)
  fprintf(ff,"%s:%s\n",gettext("Склад"),sklz);
if(korz[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Контрагент"),korz);
if(kodop[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop);
if(grz[0] != 0)
  fprintf(ff,"%s:%s\n",gettext("Группа"),grz);
if(kodmz[0] != 0)
  fprintf(ff,"%s:%s\n",gettext("Материал"),kodmz);
if(shetz[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Счёт"),shetz);
if(nomdokp[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Номер документа"),nomdokp);
if(ndsc[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("НДС"),ndsc );


memset(stroka,'\0',sizeof(stroka));
sprintf(stroka,"-----------------------------------"); //Строка на наименованием

for(i1=0; i1 < kolshet+1;i1++)
 {
  itogkol=0.;
  for(i=0; i < kolgrup;i++)
   {
    itogkol+=sumshgr[i*kolshet+i1];
   }
  if(itogkol == 0.)
    continue;
  strcat(stroka,"-----------");
 }
fprintf(ff,"%s\n",stroka);  

fprintf(ff,"Код|  Наименование группы         |");

for(i1=0; i1 < kolshet;i1++)
 {
  itogkol=0.;
  for(i=0; i < kolgrup;i++)
   {
    itogkol+=sumshgr[i*kolshet+i1];
   }
  if(itogkol == 0.)
    continue;
  strcpy(shet,SHET->ravno(i1));
  fprintf(ff,"%10s|",shet);
 }
fprintf(ff,"%*s|\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",stroka);  

for(i=0; i < kolgrup; i++)
 {
  itogkol=0.;
  for(i1=0; i1 < kolshet;i1++)
      itogkol+=sumshgr[i*kolshet+i1];
  if(itogkol == 0.)
    continue;

  nomgrup=GRUP->ravno(i);  
  memset(naimgrup,'\0',sizeof(naimgrup));
  sprintf(strsql,"select naik from Grup where kod=%d",nomgrup);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    strncpy(naimgrup,row[0],sizeof(naimgrup)-1);

  fprintf(ff,"%3d %-*.*s",
  nomgrup,
  iceb_u_kolbait(30,naimgrup),iceb_u_kolbait(30,naimgrup),naimgrup);    

  itogo=0.;
  for(i1=0; i1 < kolshet;i1++)
   {
    itogkol=0.;
    for(i2=0; i2 < kolgrup;i2++)
     {
      itogkol+=sumshgr[i2*kolshet+i1];
     }
    if(itogkol == 0.)
      continue;
    fprintf(ff," %10.2f",sumshgr[i*kolshet+i1]);
    itogo+=sumshgr[i*kolshet+i1];
   }

  fprintf(ff," %10.2f\n",itogo);
 }

fprintf(ff,"%s\n",stroka);  
fprintf(ff,"%*s:",iceb_u_kolbait(33,gettext("Итого")),gettext("Итого"));
itogo=0.;
for(i1=0; i1 < kolshet; i1++)
 {
  itogkol=0.;
  for(i=0; i < kolgrup;i++)
   {
    itogkol+=sumshgr[i*kolshet+i1];
   }
  if(itogkol == 0.)
    continue;
  fprintf(ff," %10.2f",itogkol);
  itogo+=itogkol;
 }

fprintf(ff," %10.2f\n",itogo);

iceb_podpis(ff,wpredok);
fprintf(ff,"\x12");  /*Нормальный режим печати*/
fclose(ff);

}
/***********************************************/
/*Распечатка группа-счет-счет корреспондент    */
/***********************************************/
void rasgshshk(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *shetz,
const char *kodmz,const char *grz,const char *kodop,const char *korz,
const char *sklz,const char *nomdokp,const char *ndsc,
const char *imaf,
iceb_u_spisok *GSHSHK,      //Список группа-счет-счет корреспондент
iceb_u_double *SUMGSHSHK, //Суммы к списку  группа-счет-счет корреспондент
GtkWidget *wpredok)
{
FILE	*ff;
time_t	tmm;
struct  tm      *bf;
char	strsql[512];
SQL_str	row;
int	kolstr=0;
int	kolelem=0;
int	i=0;
short   metkavgr=0;
char	kod[32];
char	shet[32];
char	shetk[32];
double	itoggr=0.;

time(&tmm);
bf=localtime(&tmm);

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }    
iceb_u_startfil(ff);

fprintf(ff,"\x1b\x6C%c",10); /*Установка левого поля*/

iceb_u_zagolov(gettext("Движение товарно-материалных ценностей по группам и счетам"),dn,mn,gn,dk,mk,gk,organ,ff);

if(sklz[0] != 0)
  fprintf(ff,"%s:%s\n",gettext("Склад"),sklz);
if(korz[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Контрагент"),korz);
if(kodop[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop);
if(grz[0] != 0)
  fprintf(ff,"%s:%s\n",gettext("Группа"),grz);
if(kodmz[0] != 0)
  fprintf(ff,"%s:%s\n",gettext("Материал"),kodmz);
if(shetz[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Счёт"),shetz);
if(nomdokp[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Номер документа"),nomdokp);
if(ndsc[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("НДС"),ndsc );

sprintf(strsql,"select kod,naik from Grup order by kod asc");

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

kolelem=GSHSHK->kolih();
while(cur.read_cursor(&row) != 0)
 {
  if(itoggr != 0)
   {
    fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),itoggr);
    itoggr=0.;
   }

  metkavgr=0;  
  for(i=0; i < kolelem; i++)
   {
    iceb_u_pole(GSHSHK->ravno(i),kod,1,'#');
    if(iceb_u_SRAV(kod,row[0],0) == 0)
     {
      if(metkavgr == 0)
       {
        fprintf(ff,"\n%s %s\n",row[0],row[1]);
        sapitsh_f2(ff); 
        metkavgr=1;
       }
      iceb_u_pole(GSHSHK->ravno(i),shet,2,'#');
      iceb_u_pole(GSHSHK->ravno(i),shetk,3,'#');
      if(shetk[0] == '\0')
       strcpy(shetk,"???");
      fprintf(ff,"%-*s %-*s %10.2f\n",
      iceb_u_kolbait(6,shet),shet,
      iceb_u_kolbait(6,shetk),shetk,
      SUMGSHSHK->ravno(i));
      itoggr+=SUMGSHSHK->ravno(i);
     }            

   }  

 }

if(itoggr != 0)
  fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),itoggr);


iceb_podpis(ff,wpredok);
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/
fclose(ff);

}

/******************************************/
/******************************************/

gint dvtmcf2_r1(class dvtmcf2_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);



int tipz=data->rk->pr;

short dn,mn,gn;
short dk,mk,gk;
iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);
SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr;

sprintf(strsql,"select * from Zkart where datdp >= '%04d-%02d-%02d' and \
datdp <= '%04d-%02d-%02d' order by sklad,nomk,datdp asc",
gn,mn,dn,gk,mk,dk);

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


sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("по"),
dk,mk,gk);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
sprintf(strsql,"Количество строк на листе %d\n",kol_strok_na_liste);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);


iceb_u_int GRUP; //Массив количества групп
iceb_u_spisok SHET;        //Список счетов
int	kolgrup=0;
int     kolshet=0;
iceb_u_spisok GSHSHK;      //Список группа-счет-счет корреспондент
iceb_u_double SUMGSHSHK(0); //Суммы к списку  группа-счет-счет корреспондент


//Создаем список счетов и массив групп
sozshgr(&GRUP,&SHET,&kolgrup,&kolshet,data->window);

double sumshgr[kolshet*kolgrup]; //Массив сумм группа-счет
memset(sumshgr,'\0',sizeof(sumshgr));

char imafsort[30];
sprintf(imafsort,"sort%d.tmp",getpid());
FILE *ff1;

if((ff1 = fopen(imafsort,"w")) == NULL)
 {
  iceb_er_op_fil(imafsort,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    
char imaf[40];

/*Открываем заранее, чтобы иметь возможность выгрузить ошибки*/
if(data->rk->metka_sort == 0)
  sprintf(imaf,"dv%d.lst",getpid());
if(data->rk->metka_sort == 1)
  sprintf(imaf,"dv1_%d.lst",getpid());
if(data->rk->metka_sort == 2)
  sprintf(imaf,"dv2_%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    
iceb_u_startfil(ff);

int kolstr2=0;
float kolstr1=0;
SQL_str row;
SQL_str row1;
int skl,nk;
char kor[40];
char kop[30];
int kodm;
char shu[30];
char ei[32];
double cena;
char naim[512];
int kgrm;
char shetsp[30];
int nomgrup,nomshet,nomel;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(LINES-1,0,kolstr,kolstr1);  
  skl=atoi(row[0]);
  nk=atoi(row[1]);

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
    continue;  

  if(tipz != 0)
   if(tipz != atoi(row[5]))
       continue;

  if(iceb_u_proverka(data->rk->nomdok.ravno(),row[2],0,0) != 0)
    continue;  

  
  /*Читаем накладную*/
  sprintf(strsql,"select kontr,kodop from Dokummat \
where datd='%s' and sklad=%d and nomd='%s'",
  row[4],skl,row[2]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден документ"));
    repl.plus(" !");
    repl.ps_plus(gettext("Номер документа"));
    repl.plus(":");
    repl.plus(row[2]);
    repl.plus(" ");
    repl.plus(gettext("Дата"));
    repl.plus(":");
    repl.plus(row[4]);
    repl.plus(" ");
    repl.plus(gettext("Склад"));
    repl.plus(":");
    repl.plus(skl);
    iceb_menu_soob(&repl,data->window);    

    fprintf(ff,"%s !\n",gettext("Не найден документ"));

    fprintf(ff,"%s %s %s=%s %s=%d\n",
    gettext("Дата"),
    row[4],
    gettext("Номер документа"),
    row[2],
    gettext("Склад"),
    skl);
    continue;
   }

  strncpy(kor,row1[0],sizeof(kor)-1);

  strncpy(kop,row1[1],sizeof(kop)-1);

  if(iceb_u_proverka(data->rk->kontr.ravno(),kor,0,0) != 0)
    continue;  

  if(iceb_u_proverka(data->rk->kodop.ravno(),kop,0,0) != 0)
    continue;  

  /*Читаем карточку материалла*/
  sprintf(strsql,"select kodm,shetu,ei,cena,mnds,nds from Kart where sklad=%d and nomk=%d",
  skl,nk);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s:%d  %s:%d\n",
    gettext("Не нашли карточку"),
    nk,
    gettext("Склад"),
    skl);

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,"%s:%d  %s:%d\n",
    gettext("Не нашли карточку"),
    nk,
    gettext("Склад"),
    skl);
    continue;
   }

//  if(provndsw(data->rk->nds.ravno(),row1) != 0)
//    continue;

  double nds=atof(row1[5]);

  if(data->rk->nds.ravno()[0] != '\0')
   {
    //Карточки с нулевым НДС
    if(data->rk->nds.ravno()[0] == '0' && atof(row1[5]) != 0.)
        continue;
    //Карточки с НДС нужного значения
    if(data->rk->nds.ravno()[0] != '+' && data->rk->nds.ravno()[0] != '0' && (fabs(nds - atof(data->rk->nds.ravno())) > 0.009 || atoi(row1[4]) != 0))
        continue;
    //Карточки в цене которых включено НДС нужного значения
    if(data->rk->nds.ravno()[0] == '+' && data->rk->nds.ravno()[1] != '\0' && (fabs(nds - atof(data->rk->nds.ravno())) > 0.009 || atoi(row1[4]) != 1) )
        continue;
    //Карточки в цене которых НДС любого значения
    if(data->rk->nds.ravno()[0] == '+' && data->rk->nds.ravno()[1] == '\0' &&  atoi(row1[4]) != 1 )
        continue;
   }

  kodm=atoi(row1[0]);
  strncpy(shu,row1[1],sizeof(shu)-1);
  strncpy(ei,row1[2],sizeof(ei)-1);
  cena=atof(row1[3]);

/**************************
  if(kodmz[0] != '\0')
   {
    sprintf(bros,"%d",kodm); 
    if(kodmz[0] == '*' && maskmat != NULL)
     {
      if(iceb_u_proverka(maskmat,bros,0,0) != 0)
        continue;  
     }
    else
*************************/
      if(iceb_u_proverka(data->rk->kodmat.ravno(),row1[0],0,0) != 0)
        continue;  
//   }

  if(iceb_u_proverka(data->rk->shet.ravno(),shu,0,0) != 0)
    continue;  

  /*Узнаем наименование материалла*/
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%d",
  kodm);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    fprintf(ff,"%s %d !\n",gettext("Не найден код материалла"),kodm);
    sprintf(strsql,"%s %d !\n",gettext("Не найден код материалла"),kodm);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    continue;
   }

  strncpy(naim,row1[0],sizeof(naim));
  kgrm=atoi(row1[1]);
  
  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[1],0,0) != 0)
    continue;  

  memset(shetsp,'\0',sizeof(shetsp));

  //Читаем дополнение к наименованию материалла
  sprintf(strsql,"select shet,dnaim from Dokummat1 where datd='%s' and sklad=%s \
and nomd='%s' and kodm=%d and nomkar=%s",
  row[4],row[0],row[2],kodm,row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    strcpy(shetsp,row1[0]);
    strcat(naim," ");
    strcat(naim,row1[1]);
   }  
  else
   {
    sprintf(strsql,"Не найдена запись !!!\n");
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
   }

  kolstr2++;
  fprintf(ff1,"%s|%d|%s|%d|%s|%s|%d|%s|%s|%.10g|%s|%s|%s|%d|%s|%s|\n",
  shu,skl,kor,kgrm,row[3],naim,kodm,ei,kop,cena,row[4],row[6],row[5],
  nk,row[2],shetsp);

  nomgrup=GRUP.find(kgrm);
  nomshet=SHET.find(shu);
  sumshgr[nomgrup*kolshet+nomshet]+=cena*atof(row[6]);
  
  sprintf(strsql,"%d#%s#%s",kgrm,shu,shetsp);
  if((nomel=GSHSHK.find(strsql)) < 0) 
    GSHSHK.plus(strsql);
  SUMGSHSHK.plus(cena*atof(row[6]),nomel);

 }


fclose(ff1);

/*****************************************/
if(data->rk->metka_sort == 0)
 {
  sprintf(strsql,"\n%s.\n",gettext("Сортировка:Счет-склад-контрагент-группа-дата подтверждения"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  /*Сортировка Счет-склад-контрагент-группа-дата подтверждения*/ 
//  sprintf(strsql,"sort -o %s -t\\| +0 -1 +1n -2 +2 -3 +3n -4 %s",imafsort,imafsort);
  sprintf(strsql,"sort -o %s -t\\| -k1,2 -k2,3n -k3,4 -k4,5n %s",imafsort,imafsort);
  system(strsql);

  dvtmcf2ot(imafsort,dn,mn,gn,dk,mk,gk,kolstr2,tipz,ff,data->rk->metka_sort,data);

 }
/*****************************************/
if(data->rk->metka_sort == 1)
 {

  sprintf(strsql,"\n%s.\n",gettext("Сортировка:Cклад-контрагент-дата подтверждения"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  /*Сортировка склад-контрагент-дата подтверждения*/ 
//  sprintf(strsql,"sort -o %s -t\\|  +1n -2 +2 -3 +4 -5 %s",imafsort,imafsort);
  sprintf(strsql,"sort -o %s -t\\|  -k2,3n -k3,4 -k4,5 %s",imafsort,imafsort);
  system(strsql);


  dvtmcf2ot(imafsort,dn,mn,gn,dk,mk,gk,kolstr2,tipz,ff,data->rk->metka_sort,data);

 }
/*****************************************/
if(data->rk->metka_sort == 2)
 {

  sprintf(strsql,"\n%s.\n",gettext("Сортировка:Счет-cклад-группа"));
  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

//  sprintf(strsql,"sort -o %s -t\\| +0 -1 +1n -2 +3n -4 %s",imafsort,imafsort);
  sprintf(strsql,"sort -o %s -t\\| -k1,2 -k2,3n -k4,5n %s",imafsort,imafsort);
  system(strsql);



  dvtmcf2ot(imafsort,dn,mn,gn,dk,mk,gk,kolstr2,tipz,ff,data->rk->metka_sort,data);

 }
iceb_podpis(ff,data->window);
fclose(ff);

unlink(imafsort);

char	imafshgr[56];

sprintf(imafshgr,"grsh%d.lst",getpid());

rasgrsh(imafshgr,dn,mn,gn,dk,mk,gk,&GRUP,&SHET,kolgrup,kolshet,sumshgr,data->rk->shet.ravno(),
data->rk->kodmat.ravno(),data->rk->grupa.ravno(),data->rk->kodop.ravno(),data->rk->kontr.ravno(),
data->rk->sklad.ravno(),data->rk->nomdok.ravno(),data->rk->nds.ravno(),
data->window);

char	imafgshshk[56];
sprintf(imafgshshk,"grshk%d.lst",getpid());

rasgshshk(dn,mn,gn,dk,mk,gk,data->rk->shet.ravno(),data->rk->kodmat.ravno(),
data->rk->grupa.ravno(),data->rk->kodop.ravno(),data->rk->kontr.ravno(),
data->rk->sklad.ravno(),data->rk->nomdok.ravno(),data->rk->nds.ravno(),
imafgshshk,&GSHSHK,&SUMGSHSHK,data->window);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Движение товарно-материальных ценностей"));

data->rk->imaf.plus(imafshgr);
data->rk->naimf.plus(gettext("Движение товарно-материальных ценностей по группам и счетам"));

data->rk->imaf.plus(imafgshshk);
data->rk->naimf.plus(gettext("Движение товарно-материальных ценностей по группам и счетам"));
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

/*********************************************/
/*Создание массивов счетов и групп материалов*/
/*********************************************/
void	sozshgr(iceb_u_int *GRUP, //Массив количества групп
iceb_u_spisok *SHET,        //Список счетов
int *kolgrup, //Количество групп
int *kolshet, //Количество счетов
GtkWidget *wpredok)
{
SQL_str		row;
char		strsql[512];


sprintf(strsql,"select kod from Grup");

SQLCURSOR cur;
if((*kolgrup=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);


while(cur.read_cursor(&row) != 0)
 {
  GRUP->plus(atoi(row[0]),-1);
 }

sprintf(strsql,"select distinct shetu from Kart");

if((*kolshet=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);


while(cur.read_cursor(&row) != 0)
 {
  SHET->plus(row[0]);
 }

}
/******************************/
/* Шапка итога по счетам      */
 /******************************/

void  sapitsh_f2(FILE *ff)
{
fprintf(ff,"\
------------------------------\n");
fprintf(ff,gettext("\
Дебет |Кредит|    Сумма      |\n"));
fprintf(ff,"\
------------------------------\n");

}
/*******************/
/*Распечатка отчета*/
/*******************/

void  dvtmcf2ot(char imafsort[],short dn,short mn,short gn,
short dk,short mk,short gk,int kolstr2,short tipz,FILE *ff,
short metoth,class dvtmcf2_r_data *data)
{
struct  tm      *bf;
time_t		tmm;
char		bros[512];
int		kodm;
short           dp,mp,gp;
short           d,m,g;
double		its,itk,br;
char		nomdok[32];
char		poi[32];
char		str[512];
int		i;
double		itgrup[2];
double		itkontr[2];
double		itsklad[2];
double		itshet[2];
short           mvstr; /*Метка выдачи строки*/
int             sli=0,kst=0;
char		strsql[512];
double		kolih,cena;
char		shu[32],shuz[32];
short		kgrm,kgrmz;
int             skl,skll,nk,nkk;
char		kop[32];
char		kor[32],korzz[32];
char		naim[512];
char            nomn[32];
FILE		*ff1;
SQL_str         row1;
SQLCURSOR       cur1;
char            org[512];
char		ei[32];
float		kolstr1;
char		shetsp[32];

its=itk=0;

time(&tmm);
bf=localtime(&tmm);

if((ff1 = fopen(imafsort,"r")) == NULL)
 {
  iceb_er_op_fil(imafsort,"",errno,data->window);
  return;
 }    



fprintf(ff,"%s\n\n\
		      %s %s %d.%d.%d%s %s %d.%d.%d%s\n\
				     %s %d.%d.%d  %s:%02d:%02d\n",
organ,
gettext("Движение товарно-материальных ценностей"),
gettext("За период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);
kst=4;

if(tipz == 0)
 {
  fprintf(ff,"\
				     %s\n",
gettext("Перечень приходов/расходов"));
  kst++;
 }

if(tipz == 1)
 {
  fprintf(ff,"\
				     %s\n",
gettext("Перечень приходов"));
  kst++;
 }
if(tipz == 2)
 {
  fprintf(ff,"\
				     %s\n",
gettext("Перечень расходов"));
  kst++;
 }

if(data->rk->sklad.ravno()[0] != 0)
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  kst++;
 }
if(data->rk->kontr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Контрагент"),data->rk->kontr.ravno());
  kst++;
 }
if(data->rk->kodop.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  kst++;
 }
if(data->rk->grupa.ravno()[0] != 0)
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),data->rk->grupa.ravno());
  kst++;
 }
if(data->rk->kodmat.ravno()[0] != 0)
 {
  fprintf(ff,"%s:%s\n",gettext("Материал"),data->rk->kodmat.ravno());
  kst++;
 }
if(data->rk->shet.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  kst++;
 }
if(data->rk->nomdok.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Номер документа"),data->rk->nomdok.ravno());
  kst++;
 }
if(data->rk->nds.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  kst++;
 }

for(i=0; i < 2 ; i++)
 itgrup[i]=itkontr[i]=itsklad[i]=itshet[i]=0.;

dvtmcf2_r_sapka(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
  
memset(shuz,'\0',sizeof(shuz));
kgrmz=skl=0;
nkk=mvstr=skll=0;
kolstr1=0;

while(fgets(str,sizeof(str),ff1) != NULL)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    
  iceb_u_polen(str,shu,sizeof(shu),1,'|');
  iceb_u_polen(str,strsql,sizeof(strsql),2,'|');
  skl=atoi(strsql);
  iceb_u_polen(str,kor,sizeof(kor),3,'|');
  iceb_u_polen(str,strsql,sizeof(strsql),4,'|');
  kgrm=atoi(strsql);
  iceb_u_polen(str,strsql,sizeof(strsql),14,'|');
  nk=atoi(strsql);

  if(kgrmz != kgrm && (metoth == 0 || metoth == 2))
   {
    if(kgrmz != 0)
     {
      sprintf(bros,"%d",kgrmz);
      rasitogo(1,itgrup,&kst,bros,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
     }
    kgrmz=kgrm;
   }

  if(iceb_u_SRAV(korzz,kor,0) != 0)
   {
    if(korzz[0] != '\0')
     {
      rasitogo(2,itkontr,&kst,korzz,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
     }
    strcpy(korzz,kor);
   }     
  if(skll != skl)
   {
    if(skll != 0)
     {
      sprintf(bros,"%d",kgrmz);
      rasitogo(1,itgrup,&kst,bros,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
      rasitogo(2,itkontr,&kst,korzz,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
      sprintf(bros,"%d",skll);
      rasitogo(3,itsklad,&kst,bros,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
            
     } 
    nkk=0;
    skll=skl;
   } 
  if(iceb_u_SRAV(shuz,shu,0) != 0 && (metoth == 0 || metoth == 2))
   {
    if(shuz[0] != '\0')
     {
      sprintf(bros,"%d",kgrmz);
      rasitogo(1,itgrup,&kst,bros,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
      rasitogo(2,itkontr,&kst,korzz,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
      sprintf(bros,"%d",skll);
      rasitogo(3,itsklad,&kst,bros,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
      rasitogo(4,itshet,&kst,shuz,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
     }
    strcpy(shuz,shu);
   }
  iceb_u_polen(str,naim,sizeof(naim),6,'|');
  iceb_u_polen(str,kop,sizeof(kop),9,'|');

  iceb_u_polen(str,strsql,sizeof(strsql),7,'|');
  kodm=atoi(strsql);
  iceb_u_polen(str,shu,sizeof(shu),1,'|');
  iceb_u_polen(str,ei,sizeof(ei),8,'|');
  iceb_u_polen(str,strsql,sizeof(strsql),10,'|');
  cena=atof(strsql);


       
  mvstr=0;
  
  iceb_u_polen(str,strsql,sizeof(strsql),5,'|');
  iceb_u_rsdat(&dp,&mp,&gp,strsql,2);
  iceb_u_polen(str,strsql,sizeof(strsql),11,'|');
  iceb_u_rsdat(&d,&m,&g,strsql,2);
  iceb_u_polen(str,strsql,sizeof(strsql),12,'|');
  kolih=atof(strsql);

  iceb_u_polen(str,shetsp,sizeof(strsql),16,'|');
  
  itgrup[0]+=kolih;  
  itkontr[0]+=kolih;
  itsklad[0]+=kolih;
  itshet[0]+=kolih;
  itk+=kolih;

  br=cena*kolih;
  br=iceb_u_okrug(br,0.01);
  itgrup[1]+=br;  
  itkontr[1]+=br;
  itsklad[1]+=br;
  itshet[1]+=br;
  its+=br;
  
  /*Узнаем наименование контрагента*/
  if(kor[0] == '0' && iceb_u_pole(kor,bros,2,'-') == 0)
   {
    strcpy(poi,bros);   
    sprintf(strsql,"select naik from Sklad where kod=%s",poi);
   }
  else
   {
    strcpy(poi,kor);   
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
    poi);
   }

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    fprintf(ff,"%s %s !\n",gettext("Не найден код контрагента"),kor);

    sprintf(strsql,"%s %s %s !\n",gettext("Не найден код контрагента"),kor,poi);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    sprintf(strsql,"%s:%d %s:%d %s:%s\n",
    gettext("Склад"),
    skl,
    gettext("Номер карточки"),
    nk,
    gettext("Документ"),
    nomdok);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(ff,gettext("Склад %d Ном. кар. %d Док.%s\n"),
    skl,nk,nomdok);
   }
  else
    strncpy(org,row1[0],sizeof(org)-1);

  gsapp1(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

  iceb_u_polen(str,strsql,sizeof(strsql),13,'|');
  if(atoi(strsql) == 1)
     strcat(kop,"+");
  if(atoi(strsql) == 2)
     strcat(kop,"-");
  iceb_u_polen(str,nomdok,sizeof(nomdok),15,'|');
 
//  printw("%d %s\n",nk,naim);
  if(mvstr == 0)
   {
    sprintf(nomn,"%d.%s.%d.%d",skl,shu,kgrm,
    nk);
     
    fprintf(ff,"\
%-4d %-*.*s %-*s %-*s %14.10g %12.12g %15.2f %-15s %02d.%02d.%d %02d.%02d.%d %-7s %3s %s %s\n",
    kodm,
    iceb_u_kolbait(35,naim),iceb_u_kolbait(35,naim),naim,
    iceb_u_kolbait(15,nomn),nomn,
    iceb_u_kolbait(5,ei),ei,
    cena,
    kolih,br,nomdok,d,m,g,dp,mp,gp,kop,kor,org,shetsp);

    if(iceb_u_strlen(naim) > 35)
     {
      gsapp1(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
      fprintf(ff,"%4s %s\n",
      " ",
      iceb_u_adrsimv(35,naim));
     }
   }
  else
   {
    fprintf(ff,"\
%4s %-35s %-15s %-5s %14.10g %12.12g %15.2f %-15s %02d.%02d.%d %02d.%02d.%d %-7s %3s %s %s\n",
    " "," "," "," ",cena,
    kolih,br,nomdok,d,m,g,dp,mp,gp,kop,kor,org,shetsp);
   }  
  
  mvstr++;

 }

fclose(ff1);

sprintf(bros,"%d",kgrmz);
rasitogo(1,itgrup,&kst,bros,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
rasitogo(2,itkontr,&kst,korzz,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
sprintf(bros,"%d",skll);
rasitogo(3,itsklad,&kst,bros,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);
rasitogo(4,itshet,&kst,shuz,ff,metoth,data,dn,mn,gn,dk,mk,gk,&sli);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%*s:%12.12g %15.2f\n",iceb_u_kolbait(77,gettext("Общий итог")),gettext("Общий итог"),itk,its);

sprintf(strsql,"%s:\n\
%-*s %12.12g\n\
%-*s %12.2f%s\n",
gettext("Итого"),
iceb_u_kolbait(10,gettext("количество")),gettext("количество"),
itk,
iceb_u_kolbait(10,gettext("сумма")),gettext("сумма"),
its,
gettext("руб.")); 

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);



}
/*******************/
/*Распечатка итогов*/
/********************/

void		rasitogo(int metka,//  1-итого по группе
                                   //  2-итого по контр
                                   //  3-итого по склад
                                   //  4-итого по счету
double mas[2],int *klst,char kod[],
FILE *ff,short metoth,
class dvtmcf2_r_data *data,
short dn,short mn,short gn,short dk,short mk,short gk,
int *sli) //Счетчик листов
{
char		strsql[512];
char		naim[512];
SQL_str         row;
SQLCURSOR       cur;
char		bros[512];

if(mas[0] == 0.)
  return;

if(metoth == 1 && (metka == 1 || metka == 4))
  return;
if(metoth == 2 && metka == 2)
  return;

memset(naim,'\0',sizeof(naim));

if(metka == 1)
 {
  sprintf(strsql,"select naik from Grup where kod=%s",kod);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !\n",gettext("Не найдена группа"),kod);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
   }
  else
   strncpy(naim,row[0],sizeof(naim)-1);
  sprintf(strsql,"%s %s %s",gettext("Итого по группе"),kod,naim);
 }

if(metka == 2)
 {
  if(kod[0] == '0' && iceb_u_pole(kod,bros,2,'-') == 0)
   {
    sprintf(strsql,"select naik from Sklad where kod=%s",bros);
   }
  else
   {
    strcpy(bros,kod);   
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
    bros);
   }

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !\n",gettext("Не найден контрагент"),bros);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
   }
  else
   strncpy(naim,row[0],sizeof(naim)-1);
  sprintf(strsql,"%s %s %s",gettext("Итого по контрагенту"),kod,naim);
 }

if(metka == 3)
 {
  sprintf(strsql,"select naik from Sklad where kod=%s",kod);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !\n",gettext("Не найден склад"),kod);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
   }
  else
   strncpy(naim,row[0],sizeof(naim)-1);
  sprintf(strsql,"%s %s %s",gettext("Итого по складу"),kod,naim);
 }

if(metka == 4)
 {
  sprintf(strsql,"select nais from Plansh where ns='%s'",kod);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),kod);

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    iceb_printw("\n",data->buffer,data->view);
   }
  else
   strncpy(naim,row[0],sizeof(naim)-1);
  sprintf(strsql,"%s %s %s",gettext("Итого по счету"),kod,naim);
 }

gsapp1(dn,mn,gn,dk,mk,gk,sli,klst,ff);
fprintf(ff,"%*s:%12.12g %15.2f\n",iceb_u_kolbait(77,strsql),strsql,mas[0],mas[1]);

mas[0]=0.;
mas[1]=0.;

}                        
/*******/
/*Шапка*/
/*******/
void gsapp1(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl, //Счетчик листов
int *kst, //Счетчик строк
FILE *ff)
{
*kst+=1;
if(*kst <= kol_strok_na_liste_l)
 return;

fprintf(ff,"\f");
*kst=1;

dvtmcf2_r_sapka(dn,mn,gn,dk,mk,gk,sl,kst,ff);

}

/*************************/
/*Шапка*/
/************************/
void dvtmcf2_r_sapka(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl, //Счетчик листов
int *kst, //Счетчик строк
FILE *ff)
{
*sl+=1;
fprintf(ff,"\
%s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %100s%s N%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
"",
gettext("Лист"),
*sl);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("Код |        Наименование               |   Номенкла-   |Един.|              |            |               |               | Д а т а  | Д а т а  | Код   |\n"));
fprintf(ff,gettext("    |    товара или материалла          | турный номер  |изме-|   Ц е н а    | Количество |     Сумма     |Номер документа| выписки  |подтверж- |опера- |         К о н т р а г е н т\n"));
fprintf(ff,gettext("    |                                   |               |рения|              |            |               |               |документа | дения    | ции   |\n"));

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
*kst+=6;
}
