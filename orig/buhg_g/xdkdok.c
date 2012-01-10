/*$Id: xdkdok.c,v 1.44 2011-06-07 08:52:27 sasa Exp $*/
/*03.06.2010	10.04.2006	Белых А.И.	xdkdok.c
Просмотр и редактирование платёжного документа
Если вернули 0- вышли по F10
             1- удалили документ
*/

#define GORIZONTAL_RIS 800
#define VERTIKAL_RIS  470
#include        <stdlib.h>
#include <errno.h>
#include "buhg_g.h"
#include <unistd.h>
#include "dok4w.h"

enum
{
 FK2,
 FK3,
 FK4,
 FK5,
 FK6,
 FK7,
 SFK7,
 FK8,
 FK9,
 FK10,
 KOL_F_KL
};

class  xdkdok_data
 {
  public:
  GtkWidget *label_nadpis;
  GtkWidget *label_prov;

  GtkWidget *window;
  GtkWidget *risunok;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int voz;
    
  int metka_proris;  
  double sump; //Сумма подтверждения документа
  char tablica[56];
  iceb_u_str naim_regim;  
  xdkdok_data()
   {
    voz=0;
   }
 };

gboolean   xdkdok_key_press(GtkWidget *widget,GdkEventKey *event,class xdkdok_data *data);
void  xdkdok_knopka(GtkWidget *widget,class xdkdok_data *data);
gboolean xdkdok_draw(GtkWidget *widget,GdkEvent *event,class xdkdok_data *data);
void xdkdok_zagolovok(class xdkdok_data *data);

//void xdkdok_menu_kor(class xdkdok_data *data);
int xdkdok_v(char *tablica,GtkWidget *wpredok);
int udpldw(const char *tabl,short d,short m,short g,const char *npp,int mu,const char *kodop,int metkasoob,int tipz,GtkWidget *wpredok);
int zappdokw(char tabl[],GtkWidget *wpredok);

void l_prov_xdk(char *tablica,GtkWidget *wpredok);
int   podzapxdk(char *tablica,GtkWidget *wpredok);
void l_xdkpz(char *tablica,GtkWidget *wpredok);
void xdkdok_menu_ras(class xdkdok_data *data);
int platpw(char *imaf1,short kp,short nomk,FILE *ff,int viddok,GtkWidget *wpredok);
int vibshbnw(char *kontr,char *tabl,GtkWidget *wpredok);
void xdkdok_smsh(class xdkdok_data *data);
//void	klient(const char *nomdok,short dd,short md,short gd,const char *tabl,GtkWidget *wpredok);

extern SQL_baza	bd;
extern char *name_system;
extern class REC rec;
extern char *imafkomtmp;
extern short    kp;    /*Количество повторов*/
extern char *organ;

int xdkdok(const char *tablica,GtkWidget *wpredok)
{
//printf("xdkdok-%s\n",tablica);

class  xdkdok_data data;
data.metka_proris=0;
char bros[512];

strcpy(data.tablica,tablica);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/

if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

data.label_nadpis=gtk_label_new ("");
data.label_prov=gtk_label_new("");

xdkdok_zagolovok(&data); //формируем заголовок меню

gtk_window_set_title (GTK_WINDOW (data.window),data.naim_regim.ravno_toutf());
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);



gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(xdkdok_key_press),&data);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_release_event",GTK_SIGNAL_FUNC(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_hbox_new (FALSE, 1);
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_vbox_new (FALSE, 1);
GtkWidget *vbox2 = gtk_vbox_new (FALSE, 1);

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);



GdkColor color;
gdk_color_parse("red",&color);
gtk_widget_modify_fg(data.label_prov,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox2),data.label_nadpis,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),data.label_prov,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

//Создаём зону рисования
data.risunok=gtk_drawing_area_new();
gtk_widget_set_usize(GTK_WIDGET(data.risunok),GORIZONTAL_RIS,VERTIKAL_RIS);

gtk_box_pack_end (GTK_BOX (vbox2), data.risunok, TRUE, TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.risunok),"event",GTK_SIGNAL_FUNC(xdkdok_draw),&data);
//Кнопки
GtkTooltips *tooltips[KOL_F_KL];

sprintf(bros,"F2 %s",gettext("Корректировать"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK2],TRUE,TRUE, 0);
tooltips[FK2]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK2],data.knopka[FK2],gettext("Корректировать реквизиты документа"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK2]), "clicked",GTK_SIGNAL_FUNC(xdkdok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK2]),(gpointer)FK2);
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK3]), "clicked",GTK_SIGNAL_FUNC(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
tooltips[FK3]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK3],data.knopka[FK3],gettext("Удалить документ"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK3]),(gpointer)FK3);
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Проводки"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK4]), "clicked",GTK_SIGNAL_FUNC(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
tooltips[FK4]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK4],data.knopka[FK4],gettext("Переход в режим работы с проводками для этого документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK4]),(gpointer)FK4);
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK5]), "clicked",GTK_SIGNAL_FUNC(xdkdok_knopka),&data);
tooltips[FK5]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK5],data.knopka[FK5],gettext("Распечатка документов"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK5]),(gpointer)FK5);
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Смена счёта"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK6]), "clicked",GTK_SIGNAL_FUNC(xdkdok_knopka),&data);
tooltips[FK6]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK6],data.knopka[FK6],gettext("Сменить счёт дебета"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK6]),(gpointer)FK6);
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Подтверждение"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK7]), "clicked",GTK_SIGNAL_FUNC(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
tooltips[FK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK7],data.knopka[FK7],gettext("Подтверждение документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK7]),(gpointer)FK7);
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"%sF7 %s",RFK,gettext("Подтверждение"));
data.knopka[SFK7]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[SFK7]), "clicked",GTK_SIGNAL_FUNC(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK7],TRUE,TRUE, 0);
tooltips[SFK7]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SFK7],data.knopka[SFK7],gettext("Работа с подтверждающими записями"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SFK7]),(gpointer)SFK7);
gtk_widget_show(data.knopka[SFK7]);

