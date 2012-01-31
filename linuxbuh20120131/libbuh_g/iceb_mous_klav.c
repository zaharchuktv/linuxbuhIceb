/*$Id: iceb_mous_klav.c,v 1.42 2011-02-21 07:36:07 sasa Exp $*/
/*06.11.2007	06.12.2004	Белых А.И.	iceb_mous_klav.c
Клавиатурное поле для ввода цифр
*/
#include "iceb_libbuh.h"
#define  SHRIFT_MOUSE_KLAV "Sans, 40"
enum
 {
  KL0,
  KL1,
  KL2,
  KL3,
  KL4,
  KL5,
  KL6,
  KL7,
  KL8,
  KL9,
  KL_TH,  //Точка
  KL_ZP,  //ЗАПЯТАЯ
  KOL_F_KL
 };

enum
 {
  F_VLEVO,
  F_VPRAVO,
  F_ZAB,
  F_VVOD,
  F_VIHOD,
  KOL_FFKL
 };
 
enum
 {
  KL_1,  //А
  KL_2,  //Б
  KL_3,  //В
  KL_4,  //Г
  KL_5,  //Д
  KL_6,  //Е
  KL_7, //Ё
  KL_8, //Ж
  KL_9,  //З
  KL_10, //И
  KL_11,//Й
  KL_12,  //К
  KL_13,  //Л
  KL_14,  //М
  KL_15,  //Н
  KL_16,  //О
  KL_17,  //П
  KL_18,  //Р
  KL_19,  //C
  KL_20,  //T
  KL_21,  //У
  KL_22,  //Ф
  KL_23,  //X
  KL_24,  //Ц
  KL_25, //Ч
  KL_26,  //Ш
  KL_27, //Щ
  KL_28, //Ъ
  KL_29, //Ы
  KL_30, //Ь
  KL_31, //Э
  KL_32, //Ю
  KL_33, //Я

  KL_34,  //І
  KL_35, //Ї
  KL_36, //Є
  
  KL_PROBEL,
  KL_CAPS_LOCK,  
  KL_RUS_LAT,
  KOL_BUKV_ZNAK
 };
 

class mouse_klav_v_data
 {
  public:
  
  GtkWidget *entry;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *label_b[KOL_BUKV_ZNAK];
  short kl_shift;
  short rus_lat;
  int voz; 
  guint     timer;
  int metka_read_card;  
  class iceb_u_str stroka_data;  

  
  mouse_klav_v_data() //Конструктор
   {
    kl_shift=0; //0-малые буквы 1-большие
    rus_lat=0;  //0-русские буквы 1-латинские
    voz=1;
       
   }
};


void  mouse_klav_v_f_knopka(GtkWidget *widget,class mouse_klav_v_data *data);
void  mouse_klav_v_knopka(GtkWidget *widget,class mouse_klav_v_data *data);
void  mouse_klav_v_b_knopka(GtkWidget *widget,class mouse_klav_v_data *data);
void  mouse_klav_v_vvod(GtkWidget *widget,class mouse_klav_v_data *data);

void mouse_klav_cifr(GtkWidget *vbox_cif,class mouse_klav_v_data *data);
void mouse_klav_fk(GtkWidget *hbox_fk,class mouse_klav_v_data *data);
gboolean   mouse_klav_v_key_press(GtkWidget *widget,GdkEventKey *event,class mouse_klav_v_data *data);
void mouse_klav_bukva(GtkWidget *vbox_bukv,class mouse_klav_v_data *data);
void mouse_klav_smena_b(class mouse_klav_v_data *data);

void iceb_mous_klav_rc(class mouse_klav_v_data *data);

extern class iceb_get_dev_data config_dev;
 
