/* $Id: iceb_errorkas.c,v 1.12 2011-02-21 07:36:06 sasa Exp $ */
/*28.12.2004	29.01.2001	Белых А.И.	iceb_errorkas.c
Анализ ответа демона работы с кассовым регистратором
Возвращяем 0 если ошибки нет
           1 если есть
*/
#include	<stdlib.h>
#include	<ctype.h>
#include	"iceb_libbuh.h"



/* errorkan() - проверка числового кода возврата функции обращения к
		iceb_server.
   Возвращает:
      0 - ошибок нет
      1 - ошибка была и о ней сообщено оператору 
*/
int		iceb_errorkas(int errn,GtkWidget *wpredok)
{
   iceb_u_str p;
   p.plus("Библиотека не подключена");   
   if (errn >= 0) return(0);
/****************************
   switch (errn) {
   case (ICEB_ERROR_NONE): 
//	p = gettext( "Операция выполнена успешно.");
	return(0);
	break;
   case (ICEB_ERROR_HOST_NOT_FOUND): 
	p.plus(gettext( "Хост не найден."));
	break;
   case (ICEB_ERROR_SERVICE_NOT_FOUND): 
	p.plus( gettext( "Сервис неизвестен."));
	break;
   case (ICEB_ERROR_CREATE_SOCKET): 
	p.plus(gettext( "Не могу создать сокет."));
	break;
   case (ICEB_ERROR_CONNECT_HOST): 
	p.plus(gettext( "Не могу соединиться с хостом."));
	break;
   case (ICEB_ERROR_SEND_HOST): 
	p.plus(gettext( "Ошибка передачи данных."));
	break;
   case (ICEB_ERROR_SEND_DATA): 
	p.plus(gettext( "Ошибка передачи данных (блок разрушен)."));
	break;
   case (ICEB_ERROR_READ_DATA): 
	p.plus(gettext( "Не могу прочитать данные."));
	break;
   case (ICEB_ERROR_LARGE_BLOCK): 
	p.plus(gettext( "Слишком большой блок данных."));
	break;
   case (ICEB_ERROR_INVALID_HANDLE): 
	p.plus(gettext( "Недопустимый номер соединения."));
	break;
   case (ICEB_ERROR_NO_MORE_HANDLE): 
	p.plus(gettext( "Превышен предел количества подключений."));
	break;
   default:
	p.plus(gettext( "Неизвестная ошибка подключения к серверу."));
	break;
   }
********************/

  iceb_menu_soob(&p,wpredok);
  return(1);
}


/* errorkas() - проверка символьной строки ответа от iceb_server.

   Возвращает:
      0 - ошибок нет
      1 - ошибка была и о ней сообщено оператору 
*/
int		iceb_errorkas(KASSA *kasr, char *str,GtkWidget *wpredok)
{
   char *p;
   int i;
   char buf[1024];
   
   p = str;
   while ((*p != '\0') && isspace(*p)) p++;			// найдем первый непробел
   if (isdigit(p[0])) {				// цифра ?
      						// да - числовой код ошибки 
      if ((i = atoi(p)) == 0) return(0);	//    ошибок нет - выход

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Надо заменить на новую
//      p = (char*) maria_error(i,fils);			//    получим текст ошибки

   }
   else {					// не цифра
      if (*p == '\0')
        p = gettext("iceb_server не отвечает !");
       /* символьное сообщение - проверяем на стандартные ответы */
      else if(iceb_u_SRAV(p,"ERROR CMD",0) == 0)
        p = gettext("Ошибочное ключевое слово !");
      else if(iceb_u_SRAV(p,"ERROR",0) == 0)
        p = gettext("Ошибочное число параметров в команде !");
      else if(iceb_u_SRAV(p,"ERROR FUNC",0) == 0)
        p = gettext("Ошибочная команда после FSTART: !");
      /* если сообщение не соответствует ни одному из известных шаблонов - выводим как есть */
   }
   //Вывод сообщения на экран
  iceb_u_str repl;
  repl.plus(p);
  iceb_menu_soob(&repl,wpredok);
/*************
   VVOD SOOB(1);
   SOOB.VVOD_SPISOK_add_MD(p);
   soobshw(&SOOB,stdscr,-1,-1,0,1);
**************/
   //Обнуляем индикатор кассы
   sprintf(buf, "FSTART: MARIA_INDICATOR\n\
   %s\n\
   %s\n\
   %d\n\
   %s:\n\
   FEND:\n",
   kasr->prodavec.ravno(), kasr->parol, 1, "");  
/*
   iceb_write_to_server(kasr->handle, buf);
   iceb_read_from_server(kasr->handle, buf, sizeof(buf)-1);
*/
   return(1);
}

/* iceb_cmd() - отправка команды серверу и анализ результата
		с выводом сообщения об ошибке (если была)
		на экран.
   Возвращает:
   0 - команда выполнена без ошибок
   1 - была ошибка
*/
int iceb_cmd(KASSA *kasr, char *cmd,GtkWidget *wpredok)
{
/**********************
  int i;
  char buf[1024];
  // отправим команду серверу
  if ((i=iceb_write_to_server(kasr->handle, cmd)) < 0)
     return(iceb_errorkas(i,wpredok));	//ошибка обращения к серверу
  else 
  {	// команда передана успешно	
     // прочитаем ответ сервера	
     iceb_read_from_server(kasr->handle, buf, sizeof(buf)-1);
     // и разберем его 
     return(iceb_errorkas(kasr, buf,wpredok));
  }
********************/
return(0);
}
