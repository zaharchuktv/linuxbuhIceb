/*$Id: plc_gk_r.c,v 1.20 2011-04-04 08:10:14 sasa Exp $*/
/*02.05.2010	21.01.2004	Белых А.И.	plc_gk_r.c
Проверка логической цеслосности базы "Главная книга"
*/
#include <unistd.h>
#include <errno.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

class plc_gk_r_data
 {
  public:
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *label_prov;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  int    kolstr;  //Количество строк в курсоре
  int    koloh;   //Количество обнаруженных ошибок
  iceb_u_spisok imaf;
  iceb_u_spisok naim;
    
  char *datan;
  char *datak;
  
  //Конструктор
  plc_gk_r_data()
   {
    kolstr=0;
    koloh=0;
   }
 };
gint plc_gk_r1(class plc_gk_r_data *data);

extern char *name_system;
extern SQL_baza	bd;
extern char *organ;

void   plc_gk_r(char *datan,char *datak)
{
plc_gk_r_data data;
char strsql[512];
data.datan=datan;
data.datak=datak;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,-1);

sprintf(strsql,"%s %s",name_system,gettext("Проверка"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

//gtk_signal_connect(GTK_OBJECT(data.window),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_destroy),NULL);
gtk_signal_connect(GTK_OBJECT(data.window),"destroy",GTK_SIGNAL_FUNC(gtk_main_quit),NULL);

GtkWidget *vbox=gtk_vbox_new(FALSE, 2);

gtk_container_add(GTK_CONTAINER(data.window), vbox);

GtkWidget *label=gtk_label_new(gettext("Ждите !!!"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.label=gtk_label_new(gettext("Проверка логической целосности базы данных"));
gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

data.label_prov=gtk_label_new("");
gtk_box_pack_start(GTK_BOX(vbox),data.label_prov,FALSE,FALSE,0);

data.view=gtk_text_view_new();
data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();

gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_CONTINUOUS);

gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(data.bar),GTK_PROGRESS_LEFT_TO_RIGHT);

gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна
gtk_widget_show_all(data.window);

gtk_idle_add((GtkFunction)plc_gk_r1,&data);

gtk_main();

if(data.koloh == 0)
 {
  unlink(data.imaf.ravno(1));
  if(data.kolstr > 0)
    iceb_menu_soob(gettext("Порядок, ошибок не найдено !"),NULL);
 }
else
 {
  iceb_ustpeh(data.imaf.ravno(0),0,NULL);
  iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);
 }
}
/******************************/
/******************************/