int  iceb_mous_klav(char *nadpis,
class iceb_u_str *stroka_data,
int max_dlinna_str,
int metka_r, //0-только цифровая клавиатура 1-цифровая и буквенная
int metka_parol, //0-ввод обычных данных 1-ввод пароля
int metka_read_card, //0-без чтения карточек 1-чтение карточек
GtkWidget *wpredok)
{
class mouse_klav_v_data data;
data.stroka_data.new_plus(stroka_data->ravno());
data.metka_read_card=metka_read_card;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

gtk_window_set_title (GTK_WINDOW (data.window),iceb_u_toutf(nadpis));
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
gtk_signal_connect_after(GTK_OBJECT(data.window),"key_press_event",GTK_SIGNAL_FUNC(mouse_klav_v_key_press),&data);

GtkWidget *label;

label=gtk_label_new(iceb_u_toutf(nadpis));

PangoFontDescription *font_pango=pango_font_description_from_string(SHRIFT_MOUSE_KLAV);

gtk_widget_modify_font(label,font_pango);


GtkWidget *vbox = gtk_vbox_new (FALSE, 2);
gtk_container_add (GTK_CONTAINER (data.window), vbox);
//gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_box_pack_start (GTK_BOX (vbox),label, FALSE, TRUE, 2);

data.entry = gtk_entry_new_with_max_length (max_dlinna_str);
if(metka_parol == 1)
 gtk_entry_set_visibility(GTK_ENTRY(data.entry),FALSE); //ввод пароля

gtk_widget_modify_font(data.entry,font_pango);
pango_font_description_free(font_pango);


gtk_box_pack_start (GTK_BOX (vbox), data.entry, FALSE, TRUE, 2);
gtk_signal_connect(GTK_OBJECT (data.entry), "activate",GTK_SIGNAL_FUNC(mouse_klav_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry),data.stroka_data.ravno_toutf());
//gtk_editable_select_region(GTK_EDITABLE(data.entry),0,-1); //Закрасить для редактирования

GtkWidget *hbox_buk_cif; //Бокс для букв и цифр
hbox_buk_cif = gtk_hbox_new (FALSE, 2);

if(metka_r == 1)
 {
  //Буквы
  GtkWidget *vbox_bukv = gtk_vbox_new (FALSE, 0);
  mouse_klav_bukva(vbox_bukv,&data);
  //gtk_container_add (GTK_CONTAINER (hbox_buk_cif),vbox_bukv);
  gtk_box_pack_start (GTK_BOX (hbox_buk_cif),vbox_bukv, TRUE, TRUE, 2);
 }
//Цифры
GtkWidget *vbox_cif = gtk_vbox_new (FALSE, 0);
mouse_klav_cifr(vbox_cif,&data);
//gtk_container_add (GTK_CONTAINER (hbox_buk_cif),vbox_cif);
gtk_box_pack_start (GTK_BOX (hbox_buk_cif),vbox_cif, TRUE, TRUE, 2);

//gtk_container_add (GTK_CONTAINER (vbox),hbox_buk_cif);
gtk_box_pack_start (GTK_BOX (vbox),hbox_buk_cif, TRUE, TRUE, 2);

//формируем строку функциональных клавиш
GtkWidget *hbox_fk;
hbox_fk = gtk_hbox_new (FALSE, 0);
mouse_klav_fk(hbox_fk,&data);

//gtk_container_add (GTK_CONTAINER (vbox),hbox_fk);
gtk_box_pack_start (GTK_BOX (vbox),hbox_fk, TRUE, TRUE, 2);


gtk_widget_show_all (data.window);


gtk_editable_select_region(GTK_EDITABLE(data.entry),0,-1); //Закрасить для редактирования
gtk_widget_grab_focus(data.entry);

#ifdef READ_CARD
extern tdcon           td_server; 
if(td_server < 0)
  data.metka_read_card=0; //Не соединились с сервером
int metka_cl=0;
if(data.metka_read_card == 1)
 {
//  if(iceb_connect_dserver(config_dev.ima_komp.ravno(),config_dev.name_dev_card.ravno()) == 0)
//   {
    metka_cl=1;
    class iceb_read_card_enter_data card;
    card.window=data.window;
    card.entry=data.entry;
    data.timer=card.timer=gtk_timeout_add(500,(GtkFunction)iceb_read_card_enter,&card);
//   }
 } 
#endif

//gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();

if(data.voz == 0)
 stroka_data->new_plus(data.stroka_data.ravno());
 
//if(metka_cl == 1)
//    iceb_close_dserver();
if(wpredok != NULL)
  gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR));

return(data.voz);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  mouse_klav_v_knopka(GtkWidget *widget,class mouse_klav_v_data *data)
{
gint pozic=gtk_editable_get_position(GTK_EDITABLE(data->entry));

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));

gint start=0; //позиция начала выделения
gint end=0;   //позиция конца выделения

gtk_editable_get_selection_bounds(GTK_EDITABLE(data->entry),&start,&end);
//printf("staft=%d end=%d\n",start,end);

if(end > start)  //если есть выделеный текст
 {
  
  gtk_entry_set_text(GTK_ENTRY(data->entry),"");
  pozic=0;
 }

//printf("***maus_klav_v_knopka knop=%d pozic=%d\n",knop,pozic);

switch (knop)
 {
  case KL0:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),"0",1,&pozic);
    break;

  case KL1:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),"1",1,&pozic);
    break;


  case KL2:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),"2",1,&pozic);
    break;

  case KL3:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),"3",1,&pozic);
    break;

  case KL4:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),"4",1,&pozic);
    break;

  case KL5:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),"5",1,&pozic);
    break;

  case KL6:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),"6",1,&pozic);
    break;

  case KL7:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),"7",1,&pozic);
    break;

  case KL8:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),"8",1,&pozic);
    break;

  case KL9:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),"9",1,&pozic);
    break;

  case KL_TH:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),".",1,&pozic);
    break;

  case KL_ZP:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),",",1,&pozic);
    break;
 }

//После установки фокуса курсор становится в конец строки
gtk_widget_grab_focus(data->entry);

gtk_entry_set_position(GTK_ENTRY(data->entry),pozic);

}
/*******************************************/
/*Создаём контейнер для цифр               */
/*******************************************/

void mouse_klav_cifr(GtkWidget *vbox_cif,class mouse_klav_v_data *data)
{
#define  KOL_STROK 4

GtkWidget *hbox[KOL_STROK];


for(int i=0; i < KOL_STROK; i++)
 {
  hbox[i] = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox_cif), hbox[i], TRUE, TRUE, 2);
 }

GtkWidget *knopka[KOL_F_KL];

