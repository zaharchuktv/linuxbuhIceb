/*$Id: uosvamotw_r.c,v 1.14 2011-08-29 07:13:44 sasa Exp $*/
/*12.07.2011	01.01.2008	Белых А.И.	uosvamotw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include "buhg_g.h"
#include <unistd.h>
#include "uosvamotw.h"

class uosvamotw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class uosvamotw_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;
  
  uosvamotw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosvamotw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvamotw_r_data *data);
gint uosvamotw_r1(class uosvamotw_r_data *data);
void  uosvamotw_r_v_knopka(GtkWidget *widget,class uosvamotw_r_data *data);

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

int uosvamotw_r(class uosvamotw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class uosvamotw_r_data data;
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(uosvamotw_r_key_press),&data);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(uosvamotw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)uosvamotw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosvamotw_r_v_knopka(GtkWidget *widget,class uosvamotw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosvamotw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvamotw_r_data *data)
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

void	uosshap(short mn,short gn,short mk,short gk,struct tm *bf,
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
-------------------------------------------------------------------");
if(mr == 1)
  fprintf(ffi,"\
--------------------------------------------------------------------------------");

strpod(mn,mk,11,ffi);

fprintf(ffi,gettext("\
Инв-рный|        Н а и м е н о в а н и е         | Номерной |Шифр |"));
if(mr == 1)
  fprintf(ffi,gettext("\
Норма|Балансовая|  Износ   |Остаточная|Ликвидаци.|Годов. сумма|Мес. сума |Коэфи|"));

strms(0,mn,mk,ffi);
fprintf(ffi,gettext("\
 номер  |             объекта                    |   знак   |н.ам.|"));
if(mr == 1)
  fprintf(ffi,gettext("\
а.отч|стоимость |          |бал. с-сть|стоимость |амортотчисл.|амортотчи.|циент|"));

strms(1,mn,mk,ffi);

fprintf(ffi,"\
-------------------------------------------------------------------");
if(mr == 1)
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
strpod(mn,mk,11,ffi);
}

/*****************/
/*Шапка документа*/
/*****************/
void shvam(short mn,short gn,short mk,short gk,int *sl,
int *kst,int orient,int mr,FILE *ff,struct tm *bf)
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

uosshap(mn,gn,mk,gk,bf,sl,mr,ff,kst);

}



/*******************************/
/*Выдача итога по подразделению*/
/*******************************/
void itam(short mn,short gn,short mk,short gk,double itg[],char nai[],int orient,int mr,
double maotp[12],FILE *ff,FILE *ffi,int *sli,int *kst,
struct tm *bf)

