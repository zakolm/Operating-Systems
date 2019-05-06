#include <linux/init.h> 
#include <linux/module.h> 
#include "md.h" 

MODULE_LICENSE( "GPL" ); 

char* md1_str_data = "Привет мир!";
int md1_int_data = 42;

extern char* md1_get_str(int n) 
{ 
	printk( "+ md1: md1_get_str() called!\n" ); 
	switch (n)
	{
	case 1:
		return "Hello world!\n";
		break;
	case 2:
		return "Привет Мир!\n";
		break;
	default:
		return "Передайте 1 для получения английского сообщения или 2 для получения русского.\n";
		break;
	}
} 

extern int md1_factorial(int n) 
{ 
	int i, ans;
	ans = 1;
		
	printk( "+ md1: md1_factorial() called!\n" ); 
	for (i = 2; i <= n; i++) ans *= i;
	
	return ans;
}

// экспортируем данные
EXPORT_SYMBOL(md1_str_data); 
EXPORT_SYMBOL(md1_int_data); 
// экспортируем функции
EXPORT_SYMBOL(md1_get_str); 
EXPORT_SYMBOL(md1_factorial); 


static int __init md_init( void ) 
{ 
   printk( "+ md1: module md1 start!\n" ); 
   return 0; 
}

static void __exit md_exit( void ) 
{ 
   printk( "+ md1: module md1 unloaded!\n" ); 
} 

module_init( md_init ); 
module_exit( md_exit ); 
