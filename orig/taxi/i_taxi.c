/*$Id: i_taxi.c,v 1.41 2011-02-21 07:36:20 sasa Exp $*/
/*07.09.2007	24.09.2003	Белых А.И.	i_taxi.c
Программа для работы диспетчеров такси
*/
#define         DVERSIQ "13.07.2007"
#include        <stdio.h>
#include        <stdlib.h>
#include        <time.h>
#include        <pwd.h>
#include        <unistd.h>
#include        <sys/stat.h>
#include        <string.h>
#include        <locale.h>
#include        "taxi.h"
#include        "../iceBw.h"
#include        "car.xpm"
enum
 {
  RABKLIENT,
  POSTKLIENT,
  VODITELI,
  KODZAV,
  ADRECAU,
  OTHET,
  FILNAST,
  VIHOD,
  KOLKNOP
 };
 
struct vmenu_data
 {
  GtkWidget *label;
  GtkWidget *window;
  GtkWidget *hrift;
  int       knopka;
 };

int      glmenu(int);
void     vmenu_knopka(GtkWidget *,struct vmenu_data *);
gboolean vmenu_destroy(GtkWidget *,GdkEvent *,int *);
void     lzaktaxi(void);
//void     ltelef(int);
int l_taxitel(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
void  hrift_des(GtkWidget *,struct vmenu_data *);
void new_hrift(GtkWidget *,struct vmenu_data *);
void     taxi_oth(void);
int      taxinast(void);
extern char		*imabaz;
extern SQL_baza	bd;
extern char            *putnansi;
extern char		*host;

const char		*name_system={NAME_SYSTEM};
const char            *version={VERSION};
extern char *organ;

extern iceb_u_str shrift_rek_raz;
extern short start_god_taxi;
uid_t kod_operatora=getuid(); //Нужен для подпрограмм которые работают с i_rest где код оператора определяестя из файла настройки

#ifdef READ_CARD
//tdcon           td_server; 
#endif

int main(int argc,char **argv)
{
struct  passwd  *ktor; /*Кто работает*/
char		*parol=NULL;
int		regim;
char		kto[20];
int		i=0;
struct  tm      *bf;
time_t          tmm;
int		kom=0;

ktor=getpwuid(getuid());
strcpy(kto,ktor->pw_name);
    
umask(0117); /*Установка маски для записи и чтения группы*/

time(&tmm);
bf=localtime(&tmm);



//gtk_set_locale();
(void)setlocale(LC_ALL,"");
gtk_init( &argc, &argv );

//Устанавливаем переменную обязательно после инициализации GTK
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа

/*Читаем ключи запуска программы*/
iceb_readkey(argc,argv,&parol,&imabaz,&host,&regim);

//printf("parol=%s imabaz=%s\n",parol,imabaz);

/*Определяем путь на файлы настройки*/
if((putnansi=getenv("PUTNANSI")) == NULL)
 {
  i=strlen(CONFIG_PATH)+1;
  putnansi=new char[i];
  strcpy(putnansi,CONFIG_PATH);
 }  


//printf("imabaz=%s host=%s kto=%s parol=%s\n",imabaz,host,kto,parol);
if(sql_openbaz(&bd,imabaz,host,kto,parol) != 0)
    iceb_eropbaz(imabaz,ktor->pw_uid,ktor->pw_name,0);
iceb_start_rf();

//printf("База открыта. %.2f\n",10.33333);
kom=0;
//char klient[20];

if(iceb_nastsys() != 0)
 iceb_exit(1);

if(taxinast() != 0)
 iceb_exit(1);

if(organ == NULL)
 {
  organ=new char[50];
  strcpy(organ,"Диспетчиризация такси");
 }

while( kom >= 0 && kom < 6 )
 {
  kom=glmenu(kom);
//  printf("kom=%d\n",kom);
  class iceb_u_str kod("");
  class iceb_u_str naim("");  
  switch(kom)
   {
    case  RABKLIENT:
     lzaktaxi();
     break;

    case POSTKLIENT:
     l_taxisk(0,&kod,&naim,NULL);
     break;

    case VODITELI:
     l_vod(0,&kod,&naim,NULL);
     break;

    case KODZAV:
      l_taxikz(0,&kod,&naim,NULL);
     break;

    case ADRECAU:
//     ltelef(0);
      l_taxitel(0,&kod,&naim,NULL);
     break;

    case FILNAST:
     iceb_f_redfil("taxinast.alx",2,NULL);
     break;

    case OTHET:
     taxi_oth();
     break;

   }

 }    

//printf("**Уровень вхождения циклов=%d\n",gtk_main_level());

sql_closebaz(&bd);
    
   
return( 0 );

}

/***************************************/
/*Главное меню                         */
/***************************************/
int       glmenu(int knfokus)
{
vmenu_data data;
struct  tm      *bf;
time_t  tmm;
iceb_u_str NADPIS;
char     bros[312];

memset(&data,'\0',sizeof(data));

//printf("glmenu\n");

time(&tmm);
bf=localtime(&tmm);


short ddd=bf->tm_mday;
short mmm=bf->tm_mon+1;
short ggg=bf->tm_year+1900;

iceb_infosys(host,VERSION,DVERSIQ,ddd,mmm,ggg,imabaz,&NADPIS,0);

data.window = gtk_window_new( GTK_WINDOW_TOPLEVEL );

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

sprintf(bros,"%s %s",NAME_SYSTEM,"Диспечеризация такси");
gtk_window_set_title(GTK_WINDOW(data.window),iceb_u_toutf(bros));

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(data.window),10);
/******************
gtk_window_set_default_size(GTK_WINDOW(data.window),
    500, //Горизонталь
    500); //вертикаль
*******************/
//gtk_widget_show(data.window); //Отрисовываем, чтобы можно было взять цвет

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(vmenu_destroy),&data.knopka);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(iceb_vihod),&data.knopka);

