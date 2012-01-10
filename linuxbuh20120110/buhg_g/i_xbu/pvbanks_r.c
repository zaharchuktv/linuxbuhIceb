/*$Id: pvbanks_r.c,v 1.22 2011-02-21 07:35:56 sasa Exp $*/
/*19.11.2009	08.04.2004	Белых А.И.	pvbanks_r.c
Перечисление на карт-счета сумм сальдо по счёту
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "pvbanks.h"


class pvbanks_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество прочитанных строк
  int    kolstr;  //Количество строк в курсоре
  short     prohod;
  iceb_u_spisok imaf;
  iceb_u_spisok naim;

  pvbanks_rr *rek_r;
  short dk,mk,gk;
  short  godn;
  time_t vremn;
    
  SQLCURSOR cur;
  short kolshet;
  iceb_u_spisok KONT;
  int kolkontr;
  iceb_u_double deb;
  iceb_u_double kre;
  FILE *ffsp;
  FILE *ffdis;
  double sumalist;  
  int  kolstrkartr;
  char rukov[512];
  char glavbuh[40];
  double itogoo;
  char  pris[60];
  short dlpris;
  
  //Конструктор  
  pvbanks_r_data()
   {
    kolstr1=0.;
    kolstr=0;
    prohod=0;

    sumalist=0.;
    kolstrkartr=0;
    itogoo=0.;
   }

 };

gboolean   pvbanks_r_key_press(GtkWidget *widget,GdkEventKey *event,class pvbanks_r_data *data);
gint pvbanks_r1(class pvbanks_r_data *data);
void  pvbanks_r_v_knopka(GtkWidget *widget,class pvbanks_r_data *data);

int pvbanks_kprovw(class iceb_u_spisok *KONT,int snom,const char *pris,int metkadk,double *sum,char *tabnom,
double *deb,double *kre,int dlpris,char *fio,char *bankshet,char *inn,const char *grupk,
int kod_banka,
GtkWidget *wpredok);

extern SQL_baza bd;
extern char *organ;
extern char *name_system;
extern short vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
extern short	startgodb; /*Стартовый год*/
extern int kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;

void pvbanks_r(class pvbanks_rr *data_rr)
{
char strsql[512];
iceb_u_str soob;

pvbanks_r_data data;

data.rek_r=data_rr;


iceb_u_rsdat(&data.dk,&data.mk,&data.gk,data.rek_r->datan.ravno(),1);
  
data.godn=startgodb;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать документы для перечисления на карт-счета"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(pvbanks_r_key_press),&data);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);
sprintf(strsql,"%s %s",gettext("Расчет с использованием проводок"),data.rek_r->shet.ravno_toutf());

soob.new_plus(strsql);

sprintf(strsql,"%s %d.%d.%d",gettext("Дата сальдо"),
data.dk,data.mk,data.gk);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno_toutf());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);


data.view=gtk_text_view_new();
gtk_widget_set_usize(GTK_WIDGET(data.view),400,300);

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
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
gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
GtkTooltips *tooltops=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltops,data.knopka,gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(pvbanks_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)pvbanks_r1,&data);

