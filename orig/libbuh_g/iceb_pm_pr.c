/*$Id: iceb_pm_pr.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*18.06.2010	18.06.2010	Белых А.И.	iceb_pm_pr.c
Пунк меню для выбора всех, только приходов, только расходов
*/
#include "iceb_libbuh.h"


/**********************/
/*Читаем              */
/**********************/

void   iceb_pm_pr_get(GtkWidget *widget,int *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
*data=enter;
}
/********************************/

void iceb_pm_pr(GtkWidget **opt,int *metka_pr)
{


*opt = gtk_option_menu_new();

GtkWidget *menu = gtk_menu_new();
int nomer=0;
GtkWidget *item;
item = gtk_menu_item_new_with_label (gettext("Все записи"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_pm_pr_get),metka_pr);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Только приходы"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_pm_pr_get),metka_pr);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Только расходы"));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_pm_pr_get),metka_pr);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (*opt), menu);
gtk_option_menu_set_history(GTK_OPTION_MENU(*opt),*metka_pr);
}