GtkWidget *hbox=gtk_hbox_new(FALSE,0);
GtkWidget *vbox=gtk_vbox_new(FALSE,0);
GtkWidget *vbox1=gtk_vbox_new(FALSE,0);

gtk_container_add(GTK_CONTAINER(data.window),hbox);
gtk_container_add(GTK_CONTAINER(hbox),vbox);
gtk_container_add(GTK_CONTAINER(hbox),vbox1);

//Установка ширины окантовки внутренней области   
gtk_container_set_border_width(GTK_CONTAINER(vbox),5);

gtk_widget_realize(data.window);

GtkStyle *style;
GdkPixmap *pixmap;
GdkBitmap *mask;
GtkWidget *pixmapWidget;

style = gtk_widget_get_style(data.window);
pixmap=gdk_pixmap_create_from_xpm_d(data.window->window,&mask,&style->bg[GTK_STATE_NORMAL],(gchar **)car);
pixmapWidget=gtk_pixmap_new(pixmap,mask);
gtk_container_add(GTK_CONTAINER(vbox1),pixmapWidget);

sprintf(bros,"%s\n%s %d","Программа диспечеризации такси","Стартовый год",start_god_taxi);
data.label=gtk_label_new(iceb_u_toutf(bros));
//data.label=gtk_label_new(iceb_u_toutf("Программа диспечеризации такси"));

PangoFontDescription *font_pango=pango_font_description_from_string("Sans 16");
gtk_widget_modify_font(data.label,font_pango);
pango_font_description_free(font_pango);


gtk_box_pack_start((GtkBox*)vbox,data.label,FALSE,FALSE,0);

font_pango=pango_font_description_from_string("Sans 14");

GtkWidget *knopka[KOLKNOP];
GtkWidget *labelk=gtk_label_new(iceb_u_toutf("Работа с клиентами"));

gtk_widget_modify_font(labelk,font_pango);


knopka[RABKLIENT]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[RABKLIENT]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[RABKLIENT],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[RABKLIENT]),"clicked",GTK_SIGNAL_FUNC(vmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[RABKLIENT]),(gpointer)RABKLIENT);