sprintf(bros,"F8 %s",gettext("Нов.докум."));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK8]), "clicked",GTK_SIGNAL_FUNC(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
tooltips[FK8]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK8],data.knopka[FK8],gettext("Сделать новый документ с использованием реквизитов этого документа"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK8]),(gpointer)FK8);
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F9 %s",gettext("В банк"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK9]), "clicked",GTK_SIGNAL_FUNC(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
tooltips[FK9]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK9],data.knopka[FK9],gettext("Установить метку предачи в банк"),NULL);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK9]),(gpointer)FK9);
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
tooltips[FK10]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[FK10],data.knopka[FK10],gettext("Завершение работы в этом окне"),NULL);
gtk_signal_connect(GTK_OBJECT(data.knopka[FK10]), "clicked",GTK_SIGNAL_FUNC(xdkdok_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[FK10]),(gpointer)FK10);
gtk_widget_show(data.knopka[FK10]);


gtk_widget_show_all(data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xdkdok_knopka(GtkWidget *widget,class xdkdok_data *data)
{
char strsql[512];
short dt,mt,gt;

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

switch (knop)
 {
  case FK2:
//    xdkdok_menu_kor(data);
    if(xdkdok_v(data->tablica,data->window) == 0)
      if(rec.nomdk_i.getdlinna() > 1) //Корректировка уже записаного документа
       {
        if(udpldw(data->tablica,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz,data->window) == 0)
          zappdokw(data->tablica,data->window);
       }

    xdkdok_zagolovok(data);
    //Для прорисовки всей области рисунка а не только той зоны, которая была накрыта предыдущим меню
    gtk_widget_hide(data->risunok);
    gtk_widget_show(data->risunok);
    return;  


  case FK3:

     
     if(iceb_pbpds(rec.md,rec.gd,data->window) != 0)
      {
       sprintf(strsql,gettext("Дата %d.%dг. заблокирована !"),rec.md,rec.gd);
       iceb_menu_soob(strsql,data->window);
       return;
      } 
     
     if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Pltt",0) == 0 )
      {
       int metka_ps=0;
       if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
        metka_ps=4;
       if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
        metka_ps=5;
       /*Смотрим заблокированы ли проводки к этому документа*/
       if(iceb_pbp(metka_ps,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),0,rec.tipz,gettext("Удалить документ невозможно!"),data->window) != 0)
         return;
      }

    
     if(iceb_pblok(rec.md,rec.gd,ICEB_PS_GK,data->window) != 0)
       {
        sprintf(strsql,gettext("На дату %d.%dг. проводки заблокированы ! Удаление невозможно !"),rec.md,rec.gd);
        iceb_menu_soob(strsql,data->window);
        return;
       }
    
     if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
      return;

     if(udpldw(data->tablica,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),0,rec.kodop.ravno(),1,rec.tipz,data->window) == 0)
      {
       data->voz=1;
       gtk_widget_destroy(data->window);
      }    
    return;  


  case FK4:
     if(rec.nomdk_i.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Документ не записан !"),data->window);
      return;
     }

    if(data->sump == 0.)
     {
      iceb_menu_soob(gettext("Документ не подтверждён !"),data->window);
      return;
     }

    l_prov_xdk(data->tablica,data->window);

    xdkdok_zagolovok(data);
    return;  

  case FK5:

    xdkdok_menu_ras(data);
    xdkdok_zagolovok(data); //обязательно
    return;  

  case FK6:
    xdkdok_smsh(data);
    xdkdok_zagolovok(data);
    //Для прорисовки всей области рисунка а не только той зоны, которая была накрыта предыдущим меню
    gtk_widget_hide(data->risunok);
    gtk_widget_show(data->risunok);
    return;  

  case FK7:
    podzapxdk(data->tablica,data->window);
    xdkdok_zagolovok(data);
    return;  

  case SFK7:
    l_xdkpz(data->tablica,data->window);
    xdkdok_zagolovok(data);
    return;  

  case FK8:
    iceb_u_poltekdat(&dt,&mt,&gt);
    
    sprintf(strsql,"%d",nomdokw(gt,data->tablica,data->window));
    rec.nomdk.new_plus(strsql);
    
    rec.dd=dt;
    rec.md=mt;
    rec.gd=gt;
    
    rec.ddi=0;
    rec.mdi=0;
    rec.gdi=0;
    rec.nomdk_i.new_plus("");
    
    sprintf(strsql,"%s N%s %s:%02d.%02d.%d",
    gettext("Делаем новый документ !"),
    rec.nomdk.ravno(),
    gettext("Дата"),
    rec.dd,rec.md,rec.gd);
    iceb_menu_soob(strsql,data->window);

    xdkdok_zagolovok(data);
    //Для прорисовки всей области рисунка а не только той зоны, которая была накрыта предыдущим меню
    gtk_widget_hide(data->risunok);
    gtk_widget_show(data->risunok);
    
    return;  
  
  case FK9:
    if(iceb_u_SRAV(data->tablica,"Tpltp",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
     {
      iceb_menu_soob(gettext("Типовые документы не передаются !"),data->window);
      return;
     }
    
    if(rec.nomdk_i.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Документ не записан !"),data->window);
      return;
     }
    sprintf(strsql,"update %s set vidpl='1' where datd='%04d-%02d-%02d' and nomd='%s'",
    data->tablica,rec.gd,rec.md,rec.dd,rec.nomdk.ravno());
    iceb_sql_zapis(strsql,1,0,data->window);
    iceb_menu_soob(gettext("Документ отмечен для передачи в банк"),data->window);
//    klient(rec.nomdk.ravno(),rec.dd,rec.md,rec.gd,data->tablica,data->window);
    return;  

    
  case FK10:
  
    if(rec.ddi != 0 && (iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Pltt",0) == 0))
       provpdw(data->tablica,data->window);
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   xdkdok_key_press(GtkWidget *widget,GdkEventKey *event,class xdkdok_data *data)
{
iceb_u_str repl;
//printf("xdkdoks_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_F2:
//    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK2]),"clicked");
//    if(data->kl_shift == 1)
//      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK2]),"clicked");
    return(TRUE);

  case GDK_F3:
  //  if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK3]),"clicked");
  //  if(data->kl_shift == 1)
  //    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK3]),"clicked");
    return(TRUE);

  case GDK_F4:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK4]),"clicked");
    return(TRUE);

  case GDK_F5:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK5]),"clicked");
    return(TRUE);

  case GDK_F6:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK6]),"clicked");
    return(TRUE);

  case GDK_F7:
    if(data->kl_shift == 0)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK7]),"clicked");
    if(data->kl_shift == 1)
      gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[SFK7]),"clicked");
    return(TRUE);

  case GDK_F8:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK8]),"clicked");
    return(TRUE);

  case GDK_F9:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK9]),"clicked");
    return(TRUE);

  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("xdkdoks_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}


/********************************/
/*прорисовка платёжного документа*/
/*********************************/
gboolean xdkdok_draw(GtkWidget *widget,GdkEvent *event,class xdkdok_data *data)
{
char strsql[1024];
char bros[1024];

//printf("xdkdok_draw-рисуем платёжку %f\n",rec.sumd);

short metka_tip_dok=0; //0-платёжное поручение 1-требование
if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
  metka_tip_dok=1;

//рисуем прямоуголиник по размерам области рисования белым цветом с заливкой его белым
gdk_draw_rectangle(widget->window,
widget->style->white_gc,
TRUE, //фон прямоугольника закрашивается цветом линии
0,
0,
GORIZONTAL_RIS-1,
VERTIKAL_RIS-1);

//Создаём новый графический контекст
GdkGC *gc=gdk_gc_new(widget->window);


/*********************************************************/
//Для начала создаём контекст Pango - PangoContext, который нужен для создания PangoLayout
PangoLayout *layout = NULL;
GdkScreen *screen = gdk_screen_get_default();
PangoContext *context = (PangoContext*)gdk_pango_context_get_for_screen(screen);

//Узнаём  шрифт по умолчанию для созданного контекста.
// шрифт
PangoFontDescription *desc = pango_context_get_font_description(context);
//PangoFontDescription *desc=pango_font_description_from_string("Sans 16"); так тоже можно но нужно в конце освоботить память

//Потом задаём характеристики шрифта - имя шрифта, стиль, жирность и размер.

//pango_font_description_set_family(desc,"courier");
pango_font_description_set_family(desc,"mono");
//pango_font_description_set_family(desc,"Areal");
//pango_font_description_set_style(desc,PANGO_STYLE_ITALIC);
//pango_font_description_set_weight(desc,PANGO_WEIGHT_BOLD);
pango_font_description_set_size(desc,18*PANGO_SCALE);

//Затем создаём PangoLayout из созданного контекста,
layout = pango_layout_new(context);










//Устанавливаем толщину линии
gdk_gc_set_line_attributes(gc,2,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);

//Рисуем рамку чёрного цвета вокруг области рисования
gdk_draw_rectangle(widget->window,gc,FALSE,1,1,GORIZONTAL_RIS-2,VERTIKAL_RIS-2);

//Устанавливаем толщину линии
gdk_gc_set_line_attributes(gc,4,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);

//Устанавливаем фонт для рисования текста

//PangoFontDescription *font_pango=pango_font_description_from_string("Regular 12");
/*С помощью программы xfontsel можно посмотреть какие есть шрифты*/
//GdkFont *font;
//font=gdk_font_load("-*-courier-*-r-normal-*-18-*-*-*-*-*-koi8-u");

if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ПЛАТЁЖНОЕ ПОРУЧЕНИЕ"),rec.nomdk.ravno());
if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ПЛАТЁЖНОЕ ТРЕБОВАНИЕ"),rec.nomdk.ravno());
if(iceb_u_SRAV(data->tablica,"Tpltp",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ТИПОВОЕ ПЛАТЁЖНОЕ ПОРУЧЕНИЕ"),rec.nomdk.ravno());
if(iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ТИПОВОЕ ПЛАТЁЖНОЕ ТРЕБОВАНИЕ"),rec.nomdk.ravno());

//gdk_draw_string(widget->window,font,widget->style->black_gc,200,20,iceb_tokoi8u(strsql));

//добавляем в него текст.
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
//Остаётся только отобразить текст на экране:
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,190.,5.,layout);

//font=gdk_font_load("-*-courier-*-r-normal-*-12-*-*-*-*-*-koi8-u");
//gdk_draw_string(widget->window,font,widget->style->black_gc,700,20,"0410001");

pango_font_description_set_size(desc,12*PANGO_SCALE);
pango_layout_set_text(layout,"0410001",-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,695.,8.,layout);


gdk_draw_rectangle(widget->window,gc,FALSE,690,7,80,20);

//font=gdk_font_load("-*-courier-*-r-normal-*-18-*-*-*-*-*-koi8-u");
pango_font_description_set_size(desc,14*PANGO_SCALE);

if(rec.gd != 0)
 {
  iceb_mesc(rec.md,1,bros);

  sprintf(strsql,"%s %02d %s %d%s",gettext("от"),rec.dd,bros,rec.gd,gettext("г."));


//  gdk_draw_string(widget->window,font,widget->style->black_gc,220,40,iceb_tokoi8u(strsql));
  pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,215.,25.,layout);
 }

//gdk_draw_string(widget->window,font,widget->style->black_gc,600,45,iceb_tokoi8u(gettext("Получено банком")));
pango_layout_set_text(layout,gettext("Получено банком"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,590.,25.,layout);

memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
 sprintf(strsql,"%s %s",gettext("Плательщик"),rec.naior.ravno());
if(metka_tip_dok == 1)
 sprintf(strsql,"%s %s",gettext("Плательщик"),rec.naior1.ravno());
sprintf(bros,"%.*s",iceb_u_kolbait(50,strsql),strsql);

//gdk_draw_string(widget->window,font,widget->style->black_gc,5,70,iceb_tokoi8u(bros));
pango_layout_set_text(layout,iceb_u_toutf(bros),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,55.,layout);

if(iceb_u_strlen(iceb_tokoi8u(strsql)) > 50)
 {
  sprintf(bros,"%s",iceb_u_adrsimv(50,strsql));
//  gdk_draw_string(widget->window,font,widget->style->black_gc,5,90,iceb_tokoi8u(bros));
  pango_layout_set_text(layout,iceb_u_toutf(bros),-1);
  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,75.,layout);
 }
sprintf(strsql,"\"___\"________%d%s",rec.gd,gettext("г."));

//gdk_draw_string(widget->window,font,widget->style->black_gc,590,70,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,580.,55.,layout);

//Прямоугольник для кода плательщика
gdk_draw_rectangle(widget->window,gc,FALSE, 50,100,150,20);

//gdk_draw_string(widget->window,font,widget->style->black_gc,5,115,iceb_tokoi8u(gettext("Код")));
pango_layout_set_text(layout,gettext("Код"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,98.,layout);

if(metka_tip_dok == 0)
 strcpy(strsql,rec.kod.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.kod1.ravno());
 
//gdk_draw_string(widget->window,font,widget->style->black_gc,55,115,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,55.,98.,layout);

//gdk_draw_string(widget->window,font,widget->style->black_gc,5,135,iceb_tokoi8u(gettext("Банк плательщика")));
pango_layout_set_text(layout,gettext("Банк плательщика"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,118.,layout);


//gdk_draw_string(widget->window,font,widget->style->black_gc,370,135,iceb_tokoi8u(gettext("Код банка")));
pango_layout_set_text(layout,gettext("Код банка"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,370.,118.,layout);

sprintf(strsql,"%s %s N",gettext("ДЕБЕТ"),gettext("счёт"));

//gdk_draw_string(widget->window,font,widget->style->black_gc,500,135,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,500.,118.,layout);

//gdk_draw_string(widget->window,font,widget->style->black_gc,700,135,iceb_tokoi8u(gettext("СУММА")));
pango_layout_set_text(layout,gettext("СУММА"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,700.,118.,layout);


memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
  sprintf(strsql,"%s в м.%s",rec.naiban.ravno(),rec.gorod.ravno());
if(metka_tip_dok == 1)
  sprintf(strsql,"%s в м.%s",rec.naiban1.ravno(),rec.gorod1.ravno());

sprintf(bros,"%.*s",iceb_u_kolbait(30,strsql),strsql);

//gdk_draw_string(widget->window,font,widget->style->black_gc,5,155,iceb_tokoi8u(bros));
pango_layout_set_text(layout,iceb_u_toutf(bros),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,138.,layout);


//Устанавливаем толщину линии
gdk_gc_set_line_attributes(gc,2,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);

//Линия под наименованием банка плательщика

gdk_draw_line(widget->window,gc,0,161,370,161);

//Устанавливаем толщину линии
gdk_gc_set_line_attributes(gc,4,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);

//Прямоугольник для МФО плательщика
gdk_draw_rectangle(widget->window,gc,FALSE,370,140,100,20);


if(metka_tip_dok == 0)
 strcpy(strsql,rec.mfo.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.mfo1.ravno());
 
//gdk_draw_string(widget->window,font,widget->style->black_gc,375,155,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,375.,138.,layout);

//Прямоугольник для счёта плательщика
gdk_draw_rectangle(widget->window,gc,FALSE,480,140,170,20);


if(metka_tip_dok == 0)
 strcpy(strsql,rec.nsh.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.nsh1.ravno());


//gdk_draw_string(widget->window,font,widget->style->black_gc,485,155,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,485.,138.,layout);

//Прямоугольник для суммы платежа
gdk_draw_rectangle(widget->window,gc,FALSE,650,140,140,105);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"%.2f",rec.sumd);
//gdk_draw_string(widget->window,font,gc,655,155,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,655.,138.,layout);

memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
  sprintf(strsql,"%s %s",gettext("Получатель"),rec.naior1.ravno());
if(metka_tip_dok == 1)
  sprintf(strsql,"%s %s",gettext("Получатель"),rec.naior.ravno());

//gdk_draw_string(widget->window,font,widget->style->black_gc,5,175,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,158.,layout);

//Прямоугольник для кода получателя
gdk_draw_rectangle(widget->window,gc,FALSE,50,185,150,20);

//gdk_draw_string(widget->window,font,widget->style->black_gc,5,200,iceb_tokoi8u(gettext("Код")));
pango_layout_set_text(layout,gettext("Код"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,183.,layout);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"%s %s N",gettext("КРЕДИТ"),gettext("счёт"));

//gdk_draw_string(widget->window,font,widget->style->black_gc,500,200,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,490.,183.,layout);

memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
 strcpy(strsql,rec.kod1.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.kod.ravno());

//printf("Код получателя-%s\n",strsql);

//gdk_draw_string(widget->window,font,widget->style->black_gc,55,200,iceb_tokoi8u(strsql));
//gdk_draw_string(widget->window,font,widget->style->black_gc,5,220,iceb_tokoi8u(gettext("Банк получателя")));
//gdk_draw_string(widget->window,font,widget->style->black_gc,370,220,iceb_tokoi8u(gettext("Код банка")));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,55.,183.,layout);

pango_layout_set_text(layout,gettext("Банк получателя"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,203.,layout);

pango_layout_set_text(layout,gettext("Код банка"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,370.,203.,layout);


memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
  sprintf(strsql,"%s в м.%s",rec.naiban1.ravno(),rec.gorod1.ravno());
if(metka_tip_dok == 1)
  sprintf(strsql,"%s в м.%s",rec.naiban.ravno(),rec.gorod.ravno());

sprintf(bros,"%.*s",iceb_u_kolbait(30,strsql),strsql);
//gdk_draw_string(widget->window,font,widget->style->black_gc,5,240,iceb_tokoi8u(bros));
pango_layout_set_text(layout,iceb_u_toutf(bros),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,223.,layout);


//Устанавливаем толщину линии
gdk_gc_set_line_attributes(gc,2,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);

//Линия под наименованием банка плательщика

gdk_draw_line(widget->window,gc,0,246,370,246);


//Устанавливаем толщину линии
gdk_gc_set_line_attributes(gc,4,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);

//Прямоугольник для МФО плательщика
gdk_draw_rectangle(widget->window,gc,FALSE,370,225,100,20);

if(metka_tip_dok == 0)
 strcpy(strsql,rec.mfo1.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.mfo.ravno());
 
//gdk_draw_string(widget->window,font,widget->style->black_gc,375,240,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,375.,223.,layout);

//Прямоугольник для счёта за услуги банка
gdk_draw_rectangle(widget->window,gc,FALSE,480,225,170,20);

//Прямоугольник для счёта плательщика
gdk_draw_rectangle(widget->window,gc,FALSE,480,205,170,20);

if(metka_tip_dok == 0)
 strcpy(strsql,rec.nsh1.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.nsh.ravno());

//gdk_draw_string(widget->window,font,widget->style->black_gc,485,220,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,485.,203.,layout);

//Устанавливаем толщину линии
gdk_gc_set_line_attributes(gc,2,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);

//Прямоугольник для кода нерезидента
if(metka_tip_dok == 0)
 {
  gdk_draw_rectangle(widget->window,gc,FALSE,650,255,140,20);

//  gdk_draw_string(widget->window,font,widget->style->black_gc,655,270,iceb_tokoi8u(rec.kodnr.ravno()));
  pango_layout_set_text(layout,iceb_u_toutf(rec.kodnr.ravno()),-1);
  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,655.,253.,layout);

 }
sprintf(strsql,"%s (%s)",gettext("Сумма"),gettext("словами"));

//gdk_draw_string(widget->window,font,widget->style->black_gc,5,265,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,248.,layout);

//Сумма словами
memset(strsql,'\0',sizeof(strsql));
iceb_u_preobr(rec.sumd+rec.uslb,strsql,0);

//gdk_draw_string(widget->window,font,widget->style->black_gc,5,285,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,268.,layout);

//Линия под cуммой словами

gdk_draw_line(widget->window,gc,0,290,600,290);

//gdk_draw_string(widget->window,font,widget->style->black_gc,5,310,iceb_tokoi8u(gettext("Назначение платежа")));
pango_layout_set_text(layout,gettext("Назначение платежа"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,293.,layout);


//font=gdk_font_load("-*-courier-*-r-normal-*-14-*-*-*-*-*-koi8-u");
//int YY=330;
pango_font_description_set_size(desc,14*PANGO_SCALE);
int YY=313;

int kol_ps=iceb_u_pole2(rec.naz_plat.ravno(),'\n');
if(kol_ps == 0)
 {
//  gdk_draw_string(widget->window,font,widget->style->black_gc,5,YY,iceb_tokoi8u(rec.naz_plat.ravno()));
  pango_layout_set_text(layout,rec.naz_plat.ravno_toutf(),-1);
  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,YY,layout);
  YY+=20;
 }
else
  for(int ii=1; ii <= kol_ps; ii++)
   {
    if(ii > 4)
     break;

    iceb_u_polen(rec.naz_plat.ravno(),strsql,sizeof(strsql),ii,'\n');  
//    gdk_draw_string(widget->window,font,widget->style->black_gc,5,YY,iceb_tokoi8u(strsql));
    pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
    gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,YY,layout);
    YY+=20;

   }

//gdk_draw_string(widget->window,font,widget->style->black_gc,20,410,iceb_tokoi8u("М.П."));
pango_layout_set_text(layout,iceb_u_toutf("М.П."),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,20.,393.,layout);

sprintf(strsql,"%s______________",gettext("Подпись"));

//gdk_draw_string(widget->window,font,widget->style->black_gc,100,410,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,100.,393.,layout);

//gdk_draw_string(widget->window,font,widget->style->black_gc,600,410,iceb_tokoi8u(gettext("Проведено банком")));
pango_layout_set_text(layout,gettext("Проведено банком"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,600.,393.,layout);

//gdk_draw_string(widget->window,font,widget->style->black_gc,160,430,iceb_tokoi8u("______________"));
pango_layout_set_text(layout,iceb_u_toutf("______________"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,170.,413.,layout);

sprintf(strsql,"\"___\"________%d%s",rec.gd,gettext("г."));

//gdk_draw_string(widget->window,font,widget->style->black_gc,600,430,iceb_tokoi8u(strsql));
pango_layout_set_text(layout,iceb_u_toutf(strsql),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,600.,413.,layout);

//gdk_draw_string(widget->window,font,widget->style->black_gc,625,450,iceb_tokoi8u(gettext("подпись банка")));
pango_layout_set_text(layout,gettext("подпись банка"),-1);
gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,625.,433.,layout);

//gtk_widget_show(widget);

//Напоследок почистим память:
//pango_font_description_free(desc);
g_object_unref(context); 
g_object_unref(layout);

return(TRUE);
}

/*************************/
/*Формирование заголовка */
/*************************/
void xdkdok_zagolovok(class xdkdok_data *data)
{
short dp=0,mp=0,gp=0;
double suma_pos_podt=0.;

data->sump=sumpzpdw(data->tablica,&dp,&mp,&gp,&suma_pos_podt,data->window);

data->naim_regim.new_plus("");
if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
  data->naim_regim.plus(gettext("Работа с платёжным поручением"));
if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
  data->naim_regim.plus(gettext("Работа с платёжным требованием"));
if(iceb_u_SRAV(data->tablica,"Тpltp",0) == 0)
  data->naim_regim.plus(gettext("Работа с типовым платёжным поручением"));
if(iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
  data->naim_regim.plus(gettext("Работа с типовым платёжным требованием"));

iceb_u_str sapka;
sapka.plus(data->naim_regim.ravno());
sapka.ps_plus(gettext("Операция"));
sapka.plus(":");
sapka.plus(rec.kodop.ravno());

sapka.plus(" ");

sapka.plus(gettext("НДС"));
sapka.plus(":");
sapka.plus(rec.nds);

sapka.plus(" ");
sapka.plus(gettext("Количество копий"));
sapka.plus(":");
sapka.plus(kp);

sapka.plus(" ");
sapka.plus(gettext("Контрагенты"));
sapka.plus(":");
sapka.plus(rec.kodor.ravno());
sapka.plus("/");
sapka.plus(rec.kodor1.ravno());

sapka.ps_plus(gettext("Подтверждённая сумма"));
sapka.plus(":");
sapka.plus(data->sump);

gtk_label_set_text(GTK_LABEL(data->label_nadpis),sapka.ravno_toutf());

if(rec.prov == 0)
  gtk_label_set_text(GTK_LABEL(data->label_prov),"");
else
  gtk_label_set_text(GTK_LABEL(data->label_prov),gettext("Проводки не сделаны !"));

}
#if 0
######################################################################333
/*************************************/
/*Меню выбора корректировки*/
/*****************************/
void xdkdok_menu_kor(class xdkdok_data *data)
{
int nomer=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Корректировка"));
char strsql[512];
memset(strsql,'\0',sizeof(strsql));
strncpy(strsql,organ,40);
zagolovok.plus_ps(strsql);

zagolovok.plus(gettext("Корректировка"));

punkt_m.plus(gettext("Корректировка реквизитов документа"));//0
punkt_m.plus(gettext("Корректировать комментарий к документу"));//1
punkt_m.plus(gettext("Корректировать контрагента из списка базы"));//2
punkt_m.plus(gettext("Корректировать контрагента из списка файла"));//3

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

switch (nomer)
 {
  case -1:
    return;

  case 0:
    if(xdkdok_v(data->tablica,data->window) == 0)
      if(rec.nomdk_i.getdlinna() > 1) //Корректировка уже записаного документа
       {
        if(udpldw(data->tablica,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz,data->window) == 0)
          zappdokw(data->tablica,data->window);
       }
     
    break;

  case 1:
    printf("imafkomtmp=%s rec.nomdk_i=%s\n",imafkomtmp,rec.nomdk_i.ravno());
    iceb_f_redfil(imafkomtmp,1,data->window);

    if(rec.nomdk_i.getdlinna() > 1) //Корректировка уже записаного документа
     {
      if(udpldw(data->tablica,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz,data->window) == 0)
        zappdokw(data->tablica,data->window);
     }
    break;

  case 2:
/**********
    vzrkbzw(data->window);
    if(rec.nomdk_i.getdlinna() > 1) //Корректировка уже записаного документа
     {
      if(udpldw(data->tablica,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz,data->window) == 0)
        zappdokw(data->tablica,data->window);
     }
************/    
    break;

  case 3:
    if(iceb_f_redfil("platpor.alx",0,data->window) != 0)
     break;

    vzrkfw(data->window);

    if(rec.nomdk_i.getdlinna() > 1) //Корректировка уже записаного документа
     {
      if(udpldw(data->tablica,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz,data->window) == 0)
        zappdokw(data->tablica,data->window);
     }
    
    break;

 }


}
########################################################################333
#endif
/***************************/
/*Меню распечатки*/
/*************************/
void xdkdok_menu_ras(class xdkdok_data *data)
{
int kom=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка"));

punkt_m.plus(gettext("Распечатка на системном принтере"));//0
punkt_m.plus(gettext("Запись в журнал регистрации"));//1
punkt_m.plus(gettext("Запись распечатки в файл"));//2
punkt_m.plus(gettext("Запись на DOS-дискету"));//3
punkt_m.plus(gettext("Просмотр перед печатью"));//4

char strsql[512];
memset(strsql,'\0',sizeof(strsql));
strncpy(strsql,organ,40);
zagolovok.new_plus(strsql);
zagolovok.ps_plus(gettext("Распечатка"));
/***************
memset(strsql,'\0',sizeof(strsql));
char *nameprinter = getenv("PRINTER");
if(nameprinter != NULL)
 sprintf(strsql,"%s:%-*.*s",gettext("Принтер"),
 iceb_u_kolbait(20,nameprinter),
 iceb_u_kolbait(20,nameprinter),
 nameprinter);
else
 sprintf(strsql,"%s:%-*.*s",gettext("Принтер"),
 iceb_u_kolbait(20,gettext("По умолчанию")),
 iceb_u_kolbait(20,gettext("По умолчанию")),
 gettext("По умолчанию"));

zagolovok.ps_plus(strsql);
*********************/



kom=iceb_menu_mv(&titl,&zagolovok,&punkt_m,kom,data->window);

if(kom == -1)
 return;


/*Проверяем номер платежного поручения*/
if(iceb_u_SRAV(rec.nomdk.ravno(),rec.nomdk_i.ravno(),0) != 0)
 {

  sprintf(strsql,"select nomd from %s where datd >= '%04d-01-01' and \
datd <= '%04d-12-31' and nomd='%s'",data->tablica,rec.gd,rec.gd,rec.nomdk.ravno());   

  if(iceb_sql_readkey(strsql,data->window) >= 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),rec.nomdk.ravno());
    iceb_menu_soob(strsql,data->window);
    return;
    
   }
 }


if(iceb_pbpds(rec.md,rec.gd,data->window) != 0)
  goto vpr;

//printf("nomdk_i=%s %d\n",rec.nomdk_i.ravno(),rec.nomdk_i.getdlinna());

//Удаляем предыдущий документ 
if(rec.nomdk_i.getdlinna() > 1)
 {
   udpldw(data->tablica,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz,data->window);
 }   
zappdokw(data->tablica,data->window);
    
if(kom == 1)
  return;
      
vpr:;

char imaf[32];
FILE *ff;
    
sprintf(imaf,"pp%d.lst",rec.nomdk.ravno_atoi());
    
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }

iceb_u_startfil(ff);

for(int kolp=0; kolp <kp ; kolp++)
 {
  if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltp",0) == 0)
   if(platpw(imafkomtmp,kp,kolp,ff,0,data->window) != 0)
    {
     fclose(ff);
     return;
    }
  if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
   if(platpw(imafkomtmp,kp,kolp,ff,1,data->window) != 0)
    {
     fclose(ff);
     return;
    }
 }
fclose(ff);
   
if(kom == 2)
 {
  sprintf(strsql,"%s %s",gettext("Распечатка выгружена в файл"),imaf);
  iceb_menu_soob(strsql,data->window);
  return;
 }

if(kom == 3)
 {
  iceb_mydoscopy(imaf,data->window);
  return;
 }

if(kom == 4) //Просмотр перед печатью
  {
   iceb_prospp(imaf,data->window);
   return;
  }

//iceb_print(imaf,data->window);
iceb_print_operation(GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,imaf,data->window);


}
/**********************************/
/*Смена счёта*/
/******************************/
void xdkdok_smsh(class xdkdok_data *data)
{
char strsql[512];
char kontr[40];
memset(kontr,'\0',sizeof(kontr));

if(vibshbnw(kontr,data->tablica,data->window) == 0)
 {
  SQL_str row;
  SQLCURSOR cur;
  
  sprintf(strsql,"select naikon,kod,naiban,mfo,nomsh,adresb from \
Kontragent where kodkon='%s'",kontr);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr);
    iceb_menu_soob(strsql,data->window);
    return;
   }
  else
   {
    if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltp",0) == 0)
     {
      rec.kodor.new_plus(kontr);

//      if(iceb_u_pole(row[0],rec.naior,1,'(') != 0)    
//       strcpy(rec.naior,row[0]);
      
      if(iceb_u_pole(row[0],strsql,1,'(') != 0)    
       strcpy(strsql,row[0]);
      rec.naior.new_plus(strsql);
            
      rec.kod.new_plus(row[1]);
      rec.naiban.new_plus(row[2]);
      rec.mfo.new_plus(row[3]);
      rec.nsh.new_plus(row[4]);
      rec.gorod.new_plus(row[5]);
     }
    else
     {
      rec.kodor1.new_plus(kontr);

      if(iceb_u_pole(row[0],strsql,1,'(') != 0)    
       strcpy(strsql,row[0]);
      rec.naior1.new_plus(strsql);
      
      rec.kod1.new_plus(row[1]);
      rec.naiban1.new_plus(row[2]);
      rec.mfo1.new_plus(row[3]);
      rec.nsh1.new_plus(row[4]);
      rec.gorod1.new_plus(row[5]);
     }
   }


 }


}
#if 0
###############################################33
/*************************************************************/
/*Работа с файлом платежек для передачи в систему Клиент-банк*/
/*************************************************************/
void	klient(const char *nomdok,short dd,short md,short gd,const char *tabl,GtkWidget *wpredok)
{
char	stt[512];
FILE	*ff,*ff1;
char	imaf1[32];
char	imaf[32];
char	bros[512];
short	d,m,g;
char	nomdok1[32];
int	mt=0;
char	strsql[512];

/*
printw("\nklient-%s %d.%d.%d\n",nomdok,dd,md,gd);
refresh();
*/
/*
Читаем комментарий, внутри подпрограммы проверка на длинну комментария и 
выдача предупреждающего сообщения
*/
readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),stt,wpredok);

/*Проверяем нет ли платежки с таким номером*/

sprintf(imaf,"platp.txt");
if((ff = fopen(imaf,"r")) == NULL)
 {
 if(errno == ENOENT)
   goto vp;
  else
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return;
   }
 }
