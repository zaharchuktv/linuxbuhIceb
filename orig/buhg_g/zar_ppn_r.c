/*$Id: zar_ppn_r.c,v 1.22 2011-08-29 07:13:44 sasa Exp $*/
/*16.02.2010	30.11.2006	Белых А.И.	zar_ppn_r.c
Перерасчёт подоходного налога
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_ppn.h"

struct ppnal1
 {
  int   tabn;      //табельный номер
  char   fio[512];  //фамилия
  double osuma[2][12]; //общая сумма начислений
  double suma_nvrpn[2][12]; //сумма не входящая в расчет для подоходного налога (общая)
  double zarpl[2][12]; //сумма зарплаты 
  double sumalgot[2][12]; // сумма социальных льгот
  double podnal[12];   // сумма подоходного налога  
  double sumasoc[12];  //сумма соц-отчислений
  double suma_ndfv[2][12]; //сумма не денежных форм выплаты
  double blag_pom[2][12];  //Сумма благотворительной помощи
  double nn_blag_pom[2][12];  //Сумма необлагоемой налогом благотворительной помощи
  double suma_bez_soc_lgot[2][12]; /*Сумма на которую не распространяестя социальная льгота*/
  
  double suma_sk_esv[2][12]; /*Сумма для расчёта единого социального взноса для которой применяется льгота*/
  double suma_sk_esv_bol[2][12]; /*Сумма для расчёта единого социального взноса c больничного*/


  char   kodlg[12][32]; //коды льгот введенные по месяцам
  short  dn,mn,gn; //Дата приёма на работу
  short  dk,mk,gk;  //Дата увольнения с работы
 };


