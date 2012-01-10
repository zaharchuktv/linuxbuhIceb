/*$Id: i_pros_vv.c,v 1.14 2011-02-21 07:36:20 sasa Exp $*/
/*17.08.2006	17.08.2006	Белых А.И.	i_pros_vv.c
Просмотр записей входов-выходов
*/
#define         DVERSIQ "02.09.2006"
#include "i_rest.h"
#include <unistd.h>
#include        "../buhg_g/icebr200.xpm"
#include        "../buhg_g/flag.xpm"

enum
 {
  PROS_ZAP,
  UD_ZAP,
  PROS_ZAP_KAS,
  VIHOD,
  KOLKNOP
 };

class glmenu_data
 {
  public:
  
  GtkWidget *window;
  GtkWidget *knopka[KOLKNOP];
  short     nomkn;

  glmenu_data() // Конструктор
   {
    nomkn=-1;
   }
 };
void  glmenu_knopka(GtkWidget *widget,class glmenu_data *data);
gboolean   glmenu_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data);
void	glmenu(int *kom);

void l_vhvi(void);
void ud_zap_vv(void);
void l_spis_opl_z(int metka_ud_zap,GtkWidget *wpredok);

extern SQL_baza	bd;

const char		*name_system={NAME_SYSTEM};
extern char		*imabaz;
extern char            *putnansi;
extern char		*organ;
extern char		*host ;
extern char		*parol;
const char            *version={VERSION};
extern iceb_u_str shrift_rek_raz;
uid_t kod_operatora=getuid();

int main(int argc,char **argv)
{
int		kom=0;

iceb_start(argc,argv);

if(iceb_nastsys() != 0)
 iceb_exit(1);
//if(buhnast_g() != 0)
// iceb_exit(1);
if(iceb_perzap(1,NULL))
 iceb_exit(1);
 
while( kom >= 0 && kom < VIHOD )   
 {
  glmenu(&kom);
  printf("kom=%d\n",kom);
  switch(kom)
   {
    case PROS_ZAP:
      l_vhvi();
      break;

    case UD_ZAP:
      ud_zap_vv();
      break;

  case PROS_ZAP_KAS: //Просмотр списка записей по кассе
      l_spis_opl_z(1,NULL);
      break;

    case VIHOD:
      
      break;


   }


 }    

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );
}

/********************************/
/*Главное меню*/
/********************************/

void	glmenu(int *kom)
{
iceb_u_str NADPIS;
class glmenu_data data;
char       bros[300];
short		dd,mm,gg;
iceb_u_poltekdat(&dd,&mm,&gg);

iceb_infosys(host,VERSION,DVERSIQ,dd,mm,gg,imabaz,&NADPIS,0);

data.window=gtk_window_new( GTK_WINDOW_TOPLEVEL );
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,gettext("Расчёт заработной платы"));
gtk_window_set_title(GTK_WINDOW(data.window),iceb_u_toutf(bros));

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
/******************
gtk_window_set_default_size(GTK_WINDOW(data.window),
    500, //Горизонталь
    500); //вертикаль
*******************/
//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(event_delete),&KNOP);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(glmenu_key_press),&data);

GtkWidget *hbox=gtk_hbox_new(FALSE,0);
gtk_container_add(GTK_CONTAINER(data.window),hbox);

GtkWidget *vbox=gtk_vbox_new(FALSE,2);
gtk_container_add(GTK_CONTAINER(hbox),vbox);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
gtk_widget_realize(data.window);
//gtk_widget_show(data.window);

GtkWidget *vbox1=gtk_vbox_new(FALSE,5);
GtkStyle *style;
GdkPixmap *pixmap;
GdkBitmap *mask;
GtkWidget *pixmapWidget;

style = gtk_widget_get_style(data.window);
pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],
                                    (gchar **)icebr200_xpm);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(vbox1),pixmapWidget);

pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],
                                    (gchar **)flag_xpm);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(vbox1),pixmapWidget);

gtk_container_add(GTK_CONTAINER(hbox),vbox1);

GtkWidget *label=gtk_label_new(gettext("Просмотр записей на вход/выход"));
//gdk_color_parse("white",&color);
//gtk_widget_modify_fg(label,GTK_STATE_NORMAL,&color);
gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);

memset(bros,'\0',sizeof(bros));
strncpy(bros,organ,40);
label=gtk_label_new(iceb_u_toutf(bros));
//gtk_widget_modify_fg(label,GTK_STATE_NORMAL,&color);
gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);

data.knopka[PROS_ZAP]=gtk_button_new_with_label(gettext("Просмотр записей на вход/выход"));
gtk_signal_connect(GTK_OBJECT(data.knopka[PROS_ZAP]),"clicked",GTK_SIGNAL_FUNC(glmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PROS_ZAP]),(gpointer)PROS_ZAP);
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[PROS_ZAP], TRUE, TRUE, 0);

data.knopka[UD_ZAP]=gtk_button_new_with_label(gettext("Удаление записей"));
gtk_signal_connect(GTK_OBJECT(data.knopka[UD_ZAP]),"clicked",GTK_SIGNAL_FUNC(glmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[UD_ZAP]),(gpointer)UD_ZAP);
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[UD_ZAP], TRUE, TRUE, 0);

data.knopka[PROS_ZAP_KAS]=gtk_button_new_with_label(gettext("Просмотр списка оплат"));
gtk_signal_connect(GTK_OBJECT(data.knopka[PROS_ZAP_KAS]),"clicked",GTK_SIGNAL_FUNC(glmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[PROS_ZAP_KAS]),(gpointer)PROS_ZAP_KAS);
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[PROS_ZAP_KAS], TRUE, TRUE, 0);


data.knopka[VIHOD]=gtk_button_new_with_label(gettext("Выход"));
gtk_signal_connect(GTK_OBJECT(data.knopka[VIHOD]),"clicked",GTK_SIGNAL_FUNC(glmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(data.knopka[VIHOD]),(gpointer)VIHOD);
gtk_box_pack_start(GTK_BOX(vbox), data.knopka[VIHOD], TRUE, TRUE,0);

label = gtk_label_new (iceb_u_toutf(NADPIS.ravno()));
//gdk_color_parse("white",&color);
//gtk_widget_modify_fg(labelinfo,GTK_STATE_NORMAL,&color);

gtk_box_pack_start (GTK_BOX (vbox1), label, TRUE, TRUE, 1);

if(shrift_rek_raz.getdlinna() > 1)
 {
  PangoFontDescription *font_pango=pango_font_description_from_string(shrift_rek_raz.ravno());
  gtk_widget_modify_font(label,font_pango);
  pango_font_description_free(font_pango);
 }

gtk_widget_grab_focus(data.knopka[*kom]);

gtk_widget_show_all (data.window);


gtk_main();

*kom=data.nomkn;

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  glmenu_knopka(GtkWidget *widget,class glmenu_data *data)
{

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->nomkn=knop;
gtk_widget_destroy(data->window);
data->window=NULL;

}



/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   glmenu_key_press(GtkWidget *widget,GdkEventKey *event,class glmenu_data *data)
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
