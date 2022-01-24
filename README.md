# exchange
simple exchange, matching orders, trade

* input.txt - input dataset
* output.txt - output dataset


Биржевой симулятор
-------------------
Биржа преставляет собой место, где совершаются сделки (Trade) между покупателями и продавцами. Покупатели хотят что-то купить(Buy), а продавцы хотят что-то продать (Sell). Сделка покупки/продажи совершается если продавец и покупатель удовлетворены условиями сделки - например цена и количество устраивает обе стороны.

Продавцы стараются продать свой товар по максимально возможно цене, а покупатели стараются купить по минимально возможной цене. На бирже желание купить или продать выражается в подаче электронным образом заявок(order), в каждой из которых указывается:

  a) уникальный номер заявки(ID)

  b) покупка или продажа(side) - указание купить или продать ((B)uy or (S)ell)

  c) наименование ценной бумаги(Instrument)

  d) количество(Qty) - сколько надо купить/продать

  e) цена покупки/продажи(Price) - желание купить/продать по цене не хуже чем указанная в заявке. Для заявок на покупку цена должна быть больше или равна указанной, для заявки на продажу цена должна быть равна или меньше указанной

Например:

O,4,B,Яблоки,10,256.12 -> заявка ((O)rder) с номером 4 в которой кто-то хочет купить((B)uy) Яблоки в количестве 10 кг по цене 256.12 руб/кг 

или

O,50,S,Яблоки,4,255.5 -> заявка ((O)rder) с номером 50 в которой кто-то хочет продать((S)ell) Яблоки в количестве 4 кг по цене 255.5 руб/кг

Если условие покупки/продажи удовлетворяет обе стороны, то совершается
сделка(Trade). В данном примере, рыночные условия удовлетворяют требованиям для
совершения сделки:

  a) есть достаточное количество в данном случае min(Buy Qty, Sell Qty) = min(10, 4) = 4
  
  b) цена покупателя позволяет продавцу выполнить условие продать не хуже, чем его цена продажи (Sell Price >= Buy Price),а покупатель имеет возможность купить по цене, которую он заявил, что тоже удовлетворяет условию покупки - купить не хуже

Т.о. цена сделки будет равна 256.12 и биржа сгенерирует итоговую сделку:

T,1,B,4,50,4,256.12 -> совершена сделка ((T)rade) на покупку с номером 1 на покупку между заявками 4 и 50 в количестве 4 кг по цене 256.12

Следует заметить, что в качестве цены совершения сделки выбирается именно цена покупателя, т.к. он подал завявку раньше чем продавец, что следует из того, что номер заявки покупателя меньше номера завяки продавца

После совершения сделки заявка продавца удаляется с биржи, т.к. она была исполнена полностью, а заявка покупателя остается на бирже, но уже c остатком равным 10 - 4 = 6. Если условий для совершения сделок нет, то заявки остаются на бирже пока они не будут отозваны. Для того чтобы отозвать заявку, необходимо послать на биржу приказ на отмену заявки: С,4 -> отменить((C)ancel) заявку за номером 4

В реальных условиях на бирже много продавцов и покупателей, поэтому алгоритм исполнения заявки будет следующий:

Для новой заявки на покупку:

  1) Все заявки на продажу сортируются по цене в сторону возрастания и совершаются сделки прежде всего с заявками с наименьшей ценой, чтобы удовлетворить правилу наилучшей покупки (по наименьшей цене). Если есть несколько заявок на продажу по одинаковой цене, то в первую очередь исполняются заявки у которых номер заявки(ID) наименьший - правило FIFO (First In First Out) 
  2) Если нет заявок для совершения сделок, то заявка на покупку остается на бирже 

Для новой заявки на продажу: 

  1) Все заявки на покупку сортируются по цене в сторону убывания и совершаются сделки прежде всего с заявками на покупку по наибольшей цене, чтобы удовлетворить правилу наилучшей продажи (по наибольшей цене). Если есть несколько заявок на покупку по одинаковой цене, то в первую очередь исполняются заявки у которых номер заявки(ID) наименьший - правило FIFO (First In First Out) 
  2) Если нет завяок для совершения сделок, то заявка на продажу остается на бирже

Пример:
1. На бирже активны следующие заявки на покупку:

  ```
  О,1,B,Яблоки,10,250.12
  
  О,2,B,Яблоки,8,249.10
  
  О,3,B,Яблоки,5,250.12
  
  О,4,B,Яблоки,15,250.6
  ```
  
2. На биржу подается новая завка на продажу 31 кг яблок:

  ```
  O,5,S,Яблоки,31,248.5
  ```
  
3. Сортируем заявки на покупку в порядке убывания по цене + номер завяки:

  ```
  О,4,B,Яблоки,15,250.6
  
  О,1,B,Яблоки,10,250.12
  
  О,3,B,Яблоки,5,250.12
  
  О,2,B,Яблоки,8,249.10
  ```
  
4. видим, что ряд заявок удовлетворяют условиям совершения сделок, поэтому биржевой алгоритм генерирует сделки:

  ```
  T,1,B,4,5,15,250.6
  
  T,2,B,1,5,10,250.12
  
  T,3,B,3,5,5,250.12
  
  T,4,B,2,5,1,249.10
  ```
  
5. заявка на покупку остается на бирже но с остатком 7:

  ```
  O,2,B,Яблоки,7,249.10
  ```
  
6. все остальные завяки удаляются, т.к. полностью были исполнены

------------

Алгоритм биржевого симулятора: 

a) биржа торгует только яблоками

b) симулятор на вход принимает сигналы и исполняет их:
  1) O,\<OID\>,\<Side\>,\<Qty\>,\<Price\> - заявка на покупку((B)uy) или продажу ((S)ell):

    • <OID> - уникальный идентификатор заявки (число). На самом деле, OID Всегда увеличивающееся число

    • <Side> - покупка(B)/продажа(S)
    
    • <Qty> - количество купить/продать. Целое число
    
    • <Price> - цена покупки/продажи. Число с плавающей точкой – максимум 2 знака после запятой

  2) C,\<OID\> - отмена ранее выставленной завяки по номеру <OID>. Если происходит попытка отмены заявки, которая была уже отменена или исполнена, то операция отмены игнорируется

c) симулятор на выходе должен генерировать сделки и печатать:
  1) T,\<ID\>,\<Side\>,\<OID1\>,\<OID2\>,\<Trade Qty\>,\<Trade Price\> - сделка:
  
    • <ID> - уникальный идентификатор сделки (число, монотонно увеличивающееся)
  
    • <Side> - сторона сделки. Buy or Sell
  
    • <OID1> - идентификатор заявки с которой совершили сделку (выставленная ранее)
  
    • <OID2> - идентификатор заявки которая инициировала сделку. <OID1> всегда меньше <OID2>
  
    • <Trade Qty> - количество купленного/проданного товара
  
    • <Trade Price> - цена сделки
  
2) X,\<OID\> - информация, о том, что заявка была отменена
  
  d) алгоритм представляет собой консольное приложение, принимающее на вход (файл) заявок(O) и операция отмены(C) и печатающее (stdout или файл) сделки(T) и подтверждения отмены(X)
  
  e) для тестирования предлагается использовать файл input.txt с заявками и операциями отмены заявок и файл output.txt со сделками и подтверждениями отмены заявок
  
 
