/*$Id: dbfhead.h,v 1.2 2011-01-13 13:49:49 sasa Exp $*/
/*29.10.1998	29.10.1998	Белых А.И.	dbfhead.h
Описание структуры заголовка dbf файла
*/



typedef struct dbase_head 
 {
    unsigned char	version;	/* 03 for dbIII and 83 for dbIII w/memo file */
    unsigned char	l_update[3];	/* yymmdd for last update*/
    unsigned long	count;		/* number of records in file*/
    unsigned short	header;		/* length of the header
					 * includes the \r at end
					 */
    unsigned short	lrecl;		/* length of a record
					 * includes the delete
					 * byte
					 */
    unsigned char   reserv[20];
 } DBASE_HEAD;

typedef struct dbase_fld 
 {
    char    name[11];               /*field name*/
    char    type;                   /*field type*/
    /* A-T uses large data model but drop it for now */
    char   *data_ptr;               /*pointer into buffer*/
    unsigned char length;           /*field length*/
    char   dec_point;               /*field decimal point*/
    char   fill[14];
 } DBASE_FIELD;