for(int i=0; i < KOL_F_KL; i++)
 {
  knopka[i]=gtk_button_new();
  gtk_signal_connect(GTK_OBJECT(knopka[i]),"clicked",GTK_SIGNAL_FUNC(mouse_klav_v_knopka),data);
  gtk_object_set_user_data(GTK_OBJECT(knopka[i]),(gpointer)i);
 }

GtkWidget *label[KOL_F_KL];

//Первая строка

label[KL7]=gtk_label_new("7");
gtk_container_add(GTK_CONTAINER(knopka[KL7]),label[KL7]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL7], TRUE, TRUE, 2);

label[KL8]=gtk_label_new("8");
gtk_container_add(GTK_CONTAINER(knopka[KL8]),label[KL8]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL8], TRUE, TRUE, 2);

label[KL9]=gtk_label_new("9");
gtk_container_add(GTK_CONTAINER(knopka[KL9]),label[KL9]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL9], TRUE, TRUE, 2);

//Вторая строка
label[KL4]=gtk_label_new("4");
gtk_container_add(GTK_CONTAINER(knopka[KL4]),label[KL4]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL4], TRUE, TRUE, 2);

label[KL5]=gtk_label_new("5");
gtk_container_add(GTK_CONTAINER(knopka[KL5]),label[KL5]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL5], TRUE, TRUE, 2);

label[KL6]=gtk_label_new("6");
gtk_container_add(GTK_CONTAINER(knopka[KL6]),label[KL6]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL6], TRUE, TRUE, 2);

//Третья строка
label[KL1]=gtk_label_new("1");
gtk_container_add(GTK_CONTAINER(knopka[KL1]),label[KL1]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL1], TRUE, TRUE, 2);

label[KL2]=gtk_label_new("2");
gtk_container_add(GTK_CONTAINER(knopka[KL2]),label[KL2]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL2], TRUE, TRUE, 2);

label[KL3]=gtk_label_new("3");
gtk_container_add(GTK_CONTAINER(knopka[KL3]),label[KL3]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL3], TRUE, TRUE, 2);

//Четвёртая строка

label[KL0]=gtk_label_new("0");
gtk_container_add(GTK_CONTAINER(knopka[KL0]),label[KL0]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL0], TRUE, TRUE, 2);

label[KL_TH]=gtk_label_new(".");
gtk_container_add(GTK_CONTAINER(knopka[KL_TH]),label[KL_TH]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_TH], TRUE, TRUE, 2);

label[KL_ZP]=gtk_label_new(",");
gtk_container_add(GTK_CONTAINER(knopka[KL_ZP]),label[KL_ZP]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_ZP], TRUE, TRUE, 2);

PangoFontDescription *font_pango=pango_font_description_from_string(SHRIFT_MOUSE_KLAV);

for(int i=0; i < KOL_F_KL; i++)
  gtk_widget_modify_font(label[i],font_pango);

pango_font_description_free(font_pango);

}
/*****************************************/
/*Чтение из строки ввода*/
/********************************/

