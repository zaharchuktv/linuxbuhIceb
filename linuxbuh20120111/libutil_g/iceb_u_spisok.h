/*$Id: iceb_u_spisok.h,v 1.17 2011-01-13 13:50:05 sasa Exp $*/
/*13.02.2010	09.04.2002	Белых А.И.	iceb_u_spisok.h
Односвязный список
*/
#include <string.h>

class iceb_u_spisok
 {
 private:
  class STROKA
   {
    friend class iceb_u_spisok;
    char *ukstr;
    char *str_filtr;
    int kolbait; /*Длинна строки в байтах*/
    class STROKA *next;

    STROKA(const char *masiv,int dlina,int metka)
     {
      kolbait=dlina+1;
      ukstr=new char[dlina+1];
      memset(ukstr,'\0',dlina+1);
      str_filtr=NULL;
      if(metka == 0)
        strncpy(ukstr,masiv,dlina);  
     }

    ~STROKA()
     {
      delete ukstr;
      if(str_filtr != NULL)
       delete str_filtr;
     }
   };

  int metka;  //0-заголовок в начале списка 1- в конце
  class STROKA *zag;
  class STROKA *kon;  

   //Создать список
   void soz_spisok(int); 
   //Удалить список
   void spisok_delete();

  public:

   //Конструктор без параметров
   iceb_u_spisok();

   //Конструктор
   iceb_u_spisok(int mr);


   //Деструктор
   ~iceb_u_spisok();


   //Добавить элемент в список 
   int plus(const char *masiv);
   /*Добавить к уже существующей строке*/
   int plus(int nomer,const char *masiv);
   
   //Добавить элемент в список с удалением предыдущего списка
   int new_plus(const char *masiv);

   //Добавить элемент в список 
   int plus(const char *masiv,int dlina);
   
   //Добавить элемент в список 
   int plus(int dlina);


   //Вернуть адрес элемента из списка
   const char *ravno(int nom);

   //Вернуть адрес элемента из списка перекодированного в UTF-8
   const char *ravno_toutf(int nom);
   /*Вернуть число целого типа*/
   int ravno_atoi(int nom);
   
   //Вернуть количество элементов в списке
   int kolih(void);
   
   //Найти в списке нужную строку полное сравнение
   int find(const char *obr);

   //Найти в списке нужную строку сравнение по метке
   int find(const char *obr,int);

   //Найти в списке нужную строку без учета регистра с полным сравнением
   int find_r(const char *obr);
   //Найти в списке нужную строку без учета регистра с полным или неполным сравнением
   int find_r(const char *obr,int metka);

   //Cоздать новый список предварительно удалив старый
   void free_class();
   /*Записать данные в уже существующий елемент списка с удалением его предыдущего содержимого*/
   int new_plus(int nomer,const char *znah);
   /*Удалить из списка*/
   int del(int nomer); 

   /*Вернуть адрес всего класса*/
   void *return_all(int nomer);
    
 };