class zar_ppn_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zar_ppn_rek *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zar_ppn_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_ppn_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_ppn_r_data *data);
gint zar_ppn_r1(class zar_ppn_r_data *data);
void  zar_ppn_r_v_knopka(GtkWidget *widget,class zar_ppn_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;

extern short *kodbl; /*Код больничного*/
extern char *organ;
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
//extern short    kodpn;   /*Код подоходного налога*/
extern class iceb_u_str kodpn_all; /*все коды подоходного налога*/
extern short kodperpn;   /*Код перерасчета подоходного налога*/
extern short    *knvr;/*Коды начислений не входящие в расчет подоходного налога*/
extern short    *kodmp;  //Коды благотворительной помощи
extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern float proc_esv; /*процент единого социального взноса с работника*/
extern float proc_esv_bol; /*процент единого социального взноса с работника*/
extern float proc_esv_inv; /*процент единого социального взноса с работника*/
extern float proc_esv_dog; /*процент единого социального взноса с работника*/
extern class iceb_u_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
extern short *knnf; //Коды начислений недежными формами 

int zar_ppn_r(class zar_ppn_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zar_ppn_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Перерасчёт подоходного налога"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zar_ppn_r_key_press),&data);

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

repl.plus(gettext("Перерасчёт подоходного налога"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zar_ppn_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zar_ppn_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_ppn_r_v_knopka(GtkWidget *widget,class zar_ppn_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_ppn_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_ppn_r_data *data)
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

/****************************************/
/*Распечатка данных и перерасчет*/
/**************************************/

double  zarppn1_rdp(short mn,short mk,short god,struct ppnal1 *PP,iceb_u_double *suma_ne_zarpl,
FILE *ff,FILE *ff_p,FILE *ff_spis,GtkWidget *wpredok)
{
double itogo[10];

memset(&itogo,'\0',sizeof(itogo));

fprintf(ff,"\n%d %s\n",PP->tabn,PP->fio);
fprintf(ff,"Дата початку роботи:%02d.%02d.%04d\n",PP->dn,PP->mn,PP->gn);
fprintf(ff,"Дата звільнення    :%02d.%02d.%04d\n",PP->dk,PP->mk,PP->gk);

fprintf(ff_p,"\n%d %s\n",PP->tabn,PP->fio);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
 Дата  |Нараховано|Не оподат.|Благодійна|Неопо.благ|  Податок |Зарплата  |Не гро.фор|Соціальні |Податок з |Соц.відра.|Код пільги|\n\
       |          |податком  |допомога  |одійна доп|загальний |          |ми нар.зар|від.загал.|зарплати  |з зарплати|          |\n");

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
double matpomb;
double nalsz;
double it_soc=0.;
double podoh_sz[12];
zarrnesvw(NULL,wpredok);

for(int i=mn-1 ; i < mk; i++)
 {
  //Вычисяем соц-отчисления с зарплаты
  zar_read_tnw(1,i+1,god,NULL,wpredok);
    
  double suma_blag_pom=0.,suma_blag_pom_b=0.;
  /*Определение необлагаемай налогом благотворительной помощи*/
  if(PP->blag_pom[0][i] != 0.)
    PP->nn_blag_pom[0][i]+=matpom1w(PP->tabn,i+1,god,kodmp,NULL,1,&matpomb,1,&suma_blag_pom,&suma_blag_pom_b,wpredok);

  nalsz=PP->podnal[i];
  it_soc=PP->sumasoc[i];

  fprintf(ff,"%02d.%04d %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %s\n",
  i+1,god,
  PP->osuma[0][i],
  PP->suma_nvrpn[0][i],
  PP->blag_pom[0][i],
  PP->nn_blag_pom[0][i],
  PP->podnal[i],
  PP->zarpl[0][i],
  PP->suma_ndfv[0][i],
  PP->sumasoc[i],
  nalsz,
  it_soc,
  PP->kodlg[i]);


  podoh_sz[i]=nalsz;
  itogo[0]+=PP->osuma[0][i];
  itogo[1]+=PP->suma_nvrpn[0][i];
  itogo[2]+=PP->blag_pom[0][i];
  itogo[3]+=PP->nn_blag_pom[0][i];
  itogo[4]+=PP->podnal[i];
  itogo[5]+=PP->zarpl[0][i];
  itogo[6]+=PP->suma_ndfv[0][i];
  itogo[7]+=PP->sumasoc[i];
  itogo[8]+=nalsz;
  itogo[9]+=it_soc;
  
 }

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%7s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
" ",itogo[0],itogo[1],itogo[2],itogo[3],itogo[4],itogo[5],itogo[6],itogo[7],itogo[8],itogo[9]);


if(kn_bsl != NULL)
 if(kn_bsl[0] != 0)
 if(suma_ne_zarpl->suma() != 0.)
  {
   double itogo_nz=0.;
   char naim_nash[512];
   char strsql[512];
   SQL_str row;
   SQLCURSOR cur;

   fprintf(ff,"\n%s:\n","Нарахування, на які не розповсюджуеться социальна пільга");
   fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");   
   fprintf(ff,"Код|  Найменування нарахування    |");

   for(int i=0; i < 12 ; i++)
    fprintf(ff," %02d.%04d|",i+1,god);

   fprintf(ff,"%-*.*s|\n",iceb_u_kolbait(8,"Разом"),iceb_u_kolbait(8,"Разом"),"Разом");
   fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");   
   double itogo_nzi[12];
   memset(&itogo_nzi,'\0',sizeof(itogo_nzi));
   for(int i=1; i <= kn_bsl[0]; i++)
    {
     //Проверяем есть ли начисления
     itogo_nz=0.;
     for(int ii=0; ii < 12; ii++)
      {
       itogo_nz+=suma_ne_zarpl->ravno((i-1)*12+ii);
       itogo_nzi[ii]+=suma_ne_zarpl->ravno((i-1)*12+ii);
      }
     if(itogo_nz == 0.)
      continue;
     memset(naim_nash,'\0',sizeof(naim_nash));
    //Читаем наименование начисления
     sprintf(strsql,"select naik from Nash where kod=%d",kn_bsl[i]);
     if(sql_readkey(&bd,strsql,&row,&cur) == 1)
      strncpy(naim_nash,row[0],sizeof(naim_nash)-1);

     fprintf(ff,"%3d %-*.*s ",
     kn_bsl[i],
     iceb_u_kolbait(30,naim_nash),iceb_u_kolbait(30,naim_nash),naim_nash);
     
     for(int ii=0; ii < 12; ii++)
       fprintf(ff,"%8.2f ",suma_ne_zarpl->ravno((i-1)*12+ii));
     fprintf(ff,"%8.2f\n",itogo_nz);     

    }
   fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");   

   fprintf(ff,"%*s ",iceb_u_kolbait(34,"Разом"),"Разом");
   for(int ii=0; ii < 12; ii++)
       fprintf(ff,"%8.2f ",itogo_nzi[ii]);
   fprintf(ff,"%8.2f\n",suma_ne_zarpl->suma());
  }

fprintf(ff,"\nРозрахунок по місяцях в рахунок яких були зроблені нарахування\n");

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
 Дата  |Нараховано|Не оподат.|Благдійна |Неопо.благ|Зарплата  |Не гро.фор|Соц.відра.|Податок   |Податок   |Різниця|\n\
       |          |податком  |допомога  |одійна доп|          |ми нар.зар|з зарплати|розрахунк.|утриманий |       |\n");

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------\n");

memset(&itogo,'\0',sizeof(itogo));

fprintf(ff_p,"\nПерерасчёт\n");

for(int i=mn-1 ; i < mk; i++)
 {
  fprintf(ff_p,"Месяц-%d.%d\n",i+1,god);
  zar_read_tnw(1,i+1,god,ff_p,wpredok);

  fprintf(ff_p,"Cумма для расчёта единого социального взноса=%.2f Больничный=%.2f\n",PP->suma_sk_esv[1][i],PP->suma_sk_esv_bol[1][i]);
  double esv_s_lgot=0.;
  double esv_s_lgot_bol=0.;
  
  zaresv_rs(i+1,god,PP->tabn,0,0.,0.,0.,0.,PP->suma_sk_esv[1][i],0.,PP->suma_sk_esv_bol[1][i],0.,0.,&esv_s_lgot,&esv_s_lgot_bol,1,"",ff_p,wpredok);
 
  it_soc=esv_s_lgot+esv_s_lgot_bol;
  fprintf(ff_p,"Сумма посчитанного единого социального взноса=%.2f+%.2f=%.2f\n",esv_s_lgot,esv_s_lgot_bol,it_soc);
     
  double suma_blag_pom=0.,suma_blag_pom_b=0.;
  /*Определение необлагаемай налогом благотворительной помощи*/
  if(PP->blag_pom[1][i] != 0.)
    PP->nn_blag_pom[1][i]+=matpom1w(PP->tabn,i+1,god,kodmp,NULL,1,&matpomb,1,&suma_blag_pom,&suma_blag_pom_b,wpredok);


  nalsz=podohrs2w(PP->tabn,i+1,god,PP->zarpl[1][i],it_soc,PP->suma_bez_soc_lgot[1][i],PP->suma_ndfv[1][i],1,1,ff_p,wpredok);

  fprintf(ff,"%02d.%04d %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %7.2f\n",
  i+1,god,
  PP->osuma[1][i],
  PP->suma_nvrpn[1][i],
  PP->blag_pom[1][i],
  PP->nn_blag_pom[1][i],
  PP->zarpl[1][i],
  PP->suma_ndfv[1][i],
  it_soc,
  nalsz,
  podoh_sz[i],
  nalsz-podoh_sz[i]);



  itogo[0]+=PP->osuma[1][i];
  itogo[1]+=PP->suma_nvrpn[1][i];
  itogo[2]+=PP->blag_pom[1][i];
  itogo[3]+=PP->nn_blag_pom[1][i];
  itogo[4]+=PP->zarpl[1][i];
  itogo[5]+=PP->suma_ndfv[1][i];
  itogo[6]+=it_soc;
  itogo[7]+=nalsz;
  itogo[8]+=podoh_sz[i];
  itogo[9]+=nalsz-podoh_sz[i];  
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%7s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %7.2f\n",
" ",itogo[0],itogo[1],itogo[2],itogo[3],itogo[4],itogo[5],itogo[6],itogo[7],itogo[8],itogo[9]);


fprintf(ff_spis,"%6d %-*.*s %10.2f\n",
PP->tabn,
iceb_u_kolbait(30,PP->fio),iceb_u_kolbait(30,PP->fio),PP->fio,
itogo[9]);

return(itogo[9]);
 

}

/******************************************/
/******************************************/

gint zar_ppn_r1(class zar_ppn_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);







short mnr=data->rk->mesn.ravno_atoi();
short mkr=data->rk->mesk.ravno_atoi();
short godr=data->rk->god.ravno_atoi();

if(mnr == 0 || mnr > 11)
 {
  iceb_menu_soob(gettext("Неправильно ввели месяц начала !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(mkr == 0 || mkr > 12 || mkr < 2)
 {
  iceb_menu_soob(gettext("Неправильно ввели месяц конца !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(godr == 0)
 {
  iceb_menu_soob(gettext("Неправильно ввели год !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr;
SQL_str row;
SQL_str row1;

sprintf(strsql,"select datz,tabn,prn,knah,suma,mesn,godn,podr from Zarp where \
datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and suma <> 0. \
order by tabn asc, datz asc",godr,mnr,godr,mkr);


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

char imaf[64];
FILE *ff;

sprintf(imaf,"ppn%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_spis[30];
FILE *ff_spis;

sprintf(imaf_spis,"ppnsp%d.lst",getpid());
if((ff_spis = fopen(imaf_spis,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_spis,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
time_t vrem;
time(&vrem);
bf=localtime(&vrem);

iceb_u_zagolov(gettext("Перерасчет подоходного налога"),1,mnr,godr,31,mkr,godr,organ,ff_spis);
fprintf(ff_spis,"\n");
char imaf_p[64];
FILE *ff_p;
sprintf(imaf_p,"ppnp%d.lst",getpid());
if((ff_p = fopen(imaf_p,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_p,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
fprintf(ff_p,"Код больничного:\n");
if(kodbl != NULL)
 for(int nom=1; nom <= kodbl[0]; nom++)
  fprintf(ff_p,"%d ",kodbl[nom]);
fprintf(ff_p,"\n");
  
long tabnz=0;
float kolstr1=0;
struct ppnal1 PP;
memset(&PP,'\0',sizeof(PP));    

double suma;
short d,m,g;
int knah;
short mesz=0;

class iceb_u_double suma_ne_zarpl;
if(kn_bsl != NULL)
 if(kn_bsl[0] != 0)
  suma_ne_zarpl.make_class(kn_bsl[0]*12);
int kol_tab_nom=0;  
short mes_vshet;
short god_vshet;
double suma_per=0.;
int tabn=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(LINES-1,0,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[1],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->podr.ravno(),row[7],0,0) != 0)
   continue;

  tabn=atol(row[1]);

  if(tabnz != tabn)
   {

    if(tabnz != 0 )
     {
      if(kol_tab_nom > 0)
       fprintf(ff,"\f");
      
      suma_per+=zarppn1_rdp(mnr,mkr,godr,&PP,&suma_ne_zarpl,ff,ff_p,ff_spis,data->window);
      kol_tab_nom++;
     }

    memset(&PP,'\0',sizeof(PP));    
    suma_ne_zarpl.clear_class();
    PP.tabn=tabnz=tabn;
    memset(PP.fio,'\0',sizeof(PP.fio));    
    sprintf(strsql,"select fio,datn,datk from Kartb where tabn=%d",PP.tabn);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      strncpy(PP.fio,row1[0],sizeof(PP.fio)-1);                   
      iceb_u_rsdat(&PP.dn,&PP.mn,&PP.gn,row1[1],2);
      iceb_u_rsdat(&PP.dk,&PP.mk,&PP.gk,row1[2],2);
      
     }
    sprintf(strsql,"%5d %s\n",PP.tabn,PP.fio);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    mesz=0;
   }


  
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  knah=atoi(row[3]);    
  suma=atof(row[4]);
  mes_vshet=atoi(row[5]);
  god_vshet=atoi(row[6]);
  
  if(mesz != m)
   {
    sprintf(strsql,"select lgot from Zarn where tabn=%d and god=%d and mes=%d",
    tabn,g,m);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(PP.kodlg[m-1],row1[0],iceb_u_kolbait(10,row1[0]));
    mesz=m;
   }  

  if(atoi(row[2]) == 1)
   {
    PP.osuma[0][m-1]+=suma;
    if(g == god_vshet)
      PP.osuma[1][mes_vshet-1]+=suma;

    if(provkodw(knvr,knah) >= 0)
     {
      PP.suma_nvrpn[0][m-1]+=suma;
      if(g == god_vshet)
        PP.suma_nvrpn[1][mes_vshet-1]+=suma;

      continue;
     }

    if(provkodw(kodmp,knah) >= 0)
     {
      PP.blag_pom[0][m-1]+=suma;
      if(g == god_vshet)
        PP.blag_pom[1][mes_vshet-1]+=suma;

     //Код благотворительности должен быть в списке не зарплаты
//      continue;
     }

    double budget=0.;
    /*добавляем НДС*/
    double suma_s_nds=knnfndsw(1,knah,"",suma,&budget,NULL); /*возвращает увеличенную сумму*/
    if(suma_s_nds > 0.)
     suma=suma_s_nds;
    int nomer=0; 

    if((nomer=provkodw(kn_bsl,knah)) >= 0) /*не применяется соц. льгота*/
     {
      if(provkodw(knnf,knah) >= 0) /*если это и неденежная форма то нужно увеличить на коэввициент*/
       rudnfvw(&suma,suma,NULL);
      
      suma_ne_zarpl.plus(suma,(nomer)*12+(m-1));
      PP.suma_bez_soc_lgot[1][mes_vshet-1]+=suma;
     
     }
    else /*применяется соц. льгота*/
     {
      PP.zarpl[0][m-1]+=suma;
      if(g == god_vshet)
        PP.zarpl[1][mes_vshet-1]+=suma;

      if(iceb_u_proverka(knvr_esv_r.ravno(),knah,0,1) != 0)
       {
        if(provkodw(kodbl,knah) >= 0) /*больничный*/
         {
          PP.suma_sk_esv_bol[0][m-1]+=suma;
          if(g == god_vshet)
            PP.suma_sk_esv_bol[1][mes_vshet-1]+=suma;
         }
        else /*всё остальное*/
         {
          PP.suma_sk_esv[0][m-1]+=suma;
          if(g == god_vshet)
            PP.suma_sk_esv[1][mes_vshet-1]+=suma;
         }       
       }

      if(provkodw(knnf,knah) >= 0)
       {


        PP.suma_ndfv[0][m-1]+=suma;
        if(g == god_vshet)
            PP.suma_ndfv[1][mes_vshet-1]+=suma;

       }

     }

   }
  else
   {

    if(kodperpn == knah)   /*Код перерасчета подоходного налога*/
     continue;
    if(iceb_u_proverka(kodpn_all.ravno(),knah,0,1) == 0)
      PP.podnal[m-1]+=suma;

    if(knah == kod_esv)
      PP.sumasoc[m-1]+=suma;

    if(knah == kod_esv_bol)
      PP.sumasoc[m-1]+=suma;

    if(knah == kod_esv_inv)
      PP.sumasoc[m-1]+=suma;

    if(knah == kod_esv_dog)
      PP.sumasoc[m-1]+=suma;

   }
   
 }

if(kol_tab_nom > 0)
     fprintf(ff,"\f");

suma_per+=zarppn1_rdp(mnr,mkr,godr,&PP,&suma_ne_zarpl,ff,ff_p,ff_spis,data->window);

fprintf(ff_spis,"%*s:%10.2f\n",iceb_u_kolbait(37,"Разом"),"Разом",suma_per);

iceb_podpis(ff,data->window);
iceb_podpis(ff_p,data->window);
iceb_podpis(ff_spis,data->window);
fclose(ff);
fclose(ff_p);
fclose(ff_spis);

data->rk->imaf.plus(imaf);
data->rk->imaf.plus(imaf_spis);
data->rk->imaf.plus(imaf_p);
data->rk->naimf.plus(gettext("Перерасчёт подоходного налога"));
data->rk->naimf.plus(gettext("Конечный результат расчёта"));
data->rk->naimf.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),1,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