void    mouse_klav_v_vvod(GtkWidget *widget,class mouse_klav_v_data *data)
{

//int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("mouse_klav_v_vvod enter=%d\n",enter);
data->stroka_data.new_plus(iceb_u_fromutf((char*)gtk_entry_get_text(GTK_ENTRY(widget))));

#ifdef READ_CARD    
 if(data->metka_read_card == 1)
      gtk_timeout_remove(data->timer);
#endif

data->voz=0;
gtk_widget_destroy(data->window);
}
/******************************/
/*Создаем строку функциональных клавиш*/
/**************************************/
void mouse_klav_fk(GtkWidget *hbox_fk,class mouse_klav_v_data *data)
{

GtkWidget *knopka[KOL_FFKL];
for(int i=0; i < KOL_FFKL; i++)
 {
  knopka[i]=gtk_button_new();
  gtk_signal_connect(GTK_OBJECT(knopka[i]),"clicked",GTK_SIGNAL_FUNC(mouse_klav_v_f_knopka),data);
  gtk_object_set_user_data(GTK_OBJECT(knopka[i]),(gpointer)i);
  gtk_box_pack_start(GTK_BOX(hbox_fk), knopka[i], TRUE, TRUE, 2);
 }
GtkWidget *label[KOL_FFKL];

label[F_VLEVO]=gtk_label_new(iceb_u_toutf("<"));
gtk_container_add(GTK_CONTAINER(knopka[F_VLEVO]),label[F_VLEVO]);

label[F_VPRAVO]=gtk_label_new(iceb_u_toutf(">"));
gtk_container_add(GTK_CONTAINER(knopka[F_VPRAVO]),label[F_VPRAVO]);

label[F_ZAB]=gtk_label_new(iceb_u_toutf("<<"));
gtk_container_add(GTK_CONTAINER(knopka[F_ZAB]),label[F_ZAB]);

label[F_VVOD]=gtk_label_new(gettext("Ввод"));
gtk_container_add(GTK_CONTAINER(knopka[F_VVOD]),label[F_VVOD]);

label[F_VIHOD]=gtk_label_new(gettext("Выход"));
gtk_container_add(GTK_CONTAINER(knopka[F_VIHOD]),label[F_VIHOD]);

PangoFontDescription *font_pango=pango_font_description_from_string(SHRIFT_MOUSE_KLAV);

for(int i=0; i < KOL_FFKL; i++)
  gtk_widget_modify_font(label[i],font_pango);

pango_font_description_free(font_pango);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  mouse_klav_v_f_knopka(GtkWidget *widget,class mouse_klav_v_data *data)
{
int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("*maus_klav_v_f_knopka knop=%d\n",knop);

gint pozic=gtk_editable_get_position(GTK_EDITABLE(data->entry));
//printf("pozic=%d\n",pozic);

gtk_widget_grab_focus(data->entry);

switch (knop)
 {
  case F_VLEVO:
    pozic--;
    break;

  case F_VPRAVO:
    pozic++;
    break;

  case F_ZAB:
    gtk_editable_delete_text(GTK_EDITABLE(data->entry),pozic-1,pozic);
    pozic--;
    break;

  case F_VVOD:
    gtk_signal_emit_by_name(GTK_OBJECT(data->entry),"activate");
    return;
    
  case F_VIHOD:
#ifdef READ_CARD    
    if(data->metka_read_card == 1)
      gtk_timeout_remove(data->timer);
#endif
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;

 }

if(pozic < 0)
 pozic=0;

gtk_editable_set_position(GTK_EDITABLE(data->entry),pozic);


}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mouse_klav_v_key_press(GtkWidget *widget,GdkEventKey *event,class mouse_klav_v_data *data)
{

//printf("mouse_klav_v_key_press\n");
switch(event->keyval)
 {

  case GDK_Escape:
  case GDK_F10:
//    gtk_signal_emit_by_name(GTK_OBJECT(data->knopka[FK10]),"clicked");

#ifdef READ_CARD    
    if(data->metka_read_card == 1)
      gtk_timeout_remove(data->timer);
#endif
    gtk_widget_destroy(data->window);
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*********************************************************/
/*Буквенная клавиатура*/
/***********************************************************/

void mouse_klav_bukva(GtkWidget *vbox_bukv,class mouse_klav_v_data *data)
{

#define  KOL_STROK_B 5

GtkWidget *hbox[KOL_STROK_B];


for(int i=0; i < KOL_STROK_B; i++)
  hbox[i] = gtk_hbox_new (FALSE, 2);

for(int i=0; i < KOL_STROK_B; i++)
 gtk_box_pack_start(GTK_BOX(vbox_bukv), hbox[i], TRUE, TRUE, 2);



GtkWidget *knopka[KOL_BUKV_ZNAK];

for(int i=0; i < KOL_BUKV_ZNAK; i++)
 {
  knopka[i]=gtk_button_new();
  gtk_signal_connect(GTK_OBJECT(knopka[i]),"clicked",GTK_SIGNAL_FUNC(mouse_klav_v_b_knopka),data);
  gtk_object_set_user_data(GTK_OBJECT(knopka[i]),(gpointer)i);
 }
//Первая строка
data->label_b[KL_1]=gtk_label_new(iceb_u_toutf("а"));
gtk_container_add(GTK_CONTAINER(knopka[KL_1]),data->label_b[KL_1]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL_1],TRUE,TRUE, 2);

data->label_b[KL_2]=gtk_label_new(iceb_u_toutf("б"));
gtk_container_add(GTK_CONTAINER(knopka[KL_2]),data->label_b[KL_2]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL_2],TRUE,TRUE, 2);

data->label_b[KL_3]=gtk_label_new(iceb_u_toutf("в"));
gtk_container_add(GTK_CONTAINER(knopka[KL_3]),data->label_b[KL_3]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL_3],TRUE,TRUE, 2);

data->label_b[KL_4]=gtk_label_new(iceb_u_toutf("г"));
gtk_container_add(GTK_CONTAINER(knopka[KL_4]),data->label_b[KL_4]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL_4],TRUE,TRUE, 2);


data->label_b[KL_5]=gtk_label_new(iceb_u_toutf("д"));
gtk_container_add(GTK_CONTAINER(knopka[KL_5]),data->label_b[KL_5]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL_5],TRUE,TRUE, 2);

data->label_b[KL_6]=gtk_label_new(iceb_u_toutf("е"));
gtk_container_add(GTK_CONTAINER(knopka[KL_6]),data->label_b[KL_6]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL_6],TRUE,TRUE, 2);

data->label_b[KL_7]=gtk_label_new(iceb_u_toutf("ё"));
gtk_container_add(GTK_CONTAINER(knopka[KL_7]),data->label_b[KL_7]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL_7],TRUE,TRUE, 2);


data->label_b[KL_8]=gtk_label_new(iceb_u_toutf("ж"));
gtk_container_add(GTK_CONTAINER(knopka[KL_8]),data->label_b[KL_8]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL_8],TRUE,TRUE, 2);

data->label_b[KL_9]=gtk_label_new(iceb_u_toutf("з"));
gtk_container_add(GTK_CONTAINER(knopka[KL_9]),data->label_b[KL_9]);
gtk_box_pack_start(GTK_BOX(hbox[0]), knopka[KL_9],TRUE,TRUE, 2);

//Вторая строка