//gtk_widget_modify_text(knopka[RABKLIENT],GTK_STATE_NORMAL,&text);
//gtk_widget_modify_bg(knopka[RABKLIENT],GTK_STATE_NORMAL,&text);
//gtk_widget_modify_fg(knopka[RABKLIENT],GTK_STATE_NORMAL,&text);

labelk=gtk_label_new(iceb_u_toutf("Постоянные клиенты"));
gtk_widget_modify_font(labelk,font_pango);

knopka[POSTKLIENT]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[POSTKLIENT]),labelk);
//knopka[POSTKLIENT]=gtk_button_new_with_label(iceb_u_toutf("Постоянные клиенты"));
gtk_box_pack_start((GtkBox*)vbox,knopka[POSTKLIENT],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[POSTKLIENT]),"clicked",GTK_SIGNAL_FUNC(vmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[POSTKLIENT]),(gpointer)POSTKLIENT);

labelk=gtk_label_new(iceb_u_toutf("Водители"));
gtk_widget_modify_font(labelk,font_pango);
knopka[VODITELI]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[VODITELI]),labelk);
//knopka[VODITELI]=gtk_button_new_with_label(iceb_u_toutf("Водители"));
gtk_box_pack_start((GtkBox*)vbox,knopka[VODITELI],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[VODITELI]),"clicked",GTK_SIGNAL_FUNC(vmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[VODITELI]),(gpointer)VODITELI);

labelk=gtk_label_new(iceb_u_toutf("Коды завершения"));
gtk_widget_modify_font(labelk,font_pango);
knopka[KODZAV]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[KODZAV]),labelk);
//knopka[KODZAV]=gtk_button_new_with_label(iceb_u_toutf("Коды завершения"));
gtk_box_pack_start((GtkBox*)vbox,knopka[KODZAV],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[KODZAV]),"clicked",GTK_SIGNAL_FUNC(vmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[KODZAV]),(gpointer)KODZAV);

labelk=gtk_label_new(iceb_u_toutf("Адреса установки телефонов"));
gtk_widget_modify_font(labelk,font_pango);
knopka[ADRECAU]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[ADRECAU]),labelk);
//knopka[ADRECAU]=gtk_button_new_with_label(iceb_u_toutf("Адреса установки телефонов"));
gtk_box_pack_start((GtkBox*)vbox,knopka[ADRECAU],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[ADRECAU]),"clicked",GTK_SIGNAL_FUNC(vmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[ADRECAU]),(gpointer)ADRECAU);

labelk=gtk_label_new(iceb_u_toutf("Получение отчетов"));
gtk_widget_modify_font(labelk,font_pango);
knopka[OTHET]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[OTHET]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[OTHET],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[OTHET]),"clicked",GTK_SIGNAL_FUNC(vmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[OTHET]),(gpointer)OTHET);

labelk=gtk_label_new(iceb_u_toutf("Установка стартового года"));
gtk_widget_modify_font(labelk,font_pango);
knopka[FILNAST]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[FILNAST]),labelk);
gtk_box_pack_start((GtkBox*)vbox,knopka[FILNAST],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[FILNAST]),"clicked",GTK_SIGNAL_FUNC(vmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[FILNAST]),(gpointer)FILNAST);

/******************
knopka[5]=gtk_button_new_with_label(iceb_u_toutf("Шрифт"));
gtk_box_pack_start((GtkBox*)vbox,knopka[5],FALSE,FALSE,0);
gtk_signal_connect(GTK_OBJECT(knopka[5]),"clicked",GTK_SIGNAL_FUNC(new_hrift),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[5]),(gpointer)"5");
****************/

labelk=gtk_label_new(iceb_u_toutf("Выход"));
gtk_widget_modify_font(labelk,font_pango);
knopka[VIHOD]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(knopka[VIHOD]),labelk);
//knopka[VIHOD]=gtk_button_new_with_label(iceb_u_toutf("Выход"));
gtk_box_pack_start((GtkBox*)vbox,knopka[VIHOD],TRUE,TRUE,0);
gtk_signal_connect(GTK_OBJECT(knopka[VIHOD]),"clicked",GTK_SIGNAL_FUNC(vmenu_knopka),&data);
gtk_object_set_user_data(GTK_OBJECT(knopka[VIHOD]),(gpointer)VIHOD);

