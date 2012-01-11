/* $Id: libsql.h,v 1.10 2011-01-13 13:50:03 sasa Exp $ */
/*15.10.2009	28.09.1999	Белых А.И.	libsql.h
*/
#include	<stdio.h>
#include	<mysql.h>
#include	<mysqld_error.h>

typedef MYSQL     SQL_baza;
typedef MYSQL_RES SQL_cursor;
typedef MYSQL_ROW SQL_str;
typedef MYSQL_ROW_OFFSET SQL_smesh;


class SQLCURSOR
 {
private:
  SQL_cursor    *cursor;
  short		metka;  //0-курсор не создан 1-создан
public:

  SQLCURSOR(); //Конструктор
  ~SQLCURSOR();         //Деструктор

  long make_cursor(SQL_baza*,const char*); //Создать курсор
  SQL_str read_cursor(SQL_str*);      //Прочитать следующюю строку курсора
  void poz_cursor(int);          //Стать на нужную строку в курсоре
  void delete_cursor(void);      //Уничтожить курсор
  SQL_smesh pozv_cursor(void); //Получить смещение текущей строки в курсоре
  SQL_smesh readv_cursor(SQL_smesh); //Прочитать строку по заданному смещению
  
 };



extern const char *sql_error(SQL_baza*);
extern unsigned int	sql_nerror(SQL_baza*);
extern int	sql_zap(SQL_baza*,const char*);
extern void    	sql_closebaz(SQL_baza*);
extern int	sql_openbaz(SQL_baza*,const char*,const char*,const char*,const char*);
extern void	sqlfiltr(char*,int);
extern int 	sql_readkey(SQL_baza*,const char*);
extern int 	sql_readkey(SQL_baza*,const char*,SQL_str*,SQLCURSOR*);
extern int	sql_flag(SQL_baza*,const char*,int,int);
extern int	sql_rows(SQL_baza*);
extern void	msql_error(SQL_baza*,const char*,const char*);