gtk_main();

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);





}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  pvbanks_r_v_knopka(GtkWidget *widget,class pvbanks_r_data *data)
{
// printf("pvbanks_r_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   pvbanks_r_key_press(GtkWidget *widget,GdkEventKey *event,class pvbanks_r_data *data)
{
// printf("pvbanks_r_key_press\n");

switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}
/*************************************/
/*Выполнение проводок*/
/***************************/
void pvbanks_make_prov(const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
int metkadk,
const char *kontr,
const char *fio,
double sum,
GtkWidget *wpredok)
{

double deb=0.;
double kre=0.;
char kontr1[20];
char kontr2[20];
memset(kontr1,'\0',sizeof(kontr1));
strncpy(kontr1,kontr,sizeof(kontr1)-1);
strncpy(kontr2,kontr,sizeof(kontr2)-1);

if(metkadk == 0) /*Дебет*/
 kre=sum;
if(metkadk == 1) /*кредит*/
 deb=sum;
char koment[512];
sprintf(koment,"%s %s",gettext("Перечислено"),fio);
 
iceb_zapprov(0,gr,mr,dr,shet,shet_kor,kontr1,kontr2,"","","",deb,kre,koment,2,0,time(NULL),0,0,0,0,0,wpredok);
}

/***************************/
/*Распечатка для банка ПИБ*/
/***************************/
void pvbavks_pib(iceb_u_spisok *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int dlpris,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
int kod_banka,
GtkWidget *wpredok)
{
int  kolstrkartr=0;
FILE *ffsp;
FILE *ffdis;
char imafsp[30];
char imafdis[30];
char  bankshet[32];
char  fio[512];
char tabnom[20];
double sumalist=0;
double sum=0;
double itogoo=0.;
SQLCURSOR cur1;


sprintf(imafsp,"pib%d.lst",getpid());
sprintf(imafdis,"pibd%d.txt",getpid());

if((ffsp = fopen(imafsp,"w")) == NULL)
 {
  iceb_er_op_fil(imafsp,"",errno,wpredok);
  return;
 }
hrvnks(ffsp,&kolstrkartr);

if((ffdis = fopen(imafdis,"w")) == NULL)
 {
  iceb_er_op_fil(imafdis,"",errno,wpredok);
  return;
 }
int kolh=0;
int kolkontr=KONT->kolih();
int kol_na_liste=0;
char inn[15];

for(int snom=0 ; snom < kolkontr; snom++)
 {
  memset(fio,'\0',sizeof(fio));
  memset(bankshet,'\0',sizeof(bankshet));
  memset(inn,'\0',sizeof(inn));
  memset(tabnom,'\0',sizeof(tabnom));
  
  if(pvbanks_kprovw(KONT,snom,pris,metkadk,&sum,tabnom,deb,kre,dlpris,fio,bankshet,inn,grupk,kod_banka,wpredok) != 0)
   continue;

  pib_itlw(&kolstrkartr,&sumalist,&kol_na_liste,rukov,glavbuh,ffsp);

  sumalist+=sum;
  itogoo+=sum;
  kolh++;
  
  pib_strw(kolh,bankshet,sum,fio,inn,ffsp);    
  
  kol_na_liste++;
  
  fprintf(ffdis,"%-16s %8.2f %s\n",bankshet,sum,fio);
  if(dr != 0) /*нужно делать проводки*/
    pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio, sum,wpredok);
 }

pib_end_dokw(kol_na_liste,sumalist,itogoo,kolh,rukov,glavbuh,ffsp);

fclose(ffsp);
fclose(ffdis);

//Записываем шапку и концовку
hdisk(kolh,itogoo,imafdis);

class iceb_u_spisok imafo;
class iceb_u_spisok naimo;

imafo.plus(imafsp);
naimo.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imafdis);
naimo.plus(gettext("Документ в электронном формате"));
iceb_ustpeh(imafo.ravno(0),1,wpredok);


}
/***************************/
/*Распечатка для Укрсоцбанка*/
/***************************/
void pvbavks_uks(iceb_u_spisok *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int dlpris,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
int kod_banka,
GtkWidget *wpredok)
{
FILE *ff;
FILE *fftmp;
char imaf[30];
char imaftmp[30];
char imafzag[30];
char  bankshet[32];
char  fio[512];
char tabnom[20];
double sumalist=0;
double sum=0;
double itogoo=0.;
char strsql[512];
SQLCURSOR cur1;


sprintf(imaftmp,"ukrsoc%d.tmp",getpid());
if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }

sprintf(imaf,"ukrsoc%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

time_t          vrem;

time(&vrem);
struct tm *bf=localtime(&vrem);

fprintf(ff,"\
   Зведена відомість сум для зарахування на спеціальні карткові рахунки N_____\n\
------------------------------------------------------------------------------\n\
| Дата                             |             %02d.%02d.%04d                  |\n\
| Назва організації                |%-41.41s|\n\
| Назва та N філії АКБ \"Укрсоцбанк\"|Вінницька облана філія N050              |\n\
| Додаткова інформація             |                                         |\n\
------------------------------------------------------------------------------\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
organ);

fprintf(ff,"\
--------------------------------------------------------------------------------------\n\
 N  |     Призвище, ім'я по батькові         | Картковий счёт |Сумма до зарахування|\n\
--------------------------------------------------------------------------------------\n"); 
int kolh=0;
int kolkontr=KONT->kolih();
char inn[30];
int nomerstr=0;

for(int snom=0 ; snom < kolkontr; snom++)
 {
  memset(fio,'\0',sizeof(fio));
  memset(bankshet,'\0',sizeof(bankshet));
  memset(inn,'\0',sizeof(inn));
  memset(tabnom,'\0',sizeof(tabnom));

  if(pvbanks_kprovw(KONT,snom,pris,metkadk,&sum,tabnom,deb,kre,dlpris,fio,bankshet,inn,grupk,kod_banka,wpredok) != 0)
   continue;

  sumalist+=sum;
  itogoo+=sum;
  kolh++;
    

  fprintf(ff,"%-4d %-*.*s %-19.19s %10.2f\n",
  ++nomerstr,
  iceb_u_kolbait(40,fio),iceb_u_kolbait(40,fio),fio,
  bankshet,
  sum);
  
  fprintf(fftmp,"@%s,0,%.2f\n",bankshet,sum);

  if(dr != 0) /*нужно делать проводки*/
    pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio,sum,wpredok);

 }
