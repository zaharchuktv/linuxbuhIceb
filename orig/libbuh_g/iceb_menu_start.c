/*$Id: iceb_menu_start.c,v 1.8 2011-02-21 07:36:07 sasa Exp $*/
/*10.10.2010	13.04.2010	Белых А.И.	iceb_menu_start.c
Главное меню подсистем
Возвращает номер нажатой клавиши
Если нажали выход возвращает -1
*/
#include        <vector>
#include        <stdlib.h>
#include        <pwd.h>
#include        <unistd.h>
#include "iceb_libbuh.h"
#include "../knopki/iceBw.xpm"
#include "../knopki/flag_ua.xpm"
enum
 {
  SMENABAZ,
  VIHOD,
  SMENAPRINT,
  OPROG,
  COPYRIGTH,
  KOLKNOP
 };

class glmenu_data
 {
  public:
  
  GtkWidget *window;
  GtkWidget *label_nameprinter;
  GtkWidget *knopka[KOLKNOP];
  int     nomkn;

  glmenu_data() // Конструктор
   {
    nomkn=-1;
   }
 };

void iceb_fotoraz(GtkWidget *wpredok);

extern char		*imabaz;
extern class iceb_u_str kodirovka_iceb;
extern char		*organ;
extern const char	*name_system;

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_menu_start_knopka(GtkWidget *widget,class glmenu_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("glkni_v_knopka knop=%d\n",knop);
data->nomkn=knop;
if(knop == (OPROG*-1))
 {
  gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  iceb_about();
  gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR));
  return;
 }

if(knop == (SMENAPRINT*-1))
 {
  iceb_l_printcap(data->window);
  char    *nameprinter=NULL;
  char bros[512];
  nameprinter = getenv("PRINTER");
  if(nameprinter != NULL)
   sprintf(bros,"%-.*s",iceb_u_kolbait(20,nameprinter),nameprinter);
  else
   sprintf(bros,"%-.*s",iceb_u_kolbait(20,gettext("По умолчанию")),gettext("По умолчанию"));
  gtk_label_set_text(GTK_LABEL(data->label_nameprinter),iceb_u_toutf(bros));
  return;
 }

if(knop == (COPYRIGTH*-1))
 {
  iceb_fotoraz(data->window);
  return;
 }
gtk_widget_destroy(data->window);
data->window=NULL;

}



/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   iceb_menu_start_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data)
{

switch(event->keyval)
 {

    
  case GDK_Escape:
  case GDK_F10:
    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[VIHOD]),"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

//    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}


/**********************************/
int iceb_menu_start(int nomstr_menu,
const char *version,
const char *dataver,
const char *naim_pods,
const char *fioop,
int nom_op,
class iceb_u_spisok *menustr,
const char *cvet_fona)
{
//struct  passwd  *ktor=getpwuid(getuid());; /*Кто работает*/
//char    *nameprinter=NULL;
glmenu_data data;
char       bros[512];
GdkColor color;



data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
//gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);


sprintf(bros,"%s %s",name_system,naim_pods);
gtk_window_set_title(GTK_WINDOW(data.window),iceb_u_toutf(bros));

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),0);
gtk_window_set_default_size(GTK_WINDOW(data.window),367,550); //горизонталь-вертикаль
//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(event_delete),&KNOP);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_menu_start_key_press),&data);


GtkWidget *vbox=gtk_vbox_new(FALSE,0);
gtk_container_add(GTK_CONTAINER(data.window),vbox);

GtkWidget *hbox=gtk_hbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE,0);


gtk_widget_realize(data.window);
//gdk_color_parse("#0080C9",&color);
gdk_color_parse(cvet_fona,&color);
gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);

GtkWidget *event_ris=gtk_event_box_new();
gtk_box_pack_start(GTK_BOX(hbox),event_ris,FALSE,FALSE,0);
gdk_color_parse("white",&color);
gtk_widget_modify_bg(event_ris,GTK_STATE_NORMAL,&color);

GtkStyle *style;
GdkPixmap *pixmap;
GdkBitmap *mask;
GtkWidget *pixmapWidget;

style = gtk_widget_get_style(data.window);
pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)iceBw);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(event_ris),pixmapWidget);

