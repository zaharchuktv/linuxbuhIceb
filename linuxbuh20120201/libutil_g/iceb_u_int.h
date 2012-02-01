/*$Id: iceb_u_int.h,v 1.10 2011-01-13 13:50:05 sasa Exp $*/
/*03.10.2010	15.12.2004	Белых А.И.	iceb_u_int.h
Класс для работы с int-массивами переменной длинны
*/
class iceb_u_int
 {
private:
  int    optimiz;  //0-записывать в масив без оптимизации 1-с оптимизацией
  int	 kolz;    //Количество элементов массива
  int *masiv_int;  //Массив 
  
public:

  iceb_u_int(int); //Конструктор
  iceb_u_int(); //Конструктор
  ~iceb_u_int();      //Деструктор

  int find(int); //Найти в какой ячейке искомое значение
  int find(char*); //Найти в какой ячейке искомое значение

  int plus(int,int);      //Записать в ячейку сложив его с существующим значением и если нужно увеличить размер массива
  int new_plus(int,int);   //Записать в ячейку обнулив предыдущее значение
  int plus(int);      //Записать новый элемент c увеличением размера массива
  int plus(char*);    //Записать новый элемент преобразовав его из символьного представления в целое число
  
  int make_class(int);     //создать массив нужного размера
  void print_masiv(); //вывести содержимое массива для контроля
  int ravno(int elem); //вернуть значение нужного елемента
  int ravno_max(); //вернуть максимальное значение елемента
  char *ravno_char(int); /*Вернуть в символьном виде*/
  int kolih(); //вернуть количество элементов массива
  void free_class();
  int suma(); /*Получить сумму всех елементов массива*/
  void clear_class(); /*записать 0 во все елементы*/
 };