/*$Id: xdkvnp.c,v 1.4 2011-02-21 07:35:59 sasa Exp $*/
/*23.05.2010	23.05.2010	Белых А.И.	xdkvnp.c
Ввод и корректировка назначения платежа
*/

#include "buhg_g.h"

/*********************************/
/*Определение количества символов в назначении платежа*/
/*******************************************************/
void xdkvnp_kolsimv(GtkTextBuffer *textbuffer,GtkWidget *ramka)
{
int kol_simv=gtk_text_buffer_get_char_count (textbuffer);
//printf("Количество символов=%d\n",kol_simv);
char bros[512];
sprintf(bros,"%s %s:%d",gettext("Назначение платежа"),
gettext("Количество символов"),kol_simv);
gtk_frame_set_label(GTK_FRAME(ramka),bros);
}

/***********************************/
/*создание виджета для ввода и корректировки текста*/
/***************************************************/

GtkWidget *xkdvnp(const char *naz_pl,GtkTextBuffer **buffer)
{
GtkWidget *ramka=gtk_frame_new(gettext("Назначение платежа"));
//gtk_box_pack_start (GTK_BOX (vbox), ramka, FALSE, FALSE, 10);

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_container_add (GTK_CONTAINER (ramka), sw);

GtkWidget *view;
   
view = gtk_text_view_new ();
gtk_widget_set_size_request(GTK_WIDGET(view),600,100);

//GtkTextBuffer *buffer;
    
*buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

printf("%s-%s\n",__FUNCTION__,naz_pl);
       
gtk_text_buffer_set_text (*buffer,iceb_u_toutf(naz_pl), -1);
gtk_container_add (GTK_CONTAINER (sw), view);
g_signal_connect(*buffer,"changed",G_CALLBACK (xdkvnp_kolsimv),ramka);

return(ramka);

}