GtkWidget *event_text=gtk_event_box_new();
gtk_box_pack_start(GTK_BOX(hbox),event_text,TRUE,TRUE,0);
gdk_color_parse("white",&color);
gtk_widget_modify_bg(event_text,GTK_STATE_NORMAL,&color);

GtkWidget *vbox_text=gtk_vbox_new(FALSE,0);
gtk_container_add(GTK_CONTAINER(event_text),vbox_text);

GtkWidget *label=gtk_label_new("");
gtk_box_pack_start(GTK_BOX(vbox_text), label, FALSE, FALSE,0);

GtkWidget *hbox_text=gtk_hbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(vbox_text), hbox_text, FALSE, FALSE,0);

label=gtk_label_new(iceb_u_toutf(naim_pods));
gtk_box_pack_start(GTK_BOX(hbox_text), label, FALSE, FALSE,0);

PangoFontDescription *font_pango=pango_font_description_from_string("Sans 16");
gtk_widget_modify_font(label,font_pango);
pango_font_description_free(font_pango);

GtkWidget *event_liniq=gtk_event_box_new();
gdk_color_parse("#0E76BC",&color);
gtk_widget_modify_bg(event_liniq,GTK_STATE_NORMAL,&color);
gtk_box_pack_start(GTK_BOX(vbox_text), event_liniq, FALSE, FALSE,0);
gtk_widget_set_usize(GTK_WIDGET(event_liniq),-1,3);

GtkWidget *hbox_text1=gtk_hbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(vbox_text), hbox_text1, FALSE, FALSE,0);
label=gtk_label_new(iceb_u_toutf(organ));
gtk_box_pack_start(GTK_BOX(hbox_text1), label, FALSE, FALSE,0);

label=gtk_label_new("");
gtk_box_pack_start(GTK_BOX(vbox_text), label, FALSE, FALSE,0);


GtkWidget *hbox_text2=gtk_hbox_new(FALSE,5);
gtk_box_pack_start(GTK_BOX(vbox_text), hbox_text2, FALSE, FALSE,0);

GtkWidget *vbox_text1=gtk_vbox_new(TRUE,0);
GtkWidget *vbox_text2=gtk_vbox_new(TRUE,0);
gtk_box_pack_start(GTK_BOX(hbox_text2), vbox_text1, FALSE, FALSE,0);
gtk_box_pack_start(GTK_BOX(hbox_text2), vbox_text2, FALSE, FALSE,0);



sprintf(bros,"%s:",gettext("Оператор"));

label=gtk_label_new(iceb_u_toutf(bros));
gtk_box_pack_start(GTK_BOX(vbox_text1), label, FALSE, FALSE,0);

//font_pango=pango_font_description_from_string("Monospace 12");
font_pango=pango_font_description_from_string("Sans 10");
gtk_widget_modify_font(label,font_pango);

//sprintf(bros,"%d %s %-.*s",ktor->pw_uid,ktor->pw_name,iceb_u_kolbait(20,ktor->pw_gecos),ktor->pw_gecos);
//sprintf(bros,"%d %s %-.*s",iceb_getuid(NULL),iceb_u_getlogin(),iceb_u_kolbait(20,iceb_getfioop(NULL)),iceb_getfioop(NULL));
sprintf(bros,"%d %s %-.*s",nom_op,iceb_u_getlogin(),iceb_u_kolbait(20,fioop),fioop);

label=gtk_label_new(iceb_u_toutf(bros));
gtk_box_pack_start(GTK_BOX(vbox_text2), label, FALSE, FALSE,0);


//Кнопки
GtkTooltips *tooltips[KOLKNOP];

data.knopka[SMENABAZ]=gtk_button_new_with_label(gettext("База"));
gtk_button_set_alignment(GTK_BUTTON(data.knopka[SMENABAZ]),0.,0.); /*Левое выравнивание текста*/
gtk_signal_connect(GTK_OBJECT(data.knopka[SMENABAZ]),"clicked",GTK_SIGNAL_FUNC(iceb_menu_start_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SMENABAZ]),(gpointer)SMENABAZ);
gtk_box_pack_start(GTK_BOX(vbox_text1), data.knopka[SMENABAZ], FALSE, FALSE, 0);
tooltips[SMENABAZ]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SMENABAZ],data.knopka[SMENABAZ],gettext("Сменить базу"),NULL);

