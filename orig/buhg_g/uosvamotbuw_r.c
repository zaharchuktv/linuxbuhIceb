/*$Id: uosvamotbuw_r.c,v 1.12 2011-07-21 12:43:27 sasa Exp $*/
/*12.07.2011	01.01.2008	Белых А.И.	uosvamotbuw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "uosvamotbuw.h"

class uosvamotbuw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class uosvamotbuw_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  uosvamotbuw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosvamotbuw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvamotbuw_r_data *data);
gint uosvamotbuw_r1(class uosvamotbuw_r_data *data);
void  uosvamotbuw_r_v_knopka(GtkWidget *widget,class uosvamotbuw_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;
extern float    nemi; /*Необлагаемый минимум*/
extern short    metkabo; //Если равна 1-бюджетная организация
extern class iceb_u_str spis_print_a3; /*Список принтеров формата А4*/

int uosvamotbuw_r(class uosvamotbuw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class uosvamotbuw_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosvamotbuw_r_key_press),&data);

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

repl.plus(gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uosvamotbuw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uosvamotbuw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosvamotbuw_r_v_knopka(GtkWidget *widget,class uosvamotbuw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosvamotbuw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvamotbuw_r_data *data)
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

/*************************************************/
/*Шапка*/
/*************************************************/

void	uosshapbu(short mn,short gn,short mk,short gk,struct tm *bf,
int *sl,int mr,FILE *ffi,int *kst)
{
*sl+=1;
*kst+=5;

fprintf(ffi,"\
%s %d.%d.%d %s %s - %d:%d %100s%s N%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,
"",gettext("Лист"),
*sl);

fprintf(ffi,"\
------------------------------------------------------------------------");
if(mr == 1)
  fprintf(ffi,"\
--------------------------------------------------------------------------------");

strpod(mn,mk,11,ffi);

fprintf(ffi,gettext("\
Инв-рный|        Н а и м е н о в а н и е         |Год | Номерной |Шифр |"));

if(mr == 1)
  fprintf(ffi,gettext("\
Норма|Балансовая|  Износ   |Остаточная|Ликвидаци.|Годов. сумма|Мес. сумма|Коэфи|"));

strms(0,mn,mk,ffi);
fprintf(ffi,gettext("\
 номер  |             объекта                    |    |   знак   |н.ам.|"));
if(mr == 1)
  fprintf(ffi,gettext("\
а.отч|стоимость |          |бал. с-сть|стоимость |амортотчисл.|амортотчи.|циент|"));

strms(1,mn,mk,ffi);

fprintf(ffi,"\
------------------------------------------------------------------------");
if(mr == 1)
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
strpod(mn,mk,11,ffi);
}
/*****************/
/*Шапка документа*/
/*****************/
void shvambu(short mn,short gn,short mk,short gk,int *sl,
int *kst,int orient,short mr,FILE *ff,struct tm *bf)
{

*kst+=1;
if(orient == 0)
 {
  if(*kst <= kol_strok_na_liste)
   return;
 }
else
  if(*kst <= kol_strok_na_liste_l)
   return;
  
fprintf(ff,"\f");

*kst=1;

uosshapbu(mn,gn,mk,gk,bf,sl,mr,ff,kst);

}
/*******************************/
/*Выдача итога по подразделению*/
/*******************************/
void itambu(short mn,short gn,short mk,short gk,double itg[],char nai[],int orient,short mr,
double maotp[12],FILE *ff,FILE *ffi,int *sli,int *kst,
struct tm *bf)

{
short           i;
double          vs;
char		bros[512];

shvambu(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"\
------------------------------------------------------------------------");
fprintf(ffi,"\
------------------------------------------------------------------------");
if(mr == 1)
 {
  fprintf(ff,"\
--------------------------------------------------------------------------------");
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
 }

strpod(mn,mk,11,ff);
strpod(mn,mk,11,ffi);

memset(bros,'\0',sizeof(bros));
sprintf(bros,"%s %s",gettext("Итого по подразделению"),nai);

shvambu(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"%-*.*s|",iceb_u_kolbait(71,bros),iceb_u_kolbait(71,bros),bros);

fprintf(ffi,"%-*.*s|",iceb_u_kolbait(71,bros),iceb_u_kolbait(71,bros),bros);

if(mr == 1)
 {
  fprintf(ff,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
  fprintf(ffi,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
 }
vs=0.;
for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maotp[i];
  fprintf(ff,"%10.2f|",maotp[i]);
  fprintf(ffi,"%10.2f|",maotp[i]);
 }
if(mn != mk)
 {
  fprintf(ff,"%10.2f|",vs);
  fprintf(ffi,"%10.2f|",vs);
 }

fprintf(ff,"\n");
fprintf(ffi,"\n");

}
/*******************************/
/*Выдача итога по шифру нормы амортотчислений или группе*/
/*******************************/
void itamhbu(short mn,short gn,short mk,short gk,double itg[],
char *kod,
char *nai,
int orient,
short mr,
double maotph[12],FILE *ff,FILE *ffi,
int metkaitog,  //0-по группе 1-по шифру
int *sli,int *kst,
struct tm *bf)
{
short           i;
double          vs;
char		bros[512];

shvambu(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"\
------------------------------------------------------------------------");
fprintf(ffi,"\
------------------------------------------------------------------------");

if(mr == 1)
 {
  fprintf(ff,"\
--------------------------------------------------------------------------------");
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
 }
strpod(mn,mk,11,ff);
strpod(mn,mk,11,ffi);
if(metkaitog == 0)
  sprintf(bros,"%s %s %s",gettext("Итого по группе"),kod,nai);
if(metkaitog == 1)
  sprintf(bros,"%s %s %s",gettext("Итого по шифру"),kod,nai);

shvambu(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"%-*.*s|",iceb_u_kolbait(71,bros),iceb_u_kolbait(71,bros),bros);
fprintf(ffi,"%-*.*s|",iceb_u_kolbait(71,bros),iceb_u_kolbait(71,bros),bros);

if(mr == 1)
 {
  fprintf(ff,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
  fprintf(ffi,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
 }
vs=0.;
for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maotph[i];
  fprintf(ff,"%10.2f|",maotph[i]);
  fprintf(ffi,"%10.2f|",maotph[i]);
 }

if(mn != mk)
 {
  fprintf(ff,"%10.2f|",vs);
  fprintf(ffi,"%10.2f|",vs);
 }
fprintf(ff,"\n");
fprintf(ffi,"\n");

}
/*******************************/
/*Выдача итога по организации*/
/*******************************/
void            itamobu(short mn,short mk,double itg[],int mr,
double maotpo[12],FILE *ff,FILE *ffi)
{
short           i;
double          vs;
char		bros[512];

fprintf(ff,"\
------------------------------------------------------------------------");
fprintf(ffi,"\
------------------------------------------------------------------------");
if(mr == 1)
 {
  fprintf(ff,"\
--------------------------------------------------------------------------------");
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
 }
strpod(mn,mk,11,ff);
strpod(mn,mk,11,ffi);

sprintf(bros,gettext("Общий итог"));

fprintf(ff,"%-*.*s|",
iceb_u_kolbait(71,bros),iceb_u_kolbait(71,bros),bros);

fprintf(ffi,"%-*.*s|",
iceb_u_kolbait(71,bros),iceb_u_kolbait(71,bros),bros);

if(mr == 1)
 {
  fprintf(ff,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
  fprintf(ffi,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
 }
vs=0.;
for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maotpo[i];
  fprintf(ff,"%10.2f|",maotpo[i]);
  fprintf(ffi,"%10.2f|",maotpo[i]);
 }

if(mn != mk)
 {
  fprintf(ff,"%10.2f|",vs);
  fprintf(ffi,"%10.2f|",vs);
 }
fprintf(ff,"\n");
fprintf(ffi,"\n");

}


/******************************************/
/******************************************/

gint uosvamotbuw_r1(class uosvamotbuw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;

short mn,gn;
short mk,gk;

mn=data->rk->mesn.ravno_atoi();
mk=data->rk->mesk.ravno_atoi();
gn=gk=data->rk->god.ravno_atoi();
if(mk == 0)
 mk=mn;




short mr=0;
int kol_mes_v_per=iceb_u_period(1,mn,gn,1,mk,gk,1);

if(kol_mes_v_per <=3)
 mr=1;
else
 mr=0;

int max_dlin_str=0;
 
if(mr == 1)
 max_dlin_str=141+(kol_mes_v_per+1)*11;
else
 max_dlin_str=72+(kol_mes_v_per+1)*11;

if(kol_mes_v_per == 1)
  max_dlin_str=141+11;

short d=1;
short mp=mn,gp=gn;
iceb_u_dpm(&d,&mp,&gp,2);

sprintf(strsql,"%s %d.%d%s %s %d.%d%s (%d.%d)\n",
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."),
mp,gp);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int kolstr=0;
sprintf(strsql,"select innom,mes,god,podr,hzt,hna,suma from Uosamor1 \
where god = %d and mes >= %d and mes <= %d order by \
hzt,podr,innom,god,mes asc",
gn,mn,mk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf[64];
sprintf(imaf,"vaoby%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int orient=1;
if(iceb_u_proverka(spis_print_a3.ravno(),getenv("PRINTER"),0,1) == 0)
  orient=0; /*Если принтер А4 то ориентация всегда портрет*/



iceb_u_zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,31,mk,gk,organ,ff);


fprintf(ff,"%s\n",gettext("Бухгалтерский учет"));
int kst=6;
if(data->rk->grupa.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Група"),data->rk->grupa.ravno());
  kst++;
 }

if(data->rk->podr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),data->rk->podr.ravno());
  kst++;
 }
if(data->rk->hzt.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),data->rk->hzt.ravno());
  kst++;
 }
if(data->rk->innom.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),data->rk->innom.ravno());
  kst++;
 }
