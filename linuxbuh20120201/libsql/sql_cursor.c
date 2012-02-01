/* $Id: sql_cursor.c,v 1.6 2011-01-13 13:50:03 sasa Exp $ */
/*16.10.2009	16.05.2001	Белых А.И.	sql_cursor.c
Подпрограммы работы с классом SQLCURSOR
*/
#include	<string.h>
#include	"libsql.h"

/********************/
/*Конструктор объекта*/
/********************/

SQLCURSOR::SQLCURSOR()
{
metka=0;
cursor=NULL;
}


/********************/
/*Деструктор объекта*/
/********************/

SQLCURSOR::~SQLCURSOR()
{
if(metka == 1)
 {
  mysql_free_result(cursor);
//  printw("Удалили курсор !!!!\n");
//  OSTANOV();
 }
}


/*****************/
/*Создать курсор*/
/****************/

long SQLCURSOR::make_cursor(SQL_baza *bd,const char *strsql)
{
long kolstr;
if(metka == 1)
  mysql_free_result(cursor);

metka=0;

if(mysql_query(bd,strsql) != 0)
  return(-1);

if((cursor=mysql_store_result(bd)) == NULL)
 if(mysql_num_fields(cursor) != 0)
  return(-1);

metka=1;

/*Определяем количество строк в курсоре*/
kolstr=mysql_num_rows(cursor);
/*
printw("kolstr=%d\n",kolstr);
OSTANOV();
*/

return(kolstr);

//return(sql_opencur(&baza,&cursor,strsql));
}

/**************************************/
/*Прочитать следующюю запись в курсоре*/
/**************************************/
SQL_str	SQLCURSOR::read_cursor(SQL_str *row)
{
return(*row=mysql_fetch_row(cursor));
//return(sql_readcur(cursor,row));
}

/**********************************/
/*Стать на нужную строку в курсоре*/
/**********************************/

void SQLCURSOR::poz_cursor(int pozz)
{
 mysql_data_seek(cursor,pozz);
}

/*******************/
/*Уничтожить курсор*/
/********************/
void SQLCURSOR::delete_cursor(void)
{
if(metka == 1)
  mysql_free_result(cursor);
metka=0;

}

/********************************************/
/*Получить смещение текущей строки в курсоре*/
/********************************************/
SQL_smesh SQLCURSOR::pozv_cursor(void)
{
return(mysql_row_tell(cursor));
}

/**************************************************/
/*Прочитать строку в курсоре по заданному смещению*/
/**************************************************/
SQL_smesh SQLCURSOR::readv_cursor(SQL_smesh smesh)
{
return(mysql_row_seek(cursor,smesh));
}