GtkWidget *hbox_v_text1=gtk_hbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(vbox_text2), hbox_v_text1, FALSE, FALSE,0);

sprintf(bros,"%s/%s",imabaz,kodirovka_iceb.ravno());

label=gtk_label_new(iceb_u_toutf(bros));
//gtk_box_pack_start(GTK_BOX(vbox_text2), label, FALSE, FALSE,0);
gtk_box_pack_start(GTK_BOX(hbox_v_text1), label, FALSE, FALSE,0);
gtk_widget_modify_font(label,font_pango);



#if 0
####################33333
data.knopka[SMENAPRINT]=gtk_button_new_with_label(gettext("Принтер"));
gtk_button_set_alignment(GTK_BUTTON(data.knopka[SMENAPRINT]),0.,0.); /*Левое выравнивание текста*/
gtk_signal_connect(GTK_OBJECT(data.knopka[SMENAPRINT]),"clicked",GTK_SIGNAL_FUNC(iceb_menu_start_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[SMENAPRINT]),(gpointer)(SMENAPRINT*-1));
gtk_box_pack_start(GTK_BOX(vbox_text1), data.knopka[SMENAPRINT], FALSE, FALSE, 0);
tooltips[SMENAPRINT]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[SMENAPRINT],data.knopka[SMENAPRINT],gettext("Сменить принтер"),NULL);
GtkWidget *hbox_v_text2=gtk_hbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(vbox_text2), hbox_v_text2, FALSE, FALSE,0);

nameprinter = getenv("PRINTER");
if(nameprinter != NULL)
 sprintf(bros,"%-.*s",iceb_u_kolbait(20,nameprinter),nameprinter);
else
 sprintf(bros,"%-.*s",iceb_u_kolbait(20,gettext("По умолчанию")),gettext("По умолчанию"));

data.label_nameprinter=gtk_label_new(iceb_u_toutf(bros));

gtk_box_pack_start(GTK_BOX(hbox_v_text2), data.label_nameprinter, FALSE, FALSE,0);
gtk_widget_modify_font(label,font_pango);
#########################333
#endif

/*разделитель между шапкой и кнопками*/
GtkWidget *event_razd=gtk_event_box_new();
gtk_box_pack_start(GTK_BOX(vbox),event_razd,FALSE,FALSE,0);
gdk_color_parse("#E7E7E8",&color);
gtk_widget_modify_bg(event_razd,GTK_STATE_NORMAL,&color);
//gtk_widget_set_usize(GTK_WIDGET(event_razd),-1,25);
gtk_widget_set_size_request(GTK_WIDGET(event_razd),-1,25);

GtkWidget *hbox_razd=gtk_hbox_new(FALSE,0);

gtk_container_add(GTK_CONTAINER(event_razd),hbox_razd);

sprintf(bros,"%s %s %s %s %s",gettext("Версия"),version,gettext("от"),dataver,gettext("г."));
label=gtk_label_new(iceb_u_toutf(bros));
gtk_box_pack_start(GTK_BOX(hbox_razd), label, FALSE, FALSE,10);
gtk_widget_modify_font(label,font_pango);

label=gtk_label_new(gettext("О программе"));
gtk_widget_modify_font(label,font_pango);
data.knopka[OPROG]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data.knopka[OPROG]),label);
gtk_button_set_alignment(GTK_BUTTON(data.knopka[OPROG]),0.,0.); /*Левое выравнивание текста*/
gtk_button_set_relief(GTK_BUTTON(data.knopka[OPROG]),GTK_RELIEF_NONE);
gtk_signal_connect(GTK_OBJECT(data.knopka[OPROG]),"clicked",GTK_SIGNAL_FUNC(iceb_menu_start_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[OPROG]),(gpointer)(OPROG*-1));
gtk_box_pack_end(GTK_BOX(hbox_razd),data.knopka[OPROG], FALSE, FALSE,0);

GtkWidget *hbox_knop=gtk_hbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(vbox), hbox_knop, TRUE, TRUE,10);

