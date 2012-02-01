/*$Id: iceb_pm_vibor.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*20.11.2007	20.11.2007	Белых А.И.	iceb_pm_vibor.c
Пунк меню для выбора из заданных значений
*/
#include "iceb_libbuh.h"


/**********************/
/*Читаем              */
/**********************/

void   iceb_pm_vibor_get(GtkWidget *widget,int *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
*data=enter;
}
/********************************/

void iceb_pm_vibor(class iceb_u_spisok *strmenu,GtkWidget **opt,int *nomer_str)
{


*opt = gtk_option_menu_new();

GtkWidget *menu = gtk_menu_new();
int nomer=0;
GtkWidget *item;
for(int ii=0; ii < strmenu->kolih(); ii++)
 {
  item = gtk_menu_item_new_with_label (strmenu->ravno_toutf(ii));
  gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_pm_vibor_get),nomer_str);
  gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
  gtk_widget_show (item);
  gtk_menu_append (GTK_MENU (menu), item);
 }

gtk_option_menu_set_menu (GTK_OPTION_MENU (*opt), menu);
gtk_option_menu_set_history(GTK_OPTION_MENU(*opt),*nomer_str);
}