gint plc_gk_r1(class plc_gk_r_data *data)
{
gdk_window_set_cursor(data->window->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
char strsql[1024];
//GtkTextIter iter;
class  SQLCURSOR cur;

short dn,mn,gn;
short dk,mk,gk;

iceb_u_rsdat(&dn,&mn,&gn,data->datan,1);
iceb_u_rsdat(&dk,&mk,&gk,data->datak,1);
  
sprintf(strsql,"select * from Prov where datp >= '%04d-%02d-%02d' and val >= 0 \
order by datp asc",gn,mn,dn);

if(dk != 0)
   sprintf(strsql,"select * from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val >= 0 order by datp asc",gn,mn,dn,gk,mk,dk);

iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

class iceb_lock_tables blokt("LOCK TABLE \
Prov READ,\
Plansh READ,\
Kontragent READ,\
Skontr READ,\
Dokummat READ,\
Ukrdok READ,\
Uosdok READ,\
Pltp READ,\
Pltt READ,\
Usldokum READ,\
Alx READ,\
Kasord READ"); //Блокируем таблицы на запись для других клиентов 
  
//После того как мы взяли проводки для проверки, необходимо чтобы пока мы проверяем
//никто из других одновременно работающих операторов не удалил записи, которые мы проверяем
//для этого блокируются таблицы для удаления и корректировки в них записей
  
if((data->kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_str repl;
repl.plus(gettext("Проверка логической целосности базы данных"));
repl.ps_plus(gettext("Количество записей"));
sprintf(strsql,": %d",data->kolstr);
repl.plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label),repl.ravno_toutf());
gtk_widget_show(data->label);

char imaf[56];

sprintf(imaf,"plc%d.lst",getpid());
printf("Имя файла:%s\n",imaf);
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
  
iceb_u_zagolov(gettext("Проверка логической целосности базы данных"),dn,mn,gn,dk,mk,gk,organ,ff);
data->imaf.plus(imaf);
data->naim.plus(gettext("Протокол ошибок"));
  

SQL_str row;
char provodka[1024];
OPSHET rek_shet;
int nomoh;
gfloat kolstr1=0.; //Количество прочитанных строк

while(cur.read_cursor(&row) != 0)
 {
  sprintf(provodka,"%s %s %s %s %s %s\n",row[1],row[2],row[3],row[4],row[9],row[10]);
//  gtk_label_set_text(GTK_LABEL(data->label_prov),iceb_u_toutf(provodka));
//  gtk_widget_show(data->label_prov);

  iceb_pbar(data->bar,data->kolstr,++kolstr1);    
  
  //Ищем парную проводку
   sprintf(strsql,"select val from Prov where \
val=%d and datp='%s' and sh='%s' and shk='%s' and kto='%s' \
and deb=%s and kre=%s and vrem=%s and nomd='%s' and oper='%s' and \
komen='%s'",
    0,row[1],row[3],row[2],row[5],row[10],row[9],row[12],row[6],row[8],row[13]);

    if(sql_readkey(&bd,strsql) != 1)
     {
      iceb_printw(iceb_u_toutf(provodka),data->buffer,data->view);

      iceb_printw(gettext("Не найдено парную проводку!"),data->buffer,data->view);

      fprintf(ff,"%s",provodka);
      fprintf(ff,"%s",repl.ravno());
      data->koloh+=1;
     }

  if((nomoh=iceb_prsh(row[2],&rek_shet,data->window)) == 0)
   {
    iceb_printw(iceb_u_toutf(provodka),data->buffer,data->view);
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),row[1]);
    repl.new_plus_ps(strsql);
    iceb_printw(repl.ravno_toutf(),data->buffer,data->view);

    fprintf(ff,"%s",provodka);
      fprintf(ff,"%s",repl.ravno());
    data->koloh+=1;
   }

  if(nomoh == 2)
   {
    iceb_printw(iceb_u_toutf(provodka),data->buffer,data->view);
    sprintf(strsql,gettext("Счет %s имеет субсчета !"),row[1]);
    repl.new_plus_ps(strsql);
    iceb_printw(repl.ravno_toutf(),data->buffer,data->view);

    fprintf(ff,"%s",provodka);
    fprintf(ff,"%s",repl.ravno());
    data->koloh+=1;
   }
  if(rek_shet.saldo == 3) //Счет с развернутым сальдо
   {
    if(row[4][0] == '\0')
     {
      iceb_printw(iceb_u_toutf(provodka),data->buffer,data->view);
      repl.new_plus_ps(gettext("Не введён код контрагента !"));
      iceb_printw(repl.ravno_toutf(),data->buffer,data->view);

      fprintf(ff,"%s",provodka);
      fprintf(ff,"%s",repl.ravno());
      data->koloh+=1;
     }

    sprintf(strsql,"select ns from Skontr where ns='%s' and kodkon='%s'",row[2],row[4]);
    if(sql_readkey(&bd,strsql) != 1)
     {
      iceb_printw(iceb_u_toutf(provodka),data->buffer,data->view);
      repl.new_plus(gettext("Не нашли код контрагента"));
      repl.plus(" ");
      repl.plus(row[4]);
      repl.plus(" ");
      repl.plus_ps(gettext("в списке счета"));
      repl.plus(" ");
      repl.plus(row[1]);
      repl.plus_ps(" !");

      iceb_printw(repl.ravno_toutf(),data->buffer,data->view);

      fprintf(ff,"%s",provodka);
      fprintf(ff,"%s",repl.ravno());
      data->koloh+=1;
     }

    sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",row[4]);
    if(sql_readkey(&bd,strsql) != 1)
     {
      iceb_printw(iceb_u_toutf(provodka),data->buffer,data->view);
      repl.new_plus_ps(gettext("Не найден код контрагента в общем списке !"));

      iceb_printw(repl.ravno_toutf(),data->buffer,data->view);

      fprintf(ff,"%s",provodka);
      fprintf(ff,"%s",repl.ravno());
      data->koloh+=1;
     }
   }

  if(row[5][0] != '\0')
   {
    //Проверяем есть ли документ к прововке
    if(iceb_u_SRAV(row[5],gettext("МУ"),0) == 0) //Материальный учёт
     {
      //Читаем шапку документа
      sprintf(strsql,"select tip from Dokummat where datd='%s' and sklad=%s and nomd='%s' and kodop='%s' \
and tip=%s",row[14],row[7],row[6],row[8],row[15]);
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        sprintf(strsql,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        sprintf(strsql,"%s !\n",gettext("Не найден документ"));
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
        
        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s !\n",gettext("Не найден документ"));
        data->koloh++;
        continue;      
       }
     }


    if(iceb_u_SRAV(row[5],gettext("УКР"),0) == 0) //Учёт командировочных расходов
     {
      //Читаем шапку документа
      sprintf(strsql,"select god from Ukrdok where datd='%s' and nomd='%s'",row[14],row[6]);
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        sprintf(strsql,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        sprintf(strsql,"%s !\n",gettext("Не найден документ"));
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s !\n",gettext("Не найден документ"));
        data->koloh++;
        continue;      
       }
     }

    if(iceb_u_SRAV(row[5],gettext("УОС"),0) == 0) //Учёт основных средств
     {
      //Читаем шапку документа
      sprintf(strsql,"select tipz from Uosdok where datd='%s' and nomd='%s' and kodop='%s' \
and tipz=%s",row[14],row[6],row[8],row[15]);
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        sprintf(strsql,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        sprintf(strsql,"%s !\n",gettext("Не найден документ"));
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s !\n",gettext("Не найден документ"));
        data->koloh++;
        continue;      
       }
     }

    if(iceb_u_SRAV(row[5],gettext("ПЛТ"),0) == 0) //Платёжные поручения
     {
      //Читаем шапку документа 
      sprintf(strsql,"select datd from Pltp where datd='%s' and nomd='%s'",row[14],row[6]);
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        sprintf(strsql,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        sprintf(strsql,"%s !\n",gettext("Не найден документ"));
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s !\n",gettext("Не найден документ"));
        data->koloh++;
        continue;      
       }
     }

    if(iceb_u_SRAV(row[5],gettext("ТРЕ"),0) == 0) //Платёжные требования
     {
      //Читаем шапку документа и узнаём приходный он или расходный
      sprintf(strsql,"select datd from Pltt where datd='%s' and nomd='%s'",row[14],row[6]);
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        sprintf(strsql,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        sprintf(strsql,"%s !\n",gettext("Не найден документ"));
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s !\n",gettext("Не найден документ"));
        data->koloh++;
        continue;      
       }
     }

    if(iceb_u_SRAV(row[5],gettext("УСЛ"),0) == 0) //Учёт услуг
     {
      //Читаем шапку документа и узнаём приходный он или расходный
      sprintf(strsql,"select tp from Usldokum where datd='%s' and podr=%s and nomd='%s' and kodop='%s' \
and tp=%s",row[14],row[7],row[6],row[8],row[15]);
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        sprintf(strsql,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        sprintf(strsql,"%s !\n",gettext("Не найден документ"));
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

        fprintf(ff,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        fprintf(ff,"%s !\n",gettext("Не найден документ"));
        data->koloh++;
        continue;      
       }
     }

    if(iceb_u_SRAV(row[5],gettext("КО"),0) == 0) //Учёт кассовых ордеров
     {
      //Читаем шапку документа и узнаём приходный он или расходный
      sprintf(strsql,"select tp from Kasord where datd='%s' and kassa=%s and nomd='%s' \
and tp=%s",row[14],row[7],row[6],row[15]);
      if(iceb_sql_readkey(strsql,data->window) != 1)
       {
        sprintf(strsql,"%s %s %s %s %s %s %s %s %s %s %s\n",
        row[1],row[2],row[3],row[4],row[5],row[6],row[9],row[10],row[14],row[7],row[8]);

        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
        fprintf(ff,"%s",strsql);
        
        sprintf(strsql,"%s !\n",gettext("Не найден документ"));
        iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
        fprintf(ff,"%s",strsql);

        data->koloh++;
        continue;      
       }
     }
      
   }  


 }

fclose(ff);
 
gtk_widget_destroy(data->window);

return(FALSE);

}



