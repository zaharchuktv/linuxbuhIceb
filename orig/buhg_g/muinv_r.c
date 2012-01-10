/*$Id: muinv_r.c,v 1.17 2011-08-29 07:13:43 sasa Exp $*/
/*11.01.2010	21.11.2004	Белых А.И.	muinv_r.c
Расчёт инвентаризации в материальном учёте
*/
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "buhg_g.h"
#include "muinv.h"

class muinv_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class muinv_data *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;

  muinv_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   muinv_r_key_press(GtkWidget *widget,GdkEventKey *event,class muinv_r_data *data);
gint muinv_r1(class muinv_r_data *data);
void  muinv_r_v_knopka(GtkWidget *widget,class muinv_r_data *data);

extern SQL_baza bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern double	okrg1;  /*Округление 1*/
extern double	okrcn;  /*Округление цены*/
extern iceb_u_str   shrift_ravnohir;

int muinv_r(class muinv_data *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class muinv_r_data data;

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Распечатать инвентаризационные ведомости"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(muinv_r_key_press),&data);

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

repl.plus(gettext("Распечатать инвентаризационные ведомости"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(muinv_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)muinv_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  muinv_r_v_knopka(GtkWidget *widget,class muinv_r_data *data)
{
//printf("muinv_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   muinv_r_key_press(GtkWidget *widget,GdkEventKey *event,class muinv_r_data *data)
{
// printf("muinv_r_key_press\n");
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

gint muinv_r1(class muinv_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);


int kolstr=0;
sprintf(strsql,"select sklad,nomk,kodm,ei,shetu,cena,innom,datv,rnd from Kart order by sklad,shetu asc");
SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной карточки !"),data->window);

  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char imaf[32];
FILE *ff,*fftmp;
char dopstr[1024];
char innom[32];
char regnom[32];
double cena=0.;
double summa=0.;
double ostatok=0.;
SQL_str row,row1;
int kgrm=0;
int nk=0;
char ei[32];
short  dv,mv,gv;
int kolstriv=0,kollistiv=0;
int nomerstr=0;
char sklad[32];
char shetu[32];
double  kolihsklad=0.;
double  sumasklad=0.;
double  kolihshet=0.;
double  sumashet=0.;
char matot[512];
char mesqc[32];
char naish[512];
char artikul[40];
char imaftmp[32];
short mos,dos,gos;
char naim[512];

memset(matot,'\0',sizeof(matot));
memset(sklad,'\0',sizeof(sklad));
memset(shetu,'\0',sizeof(shetu));

iceb_u_rsdat(&dos,&mos,&gos,data->rk->datao.ravno(),1);
memset(mesqc,'\0',sizeof(mesqc));
iceb_mesc(mos,1,mesqc);



sprintf(imaftmp,"iv%d.tmp",getpid());

if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    

class ostatok ost;

float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
    continue;
/******************
  if(kodm[0] != '\0')
   {
    if(kodm[0] == '*' && maskmat != NULL)
     {
      if(iceb_u_proverka(maskmat,row[2],0,0) != 0)
        continue;
     }
    else
      if(iceb_u_proverka(kodm,row[2],0,0) != 0)
        continue;
   }
*****************/
  if(iceb_u_proverka(data->rk->kodmat.ravno(),row[2],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
    continue;

  if(data->rk->metkainnom == 1)
   if(row[6][0] == '\0')
     continue;

  memset(naim,'\0',sizeof(naim));
  memset(artikul,'\0',sizeof(artikul));
  kgrm=0;
  /*Узнаем наименование материалла*/
  sprintf(strsql,"select * from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("Не нашли наименование материалла по коду %s\n",row[2]);
   }
  else
   {
    strncpy(naim,row1[2],sizeof(naim)-1);
    kgrm=atoi(row1[1]);
    strncpy(artikul,row1[14],sizeof(artikul)-1);
   }

  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[1],0,0) != 0)
    continue;


  nk=atoi(row[1]);

  memset(ei,'\0',sizeof(ei));
  strncpy(ei,row[3],sizeof(ei)-1);
  ostkarw(1,1,gos,dos,mos,gos,atoi(row[0]),nk,&ost);

  if(ost.ostg[3] < -0.0000001)
   {

    sprintf(strsql,"%s !!!",gettext("Внимание"));
    repl.new_plus(strsql);
   
    sprintf(strsql,"%s:%s %s:%d",gettext("Склад"),
    row[0],gettext("Карточка"),nk);
    repl.ps_plus(strsql);
    
    repl.ps_plus(gettext("Отрицательный остаток"));
    iceb_menu_soob(&repl,data->window);
   }

  if(fabs(ost.ostg[3]) < 0.0000001 )
    continue;   
  
  fprintf(fftmp,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%10.10g|\n",
  row[0],row[4],naim,row[1],row[2],row[3],row[5],row[6],row[7],row[8],artikul,ost.ostg[3]);
//  1      2     3    4       5      6      7      8      9      10     11

//  sklad,nomk,kodm,ei,shetu,cena,innom,datv,rnd  
//    0    1    2   3    4     5    6    7    8 

 }
fclose(fftmp);

//Сортировка предпринята для того чтобы отсортировать и по наименованию
//sprintf(strsql,"sort -o %s -t\\|  +0n -1 +1n -2 +2 -3 %s",imaftmp,imaftmp);
sprintf(strsql,"sort -o %s -t\\|  -k1,2n -k2,3n -k3,4 %s",imaftmp,imaftmp);
system(strsql);


if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    
unlink(imaftmp);

sprintf(imaf,"iv%d.lst",getpid());
data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Распечатка инвентаризационной ведомости"));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    

FILE *ffipsh;
char imafipsh[50];
sprintf(imafipsh,"ivs%d.lst",getpid());

data->rk->imaf.plus(imafipsh);
data->rk->naimf.plus(gettext("Дополнение 12"));

if((ffipsh = fopen(imafipsh,"w")) == NULL)
 {
  iceb_er_op_fil(imafipsh,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

in_d12_startw(ffipsh);

class iceb_u_spisok sp_shet;
class iceb_u_double sum_shet;
class iceb_u_double kol_shet;

char stroka[1024];
char skladf[20];
char shetuf[20];
char cenaf[20];
char datvf[11];
int nomer_sheta=0;

while(fgets(stroka,sizeof(stroka),fftmp) != NULL)
 {

  iceb_u_polen(stroka,skladf,sizeof(skladf),1,'|');
  iceb_u_polen(stroka,shetuf,sizeof(shetuf),2,'|');
  iceb_u_polen(stroka,naim,sizeof(naim),3,'|');
  iceb_u_polen(stroka,ei,sizeof(ei),6,'|');

  iceb_u_polen(stroka,cenaf,sizeof(cenaf),7,'|');
  iceb_u_polen(stroka,innom,sizeof(innom),8,'|');
  iceb_u_polen(stroka,datvf,sizeof(datvf),9,'|');
  iceb_u_polen(stroka,regnom,sizeof(regnom),10,'|');
  iceb_u_polen(stroka,artikul,sizeof(artikul),11,'|');
  iceb_u_polen(stroka,strsql,sizeof(strsql),12,'|');
  ostatok=atof(strsql);
  
  if(iceb_u_SRAV(sklad,skladf,0) != 0)
   {
    
    if(sklad[0] != '\0')
     {
      if((nomer_sheta=sp_shet.find(shetu)) < 0)
       sp_shet.plus(shetu);

      sum_shet.plus(sumashet,nomer_sheta);      
      kol_shet.plus(kolihshet,nomer_sheta);      

      sapkk1ivw(&kolstriv,&kollistiv,ff);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1ivw(&kolstriv,&kollistiv,ff);
      fprintf(ff,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по счету")),gettext("Итого по счету"),
      kolihshet,sumashet);

      sprintf(strsql,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(20,gettext("Итого по счету")),gettext("Итого по счету"),
      kolihshet,sumashet);
      
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

      sapkk1ivw(&kolstriv,&kollistiv,ff);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1ivw(&kolstriv,&kollistiv,ff);
      fprintf(ff,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по складу")),gettext("Итого по складу"),
      kolihsklad,sumasklad);

      sprintf(strsql,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(20,gettext("Итого по складу")),gettext("Итого по складу"),
      kolihsklad,sumasklad);
      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

      koncinvw(mesqc,gos,ff);

      fprintf(ff,"\f");
     }

    memset(matot,'\0',sizeof(matot));
    sprintf(strsql,"select fmol from Sklad where kod=%s",
    skladf);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      repl.new_plus(gettext("Не найдено код складу"));
      repl.plus(" ");
      repl.plus(skladf);
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);      
     }
    else
     strncpy(matot,row1[0],sizeof(matot)-1);

     
    kolstriv=0;
    sapvinw(matot,mesqc,gos,gos,&kolstriv,&kollistiv,ff);

    kolihsklad=sumasklad=0.;
    strcpy(sklad,skladf);
    memset(shetu,'\0',sizeof(shetu));
   }

  if(iceb_u_SRAV(shetu,shetuf,0) != 0)
   {
    if(shetu[0] != '\0')
     {
      if((nomer_sheta=sp_shet.find(shetu)) < 0)
       sp_shet.plus(shetu);
      sum_shet.plus(sumashet,nomer_sheta);      
      kol_shet.plus(kolihshet,nomer_sheta);      

      sapkk1ivw(&kolstriv,&kollistiv,ff);

      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1ivw(&kolstriv,&kollistiv,ff);
      fprintf(ff,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по счету")),gettext("Итого по счету"),
      kolihshet,sumashet);

      sprintf(strsql,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(20,gettext("Итого по счету")),gettext("Итого по счету"),
      kolihshet,sumashet);

      iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

     }
    memset(naish,'\0',sizeof(naish));
    //Читаем наименование счета
    sprintf(strsql,"select nais from Plansh where ns='%s'",shetuf);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      strncpy(naish,row1[0],sizeof(naish)-1);
                  
    sapkk1ivw(&kolstriv,&kollistiv,ff);
    fprintf(ff,"\n");
    sapkk1ivw(&kolstriv,&kollistiv,ff);
    fprintf(ff,"%s: %s %s\n",gettext("Счет"),shetuf,naish);

    sprintf(strsql,"%s: %s %s\n",gettext("Счет"),shetuf,naish);
    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    kolihshet=sumashet=0.;
    strcpy(shetu,shetuf);
   }

  cena=atof(cenaf);
  cena=iceb_u_okrug(cena,okrcn);
  
  summa=ostatok*cena;
  summa=iceb_u_okrug(summa,okrg1);
  
  kolihsklad+=ostatok;
  sumasklad+=summa;
  kolihshet+=ostatok;
  sumashet+=summa;
  
  
  iceb_u_rsdat(&dv,&mv,&gv,datvf,2);

  memset(dopstr,'\0',sizeof(dopstr));
  
  if(innom[0] != '\0')
   {
    if( regnom[0] == '\0')
     {
      sprintf(dopstr,"%s:%s",gettext("Инвентарный номер"),innom);
      if(dv != 0)
       {
        memset(strsql,'\0',sizeof(strsql));
        sprintf(strsql," %s:%d.%d.%d%s",
        gettext("Дата ввода в эксплуатацию"),
        dv,mv,gv,gettext("г."));
        strcat(dopstr,strsql);
       }
     }
    else
     {
      sprintf(dopstr,"%s:%s",gettext("Регистрационный номер"),regnom);
      memset(strsql,'\0',sizeof(strsql));
      sprintf(strsql," %s:%s",gettext("Серия"),innom);
      strcat(dopstr,strsql);
      if(dv != 0)
       {
        memset(strsql,'\0',sizeof(strsql));
        sprintf(strsql," %s:%d.%d.%d%s",
        gettext("Дата регистрации"),
        dv,mv,gv,gettext("г."));
        strcat(dopstr,strsql);
       }
     }
   }
  if(artikul[0] != '\0')
   {
    sprintf(strsql," %s:%s",gettext("Артикул"),artikul);
    strcat(dopstr,strsql);
   }

  sapkk1ivw(&kolstriv,&kollistiv,ff);
  
  fprintf(ff,"%3d|%-*.*s|%-*s|%10.10g|%10.10g|%10.2f|%10s|%10s|%10s|%10s|\n",
  ++nomerstr,
  iceb_u_kolbait(40,naim),iceb_u_kolbait(40,naim),naim,
  iceb_u_kolbait(6,ei),ei,
  cena,ostatok,summa," "," "," "," ");

  if(dopstr[0] != '\0')
   {
    sapkk1ivw(&kolstriv,&kollistiv,ff);
    fprintf(ff,"%s\n",dopstr);
   }
  sapkk1ivw(&kolstriv,&kollistiv,ff);
  fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");


 }
fclose(fftmp);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%-*s %10.10g %10.2f\n",
iceb_u_kolbait(62,gettext("Итого по счету")),gettext("Итого по счету"),
kolihshet,sumashet);

sprintf(strsql,"%-*s %10.10g %10.2f\n",
iceb_u_kolbait(20,gettext("Итого по счету")),gettext("Итого по счету"),
kolihshet,sumashet);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%-*s %10.10g %10.2f\n",
iceb_u_kolbait(62,gettext("Итого по складу")),gettext("Итого по складу"),
kolihsklad,sumasklad);


sprintf(strsql,"%-*s %10.10g %10.2f\n",
iceb_u_kolbait(20,gettext("Итого по складу")),gettext("Итого по складу"),
kolihsklad,sumasklad);
iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

/*Распечатка формы додаток 12*/
in_d12_itw(&sp_shet,&kol_shet,&sum_shet,ffipsh,data->window);

koncinvw(mesqc,gos,ff);



fclose(ff);

iceb_insfil("in_d12_end.alx",ffipsh,data->window);

iceb_podpis(ffipsh,data->window);
fclose(ffipsh);

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