{
short           i;
double          vs;
char		bros[512];

shvam(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"\
-------------------------------------------------------------------");
fprintf(ffi,"\
-------------------------------------------------------------------");
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

shvam(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"%-*.*s|",iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);

fprintf(ffi,"%-*.*s|",iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);

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
void            itamh(short mn,short gn,short mk,short gk,double itg[],
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

shvam(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"\
-------------------------------------------------------------------");
fprintf(ffi,"\
-------------------------------------------------------------------");

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

shvam(mn,gn,mk,gk,sli,kst,orient,mr,ff,bf);
fprintf(ff,"%-*.*s|",
iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);
fprintf(ffi,"%-*.*s|",
iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);

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
void            itamo(short mn,short mk,double itg[],short mr,
double maotpo[12],FILE *ff,FILE *ffi)
{
short           i;
double          vs;
char		bros[512];

fprintf(ff,"\
-------------------------------------------------------------------");
fprintf(ffi,"\
-------------------------------------------------------------------");
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

fprintf(ff,"%-*.*s|",iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);

fprintf(ffi,"%-*.*s|",iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);

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

gint uosvamotw_r1(class uosvamotw_r_data *data)
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




int metka23=0;

if(iceb_sql_readkey("select kod from Uosgrup where ta=1 limit 1",data->window) >= 1)
 {
  metka23=1;
 }
else
 metka23=2;
 

short mr=0;
int kol_mes_v_per=iceb_u_period(1,mn,gn,1,mk,gk,1);

if(kol_mes_v_per <=3)
 mr=1;
else
 mr=0;

int max_dlin_str=0;
 
if(mr == 1)
 max_dlin_str=136+(kol_mes_v_per+1)*11;
else
 max_dlin_str=67+(kol_mes_v_per+1)*11;

if(kol_mes_v_per == 1)
  max_dlin_str=136+11;

short d=1;
short mp=mn;
short gp=gn;
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

sprintf(strsql,"select innom,mes,god,podr,hzt,hna,suma from Uosamor \
where god = %d and mes >= %d and mes <= %d order by \
hna,podr,innom,god,mes asc",
gn,mn,mk);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
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
sprintf(imaf,"vao%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  sss.clear_data();
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int orient=1;
if(iceb_u_proverka(spis_print_a3.ravno(),getenv("PRINTER"),0,1) == 0)
  orient=0; /*Если принтер А4 то ориентация всегда портрет*/



iceb_u_zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,31,mk,gk,organ,ff);


fprintf(ff,"%s.\n",gettext("Налоговый учет"));
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
sprintf(imafi,"vaoi%d.lst",getpid());
FILE *ffi;
if((ffi = fopen(imafi,"w")) == NULL)
 {
  sss.clear_data();
  iceb_er_op_fil(imafi,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,31,mk,gk,organ,ffi);


fprintf(ffi,"%s\n",gettext("Налоговый учет"));
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
uosshap(mn,gn,mk,gk,bf,&slii,mr,ffi,&ksti);


double          maot[12];
double          maotp[12];
double          maotph[12];
double          maotpo[12];
int innom=0;
for(int i=0; i< 12; i++)
    maot[i]=maotp[i]=maotph[i]=maotpo[i]=0.;
int mpg=0;
const int RAZMER_MAS=6;
double          itg[RAZMER_MAS];
double          itg1[RAZMER_MAS];
double          itg2[RAZMER_MAS];

for(int i=0;i< RAZMER_MAS;i++)
  itg[i]=itg1[i]=itg2[i]=0.;
char naipod[512];
char naimhz[512];

naipod[0]=naimhz[0]='\0';
strncpy(naipod,organ,sizeof(naipod)-1);
char		nomz[40];  /*Номерной знак*/
memset(nomz,'\0',sizeof(nomz));
int pod1=0;
float kolstr1=0.;
int in1=0,mvr=0;
char hna[64],hnaz1[64];
memset(hnaz1,'\0',sizeof(hnaz1));
class iceb_u_str filnast("uosnast.alx");
double          ostt=0.;
int sli=0;
int kolos=0;
short m=0,g=0;
double sao=0.;
SQL_str row1;
class SQLCURSOR curr;
char naios[512];
char bros[512];
char bros1[512];
float ppkf=1;   /*Поправочный коэффициент*/
char hnap[64];
float		kof;
double          god=0,mes=0;
char		invn[16];

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

  innom=atol(row[0]);

  if(metka23 == 1)
   if(uosprovgrw(row[5],data->window) == 1)
     if(innom > 0)
      continue;

  if(metka23 == 2 )
    if(uosprovgrw(row[5],data->window) == 1)
     if(innom < 0)
      continue;
     
  if(data->rk->mat_ot.ravno()[0] != '\0')
   {
    int pdd=0;
    int kol=0;
    poiinw(innom,1,mn,gn,&pdd,&kol,data->window);
    char kodot[20];
    sprintf(kodot,"%d",kol);
    if(iceb_u_proverka(data->rk->mat_ot.ravno(),kodot,0,0) != 0)
      continue;
   }

  mvr++;

  m=atoi(row[1]);
  g=atoi(row[2]);
  int pod=atoi(row[3]);
  strncpy(hna,row[5],sizeof(hna)-1);
  sao=atof(row[6]);

  if(mpg == 0)  
   if(iceb_u_SRAV(hna,"1",0) == 0)
    mpg=1;

  if(iceb_u_SRAV(hnaz1,hna,0) != 0)
   {

    if(hnaz1[0] != '\0')
     {
      if(mvr > 0)
       {
        rasmes(mn,mk,maot,ostt,hnaz1,ff);
        mvr=0;
        if(uosprovgrw(hnaz1,data->window) != 1)
         {
          itam(mn,gn,mk,gk,itg1,naipod,orient,mr,maotp,ff,ffi,&sli,&kst,bf);
          pod1=0;
         }
       }
      itamh(mn,gn,mk,gk,itg2,hnaz1,naimhz,orient,mr,maotph,ff,ffi,0,&sli,&kst,bf);
      for(int i=0;i< RAZMER_MAS;i++)
	itg1[i]=itg2[i]=0.;

      fprintf(ff,"\f");
      kst=0;
     }

    /*Читаем группу налогового учета*/
    memset(naimhz,'\0',sizeof(naimhz));
    sprintf(strsql,"select naik from Uosgrup where kod='%s'",hna);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %s !\n",gettext("Не найдена группа"),hna);
      iceb_menu_soob(strsql,data->window);
     }
    else
     strncpy(naimhz,row1[0],sizeof(naimhz)-1);
          
    sprintf(strsql,"%s: %s %s\n",gettext("Группа"),hna,naimhz);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    shvam(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);

    fprintf(ff,"%s: %s %s\n",gettext("Группа"),hna,naimhz);
    uosshap(mn,gn,mk,gk,bf,&sli,mr,ff,&kst);

    for(int i=0; i< 12; i++)
     maotp[i]=maotph[i]=0.;

    strcpy(hnaz1,hna);
   }
  if(pod1 != pod)
   {
    if(pod1 != 0)
     {
      rasmes(mn,mk,maot,ostt,hnaz1,ff);
      mvr=0;
      if(uosprovgrw(hnaz1,data->window) != 1)
         itam(mn,gn,mk,gk,itg1,naipod,orient,mr,maotp,ff,ffi,&sli,&kst,bf);
      for(int i=0;i< RAZMER_MAS;i++)
 	itg1[i]=0.;
     }
    /*Читаем наименование подразделения*/
    memset(naipod,'\0',sizeof(naipod));
    if(innom > 0)
     {
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,"%s %d !\n",gettext("Не найден код подразделения"),pod);
        iceb_menu_soob(strsql,data->window);
       }
      else
       strncpy(naipod,row1[0],sizeof(naipod)-1);
     }
    else
      strncpy(naipod,organ,sizeof(naipod)-1);

    sprintf(strsql,"%s %d %s\n",gettext("Подразделение"),pod,naipod);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    shvam(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);
    
    fprintf(ff,"%s %d %s\n",gettext("Подразделение"),pod,naipod);

    for(int i=0; i< 12; i++)
     maotp[i]=0.;

    pod1=pod;
   }
  if(innom > 0)
   {
    class poiinpdw_data rekin;
    /*Узнаем поправочный коэффициент счёт учёта в этом месяце*/
    if(poiinpdw(innom,m,g,&rekin,data->window) != 0)
     {
      
      shvam(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);
      fprintf(ff,"%d инвентарный номер не числится %d.%dг. !!!\n",innom,m,g);

      sprintf(strsql,"%d инвентарный номер не числится %d.%dг. !!!",innom,m,g);
      iceb_menu_soob(strsql,data->window);
     }
    if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
      continue;
    if(data->rk->sost_ob == 1)
     if(rekin.soso == 1 || rekin.soso == 2)
       continue;
    if(data->rk->sost_ob == 2)
     if(rekin.soso == 0 || rekin.soso == 3)
       continue;
    strncpy(hnap,rekin.hna.ravno(),sizeof(hnap)-1);         
   }

