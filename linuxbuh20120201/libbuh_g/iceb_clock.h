/*$Id: iceb_clock.h,v 1.5 2011-01-13 13:49:58 sasa Exp $*/
/*26.11.2005	23.04.2005	Белых А.И.	iceb_clock.c
Установка курсора 
*/

class iceb_clock
 {
  GtkWidget *www;
  public:
  
  //Конструктор    
  iceb_clock(GtkWidget *window)
   {
    
    www=window;
    if(www != NULL)
     {
//      printf("iceb_clock-установили курсор Ждите !!!\n");
  
      gdk_window_set_cursor(www->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

      while(gtk_events_pending())
       gtk_main_iteration();
     }
   } 
  //Деструктор
  ~iceb_clock()
   {
    clear_data();
   }
  void clear_data()
   {
    if(www != NULL)
     {
      gdk_window_set_cursor(www->window,gdk_cursor_new(ICEB_CURSOR));
//      printf("iceb_clock-Сняли курсор Ждите !!!\n");
     }
    www=NULL;
   }
 };
