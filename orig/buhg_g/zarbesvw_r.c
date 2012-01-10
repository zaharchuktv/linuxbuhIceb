/*$Id: zarbesvw_r.c,v 1.3 2011-05-06 08:37:47 sasa Exp $*/
/*06.05.2011	10.03.2006	Белых А.И.	zarbesvw_r.c
Расчёт отчёта 
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "zarbesvw.h"
#define		KOLSTRHAP 9 //Количество строк в шапке

class zarbesvw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class zarbesvw_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  zarbesvw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zarbesvw_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarbesvw_r_data *data);
gint zarbesvw_r1(class zarbesvw_r_data *data);
void  zarbesvw_r_v_knopka(GtkWidget *widget,class zarbesvw_r_data *data);


extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;
extern int kod_esv;
extern int kod_esv_bol;
extern int kod_esv_inv;
extern int kod_esv_dog;
//extern float  max_sum_for_soc; //Максимальная сумма с которой начисляются соц.отчисления
extern char *shetb; /*Бюджетные счета начислений*/
extern char *shetbu; /*Бюджетные счета удержаний*/
extern short *kodbl;  /*Код начисления больничного*/

extern class iceb_u_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/

int zarbesvw_r(class zarbesvw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class zarbesvw_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатка баз налогообложения для единого соц. взноса"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(zarbesvw_r_key_press),&data);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(zarbesvw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)zarbesvw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zarbesvw_r_v_knopka(GtkWidget *widget,class zarbesvw_r_data *data)
{
//printf("zarbesvw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zarbesvw_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarbesvw_r_data *data)
{
// printf("zarbesvw_r_key_press\n");
switch(event->keyval)
 {
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka),"clicked");
    break;
 }
return(TRUE);
}

/***********************************/
/*Распечатка*/
/**************************/
int zarbesv_r(int metka_r,
short mr,short gr,
double itogo_nah,
double itogo_suma_nvr,
double suma_prev,
double itogo_suma,
double itogo_suma_p,
double suma_esv_ob,
double itogo_suma_inv,
double itogo_suma_inv_p,
double suma_esv_inv,
double itogo_suma_dog,
double itogo_suma_dog_p,
double suma_esv_dog,
double itogo_suma_bol,
double suma_esv_bol,
float max_sum_for_soc,
const char *imaf,
GtkWidget *wpredok)
{
char strsql[512];
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }


if(metka_r == 0)
 iceb_u_zagolov(gettext("Расчет баз налогообложения для удержания единого социального взноса"),0,mr,gr,0,0,0,organ,ff);

if(metka_r == 1)
 {
  sprintf(strsql,"%s %s",gettext("Расчет баз налогообложения для удержания единого социального взноса"),gettext("Хозрасчёт"));
  iceb_u_zagolov(strsql,0,mr,gr,0,0,0,organ,ff);
 }
if(metka_r == 2)
 {
  sprintf(strsql,"%s %s",gettext("Расчет баз налогообложения для удержания единого социального взноса"),gettext("Бюджет"));
  iceb_u_zagolov(strsql,0,mr,gr,0,0,0,organ,ff);
 }
fprintf(ff,"\
%s:%d\n\
%s:%d\n\
%s:%d\n\
%s:%d\n\
%s:%s\n",
gettext("Код удержания единого социального взноса"),kod_esv,
gettext("Код удержания единого социального взноса для больничных"),kod_esv_bol,
gettext("Код удержания единого социального взноса для инвалидов"),kod_esv_inv,
gettext("Код удержания единого социального взноса для договоров"),kod_esv_dog,
gettext("Коды начислений не входящие в расчёт удержания единого социального взноса"),knvr_esv_r.ravno());


sprintf(strsql,"%s %.2f",gettext("Cумма больще"),max_sum_for_soc);

fprintf(ff,"\n\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n",
iceb_u_kolbait(30,gettext("Начислено")),gettext("Начислено"),itogo_nah,
iceb_u_kolbait(30,gettext("Не вошло в расчёт")),gettext("Не вошло в расчёт"),itogo_suma_nvr,
iceb_u_kolbait(30,strsql),strsql,suma_prev,
iceb_u_kolbait(30,gettext("Сумма взятая в расчёт")),gettext("Сумма взятая в расчёт"),itogo_nah-itogo_suma_nvr-suma_prev);

