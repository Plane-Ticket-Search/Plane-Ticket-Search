首先需要四个类型的txt文件，分别是
customer.txt(这个是总的客户名单）
客户名.txt（这个是每个客户的航班信息，以客户的姓名作为名字）
航班号HC.txt（这个是每个航班的候补客户名单，以航班号+HC作为名字）
航班号BC.txt（这个是每个航班的订票客户名单，以航班号+BC作为名字）
在main函数里要初始化客户表、哈希表、航班信息如下既写入insert_customer();
   inithashtable();
   insert_hashtable();语句，全都是void不用加任何东西
查询功能通过search_cnumber();即可实现，全都是void不用加任何东西
四种txt文件的内容另外说明

***：客户姓名等信息避免中文