/*$Id: iceb_pm_nds.c,v 1.9 2011-08-29 07:13:46 sasa Exp $*/
/*10.08.2011	24.10.2007	Белых А.И.	iceb_pm_nds.c
Пунк меню для ввода НДС
*/
#include "iceb_libbuh.h"
#include <glib/gi18n.h>





/**********************/
/*Читаем              */
/**********************/

void   iceb_pm_nds_get(GtkWidget *widget,int *data)
{
int enter=GPOINTER_TO_INT(gtk_object_get_user_data(GTK_OBJECT(widget)));
*data=enter;
}
/********************************/
void iceb_pm_nds(const char *data,GtkWidget **opt,int *nds)
{
short d,m,g;
iceb_u_rsdat(&d,&m,&g,data,0);
/******************************/
iceb_pm_nds(d,m,g,opt,nds);

}
/******************************/
void iceb_pm_nds(short d,short m,short g,GtkWidget **opt,int *nds)
{
char bros[512];

if(d == 0)
 iceb_u_poltekdat(&d,&m,&g);
 
double pnds=iceb_pnds(d,m,g,NULL);

*opt = gtk_option_menu_new();

GtkWidget *menu = gtk_menu_new();
int nomer=0;
GtkWidget *item;
sprintf(bros,"%.f %s",pnds,gettext("НДС"));
item = gtk_menu_item_new_with_label (bros);
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_pm_nds_get),nds);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (iceb_u_toutf(gettext("0% НДС реализация на територии России")));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_pm_nds_get),nds);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (iceb_u_toutf(gettext("0% НДС экспорт")));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_pm_nds_get),nds);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (iceb_u_toutf(gettext("Освобождение от НДС")));
gtk_signal_connect (GTK_OBJECT (item), "activate",GtkSignalFunc(iceb_pm_nds_get),nds);
gtk_object_set_user_data(GTK_OBJECT(item),(gpointer)nomer++);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

gtk_option_menu_set_menu (GTK_OPTION_MENU (*opt), menu);
gtk_option_menu_set_history(GTK_OPTION_MENU(*opt),*nds);
}