GtkWidget *label = gtk_label_new (iceb_u_toutf(NADPIS.ravno()));

gtk_box_pack_start (GTK_BOX (vbox1), label, TRUE, TRUE, 0);

gtk_widget_grab_focus(knopka[knfokus]);

if(shrift_rek_raz.getdlinna() > 1)
 {
  PangoFontDescription *font_pango=pango_font_description_from_string(shrift_rek_raz.ravno());
  gtk_widget_modify_font(label,font_pango);
  pango_font_description_free(font_pango);
 }

gtk_widget_show_all( data.window );
//pango_font_description_free(font_pango);

gtk_main();


return(data.knopka);

}



/***************************/
/*Обработчик нажатия кнопок*/
/***************************/

void   vmenu_knopka(GtkWidget *widget,struct vmenu_data *data)
{

//int knop=GPOINTER_TO_INT(gtk_widget_get_name(widget);
//int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

data->knopka=(int)knop;

gtk_widget_destroy(data->window);


}
/*************************************/
/*Обработчик  сигнала destroy_event*/
/************************************/
gboolean vmenu_destroy(GtkWidget *widget,GdkEvent *event,int *knopka)
{

*knopka=-1;

gtk_widget_destroy(widget);
return(FALSE);
}

/*************************************/

void new_hrift(GtkWidget *widget,struct vmenu_data *data)
{

data->hrift=gtk_font_selection_dialog_new(iceb_u_toutf("Выбор шрифта"));
g_return_if_fail(GTK_IS_FONT_SELECTION_DIALOG(data->hrift));

gtk_signal_connect(GTK_OBJECT (GTK_FONT_SELECTION_DIALOG(data->hrift)->ok_button), "clicked",\
GTK_SIGNAL_FUNC(hrift_des),data);

//gtk_signal_connect(GTK_OBJECT (GTK_FONT_SELECTION_DIALOG(data->hrift)->cancel_button), "clicked",
//GTK_SIGNAL_FUNC(hrift_des),data);

gtk_widget_show(data->hrift);
gtk_widget_show(GTK_FONT_SELECTION_DIALOG(data->hrift)->apply_button);


}
/****************************/

void  hrift_des(GtkWidget *widget,struct vmenu_data *data)
{
printf("hrift\n");
PangoFontDescription *font_pango;
gchar *name_font=gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(data->hrift));
printf("Имя шрифта=%s\n",name_font);

font_pango=pango_font_description_from_string(name_font);
gtk_widget_modify_font(data->window,font_pango);
pango_font_description_free(font_pango);

//if(gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(data->hrift),name_font) == TRUE)
// printf("Шрифт найден\n");

//gtk_widget_pop_composite_child();
//gtk_widget_push_composite_child();

/********************************

//GdkFont *fontn=gdk_font_load(name_font);
GdkFont *fontn=gtk_font_selection_dialog_get_font(GTK_FONT_SELECTION_DIALOG(data->hrift));
if(fontn != NULL);
 printf("111111111\n");
//GtkStyle *oldStyle=gtk_widget_get_style(data->window);
GtkStyle *oldStyle=gtk_widget_get_default_style();
//GtkStyle *newStyle=gtk_style_copy(oldStyle);

//newStyle->font_desc=(PangoFontDescription*)fontn;
//gtk_widget_set_style(data->window,newStyle);
oldStyle->font_desc=(PangoFontDescription*)fontn;

//gtk_widget_set_default_style(oldStyle);
//gtk_widget_set_default_style(oldStyle);
gtk_widget_pop_composite_child();
gtk_widget_push_composite_child();
//gtk_style_unref(oldStyle);

gtk_widget_show_all(data->window);


*******************/
gtk_widget_destroy(data->hrift);
printf("hrift\n");

}