data->label_b[KL_10]=gtk_label_new(iceb_u_toutf("и"));
gtk_container_add(GTK_CONTAINER(knopka[KL_10]),data->label_b[KL_10]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL_10],TRUE,TRUE, 2);

data->label_b[KL_11]=gtk_label_new(iceb_u_toutf("й"));
gtk_container_add(GTK_CONTAINER(knopka[KL_11]),data->label_b[KL_11]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL_11],TRUE,TRUE, 2);

data->label_b[KL_12]=gtk_label_new(iceb_u_toutf("к"));
gtk_container_add(GTK_CONTAINER(knopka[KL_12]),data->label_b[KL_12]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL_12],TRUE,TRUE, 2);

data->label_b[KL_13]=gtk_label_new(iceb_u_toutf("л"));
gtk_container_add(GTK_CONTAINER(knopka[KL_13]),data->label_b[KL_13]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL_13],TRUE,TRUE, 2);

data->label_b[KL_14]=gtk_label_new(iceb_u_toutf("м"));
gtk_container_add(GTK_CONTAINER(knopka[KL_14]),data->label_b[KL_14]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL_14],TRUE,TRUE, 2);

data->label_b[KL_15]=gtk_label_new(iceb_u_toutf("н"));
gtk_container_add(GTK_CONTAINER(knopka[KL_15]),data->label_b[KL_15]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL_15],TRUE,TRUE, 2);

data->label_b[KL_16]=gtk_label_new(iceb_u_toutf("о"));
gtk_container_add(GTK_CONTAINER(knopka[KL_16]),data->label_b[KL_16]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL_16],TRUE,TRUE, 2);

data->label_b[KL_17]=gtk_label_new(iceb_u_toutf("п"));
gtk_container_add(GTK_CONTAINER(knopka[KL_17]),data->label_b[KL_17]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL_17],TRUE,TRUE, 2);

data->label_b[KL_18]=gtk_label_new(iceb_u_toutf("р"));
gtk_container_add(GTK_CONTAINER(knopka[KL_18]),data->label_b[KL_18]);
gtk_box_pack_start(GTK_BOX(hbox[1]), knopka[KL_18],TRUE,TRUE, 2);

//Третья строка

data->label_b[KL_19]=gtk_label_new(iceb_u_toutf("с"));
gtk_container_add(GTK_CONTAINER(knopka[KL_19]),data->label_b[KL_19]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL_19],TRUE,TRUE, 2);

data->label_b[KL_20]=gtk_label_new(iceb_u_toutf("т"));
gtk_container_add(GTK_CONTAINER(knopka[KL_20]),data->label_b[KL_20]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL_20],TRUE,TRUE, 2);

data->label_b[KL_21]=gtk_label_new(iceb_u_toutf("у"));
gtk_container_add(GTK_CONTAINER(knopka[KL_21]),data->label_b[KL_21]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL_21],TRUE,TRUE, 2);

data->label_b[KL_22]=gtk_label_new(iceb_u_toutf("ф"));
gtk_container_add(GTK_CONTAINER(knopka[KL_22]),data->label_b[KL_22]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL_22],TRUE,TRUE, 2);

data->label_b[KL_23]=gtk_label_new(iceb_u_toutf("х"));
gtk_container_add(GTK_CONTAINER(knopka[KL_23]),data->label_b[KL_23]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL_23],TRUE,TRUE, 2);

data->label_b[KL_24]=gtk_label_new(iceb_u_toutf("ц"));
gtk_container_add(GTK_CONTAINER(knopka[KL_24]),data->label_b[KL_24]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL_24],TRUE,TRUE, 2);

data->label_b[KL_25]=gtk_label_new(iceb_u_toutf("ч"));
gtk_container_add(GTK_CONTAINER(knopka[KL_25]),data->label_b[KL_25]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL_25],TRUE,TRUE, 2);

data->label_b[KL_26]=gtk_label_new(iceb_u_toutf("ш"));
gtk_container_add(GTK_CONTAINER(knopka[KL_26]),data->label_b[KL_26]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL_26],TRUE,TRUE, 2);

data->label_b[KL_27]=gtk_label_new(iceb_u_toutf("щ"));
gtk_container_add(GTK_CONTAINER(knopka[KL_27]),data->label_b[KL_27]);
gtk_box_pack_start(GTK_BOX(hbox[2]), knopka[KL_27],TRUE,TRUE, 2);

//Четвертая строка

data->label_b[KL_28]=gtk_label_new(iceb_u_toutf("ъ"));
gtk_container_add(GTK_CONTAINER(knopka[KL_28]),data->label_b[KL_28]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_28],TRUE,TRUE, 2);


data->label_b[KL_29]=gtk_label_new(iceb_u_toutf("ы"));
gtk_container_add(GTK_CONTAINER(knopka[KL_29]),data->label_b[KL_29]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_29],TRUE,TRUE, 2);

data->label_b[KL_30]=gtk_label_new(iceb_u_toutf("ь"));
gtk_container_add(GTK_CONTAINER(knopka[KL_30]),data->label_b[KL_30]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_30],TRUE,TRUE, 2);

data->label_b[KL_31]=gtk_label_new(iceb_u_toutf("э"));
gtk_container_add(GTK_CONTAINER(knopka[KL_31]),data->label_b[KL_31]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_31],TRUE,TRUE, 2);

