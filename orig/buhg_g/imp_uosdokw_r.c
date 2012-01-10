/*$Id: imp_uosdokw_r.c,v 1.8 2011-02-21 07:35:52 sasa Exp $*/
/*20.12.2007	20.12.2007	Белых А.И.	imp_uosdokw_r.c
импорт документов в подсистему "Учёт основных средств"
*/
#include  <stdlib.h>
#include  <math.h>
#include  <errno.h>
#include  <unistd.h>
#include        <sys/stat.h>
#include "buhg_g.h"

class imp_uosdokw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
    
  class iceb_u_str imafz;
        
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчет завершен 1-нет
  imp_uosdokw_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_uosdokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_uosdokw_r_data *data);
gint imp_uosdokw_r1(class imp_uosdokw_r_data *data);
void  imp_uosdokw_r_v_knopka(GtkWidget *widget,class imp_uosdokw_r_data *data);

int imp_kasord_zag(int metka,int razmer_fil,class iceb_u_file *ff,class imp_uosdokw_r_data*);

extern SQL_baza bd;
extern char *name_system;
extern double okrcn;
extern iceb_u_str shrift_ravnohir;

int imp_uosdokw_r(const char *imaf,
GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class imp_uosdokw_r_data data;

data.imafz.new_plus(imaf);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
sprintf(strsql,"%s %s",name_system,gettext("Импорт кассовых ордеров"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(imp_uosdokw_r_key_press),&data);

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

GtkWidget *label=NULL;
label=gtk_label_new(gettext("Импорт кассовых ордеров"));
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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(imp_uosdokw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)imp_uosdokw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_uosdokw_r_v_knopka(GtkWidget *widget,class imp_uosdokw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   imp_uosdokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_uosdokw_r_data *data)
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

gint imp_uosdokw_r1(class imp_uosdokw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_clock sss(data->window);
struct stat work;

if(stat(data->imafz.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Нет найден файл"),data->imafz.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

class iceb_u_file fil;

if((fil.ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolih_oh=0;
if((kolih_oh=imp_kasord_zag(0,work.st_size,&fil,data)) == 0)
 {
  rewind(fil.ff);
  imp_kasord_zag(1,work.st_size,&fil,data);
 }
else
 {
   sprintf(strsql,"%s %d !\n%s",gettext("Количество ошибок"),kolih_oh,
   gettext("Загрузка невозможна !"));
  iceb_menu_soob(strsql,data->window);
 }
fil.close();





data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);



//repl.new_plus(gettext("Загрузка завершена"));
//repl.ps_plus(gettext("Количество документов"));
//repl.plus(":");
//repl.plus(koldok);

//iceb_menu_soob(&repl,data->window);


data->voz=0;

return(FALSE);
}
/*************************/
/*Импорт*/
/**************************/

int imp_kasord_zag(int metka, //0-проверка на ошибки 1- загрузка
int razmer_fil,
class iceb_u_file *fil,class imp_uosdokw_r_data *data)
{
char strsql[512];
if(metka == 0)
 sprintf(strsql,"%s\n",gettext("Проверка на наличие ошибок"));
if(metka == 1)
 sprintf(strsql,"\n\n%s\n",gettext("Загружаем документы из файла"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

int		koloh=0;

char		strok[1024];
short		dd,md,gd;
int		tipz=0;
char		kodop[32];
char		kontr[32];
class iceb_u_str nomdok("");
char		podr[32];
char		motv[32];
time_t		vrem;
char		innom[32];
double		bsnu=0.;
double		iznu=0.;
double		bsbu=0.;
double		izbu=0.;
char		shetu[32];
struct OPSHET	shetv;
char		hzatrat[32];
char		hanuh[32];
char		grupnu[32];
char		grupbu[32];
double		popkfnu=0.;
double		popkfbu=0.;
char		sost[32];
char		naim[512];
short		dv,mv,gv;
int		kol=0;
int		koldok=0;
int		i=0,razmer=0;
double dpnds=0.; /*Действующий на момент создания документа процент НДС*/
time(&vrem);

while(fgets(strok,sizeof(strok),fil->ff) != NULL)
 {
  i=strlen(strok);
  razmer+=i;
  if(strok[i-2] == '\r') //Возврат каретки меняем на пробел
   strok[i-2]=' ';

  iceb_printw(iceb_u_toutf(strok),data->buffer,data->view);

//  printf("%s",strok);

/**************************************/
  if(iceb_u_SRAV(strok,"DOC|",1) == 0)
   {
    nomdok.new_plus("");
    dd=md=gd=0;
    tipz=0;
    kol=0;
    
    iceb_u_polen(strok,strsql,sizeof(strsql),2,'|');
    if(iceb_u_rsdat(&dd,&md,&gd,strsql,1) != 0)
     {
      iceb_menu_soob(gettext("Не верно введена дата!"),data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,strsql,sizeof(strsql),3,'|');

    tipz=0;
    if(strsql[0] == '+')
     {
      tipz=1;
      kol=1;
     }
    if(strsql[0] == '-')
     {
      tipz=2;
      kol=-1;
     }
    if(tipz == 0)
     {
      iceb_menu_soob(gettext("Не определён вид документа (приход/расход)!"),data->window);
      koloh++;
      continue;
     }
    
    iceb_u_polen(strok,kodop,sizeof(kodop),4,'|');

    if(tipz == 1)
      sprintf(strsql,"select kod from Uospri where kod='%s'",kodop);
    if(tipz == 2)
      sprintf(strsql,"select kod from Uosras where kod='%s'",kodop);

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код операции"),kodop);
      
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,kontr,sizeof(kontr),5,'|');
    sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",
    kontr);
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr);
      
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,podr,sizeof(podr),7,'|');
    sprintf(strsql,"select kod from Uospod where kod=%s",
    podr);
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код подразделения"),podr);
      
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,motv,sizeof(motv),8,'|');
    sprintf(strsql,"select kod from Uosol where kod=%s",motv);
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден мат. ответственный"),motv);
      
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }
   
    iceb_u_polen(strok,&dpnds,9,'|');

    //Номер документа определяем последним так как он является индикатором
    //наличия шапки документа
    iceb_u_polen(strok,&nomdok,6,'|');
    if(iceb_u_SRAV(nomdok.ravno(),"авто",0) == 0)
      uosgetnd(gd,tipz,&nomdok,data->window);
    
    
    sprintf(strsql,"select nomd from Uosdok where tipz=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomd='%s'",
    tipz,gd,gd,nomdok.ravno());
    if(iceb_sql_readkey(strsql,data->window) >= 1)
     {
      sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdok.ravno());
      
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    //Запись в базу
    if(metka != 0 && nomdok.getdlinna() > 1 && koloh == 0)
     {
//      printw("Пишем\n");
//      refresh();
      iceb_printw(iceb_u_toutf("Запись шапки\n"),data->buffer,data->view);

      sprintf(strsql,"insert into Uosdok (datd,tipz,kodop,kontr,nomd,podr,kodol,prov,ktoz,vrem,pn) \
values ('%04d-%02d-%02d',%d,'%s','%s','%s',%s,%s,%s,%d,%ld,%.10g)",
      gd,md,dd,tipz,kodop,kontr,nomdok.ravno(),podr,motv,"1",iceb_getuid(data->window),vrem,dpnds);

      if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
       koloh++;
      else
       koldok++;

     }        
     

   }

/**************************************/
  if(iceb_u_SRAV(strok,"ZP1|",1) == 0)
   {
    iceb_u_polen(strok,innom,sizeof(innom),2,'|');
    sprintf(strsql,"select innom from Uosin where innom=%s",innom);
    if(iceb_sql_readkey(strsql,data->window) >= 1)
     {
      if(tipz == 1)
       {
        sprintf(strsql,"%s %s",innom,gettext("Такой инвентарный номер уже есть !"));
        iceb_menu_soob(strsql,data->window);
        koloh++;
        continue;
       }
     }
    else
     if(tipz == 2)
      {
        sprintf(strsql,"%s %s",gettext("Не найден инвентарный номер"),innom);
        iceb_menu_soob(strsql,data->window);
        koloh++;
        continue;

      }

    iceb_u_polen(strok,strsql,sizeof(strsql),3,'|');
    bsnu=iceb_u_atof(strsql);

    iceb_u_polen(strok,strsql,sizeof(strsql),4,'|');
    iznu=iceb_u_atof(strsql);

    iceb_u_polen(strok,strsql,sizeof(strsql),5,'|');
    bsbu=iceb_u_atof(strsql);

    iceb_u_polen(strok,strsql,sizeof(strsql),6,'|');
    izbu=iceb_u_atof(strsql);

    iceb_u_polen(strok,shetu,sizeof(shetu),7,'|');
    if(iceb_prsh1(shetu,&shetv,data->window) != 0)
     {
      koloh++;
      continue;
     }

    iceb_u_polen(strok,hzatrat,sizeof(hzatrat),8,'|');

    sprintf(strsql,"select kod from Uoshz where kod='%s'",hzatrat);

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s",innom,gettext("Не найден шифр аморт-отчислений"),hzatrat);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,hanuh,sizeof(hanuh),9,'|');

    sprintf(strsql,"select kod from Uoshau where kod='%s'",hanuh);

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s",innom,gettext("Не найден шифр аналитаческого учета"),hanuh);
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,grupnu,sizeof(grupnu),10,'|');

    sprintf(strsql,"select kod from Uosgrup where kod='%s'",grupnu);

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s\n%s",innom,gettext("Не найден код группы"),grupnu,gettext("Налоговый учёт"));
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,strsql,sizeof(strsql),11,'|');
    popkfnu=iceb_u_atof(strsql);
    
    iceb_u_polen(strok,grupbu,sizeof(grupbu),12,'|');

    sprintf(strsql,"select kod from Uosgrup1 where kod='%s'",grupbu);

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s\n%s",innom,gettext("Не найден код группы"),grupbu,gettext("Бухгалтерский учёт"));
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok,strsql,sizeof(strsql),13,'|');
    popkfbu=iceb_u_atof(strsql);

    iceb_u_polen(strok,sost,sizeof(sost),14,'|');

    iceb_u_polen(strok,naim,sizeof(naim),15,'|');
    sqlfiltr(naim,sizeof(naim));
    
    iceb_u_polen(strok,strsql,sizeof(strsql),16,'|');
    iceb_u_rsdat(&dv,&mv,&gv,strsql,1);

    //Запись в базу
    if(metka != 0 && nomdok.getdlinna() > 1 && innom[0] != '\0' && koloh == 0)
     {
      if(tipz == 1)
       {
        iceb_printw(iceb_u_toutf("Запись инвентарного номера\n"),data->buffer,data->view);

        sprintf(strsql,"insert into Uosin \
values (%s,%s,'%s','%s','%s','%s','%s','%04d-%d-%d',%d,%ld)",
        innom,"0",naim,"","","","",gv,mv,dv,iceb_getuid(data->window),vrem);

        if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
          continue;
        }      

      sprintf(strsql,"insert into Uosdok1 \
values ('%04d-%02d-%02d',%d,%s,%s,'%s',%s,%s,%d,%.2f,%.2f,%.2f,'%s',%d,%ld,\
%.2f,%.2f,%.2f,'%s')",
      gd,md,dd,tipz,"0",innom,nomdok.ravno(),podr,motv,kol,bsnu,iznu,1.,kodop,iceb_getuid(data->window),vrem,\
      bsbu,izbu,0.,"");

      if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
       continue;

      if(tipz == 1)
       { 
        sprintf(strsql,"insert into Uosinp \
values (%s,%d,%d,'%s','%s','%s','%s',%.2f,%s,'%s',%d,%ld,'%s',%.2f)",
        innom,md,gd,shetu,hzatrat,hanuh,grupnu,popkfnu,sost,"",iceb_getuid(data->window),vrem,\
        grupbu,popkfbu);

      
        iceb_sql_zapis(strsql,1,0,data->window);
       }
     }
   }

 }




return(koloh);
}
