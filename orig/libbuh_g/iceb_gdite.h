/*$Id: iceb_gdite.h,v 1.6 2011-01-13 13:49:59 sasa Exp $*/
/*23.08.2006	21.08.2006	Белых А.И.	iceb_gdite.h
*/

class iceb_gdite_data
 {
  public:

  GtkWidget *window;
  GtkWidget *bar;

  //Конструктор
  iceb_gdite_data()
   {
    window=NULL;
    bar=NULL;
   }      

  ~iceb_gdite_data()
   {
    close();
   }
  
  void close()
   {
    if(window != NULL)
     gtk_widget_destroy(window);
    window=NULL;
   }
 };