class zarrnesvp_rek esvp;

zarrnesvpw(mr,gr,&esvp,NULL,wpredok);
 
fprintf(ff,"\n%s %.2f%%\n\
----------------------------------------------------------------\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n",
gettext("Единый социальный взнос"),esvp.proc_esv,
iceb_u_kolbait(30,gettext("Начислено")),gettext("Начислено"),itogo_suma,
iceb_u_kolbait(30,gettext("Сумма превышения")),gettext("Сумма превышения"),itogo_suma_p,
iceb_u_kolbait(30,gettext("Взято в расчёт")),gettext("Взято в расчёт"),itogo_suma-itogo_suma_p,
iceb_u_kolbait(30,gettext("Сумма удержания")),gettext("Сумма удержания"),suma_esv_ob);

fprintf(ff,"\n%s %.2f%%\n\
----------------------------------------------------------------\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n",
gettext("Единый социальный взнос c инвалидов"),esvp.proc_esv_inv,
iceb_u_kolbait(30,gettext("Начислено")),gettext("Начислено"),itogo_suma_inv,
iceb_u_kolbait(30,gettext("Сумма превышения")),gettext("Сумма превышения"),itogo_suma_inv_p,
iceb_u_kolbait(30,gettext("Взято в расчёт")),gettext("Взято в расчёт"),itogo_suma_inv-itogo_suma_inv_p,
iceb_u_kolbait(30,gettext("Сумма удержания")),gettext("Сумма удержания"),suma_esv_inv);

fprintf(ff,"\n%s %.2f%%\n\
----------------------------------------------------------------\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n\
%*s:%10.2f\n",
gettext("Единый социальный взнос c работающих по договорам"),esvp.proc_esv_dog,
iceb_u_kolbait(30,gettext("Начислено")),gettext("Начислено"),itogo_suma_dog,
iceb_u_kolbait(30,gettext("Сумма превышения")),gettext("Сумма превышения"),itogo_suma_dog_p,
iceb_u_kolbait(30,gettext("Взято в расчёт")),gettext("Взято в расчёт"),itogo_suma_dog-itogo_suma_dog_p,
iceb_u_kolbait(30,gettext("Сумма удержания")),gettext("Сумма удержания"),suma_esv_dog);
fprintf(ff,"\n%s %.2f%%\n\
----------------------------------------------------------------\n\
%*s:%10.2f\n\
%*s:%10.2f\n",
gettext("Единый социальный взнос c больничных"),esvp.proc_esv_bol,
iceb_u_kolbait(30,gettext("Начислено")),gettext("Начислено"),itogo_suma_bol,
iceb_u_kolbait(30,gettext("Сумма удержания")),gettext("Сумма удержания"),suma_esv_bol);


iceb_podpis(ff,wpredok);
fclose(ff);
return(0);
}


/******************************************/
/******************************************/

gint zarbesvw_r1(class zarbesvw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);



short mr,gr;