fprintf(ff,"\
--------------------------------------------------------------------------------------\n"); 

fprintf(ff,"\
                                                          Всього: %10.2f\n",itogoo);

fprintf(ff,"\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov,glavbuh);

fprintf(ff,"\n\
Відмітки банку:\n\
Дата приймання Зведеної відомості на паперовому носії та магнітному носії_____________\n\
Посада та призвище, ім'я та по батькові працівника Банку, який прийняв Зведену відомість\n\n\
_____________________________________________________\n\n\
_____________________________________________________\n\n\
підпис_______________________________________________\n");

iceb_podpis(ff,wpredok);

fclose(ff);
fclose(fftmp);

if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }

sprintf(imafzag,"ukrsoc%d.txt",getpid());
if((ff = fopen(imafzag,"w")) == NULL)
 {
  iceb_er_op_fil(imafzag,"",errno,wpredok);
  return;
 }

fprintf(ff,"@29244050010001,0,%.2f\n\
@302010,0,0,0.00\n",itogoo);

while(fgets(strsql,sizeof(strsql),fftmp) != NULL)
  fprintf(ff,"%s",strsql);
  
fclose(ff);
fclose(fftmp);
unlink(imaftmp);

class iceb_u_spisok imafo;
class iceb_u_spisok naimo;

imafo.plus(imaf);
naimo.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imafzag);
naimo.plus(gettext("Документ в электронном формате"));
iceb_ustpeh(imafo.ravno(0),1,wpredok);
iceb_rabfil(&imafo,&naimo,"",0,NULL);

}
/**************************************/
/*Для файлов в формате txt*/
/****************************************/

void pvbanks_txt(int kod_banka,iceb_u_spisok *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int dlpris,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
const char *kod00,
int kod_banka_tabl,
GtkWidget *wpredok)
{
char imafsp[32];
char imafdis[32];
double sum=0.;
char  fio[512];
char  bankshet[32];
char inn[15];
char tabnom[20];
double sumai=0.;
short d_pp=0,m_pp=0,g_pp=0; /*Дата платёжки*/
class iceb_u_str vidnahis("Заробітна плата та аванси");
int kolkontr=KONT->kolih();

char imaftmp[30];
FILE *ffr;
FILE *fftmp;

if(kod_banka == 7) /*Укрэксимбанк*/
  if(ukreksim_hapw(imafsp,imaftmp,&fftmp,&ffr,wpredok) != 0)
   return;

if(kod_banka == 8) /*хресщатик*/
 {
  for(int snom=0 ; snom < kolkontr; snom++)
   {
    if(pvbanks_kprovw(KONT,snom,pris,metkadk,&sum,tabnom,deb,kre,dlpris,fio,bankshet,inn,grupk,kod_banka_tabl,wpredok) != 0)
     continue;

    sumai+=sum;
   }
  if(zvb_hreshatik_startw(imafsp,imafdis,sumai,&d_pp,&m_pp,&g_pp,&fftmp,&ffr,wpredok) != 0)
   return;

 }
 
if(kod_banka == 9) /*ibank2ua*/
 if(zvb_ibank2ua_startw(imafsp,imafdis,&vidnahis,kod00,&fftmp,&ffr,wpredok) != 0)
  return;

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
int nomer=0;
sumai=0.;

for(int snom=0 ; snom < kolkontr; snom++)
 {
  memset(fio,'\0',sizeof(fio));
  memset(bankshet,'\0',sizeof(bankshet));
  memset(inn,'\0',sizeof(inn));
  memset(tabnom,'\0',sizeof(tabnom));

  if(pvbanks_kprovw(KONT,snom,pris,metkadk,&sum,tabnom,deb,kre,dlpris,fio,bankshet,inn,grupk,kod_banka_tabl,wpredok) != 0)
   continue;

  sumai+=sum;

  if(kod_banka == 7) /*Укрэксимбанк*/
    ukreksim_zstrw(fftmp,ffr,&nomer,sum,fio,bankshet,tabnom,dt,mt,gt);

  if(kod_banka == 8) /*хресщатик*/
    zvb_hreshatik_strw(d_pp,m_pp,g_pp,&nomer,fio,bankshet,inn,sum,fftmp,ffr);

  if(kod_banka == 9) /*ibank2ua*/
   zvb_ibank2ua_strw(&nomer,fio,bankshet,inn,sum,fftmp,ffr);

  if(dr != 0) /*нужно делать проводки*/
      pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio,sum,wpredok);

 }

