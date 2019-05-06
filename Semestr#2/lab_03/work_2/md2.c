#include <linux/init.h> 
#include <linux/module.h> 
#include "md.h" 

MODULE_LICENSE( "GPL" ); 

static int __init md_init( void ) 
{ 
   printk( "+ md2: module md2 start!\n" ); 
   printk( "+ md2: Число экспортированное из md1 : %d\n", md1_int_data ); 
   printk( "+ md2: Строка экспортированная из md1 : %s\n", md1_str_data ); 
   printk( "+ md2: Результат работы функции md1_get_str(0) : %s\n", md1_get_str(0) );
   printk( "+ md2: Результат работы функции md1_get_str(1) : %s\n", md1_get_str(1) );
   printk( "+ md2: Результат работы функции md1_get_str(2) : %s\n", md1_get_str(2) );
   printk( "+ md2: Результат работы функции md1_factorial(4) : %d\n", md1_factorial(4) );  
   return 0; 
} 

static void __exit md_exit( void ) 
{ 
   printk( "+ md2: module md2 unloaded!\n" ); 
} 

module_init( md_init ); 
module_exit( md_exit );
