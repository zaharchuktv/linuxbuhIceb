/*$Id: gr_klient.h,v 1.12 2011-01-13 13:50:09 sasa Exp $*/
/*15.05.2010
Общий файл для нескольких программ
*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создаём таблицу групп клиентов Grupklient.\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Grupklient \
(kod    char(10) not null primary key,\
naik    varchar(100) not null,\
ktoz	smallint unsigned not null,\
vrem	int unsigned not null)");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/
if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
//  repl.plus_ps(strsql);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана !\n"),data->buffer,data->view);

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw(iceb_u_toutf("Создание таблицы клиентов Taxiklient !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Taxiklient (\
kod     char(20) not null primary key,\
fio     varchar(60) not null,\
adres   varchar(100) not null,\
telef	varchar(40) not null,\
ktoi    smallint unsigned not null,\
vrem    int unsigned not null,\
ps      float not null,\
grup    char(10) not null,\
sp varchar(100) not null,\
mk tinyint not null,\
dvk DATETIME NOT NULL,\
pl tinyint not null,\
denrog DATE NOT NULL,\
kom VARCHAR(100) NOT NULL,\
mb tinyint not null)");

/*
 0 kod	код
 1 fio     фамилия
 2 adres   адрес
 3 telef	телефон
 4 ktoi    кто записал
 5 vrem    время записи
 6 ps      процент скидки
 7 grup    код группы
 8 sp      список подразделений вход в которые запрещён
 9 mk      метка контроля вход-выход 0-включена 1-выключена
10 dvk     Дата возврата карточки.
11 pl      пол 0-мужчина 1-женщина
12 denrog  день рождения клиента
13 kom     коментарий
14 mb      метка блокировки карточки 0-активная 1-пассивная
*/

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана.\n\n"),data->buffer,data->view);

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw(iceb_u_toutf("Создание таблицы настроек Alx !\n"),data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Alx \
(fil varchar(255) not null,\
ns int not null,\
ktoz int not null,\
vrem int unsigned not null,\
str TEXT not null,\
unique(fil,ns))");

if(sql_zap(&bd,strsql) != 0)
 {
  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  repl.new_plus(bros);
  iceb_printw(repl.ravno_toutf(),data->buffer,data->view);
 } 
else
  iceb_printw(iceb_u_toutf("Таблица создана\n\n"),data->buffer,data->view);

/*
0 fil  - имя файла
1 ns   - номер строки
2 ktoz - номер оператора сделавшего запись
3 vrem - время записи
4 str  - строка текста
*/