if(kod_banka == 7) /*Укрэксимбанк*/
  ukreksim_endw(nomer,imaftmp,imafdis,sumai,rukov,glavbuh,fftmp,ffr,wpredok);

if(kod_banka == 8) /*хресщатик*/
 zvb_hreshatik_endw(sumai,nomer,ffr,fftmp,wpredok);

if(kod_banka == 9) /*ibank2ua*/
 zvb_ibank2ua_endw(sumai,vidnahis.ravno(),ffr,fftmp,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimo;

imafo.plus(imafsp);
naimo.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imafdis);
naimo.plus(gettext("Документ в электронном формате"));
iceb_ustpeh(imafo.ravno(0),1,wpredok);

iceb_rabfil(&imafo,&naimo,"",0,NULL);

}
/**************************************/
/*Для банков у которых електронный формат dbf*/
/****************************************/

void pbanks_dbf(int metka_banka, /*1-ukrsib 2-ukrgaz*/
iceb_u_spisok *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int dlpris,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
int kod_banka,
GtkWidget *wpredok)
{
char imafsp[30];
char imafdis[30];

int kolkontr=KONT->kolih();

char imaftmp[30];
FILE *ffr;
FILE *fftmp;
class iceb_u_str ope_tor("");

if(metka_banka == 1)
 ukrsib_startw(imafsp,imaftmp,&ope_tor,&fftmp,&ffr,wpredok);

if(metka_banka == 2)
  zvb_ukrgaz_startw(imafsp,imafdis,imaftmp,&fftmp,&ffr,wpredok);

double sum=0.;
char  fio[512];
char  bankshet[32];
char inn[15];
char tabnom[20];
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
int nomer=0;
double sumai=0.;

for(int snom=0 ; snom < kolkontr; snom++)
 {
  memset(fio,'\0',sizeof(fio));
  memset(bankshet,'\0',sizeof(bankshet));
  memset(inn,'\0',sizeof(inn));
  memset(tabnom,'\0',sizeof(tabnom));

  if(pvbanks_kprovw(KONT,snom,pris,metkadk,&sum,tabnom,deb,kre,dlpris,fio,bankshet,inn,grupk,kod_banka,wpredok) != 0)
   continue;

  sumai+=sum;
  if(metka_banka == 1)
    ukrsib_strw(dt,mt,gt,sum,bankshet,fio,&nomer,tabnom,ope_tor.ravno(),ffr,fftmp);
  if(metka_banka == 2)
    zvb_ukrgaz_strw(++nomer,fio,bankshet,sum,inn,tabnom,fftmp,ffr);

  if(dr != 0) /*нужно делать проводки*/
      pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio,sum,wpredok);
 }

if(metka_banka == 1)
  ukrsib_endw(imafdis,imaftmp,sumai,nomer,rukov,glavbuh,ffr,fftmp,wpredok);
if(metka_banka == 2)
  zvb_ukrgaz_endw(imafdis,imaftmp,sumai,nomer,ffr,fftmp,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimo;

imafo.plus(imafsp);
naimo.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imafdis);
naimo.plus(gettext("Документ в электронном формате"));

iceb_ustpeh(imafo.ravno(0),1,wpredok);

iceb_rabfil(&imafo,&naimo,"",0,NULL);

}

/******************************************/
/******************************************/

gint pvbanks_r1(class pvbanks_r_data *data)
{
char strsql[512];
int kolstr;
class SQLCURSOR cur;
SQL_str row;
//printf("pvbanks_r1 data->prohod=%d %d %f \n",data->prohod,data->kolstr,data->kolstr1);


int kolshet=iceb_u_pole2(data->rek_r->shet.ravno(),',');

//Составляем список всех контрагентов
if(kolshet == 0)
  sprintf(strsql,"select ns,kodkon from Skontr where ns='%s'",data->rek_r->shet.ravno());
else
  sprintf(strsql,"select ns,kodkon from Skontr");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kolkontr=0;
class iceb_u_spisok KONT;
//kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rek_r->shet.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[1],0,0) != 0)
    continue;
  if(KONT.find_r(row[1]) < 0)
   {
    kolkontr++;
    KONT.plus(row[1]);
   }
 }