sprintf(imaf1,"ttt%d.tmp",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
   {
    iceb_er_op_fil(imaf1,"",errno,wpredok);
    return;
   }

mt=0;
while(fgets(stt,sizeof(stt),ff) != NULL)
 {
  iceb_u_polen(stt,bros,sizeof(bros),1,'|');
  iceb_u_polen(stt,nomdok1,sizeof(bros),2,'|');
  iceb_u_rsdat(&d,&m,&g,bros,1);      
  if(iceb_u_sravmydat(d,m,g,dd,md,gd) == 0 && iceb_u_SRAV(nomdok,nomdok1,0) == 0)
   {
    iceb_u_str repl;
    sprintf(strsql,gettext("Документ %s уже есть !"),rec.nomdk.ravno());
    repl.plus(strsql);
    repl.ps_plus(gettext("Удаляем старую запись !"));
    iceb_menu_soob(&repl,wpredok);
    mt=1;
    continue;
   }   
  fprintf(ff1,"%s",stt);
 }

fclose(ff);
fclose(ff1);

if(mt == 1)
 {
  unlink(imaf);
  if((ff = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return;
   }

  if((ff1 = fopen(imaf1,"r")) == NULL)
   {
    iceb_er_op_fil(imaf1,"",errno,wpredok);
    return;
   }
  while(fgets(stt,sizeof(stt),ff1) != NULL)
    fprintf(ff,"%s",stt);

  fclose(ff);
  fclose(ff1);
 }

unlink(imaf1);

vp:;
/*
printw(gettext("Запись документа N%s в файл для передачи.\n"),rec.nomdk);
refresh();
*/

if((ff = fopen(imaf,"a")) == NULL)
 {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return;
 }

fprintf(ff,"%02d.%02d.%d|%s\n",dd,md,gd,nomdok);

fclose(ff);

sprintf(strsql,"%s %s",gettext("Документ записан в файл"),imaf);
iceb_menu_soob(strsql,wpredok);

}
#######################################################333
#endif