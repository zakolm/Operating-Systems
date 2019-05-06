#include <linux/init.h> 
#include <linux/module.h>

MODULE_LICENSE( "GPL" ); 

static int __init md_init( void ) 
{ 
   printk( "+ md: module md start!\n" ); 
   return 0; 
}

static void __exit md_exit( void ) 
{ 
   printk( "+ md: module md unloaded!\n" ); 
} 

module_init( md_init ); 
module_exit( md_exit ); 
