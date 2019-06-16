#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <linux/interrupt.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");

static int irq=1;
static int irq_counter = 0;
struct timeval time;

module_param(irq, int, S_IRUGO );
struct workqueue_struct *wq;

void hardwork_function(struct work_struct *work);

DECLARE_WORK(hardwork, hardwork_function);

void hardwork_function(struct work_struct *work)
{
    irq_counter++;

    printk("In the ISR: counter = %d ", irq_counter);
    do_gettimeofday(&time);
    printk("TIME: %.2lu:%.2lu:%.2lu: \r\n",
               (time.tv_sec / 3600) % (24),
               (time.tv_sec / 60) % (60),
               time.tv_sec % 60);
    return;
}

static irqreturn_t keybd_interrupt( int irq, void *dev_id )
{
    if (irq == 1) {

        queue_work(wq, &hardwork);
        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}

static int __init my_wokqueue_init(void)
{
    if( request_irq( irq, keybd_interrupt, IRQF_SHARED, "keybd_interrupt", (void*)keybd_interrupt ))
    	return -1;
    
    printk( KERN_INFO "Successfully loading ISR handler on IRQ %d\n", irq );
    wq = alloc_workqueue( "workqueue" ); //alloc 
    if ( wq )
        printk(KERN_INFO "Workqueue created.\n");

	printk(KERN_INFO "Module was load.\n");
	return 0;
}

static void __exit my_wokqueue_exit(void)
{
   flush_workqueue( wq );
   destroy_workqueue( wq );
   free_irq( irq, (void*)keybd_interrupt);
   printk( KERN_INFO "Module was unload" );
  return;
}

module_init(my_wokqueue_init);
module_exit(my_wokqueue_exit);