double deb[kolkontr];
double kre[kolkontr];
memset(&deb,'\0',sizeof(deb));
memset(&kre,'\0',sizeof(kre));
short dk,mk,gk;
iceb_u_rsdat(&dk,&mk,&gk,data->rek_r->datan.ravno(),1);

short       gods=startgodb;
if(gods == 0)
 gods=gk;

//Читаем сальдо
if(kolshet == 0)
  sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk=1 and gs=%d and ns='%s'",gods,data->rek_r->shet.ravno());
else
  sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk=1 and gs=%d",gods);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int nomer=0;
//kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  
//  strzag(LINES-1,0,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rek_r->shet.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[1],0,0) != 0)
    continue;
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);

  nomer=KONT.find_r(row[1]);
  deb[nomer]+=atof(row[2]);
  kre[nomer]+=atof(row[3]);
 }
//Читаем все проводки
if(kolshet == 0)
  sprintf(strsql,"select sh,kodkon,deb,kre from Prov where datp >= '%d-%d-%d' \
and datp <= '%d-%d-%d' and kodkon <> '' and sh='%s'",gods,1,1,gk,mk,dk,data->rek_r->shet.ravno());
else
  sprintf(strsql,"select sh,kodkon,deb,kre from Prov where datp >= '%d-%d-%d' \
and datp <= '%d-%d-%d' and kodkon <> ''",gods,1,1,gk,mk,dk);

//printw("%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
//kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rek_r->shet.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[1],0,0) != 0)
    continue;

  nomer=KONT.find_r(row[1]);
  deb[nomer]+=atof(row[2]);
  kre[nomer]+=atof(row[3]);
 }

char rukov[512];
char glavbuh[40];
int dlpris;
char  pris[60];
SQL_str row1;
SQLCURSOR cur1;

memset(rukov,'\0',sizeof(rukov));
memset(glavbuh,'\0',sizeof(glavbuh));

iceb_poldan("Приставка к табельному номеру",pris,"zarnast.alx",data->window);
dlpris=strlen(pris);
char bros[512];
iceb_poldan("Табельный номер руководителя",bros,"zarnast.alx",data->window);
if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(rukov,row1[0],sizeof(rukov)-1);
 } 

iceb_poldan("Табельный номер бухгалтера",bros,"zarnast.alx",data->window);
if(bros[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    strncpy(glavbuh,row1[0],sizeof(glavbuh)-1);
 }

short dr,mr,gr;
iceb_u_rsdat(&dr,&mr,&gr,data->rek_r->datap.ravno(),1);
switch(data->rek_r->kod_banka)
 {
  case 1: /*Проминвестбанк*/
    pvbavks_pib(&KONT,data->rek_r->metka_r,deb,kre,glavbuh,rukov,dlpris,pris,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  case 3: /*Укрсоцбанк*/
    pvbavks_uks(&KONT,data->rek_r->metka_r,deb,kre,glavbuh,rukov,dlpris,pris,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  case 4: /*Укрсиббанк*/
    pbanks_dbf(1,&KONT,data->rek_r->metka_r,deb,kre,glavbuh,rukov,dlpris,pris,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  case 7: /*Укрэксимбанк*/
    pvbanks_txt(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh,rukov,dlpris,pris,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod00.ravno(),data->rek_r->kod_banka_tabl,data->window);
    break;

  case 8: /*хресщатик*/
    pvbanks_txt(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh,rukov,dlpris,pris,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod00.ravno(),data->rek_r->kod_banka_tabl,data->window);
    break;

  case 9: /*ibank2ua*/
    pvbanks_txt(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh,rukov,dlpris,pris,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod00.ravno(),data->rek_r->kod_banka_tabl,data->window);
    break;

  case 11: /*Укргазбанк*/
    pbanks_dbf(2,&KONT,data->rek_r->metka_r,deb,kre,glavbuh,rukov,dlpris,pris,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  default:
   iceb_menu_soob("Для этого банка режим не реализован!",data->window);
   break;  
 }

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
return(FALSE);

}