if(iceb_u_rsdat1(&mr,&gr,data->rk->datan.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не верно введена дата !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class zar_read_tn1h nastr;
zar_read_tn1w(1,mr,gr,&nastr,NULL,data->window);

double itogo_nah[2]={0.,0.}; /*общая сумма всех начилений*/
double itogo_suma_nvr[2]={0.,0.}; /*сумма не вошедшая в расчёт*/
double itogo_suma[2]={0.,0.};
double itogo_suma_bol[2]={0.,0.};
double itogo_suma_inv[2]={0.,0.};
double itogo_suma_dog[2]={0.,0.};

double itogo_suma_p[2]={0.,0.}; /*Cумма превышения*/
double itogo_suma_inv_p[2]={0.,0.};
double itogo_suma_dog_p[2]={0.,0.};

double suma_esv_ob[2]={0.,0.};
double suma_esv_bol[2]={0.,0.};
double suma_esv_inv[2]={0.,0.};
double suma_esv_dog[2]={0.,0.};
double suma_zap=0.;
double suma_tab[2]={0.,0.};
double suma_prev[2]={0.,0.};
int metka_inv=0;
int metka_dog=0;
int kod_zap=0;


int kolstr=0,kolstr2=0;
float kolstr1=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d",gr,mr);

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

/*чтение величины минимальной зарплаты и прожиточного минимума*/
zar_read_tnw(1,mr,gr,NULL,data->window); 
/*Чтение настроек для расчёта единого социального взноса*/
zarrnesvw(NULL,data->window);


while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"select prn,knah,suma,shet,godn,mesn from Zarp \
where tabn=%s and datz >= '%04d-%d-01' and datz <= '%04d-%d-31' \
and suma <> 0.",row[0],gr,mr,gr,mr);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
   continue;
  
  memset(suma_tab,'\0',sizeof(suma_tab));
  memset(suma_prev,'\0',sizeof(suma_prev));
  metka_inv=metka_dog=0;  
  while(cur1.read_cursor(&row1) != 0)
   {
    kod_zap=atoi(row1[1]);
    suma_zap=atof(row1[2]);    

    if(atoi(row1[0]) == 1) /*Начисления*/
     {
      itogo_nah[0]+=suma_zap;
      if(iceb_u_proverka(shetb,row1[3],0,0) == 0)
        itogo_nah[1]+=suma_zap;
       
      if(iceb_u_proverka(knvr_esv_r.ravno(),row1[1],0,1) == 0)
       {
        itogo_suma_nvr[0]+=suma_zap;
        if(iceb_u_proverka(shetb,row1[3],0,0) == 0)
          itogo_suma_nvr[1]+=suma_zap;
        continue;
       }
  
      suma_tab[0]+=suma_zap;
      if(iceb_u_proverka(shetb,row1[3],0,0) == 0)
        suma_tab[1]+=suma_zap;

      if(provkodw(kodbl,kod_zap) >= 0)
       {
        itogo_suma_bol[0]+=suma_zap;
        if(iceb_u_proverka(shetb,row1[3],0,0) == 0)
          itogo_suma_bol[1]+=suma_zap;
        continue;
       }              

     if((metka_inv=zarprtnw(mr,gr,atoi(row[0]),"zarinv.alx",NULL,data->window)) == 1)
      {
       itogo_suma_inv[0]+=suma_zap;
       if(iceb_u_proverka(shetb,row1[3],0,0) == 0)
         itogo_suma_inv[1]+=suma_zap;
       continue;
      }

     if((metka_dog=zarprtnw(mr,gr,atoi(row[0]),"zardog.alx",NULL,data->window)) == 1)
      {
       itogo_suma_dog[0]+=suma_zap;
       if(iceb_u_proverka(shetb,row1[3],0,0) == 0)
         itogo_suma_dog[1]+=suma_zap;
       continue;
      }

     itogo_suma[0]+=suma_zap;
     if(iceb_u_proverka(shetb,row1[3],0,0) == 0)
       itogo_suma[1]+=suma_zap;

     }
    else /*удержания*/
     {
      if(kod_zap == kod_esv)
       suma_esv_ob[0]+=suma_zap;

      if(kod_zap == kod_esv_bol)
       suma_esv_bol[0]+=suma_zap;

      if(kod_zap == kod_esv_inv)
       suma_esv_inv[0]+=suma_zap;

      if(kod_zap == kod_esv_dog)
       suma_esv_dog[0]+=suma_zap;

      if(iceb_u_proverka(shetbu,row1[3],0,0) == 0)
       {
        if(kod_zap == kod_esv)
         suma_esv_ob[1]+=suma_zap;

        if(kod_zap == kod_esv_bol)
         suma_esv_bol[1]+=suma_zap;

        if(kod_zap == kod_esv_inv)
         suma_esv_inv[1]+=suma_zap;

        if(kod_zap == kod_esv_dog)
         suma_esv_dog[1]+=suma_zap;

       }
     }
     
   }

  if(suma_tab[0] > nastr.max_sum_for_soc)
   {
    suma_prev[0]+=suma_tab[0]-nastr.max_sum_for_soc;
    if(metka_inv == 1)
     itogo_suma_inv_p[0]+=suma_tab[0]-nastr.max_sum_for_soc;
    if(metka_dog == 1)
     itogo_suma_dog_p[0]+=suma_tab[0]-nastr.max_sum_for_soc;
    if(metka_dog == 0 && metka_inv == 0)
     itogo_suma_p[0]+=suma_tab[0]-nastr.max_sum_for_soc;
   }

  if(suma_tab[1] > nastr.max_sum_for_soc)
   {
    suma_prev[1]+=suma_tab[1]-nastr.max_sum_for_soc;
    if(metka_inv == 1)
     itogo_suma_inv_p[1]+=suma_tab[1]-nastr.max_sum_for_soc;
    if(metka_dog == 1)
     itogo_suma_dog_p[1]+=suma_tab[1]-nastr.max_sum_for_soc;
    if(metka_dog == 0 && metka_inv == 0)
     itogo_suma_p[1]+=suma_tab[1]-nastr.max_sum_for_soc;
   }
 }
