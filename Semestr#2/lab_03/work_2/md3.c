#include <linux/init.h> 
#include <linux/module.h> 
#include "md.h" 

MODULE_LICENSE( "GPL" ); 

static int __init md_init( void ) 
{ 
   printk( "+ md3: module md3 start!\n" ); 
   printk( "+ md3: Число экспортированное из md1 : %d\n", md1_int_data ); 
   printk( "+ md3: Строка экспортированная из md1 : %s\n", md1_str_data ); 
   printk( "+ md3: Результат работы функции md1_get_str(0) : %s\n", md1_get_str(0) );
   printk( "+ md3: Результат работы функции md1_get_str(1) : %s\n", md1_get_str(1) );
   printk( "+ md3: Результат работы функции md1_get_str(2) : %s\n", md1_get_str(2) );
   printk( "+ md3: Результат работы функции md1_factorial(4) : %d\n", md1_factorial(4) );  
   return -1; 
} 

module_init( md_init ); 