GtkWidget *vbox_knop=gtk_vbox_new(FALSE,0);
gtk_box_pack_start(GTK_BOX(hbox_knop), vbox_knop, FALSE, FALSE,10);

int kolih_str_menu=menustr->kolih();

using namespace std;
vector <GtkWidget*> knopka(kolih_str_menu);

for(int nom=0; nom < kolih_str_menu; nom++)
 {
  knopka[nom]=gtk_button_new_with_label(iceb_u_toutf(menustr->ravno(nom)));
  gtk_button_set_alignment(GTK_BUTTON(knopka[nom]),0.,0.); /*Левое выравнивание текста*/
  gtk_signal_connect(GTK_OBJECT(knopka[nom]),"clicked",GTK_SIGNAL_FUNC(iceb_menu_start_knopka),&data);
  gtk_object_set_user_data(GTK_OBJECT(knopka[nom]),(gpointer)(nom+1));
  gtk_box_pack_start(GTK_BOX(vbox_knop),knopka[nom], FALSE, FALSE,0);
 }

data.knopka[VIHOD]=gtk_button_new_with_label(gettext("Выход"));
gtk_button_set_alignment(GTK_BUTTON(data.knopka[VIHOD]),0.,0.); /*Левое выравнивание текста*/
gtk_signal_connect(GTK_OBJECT(data.knopka[VIHOD]),"clicked",GTK_SIGNAL_FUNC(iceb_menu_start_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[VIHOD]),(gpointer)(VIHOD*-1));
gtk_box_pack_end(GTK_BOX(vbox_knop), data.knopka[VIHOD], FALSE, FALSE, 0);

if(nomstr_menu-VIHOD < 0)
 gtk_widget_grab_focus(knopka[0]);
else
 if(nomstr_menu-VIHOD >= 0 && nomstr_menu-VIHOD < kolih_str_menu)
   gtk_widget_grab_focus(knopka[nomstr_menu-VIHOD]);

/*завершающая строка*/
GtkWidget *event_end=gtk_event_box_new();
gtk_box_pack_end(GTK_BOX(vbox),event_end,FALSE,FALSE,0);
gdk_color_parse("#000000",&color);
gtk_widget_modify_bg(event_end,GTK_STATE_NORMAL,&color);
gtk_widget_set_size_request(GTK_WIDGET(event_end),-1,25);

GtkWidget *hbox_end=gtk_hbox_new(FALSE,0);
gtk_container_add(GTK_CONTAINER(event_end),hbox_end);

pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)flag_ua_xpm);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_box_pack_start(GTK_BOX(hbox_end), pixmapWidget, FALSE, FALSE,10);



data.knopka[COPYRIGTH]=gtk_button_new();
gtk_box_pack_start(GTK_BOX(hbox_end), data.knopka[COPYRIGTH], FALSE, FALSE,0);
gtk_button_set_alignment(GTK_BUTTON(data.knopka[COPYRIGTH]),0.,0.); /*Левое выравнивание текста*/
gtk_button_set_relief(GTK_BUTTON(data.knopka[COPYRIGTH]),GTK_RELIEF_NONE);
gtk_signal_connect(GTK_OBJECT(data.knopka[COPYRIGTH]),"clicked",GTK_SIGNAL_FUNC(iceb_menu_start_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[COPYRIGTH]),(gpointer)(COPYRIGTH*-1));
tooltips[COPYRIGTH]=gtk_tooltips_new();
gtk_tooltips_set_tip(tooltips[COPYRIGTH],data.knopka[COPYRIGTH],gettext("Просмотр фотографии разработчика"),NULL);

//sprintf(bros,"Copyright \302\251 Aleksandr Belykh 2010");
sprintf(bros,"Copyright © Aleksandr Belykh 2010");
label=gtk_label_new(iceb_u_toutf(bros));
gdk_color_parse("white",&color);
gtk_widget_modify_fg(label,GTK_STATE_NORMAL,&color);
gtk_widget_modify_font(label,font_pango);
gtk_container_add(GTK_CONTAINER(data.knopka[COPYRIGTH]),label);

pango_font_description_free(font_pango);

gtk_widget_show_all (data.window);

gtk_main();

return(data.nomkn);
}