  if(in1 != innom)
   {
    if(mvr > 0 && in1 != 0)
     {
      rasmes(mn,mk,maot,ostt,hnaz1,ff);
      mvr=0;
     }
    in1=innom;

    for(int i=0; i< 12; i++)
     maot[i]=0.;

    memset(naios,'\0',sizeof(naios));

    if(innom > 0)
     {
      /*Читаем наименование объекта*/
      memset(naios,'\0',sizeof(naios));
      sprintf(strsql,"select naim from Uosin where innom=%d",innom);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,"%s %d !",
        gettext("Не найден инвентарный номер"),innom);
        iceb_menu_soob(strsql,data->window);
       }
      else
       strncpy(naios,row1[0],sizeof(naios)-1);

     }
    else
     {
      sprintf(bros,"Поправочный коэффициент для группы %d",innom*-1);
      if(iceb_poldan(bros,bros1,filnast.ravno(),data->window ) != 0)
       {
        sprintf(strsql,"Не найден %s !!!\nВ файле %s",bros,filnast.ravno());
        iceb_menu_soob(strsql,data->window);
       }
      ppkf=iceb_u_atof(bros1); 
     } 

    /*Читаем коэффициент*/

    if(innom < 0)
     sprintf(hnap,"%d",innom*(-1));

    sprintf(bros,"%s",hnap);

    sprintf(strsql,"select naik,kof from Uosgrup where kod='%s'",bros);

    kof=0;
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найдено группу"),bros);
      iceb_menu_soob(strsql,data->window); 
      continue;
     }
    else
     {
      kof=atof(row1[1]);
      if(innom < 0)
       strncpy(naios,row1[0],sizeof(naios)-1);
     }

    /*На начало периода*/
    double bs=0.,iz=0.;
    if(innom > 0)
     {
      class bsizw_data bal_st;
      bsizw(innom,pod1,1,mn,gn,&bal_st,NULL,data->window);
      
      bs=bal_st.sbs+bal_st.bs;
/**********
      if(bs == 0.) //Произошел расход
       {
        continue;      
       }
************/
      iz=bal_st.iz+bal_st.iz1+bal_st.siz;
     }

    if(innom < 0)
     {
      sprintf(bros,"Стартовая балансовая стоимость для группы %d",innom*-1);
      if(iceb_poldan(bros,bros1,filnast.ravno(),data->window) != 0)
       {
        sprintf(strsql,"Не найдена %s",bros);
        iceb_menu_soob(strsql,data->window);
       }
      bs=iceb_u_atof(bros1);
      sprintf(bros,"Стартовый износ для группы %d",innom*-1);
      if(iceb_poldan(bros,bros1,filnast.ravno(),data->window) != 0)
       {
        sprintf(strsql,"Не найден %s",bros);
        iceb_menu_soob(strsql,data->window);
       }

      iz=iceb_u_atof(bros1);

      sprintf(strsql,"Группа %d bs=%f iz=%f\n",innom*-1,bs,iz);
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

      double iz1=0.,iz11=0.,bs1=0.;
      bsiz23w(hnap,1,mn,gn,&bs1,&iz1,&iz11,NULL,data->window);
      iz+=iz1+iz11;
      bs+=bs1;
      sprintf(strsql,"hnap=%s bs1=%f iz1=%f iz11=%f iz=%f\n",hnap,bs1,iz1,iz11,iz);
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
     }
    double lik_st=uosgetlsw(innom,1,mn,gn,0,data->window);
    if(uosprovarw(hnap,0,data->window) == 1)
     {
      ostt=bs-iz;
      if(metkabo == 0)
       god=(bs-iz)*kof/100.;
      if(metkabo == 1)
        god=(bs-lik_st)*kof/100.;
     }
    else
     {
      ostt=bs-iz;
      god=(bs-lik_st)*kof/100.;
     }

    mes=0.;
    if(god != 0.)
      mes=god/12.;

    itg[0]+=bs;
    itg[1]+=iz;
    itg[2]+=ostt;
    itg[3]+=lik_st;
    itg[4]+=god;
    itg[5]+=mes;

    itg1[0]+=bs;
    itg1[1]+=iz;
    itg1[2]+=ostt;
    itg1[3]+=lik_st;
    itg1[4]+=god;
    itg1[5]+=mes;

    itg2[0]+=bs;
    itg2[1]+=iz;
    itg2[2]+=ostt;
    itg2[3]+=lik_st;
    itg2[4]+=god;
    itg2[5]+=mes;

    if(innom > 0)
     sprintf(invn,"%d",innom);
    else
     {
      sprintf(invn,"%d %s",innom*(-1),gettext("группа"));
     }     

    shvam(mn,gn,mk,gk,&sli,&kst,orient,mr,ff,bf);

    fprintf(ff,"%-8s|%-*.*s|%-*.*s|%-*s|",
    invn,
    iceb_u_kolbait(40,naios),iceb_u_kolbait(40,naios),naios,
    iceb_u_kolbait(10,nomz),iceb_u_kolbait(10,nomz),nomz,
    iceb_u_kolbait(5,hnap),hnap);
    
    if(mr == 1)
      fprintf(ff,"%5.2f|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5.2f|",
      kof,bs,iz,ostt,lik_st,god,mes,ppkf);
    kolos++;
   }

  maot[m-1]=sao;
  maotp[m-1]=maotp[m-1]+sao;
  maotph[m-1]=maotph[m-1]+sao;
  maotpo[m-1]=maotpo[m-1]+sao;

 }

rasmes(mn,mk,maot,ostt,hnaz1,ff);

if(uosprovgrw(hnaz1,data->window) != 1)
   itam(mn,gn,mk,gk,itg1,naipod,orient,mr,maotp,ff,ffi,&sli,&kst,bf);
itamh(mn,gn,mk,gk,itg2,hnaz1,naimhz,orient,mr,maotph,ff,ffi,0,&sli,&kst,bf);
itamo(mn,mk,itg,mr,maotpo,ff,ffi);

if(mpg == 1)
 fprintf(ff,gettext("* Остаточная стоимость меньше (равняется) 100 необлагаемых минимумов.\n"));

sprintf(strsql,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ffi,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

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