data->label_b[KL_32]=gtk_label_new(iceb_u_toutf("ю"));
gtk_container_add(GTK_CONTAINER(knopka[KL_32]),data->label_b[KL_32]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_32],TRUE,TRUE, 2);

data->label_b[KL_33]=gtk_label_new(iceb_u_toutf("я"));
gtk_container_add(GTK_CONTAINER(knopka[KL_33]),data->label_b[KL_33]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_33],TRUE,TRUE, 2);

data->label_b[KL_34]=gtk_label_new(iceb_u_toutf("і"));
gtk_container_add(GTK_CONTAINER(knopka[KL_34]),data->label_b[KL_34]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_34],TRUE,TRUE, 2);

data->label_b[KL_35]=gtk_label_new(iceb_u_toutf("ї"));
gtk_container_add(GTK_CONTAINER(knopka[KL_35]),data->label_b[KL_35]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_35],TRUE,TRUE, 2);

data->label_b[KL_36]=gtk_label_new(iceb_u_toutf("є"));
gtk_container_add(GTK_CONTAINER(knopka[KL_36]),data->label_b[KL_36]);
gtk_box_pack_start(GTK_BOX(hbox[3]), knopka[KL_36],TRUE,TRUE, 2);

//Пятая строка
data->label_b[KL_CAPS_LOCK]=gtk_label_new("Caps Lock");
gtk_container_add(GTK_CONTAINER(knopka[KL_CAPS_LOCK]),data->label_b[KL_CAPS_LOCK]);
gtk_box_pack_start(GTK_BOX(hbox[4]), knopka[KL_CAPS_LOCK],TRUE,TRUE, 2);

data->label_b[KL_PROBEL]=gtk_label_new(gettext("Пробел "));
gtk_container_add(GTK_CONTAINER(knopka[KL_PROBEL]),data->label_b[KL_PROBEL]);
gtk_box_pack_start(GTK_BOX(hbox[4]), knopka[KL_PROBEL],TRUE,TRUE, 2);

data->label_b[KL_RUS_LAT]=gtk_label_new(iceb_u_toutf("У/Л"));
gtk_container_add(GTK_CONTAINER(knopka[KL_RUS_LAT]),data->label_b[KL_RUS_LAT]);
gtk_box_pack_start(GTK_BOX(hbox[4]), knopka[KL_RUS_LAT],TRUE,TRUE, 2);

PangoFontDescription *font_pango=pango_font_description_from_string(SHRIFT_MOUSE_KLAV);

for(int i=0; i < KOL_BUKV_ZNAK; i++)
  gtk_widget_modify_font(data->label_b[i],font_pango);