if(data->rk->mat_ot.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Мат.ответственный"),data->rk->mat_ot.ravno());
  kst++;
 }
if(data->rk->sost_ob != 0)
 {
  fprintf(ff,"%s: %d\n",gettext("Состояние объекта"),data->rk->sost_ob);
  kst++;
 }
if(data->rk->shetu.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счёт"),data->rk->shetu.ravno());
  kst++;
 }

char imafi[64];
sprintf(imafi,"vaobyi%d.lst",getpid());
FILE *ffi;
if((ffi = fopen(imafi,"w")) == NULL)
 {
  iceb_er_op_fil(imafi,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,31,mk,gk,organ,ffi);


fprintf(ffi,"%s\n",gettext("Бухгалтерский учет"));
int ksti=6;

if(data->rk->grupa.ravno()[0] != '\0')
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Група"),data->rk->grupa.ravno());
 }
if(data->rk->podr.ravno()[0] != '\0')
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Подразделение"),data->rk->podr.ravno());
 }
if(data->rk->hzt.ravno()[0] != '\0')
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Шифр затрат"),data->rk->hzt.ravno());
 }
if(data->rk->innom.ravno()[0] != '\0')
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Инвентарный номер"),data->rk->innom.ravno());
 }
if(data->rk->shetu.ravno()[0] != '\0')
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Счёт"),data->rk->shetu.ravno());
 }