/*Общая сумма*/
char imaf[64];
sprintf(imaf,"esv%d.lst",getpid());

zarbesv_r(0,mr,gr,itogo_nah[0],itogo_suma_nvr[0],suma_prev[0],
itogo_suma[0],itogo_suma_p[0],suma_esv_ob[0],
itogo_suma_inv[0],itogo_suma_inv_p[0],suma_esv_inv[0],
itogo_suma_dog[0],itogo_suma_dog_p[0],suma_esv_dog[0],
itogo_suma_bol[0],suma_esv_bol[0],nastr.max_sum_for_soc,imaf,data->window);

char imaf_hoz[64];
char imaf_bud[64];

if(shetb != NULL)
 {

  sprintf(imaf_hoz,"hesv%d.lst",getpid());
  zarbesv_r(1,mr,gr,itogo_nah[0]-itogo_nah[1],itogo_suma_nvr[0]-itogo_suma_nvr[1],suma_prev[0]-suma_prev[1],
  itogo_suma[0]-itogo_suma[1],itogo_suma_p[0]-itogo_suma_p[1],suma_esv_ob[0]-suma_esv_ob[1],
  itogo_suma_inv[0]-itogo_suma_inv[1],itogo_suma_inv_p[0]-itogo_suma_inv_p[1],suma_esv_inv[0]-suma_esv_inv[1],
  itogo_suma_dog[0]-itogo_suma_dog[1],itogo_suma_dog_p[0]-itogo_suma_dog_p[1],suma_esv_dog[0]-suma_esv_dog[1],
  itogo_suma_bol[0]-itogo_suma_bol[1],suma_esv_bol[0]-suma_esv_bol[1],nastr.max_sum_for_soc,imaf_hoz,data->window);

  sprintf(imaf_bud,"besv%d.lst",getpid());
  zarbesv_r(2,mr,gr,itogo_nah[1],itogo_suma_nvr[1],suma_prev[1],
  itogo_suma[1],itogo_suma_p[1],suma_esv_ob[1],
  itogo_suma_inv[1],itogo_suma_inv_p[1],suma_esv_inv[1],
  itogo_suma_dog[1],itogo_suma_dog_p[1],suma_esv_dog[1],
  itogo_suma_bol[1],suma_esv_bol[1],nastr.max_sum_for_soc,imaf_bud,data->window);
 }

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчет баз налогообложения для удержания единого социального взноса"));

if(shetb != NULL)
 {
  data->rk->imaf.plus(imaf_hoz);
  sprintf(strsql,"%s %s",gettext("Расчет баз налогообложения для удержания единого социального взноса"),gettext("Хозрасчёт"));
  data->rk->naimf.plus(strsql);

  data->rk->imaf.plus(imaf_bud);
  sprintf(strsql,"%s %s",gettext("Расчет баз налогообложения для удержания единого социального взноса"),gettext("Бюджет"));
  data->rk->naimf.plus(strsql);
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