pango_font_description_free(font_pango);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  mouse_klav_v_b_knopka(GtkWidget *widget,class mouse_klav_v_data *data)
{
gint pozic=gtk_editable_get_position(GTK_EDITABLE(data->entry));

gint start=0; //позиция начала выделения
gint end=0;   //позиция конца выделения

gtk_editable_get_selection_bounds(GTK_EDITABLE(data->entry),&start,&end);
//printf("staft=%d end=%d\n",start,end);

if(end > start)  //если есть выделеный текст
 {
  
  gtk_entry_set_text(GTK_ENTRY(data->entry),"");
  pozic=0;
 }

int knop=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
//g_print("maus_klav_v_b_knopka knop=%d\n",knop);
char simvol[20];
int dlinna=0;
switch (knop)
 {
  case KL_1:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("а"));    
      else
        strcpy(simvol,"a");    
     }
    else
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("А"));    
      else
       strcpy(simvol,"A");    
     }
    dlinna=strlen(simvol);    
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_2:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("б"));    
      else
       strcpy(simvol,"b");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Б"));    
      else
        strcpy(simvol,"B");    
     }
    dlinna=strlen(simvol);    
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_3:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("в"));
      else
        strcpy(simvol,"c");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("В"));
      else
        strcpy(simvol,"C");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_4:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("г"));
      else
        strcpy(simvol,"d");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Г"));
      else
        strcpy(simvol,"D");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_5:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("д"));
      else
        strcpy(simvol,"e");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Д"));
      else
        strcpy(simvol,"E");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_6:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("е"));
      else
        strcpy(simvol,"f");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Е"));
      else
        strcpy(simvol,"F");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_7:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("ё"));
      else
        strcpy(simvol,"g");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Ё"));
      else
        strcpy(simvol,"G");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_8:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("ж"));
      else
        strcpy(simvol,"h");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Ж"));
      else
        strcpy(simvol,"H");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_9:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("з"));
      else
        strcpy(simvol,"i");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("З"));
      else
        strcpy(simvol,"I");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_10:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("и"));
      else
        strcpy(simvol,"j");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("И"));
      else
        strcpy(simvol,"J");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_11:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("й"));
      else
        strcpy(simvol,"k");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Й"));
      else
        strcpy(simvol,"K");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_12:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("к"));
      else
        strcpy(simvol,"l");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("К"));
      else
        strcpy(simvol,"L");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_13:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("л"));
      else
        strcpy(simvol,"m");    
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Л"));
      else
        strcpy(simvol,"M");    
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_14:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("м"));
      else
        strcpy(simvol,"n");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("М"));
      else
        strcpy(simvol,"N");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_15:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("н"));
      else
        strcpy(simvol,"o");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Н"));
      else
        strcpy(simvol,"O");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_16:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("о"));
      else
        strcpy(simvol,"p");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("О"));
      else
        strcpy(simvol,"P");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_17:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("п"));
      else
        strcpy(simvol,"q");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("П"));
      else
        strcpy(simvol,"Q");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_18:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("р"));
      else
        strcpy(simvol,"r");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Р"));
      else
        strcpy(simvol,"R");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_19:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("с"));
      else
        strcpy(simvol,"s");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("С"));
      else
        strcpy(simvol,"S");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_20:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("т"));
      else
        strcpy(simvol,"t");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Т"));
      else
        strcpy(simvol,"T");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_21:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("у"));
      else
        strcpy(simvol,"u");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("У"));
      else
        strcpy(simvol,"U");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_22:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("ф"));
      else
        strcpy(simvol,"v");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Ф"));
      else
        strcpy(simvol,"V");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_23:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("х"));
      else
        strcpy(simvol,"w");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Х"));
      else
        strcpy(simvol,"W");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;


  case KL_24:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("ц"));
      else
        strcpy(simvol,"x");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Ц"));
      else
        strcpy(simvol,"X");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_25:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("ч"));
      else
        strcpy(simvol,"y");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Ч"));
      else
        strcpy(simvol,"Y");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_26:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("ш"));
      else
        strcpy(simvol,"z");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Ш"));
      else
        strcpy(simvol,"Z");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_27:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("щ"));
      else
        strcpy(simvol,":");
      }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Щ"));
      else
        strcpy(simvol,":");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_28:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("ъ"));
      else
        strcpy(simvol,";");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Ъ"));
      else
        strcpy(simvol,";");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;


  case KL_29:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("ы"));
      else
        strcpy(simvol,"*");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Ы"));
      else
        strcpy(simvol,"*");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_30:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("ь"));
      else
        strcpy(simvol,"/");
     }
    else
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("Ь"));
      else
        strcpy(simvol,"/");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_31:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
        strcpy(simvol,iceb_u_toutf("э"));
      else
        strcpy(simvol,"\"");
     }
    else
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("Э"));
      else
        strcpy(simvol,"\"");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_32:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("ю"));
      else
        strcpy(simvol,"'");
     }
    else
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("Ю"));
      else
        strcpy(simvol,"'");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_33:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("я"));
      else
        strcpy(simvol,"-");
     }
    else
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("Я"));
      else
        strcpy(simvol,"-");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_34:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("і"));
      else
        strcpy(simvol,"+");
     }
    else
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("І"));
      else
        strcpy(simvol,"+");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_35:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("ї"));
      else
        strcpy(simvol,"(");
     }
    else
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("Ї"));
      else
        strcpy(simvol,"(");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_36:
    if(data->kl_shift == 0)
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("є"));
      else
        strcpy(simvol,")");
     }
    else
     {
      if(data->rus_lat == 0)
       strcpy(simvol,iceb_u_toutf("Є"));
      else
        strcpy(simvol,")");
     }
    dlinna=strlen(simvol);
    gtk_editable_insert_text(GTK_EDITABLE(data->entry),simvol,dlinna,&pozic);
    break;

  case KL_PROBEL:
    gtk_editable_insert_text(GTK_EDITABLE(data->entry)," ",1,&pozic);
    break;

  case KL_RUS_LAT:

    data->rus_lat++;
    if(data->rus_lat > 1)
     data->rus_lat=0;

    mouse_klav_smena_b(data);
    break;

  case KL_CAPS_LOCK:

    data->kl_shift++;
    if(data->kl_shift > 1)
     data->kl_shift=0;

    mouse_klav_smena_b(data);

    break;




 }

//После установки фокуса курсор становится в конец строки
gtk_widget_grab_focus(data->entry);

gtk_entry_set_position(GTK_ENTRY(data->entry),pozic);

}
/**********************************/
/*смена букв */
/*********************************/

