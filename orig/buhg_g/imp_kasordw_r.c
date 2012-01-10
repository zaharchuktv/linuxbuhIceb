/*$Id: imp_kasordw_r.c,v 1.18 2011-02-21 07:35:52 sasa Exp $*/
/*16.09.2010	13.04.2007	Белых А.И.	imp_kasordw_r.c
импорт кассовых ордеров
*/
#include  <stdlib.h>
#include  <math.h>
#include  <errno.h>
#include  <unistd.h>
#include        <sys/stat.h>
#include "buhg_g.h"

class imp_kasordw_r_data
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
  imp_kasordw_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_kasordw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_kasordw_r_data *data);
gint imp_kasordw_r1(class imp_kasordw_r_data *data);
void  imp_kasordw_r_v_knopka(GtkWidget *widget,class imp_kasordw_r_data *data);

int imp_kasord_zag(int metka,int razmer_fil,FILE *ff,class imp_kasordw_r_data*);

extern SQL_baza bd;
extern char *name_system;
extern double okrcn;
extern iceb_u_str shrift_ravnohir;

int imp_kasordw_r(const char *imaf,
GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class imp_kasordw_r_data data;

data.imafz.new_plus(imaf);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);
sprintf(strsql,"%s %s",name_system,gettext("Импорт кассовых ордеров"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(imp_kasordw_r_key_press),&data);

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
gtk_signal_connect(GTK_OBJECT(data.knopka),"clicked",GTK_SIGNAL_FUNC(imp_kasordw_r_v_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka),(gpointer)0);
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_idle_add((GtkFunction)imp_kasordw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_kasordw_r_v_knopka(GtkWidget *widget,class imp_kasordw_r_data *data)
{
if(data->kon_ras == 1)
 return;
gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   imp_kasordw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_kasordw_r_data *data)
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

gint imp_kasordw_r1(class imp_kasordw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_clock sss(data->window);
struct stat work;

if(stat(data->imafz.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Не найден файл"),data->imafz.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

FILE *ff=NULL;

if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolih_oh=0;
if((kolih_oh=imp_kasord_zag(0,work.st_size,ff,data)) == 0)
 {
  rewind(ff);
  imp_kasord_zag(1,work.st_size,ff,data);
  unlink(data->imafz.ravno());
 }
else
 {
   sprintf(strsql,"%s %d !\n%s",gettext("Количество ошибок"),kolih_oh,
   gettext("Загрузка невозможна !"));
  iceb_menu_soob(strsql,data->window);
 }
fclose(ff);





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
FILE *ff,class imp_kasordw_r_data *data)
{
char strsql[512];
if(metka == 0)
 sprintf(strsql,"%s\n",gettext("Проверка на наличие ошибок"));
if(metka == 1)
 sprintf(strsql,"\n\n%s\n",gettext("Загружаем документы из файла"));

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

char stroka[1024];
char kod_str[512];
char bros[512];
int kol_oh=0; //количество ошибок
int tipzap=0;
char tip[56];
char kassa[56];
short dd,md,gd;
class iceb_u_str nomdok("");
char shetd[56];
char shetk[56];
char kodop[56];
class iceb_u_str osnov("");
class iceb_u_str dopol("");
class iceb_u_str fio("");
class iceb_u_str dokum("");
class iceb_u_str nomer_bl("");
float razmer=0;
int i=0;
SQL_str row;
SQLCURSOR cur;
struct OPSHET rek_shet;
class iceb_u_str kontr("");
double suma=0.;
class iceb_u_str fam("");
class iceb_u_str inn("");
int koldok_p=0;
int koldok_r=0;
double i_sumap=0.;
double i_sumar=0.;
while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  i=strlen(stroka);
  razmer+=i;
  if(stroka[i-2] == '\r') //Возврат каретки меняем на пробел
   stroka[i-2]=' ';

//  printw("%s",stroka);
//  strzag(LINES-1,0,razmer_fil,razmer);
  iceb_printw(iceb_u_toutf(stroka),data->buffer,data->view);

  iceb_pbar(data->bar,razmer_fil,razmer);    

  if(iceb_u_polen(stroka,kod_str,sizeof(kod_str),1,'|') != 0)
   continue;  
  
  if(iceb_u_SRAV("KDOK",kod_str,0) == 0) //Строка с шапкой документа
   {
    iceb_u_polen(stroka,tip,sizeof(tip),2,'|');
    
    if(tip[0] == '+')
     tipzap=1;
    if(tip[0] == '-')
     tipzap=2;
     
    iceb_u_polen(stroka,kassa,sizeof(kassa),3,'|');
    iceb_u_polen(stroka,bros,sizeof(bros),4,'|');
    if(iceb_u_rsdat(&dd,&md,&gd,bros,1) != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
      kol_oh++;
     }

    if(iceb_pbpds(md,gd,data->window) != 0) /*проверка блокировки*/
      kol_oh++;

    iceb_u_polen(stroka,&nomdok,5,'|');
    iceb_u_polen(stroka,shetd,sizeof(shetd),6,'|');
    iceb_u_polen(stroka,kodop,sizeof(kodop),7,'|');
    iceb_u_polen(stroka,&osnov,8,'|');
    iceb_u_polen(stroka,&dopol,9,'|');
    iceb_u_polen(stroka,&fio,10,'|');
    iceb_u_polen(stroka,&dokum,11,'|');
    iceb_u_polen(stroka,&nomer_bl,12,'|');

    if(metka == 0) //проверка на ошибки
     {
      //Проверяем код кассы
      sprintf(strsql,"select kod from Kas where kod=%s",kassa);
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        sprintf(bros,"%s %s !",gettext("Не найден код кассы"),kassa);
        iceb_menu_soob(bros,data->window);
        kol_oh++;
       }
      //проверяем код операции
      if(tip[0] == '+')
        sprintf(strsql,"select kod from Kasop1 where kod='%s'",kodop);
      if(tip[0] == '-')
        sprintf(strsql,"select kod from Kasop2 where kod='%s'",kodop);
        
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        sprintf(bros,"%s %s !",gettext("Не найден код операции"),kodop);
        iceb_menu_soob(bros,data->window);
        kol_oh++;
       }

      //проверяем счёт
          
      if(iceb_prsh1(shetd,&rek_shet,data->window) != 0)
        kol_oh++;

      //проверяем номер документа
      if(nomdok.getdlinna() > 1)
       {
        
        sprintf(strsql,"select nomd from Kasord where nomd='%s' and kassa=%s and tp=%d and god=%d",
        nomdok.ravno(),kassa,tipzap,gd);
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
         {
          sprintf(bros,gettext("Документ с номером %s уже есть в базе !"),nomdok.ravno());
          iceb_menu_soob(bros,data->window);
          kol_oh++;
         }
           
       
       }
    
      /*Проверяем номер бланка кассового ордера*/
      if(nomer_bl.getdlinna() > 1)
       {
        sprintf(strsql,"select nomd from Kasord where nb='%s'",nomer_bl.ravno_filtr());
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
         {
          sprintf(bros,gettext("Номер бланка %s уже имеет документ с номером %s!"),nomer_bl.ravno(),row[0]);
          iceb_menu_soob(bros,data->window);
          kol_oh++;
         }
       }             
     }
      
    if(metka == 1) //запись в базу шапки документа
     {
      class iceb_lock_tables kkk("LOCK TABLE Kasord WRITE,icebuser READ");
      if(nomdok.getdlinna() <= 1)
          iceb_nomnak(gd,kassa,&nomdok,tipzap,2,1,data->window);

      if(zaphkorw(0,kassa,tipzap,dd,md,gd,nomdok.ravno(),shetd,kodop,osnov.ravno_filtr(),dopol.ravno_filtr(),fio.ravno_filtr(),
      dokum.ravno_filtr(),"","",0,0,0,nomer_bl.ravno(),"",data->window) != 0)
       {
        iceb_menu_soob("Ошибка записи шапки документа !",data->window);
        return(1);
       }
      if(tipzap == 1)
       koldok_p++;
      else
       koldok_r++;
     }
   }
  
  if(iceb_u_SRAV("KZAP",kod_str,0) == 0) //Строка содержимым документа
   {
    iceb_u_polen(stroka,&kontr,2,'|');
    
    if(metka == 0 && kontr.getdlinna() > 1) //проверка на ошибки
     {
      //проверяем код контрагента
      sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",kontr.ravno_filtr());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) <= 0)
       {
        sprintf(bros,"%s %s !",gettext("Не найден код контрагента в общем списке !"),kontr.ravno());
        iceb_menu_soob(bros,data->window);
        kol_oh++;
       }
      
     }    

    iceb_u_polen(stroka,&suma,3,'|');
    iceb_u_polen(stroka,&inn,4,'|');
    iceb_u_polen(stroka,&fam,5,'|');

    if(metka == 0)
     if(kontr.getdlinna() <= 1 && ( inn.getdlinna() <= 1 && fam.getdlinna() <= 1))
      {
       iceb_menu_soob("Немогу определить контрагента!",data->window);
       kol_oh++;
      }

    if(metka == 1) //запись в базу строки документа
     {
      if(kontr.getdlinna() <= 1)
       {
        /*Проверяем индивидуальный налоговый номер*/
        if(inn.getdlinna() > 1)
         {         
          sprintf(strsql,"select kodkon from Kontragent where kod='%s'",inn.ravno());
          if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
            kontr.new_plus(row[0]);
         }
        if(kontr.getdlinna() <= 1)
         {
          /*Проверяем по фамилии*/
           
          sprintf(strsql,"select kodkon from Kontragent where naikon='%s'",fam.ravno());
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
            kontr.new_plus(row[0]);
         }        

        if(kontr.getdlinna() <= 1)
         {
          /*Записываем в базу данных нового контрагента*/
          class iceb_lock_tables kkkk("LOCK TABLE Kontragent WRITE,icebuser READ");
          kontr.new_plus(iceb_get_new_kod("Kontragent",1,data->window));
          
          sprintf(strsql,"insert into Kontragent (kodkon,naikon,kod,ktoz,vrem) \
          values ('%s','%s','%s',%d,%ld)",
          kontr.ravno_filtr(),fam.ravno_filtr(),inn.ravno(),iceb_getuid(data->window),time(NULL));
          
          if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
           continue;
         }
       }        
      zapzkorw(kassa,tipzap,dd,md,gd,nomdok.ravno(),kontr.ravno_filtr(),suma,"","",data->window);
      if(tipzap == 1)
       i_sumap+=suma;
      else
       i_sumar+=suma;

      /*подтверждаем документ*/
      sprintf(strsql,"UPDATE Kasord1 set datp='%d-%d-%d' where \
kassa=%s and god=%d and tp=%d and nomd='%s' and datp='0000-00-00'",
      gd,md,dd,kassa,gd,tipzap,nomdok.ravno());

      iceb_sql_zapis(strsql,0,0,data->window);
      
      /*выполняем проводки*/

      //Читаем счет кассы в коде операции если он там есть
      memset(shetk,'\0',sizeof(shetk));

      if(tipzap == 1)
       sprintf(strsql,"select shetkas,metkapr from Kasop1 where kod='%s'",kodop);
      if(tipzap == 2)
       sprintf(strsql,"select shetkas,metkapr from Kasop2 where kod='%s'",kodop);

      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        strncpy(shetk,row[0],sizeof(shetk)-1);
        if(row[1][0] == '0')
         continue; /*проводок делать для этой операции не нужно*/
       }

      //Читаем счет кассы
      if(shetk[0] == '\0')
       {
        sprintf(strsql,"select shet from Kas where kod=%s",kassa);
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найдена касса"),kassa);
          iceb_menu_soob(strsql,data->window);
          continue;
         }
        else
         strncpy(shetk,row[0],sizeof(shetk)-1);
        
       }
      if(shetk[0] == '\0')
       continue;
      
      avtprkasw(kassa,tipzap,nomdok.ravno(),dd,md,gd,shetk,shetd,kodop,data->window); /*выполняем проводки*/

      provpodkow(kassa,nomdok.ravno(),dd,md,gd,tipzap,data->window); /*проверяем подтверждение документа*/
      provprkasw(kassa,tipzap,nomdok.ravno(),dd,md,gd,kodop,data->window); /*проверяем выполнение проводок*/
     }
   }




 }

if(metka == 1)
 {
  class iceb_u_str repl;
  
  sprintf(strsql,"%s %d %s %.2f",
  gettext("Приходных документов"),
  koldok_p,
  gettext("на сумму"),
  i_sumap);
  
  repl.plus(strsql);
  
  sprintf(strsql,"%s %d %s %.2f",
  gettext("Расходных документов"),
  koldok_r,
  gettext("на сумму"),
  i_sumar);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,data->window);  
 }

return(kol_oh);
}