if(data->rk->sost_ob != 0)
 {
  ksti++;
  fprintf(ffi,"%s: %d\n",gettext("Состояние объекта"),data->rk->sost_ob);
 }

struct  tm      *bf;
bf=localtime(&vremn);
int slii=0;
uosshapbu(mn,gn,mk,gk,bf,&slii,mr,ffi,&ksti);
char hzt2[64];
memset(hzt2,'\0',sizeof(hzt2));
int in=0;
double          maot[12];
double          maotp[12];
double          maotph[12];
double          maotpo[12];
const int RAZMER_MAS=6;
double          itg[RAZMER_MAS];
double          itg1[RAZMER_MAS];
double          itg2[RAZMER_MAS];

for(int i=0; i< 12; i++)
    maot[i]=maotp[i]=maotph[i]=maotpo[i]=0.;
for(int i=0;i< RAZMER_MAS;i++)
  itg[i]=itg1[i]=itg2[i]=0.;
char naipod[512];
char naimhz[512];
naipod[0]=naimhz[0]='\0';

strncpy(naipod,organ,sizeof(naipod)-1);
char nomz[64];
memset(nomz,'\0',sizeof(nomz));
int pod1=-100;
float kolstr1=0.;
double          ostt=0;
int in1=0,mvr=0;
int sli=0;
short m,g;
char hzt1[64];
char hna[64];
double sao=0.;
double bsby=0.,izby=0.;
double mes=0,god=0;
char invn[20];
char naios[512];
char gvve[8]; //Год ввода в эксплуатацию
char hnaz1[64];
SQL_str row1;
class SQLCURSOR curr;
char bros[512];
int vrabu=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->hzt.ravno(),row[4],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->podr.ravno(),row[3],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->grupa.ravno(),row[5],0,0) != 0)
    continue;

  in=atol(row[0]);
  m=atoi(row[1]);
  g=atoi(row[2]);
  int pod=atoi(row[3]);
  strncpy(hzt1,row[4],sizeof(hzt1)-1);
  strncpy(hna,row[5],sizeof(hna)-1);
  sao=atof(row[6]);

  if(iceb_u_SRAV(hzt2,hzt1,0) != 0)
   {

    if(hzt2[0] != '\0')
     {
      rasmes(mn,mk,maot,ostt,hnaz1,ff);
      mvr=0;

      itambu(mn,gn,mk,gk,itg1,naipod,orient,mr,maotp,ff,ffi,&sli,&kst,bf);
      itamhbu(mn,gn,mk,gk,itg2,hzt2,naimhz,orient,mr,maotph,ff,ffi,1,&sli,&kst,bf);
      for(int i=0;i< RAZMER_MAS;i++)
	itg1[i]=itg2[i]=0.;
      fprintf(ff,"\f");
      kst=0;
      pod1=0;
     }

    /*Читаем шифр производственных затрат*/
    memset(naimhz,'\0',sizeof(naimhz));
    sprintf(strsql,"select naik from Uoshz where kod='%s'",hzt1);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден шифр производственных затрат"),hzt1);
      iceb_menu_soob(strsql,data->window);
     }
    else
     {
      strncpy(naimhz,row1[0],sizeof(naimhz)-1);
     }          

    sprintf(strsql,"%s: %s %s\n",gettext("Шифр производственных затрат"),hzt1,naimhz);

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);

    fprintf(ff,"%s: %s %s\n",gettext("Шифр производственных затрат"),hzt1,naimhz);
    uosshapbu(mn,gn,mk,gk,bf,&sli,mr,ff,&kst);

    for(int i=0; i< 12; i++)
     maotp[i]=maotph[i]=0.;

    strcpy(hzt2,hzt1);
   }
  if(pod1 != pod)
   {
    if(mvr > 0 )
     {
      rasmes(mn,mk,maot,ostt,hnaz1,ff);
      mvr=0;

      itambu(mn,gn,mk,gk,itg1,naipod,orient,mr,maotp,ff,ffi,&sli,&kst,bf);
      for(int i=0;i< RAZMER_MAS;i++)
	itg1[i]=0.;
     }
    /*Читаем наименование подразделения*/
    memset(naipod,'\0',sizeof(naipod));
    if(in > 0)
     {
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),pod);
        iceb_menu_soob(strsql,data->window);
       }
      else
       strncpy(naipod,row1[0],sizeof(naipod)-1);
     }
    else
      strncpy(naipod,organ,sizeof(naipod)-1);

    sprintf(strsql,"%s %d %s\n",gettext("Подразделение"),pod,naipod);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);
    fprintf(ff,"%s %d %s\n",gettext("Подразделение"),pod,naipod);

    shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);
    fprintf(ff,"------------------------------------\n");

    for(int i=0; i< 12; i++)
     maotp[i]=0.;

    pod1=pod;
   }

  class poiinpdw_data rekin;
  /*Узнаем поправочный коэффициент счёт учёта в этом месяце*/
  if(poiinpdw(in,m,g,&rekin,data->window) != 0)
   {
    sprintf(strsql,"%d инвентарный номер не числится %d.%dг. !!!",in,m,g);
    iceb_menu_soob(strsql,data->window);
    
    shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);
    fprintf(ff,"%d инвентарный номер не числится %d.%dг. !!!\n",in,m,g);

   }

  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;

  if(in1 != in)
   {
//    printw("mvr=%d in1=%d\n",mvr,in1);
//    refresh();
    if(data->rk->mat_ot.ravno()[0] != '\0')
     {
      int podr=0;
      int kod_ot=0;
      poiinw(in,1,mn,gn,&podr,&kod_ot,data->window);
      if(iceb_u_proverka(data->rk->mat_ot.ravno(),kod_ot,0,0) != 0)
        continue;
     }

    if(mvr > 0 && in1 != 0)
     {
      rasmes(mn,mk,maot,ostt,hnaz1,ff);
      mvr=0;
     }
    in1=in;

    for(int i=0; i< 12; i++)
     maot[i]=0.;

    memset(naios,'\0',sizeof(naios));
    memset(gvve,'\0',sizeof(gvve));
    class poiinpdw_data rekin;
    /*Узнаем поправочный коэффициент*/
    if(poiinpdw(in,m,g,&rekin,data->window) != 0)
     {
      sprintf(strsql,"%d инвентарный номер не числится %d.%dг. !!!",in,m,g);
      iceb_menu_soob(strsql,data->window);
      
      shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);
      fprintf(ff,"%d инвентарный номер не числится %d.%dг. !!!\n",in,m,g);

     }

    if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
      continue;
    
    /*Читаем наименование объекта*/
    memset(naios,'\0',sizeof(naios));
    sprintf(strsql,"select naim,datvv from Uosin where innom=%d",in);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден инвентарный номер"),in);
      iceb_menu_soob(strsql,data->window);
     }
    else
     {
      strncpy(naios,row1[0],sizeof(naios)-1);
      if(row1[1][0] != '\0')
       {
        short dv,mv,gv;
        iceb_u_rsdat(&dv,&mv,&gv,row1[1],2);
        sprintf(gvve,"%d",gv);
       }
     }

    /*Читаем коэффициент*/

    sprintf(bros,"%s",rekin.hnaby.ravno());

    sprintf(strsql,"select naik,kof from Uosgrup1 where kod='%s'",bros);

    float kof=0;
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%d %s %s !",in,gettext("Не найдено группу"),bros);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    else
     {
      kof=atof(row1[1]);
     }

    /*На начало периода*/
    class bsizw_data bal_st;
    bsizw(in,pod1,1,mn,gn,&bal_st,NULL,data->window);
    bsby=bal_st.sbsby+bal_st.bsby;
    izby=bal_st.iz1by+bal_st.sizby+bal_st.izby;

    vrabu=uosprovarw(rekin.hnaby.ravno(),1,data->window);
    double lik_st=uosgetlsw(in,1,mn,gn,1,data->window);
    ostt=bsby-izby;
    if(vrabu == 1)
       god=(bsby-izby)*kof/100.;
    if(vrabu == 0)
       god=(bsby-lik_st)*kof/100.;

    mes=0.;
    if(god != 0.)
      mes=god/12.;



    itg[0]+=bsby;
    itg[1]+=izby;
    itg[2]+=ostt;
    itg[3]+=lik_st;
    itg[4]+=god;
    itg[5]+=mes;

    itg1[0]+=bsby;
    itg1[1]+=izby;
    itg1[2]+=ostt;
    itg1[3]+=lik_st;
    itg1[4]+=god;
    itg1[5]+=mes;

    itg2[0]+=bsby;
    itg2[1]+=izby;
    itg2[2]+=ostt;
    itg2[3]+=lik_st;
    itg2[4]+=god;
    itg2[5]+=mes;

    sprintf(invn,"%d",in);

    shvambu(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);

    fprintf(ff,"%-8s|%-*.*s|%4s|%-*.*s|%-*s|",
    invn,
    iceb_u_kolbait(40,naios),iceb_u_kolbait(40,naios),naios,
    gvve,
    iceb_u_kolbait(10,nomz),iceb_u_kolbait(10,nomz),nomz,
    iceb_u_kolbait(5,rekin.hnaby.ravno()),rekin.hnaby.ravno());

    if(mr == 1)
      fprintf(ff,"%5.2f|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5.2f|",kof,bsby,izby,ostt,lik_st,god,mes,rekin.popkfby);
/*
    printw("*************mr=%d\n",mr);
    OSTANOV();
*/    
    mvr++;
    strcpy(hnaz1,hna);
   }

  maot[m-1]=sao;
  maotp[m-1]=maotp[m-1]+sao;
  maotph[m-1]=maotph[m-1]+sao;
  maotpo[m-1]=maotpo[m-1]+sao;

 }

rasmes(mn,mk,maot,ostt,hnaz1,ff);

itambu(mn,gn,mk,gk,itg1,naipod,orient,mr,maotp,ff,ffi,&sli,&kst,bf);
itamhbu(mn,gn,mk,gk,itg2,hzt2,naimhz,orient,mr,maotph,ff,ffi,1,&sli,&kst,bf);
itamobu(mn,mk,itg,mr,maotpo,ff,ffi);

iceb_podpis(ff,data->window);
fclose(ff);

iceb_podpis(ffi,data->window);
fclose(ffi);



data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Ведомость амортизационных отчислений"));
data->rk->imaf.plus(imafi);
data->rk->naim.plus(gettext("Общий итог"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