void mouse_klav_smena_b(class mouse_klav_v_data *data)
{
if(data->kl_shift == 1)
 {
  if(data->rus_lat == 0)
   {
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_1]),iceb_u_toutf("А"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_2]),iceb_u_toutf("Б"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_3]),iceb_u_toutf("В"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_4]),iceb_u_toutf("Г"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_5]),iceb_u_toutf("Д"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_6]),iceb_u_toutf("Е"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_7]),iceb_u_toutf("Ё"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_8]),iceb_u_toutf("Ж"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_9]),iceb_u_toutf("З"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_10]),iceb_u_toutf("И"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_11]),iceb_u_toutf("Й"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_12]),iceb_u_toutf("К"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_13]),iceb_u_toutf("Л"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_14]),iceb_u_toutf("М"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_15]),iceb_u_toutf("Н"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_16]),iceb_u_toutf("О"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_17]),iceb_u_toutf("П"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_18]),iceb_u_toutf("Р"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_19]),iceb_u_toutf("С"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_20]),iceb_u_toutf("Т"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_21]),iceb_u_toutf("У"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_22]),iceb_u_toutf("Ф"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_23]),iceb_u_toutf("Х"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_24]),iceb_u_toutf("Ц"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_25]),iceb_u_toutf("Ч"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_26]),iceb_u_toutf("Ш"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_27]),iceb_u_toutf("Щ"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_28]),iceb_u_toutf("Ъ"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_29]),iceb_u_toutf("Ы"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_30]),iceb_u_toutf("Ь"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_31]),iceb_u_toutf("Э"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_32]),iceb_u_toutf("Ю"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_33]),iceb_u_toutf("Я"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_34]),iceb_u_toutf("І"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_35]),iceb_u_toutf("Ї"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_36]),iceb_u_toutf("Є"));
   }
  else
   {
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_1]),"A");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_2]),"B");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_3]),"C");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_4]), "D");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_5]), "E");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_6]), "F");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_7]), "G");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_8]), "H");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_9]), "I");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_10]), "J");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_11]), "K");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_12]), "L");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_13]), "M");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_14]), "N");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_15]), "O");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_16]), "P");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_17]), "Q");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_18]), "R");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_19]), "S");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_20]), "T");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_21]), "U");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_22]), "V");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_23]), "W");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_24]), "X");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_25]), "Y");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_26]), "Z");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_27]), ":");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_28]), ";");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_29]), "*");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_30]), "/");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_31]), "\"");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_32]), "'");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_33]), "-");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_34]), "+");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_35]), "(");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_36]), ")");

   }
  return;
 }

if(data->kl_shift == 0)
 {
  if(data->rus_lat == 0)
   {
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_1]),iceb_u_toutf("а"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_2]),iceb_u_toutf("б"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_3]),iceb_u_toutf("в"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_4]),iceb_u_toutf("г"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_5]),iceb_u_toutf("д"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_6]),iceb_u_toutf("е"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_7]),iceb_u_toutf("ё"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_8]),iceb_u_toutf("ж"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_9]),iceb_u_toutf("з"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_10]),iceb_u_toutf("и"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_11]),iceb_u_toutf("й"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_12]),iceb_u_toutf("к"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_13]),iceb_u_toutf("л"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_14]),iceb_u_toutf("м"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_15]),iceb_u_toutf("н"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_16]),iceb_u_toutf("о"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_17]),iceb_u_toutf("п"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_18]),iceb_u_toutf("р"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_19]),iceb_u_toutf("с"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_20]),iceb_u_toutf("т"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_21]),iceb_u_toutf("у"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_22]),iceb_u_toutf("ф"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_23]),iceb_u_toutf("х"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_24]),iceb_u_toutf("ц"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_25]),iceb_u_toutf("ч"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_26]),iceb_u_toutf("ш"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_27]),iceb_u_toutf("щ"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_28]),iceb_u_toutf("ъ"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_29]),iceb_u_toutf("ы"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_30]),iceb_u_toutf("ь"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_31]),iceb_u_toutf("э"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_32]),iceb_u_toutf("ю"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_33]),iceb_u_toutf("я"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_34]),iceb_u_toutf("і"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_35]),iceb_u_toutf("ї"));
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_36]),iceb_u_toutf("є"));
   }
  else
   {
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_1]),"a");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_2]),"b");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_3]),"c");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_4]), "d");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_5]), "e");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_6]), "f");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_7]), "g");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_8]), "h");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_9]), "i");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_10]), "j");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_11]), "k");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_12]), "l");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_13]), "m");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_14]), "n");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_15]), "o");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_16]), "p");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_17]), "q");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_18]), "r");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_19]), "s");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_20]), "t");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_21]), "u");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_22]), "v");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_23]), "w");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_24]), "x");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_25]), "y");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_26]), "z");

    gtk_label_set_text(GTK_LABEL(data->label_b[KL_27]), ":");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_28]), ";");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_29]), "*");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_30]), "/");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_31]), "\"");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_32]), "'");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_33]), "-");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_34]), "+");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_35]), "(");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_36]), ")");
/*****
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_27]), "");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_28]), "");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_29]), "");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_30]), "");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_31]), "");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_32]), "");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_33]), "");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_34]), "");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_35]), "");
    gtk_label_set_text(GTK_LABEL(data->label_b[KL_36]), "");
********/
   }

  return;
 }


}

/******************************/
/*Чтение карточки*/
/*****************************/
void iceb_mous_klav_rc(class mouse_klav_v_data *data)
{

printf("iceb_mous_klav_rc\n");
iceb_refresh();

char kod_kart[30];
memset(kod_kart,'\0',sizeof(kod_kart));
#ifdef READ_CARD
extern tdcon           td_server; 
int i=0;
if((i=iceb_read_card(td_server,kod_kart,sizeof(kod_kart),0,data->window)) < 0)
 {
  gtk_timeout_remove(data->timer);
  return;
 }    

if(i > 0) //Не дождались чтения
 return;

//printf("iceb_mous_klav_rc-kod_kart=%s\n",kod_kart);

gtk_entry_set_text(GTK_ENTRY(data->entry),iceb_u_toutf(kod_kart));
//Читаем только для того, чтобы не останавливался аварийно опрос считывателя
gtk_entry_get_text(GTK_ENTRY(data->entry));


#endif 

}
