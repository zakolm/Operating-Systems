#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fortune Cookie Kernel Module");

#define COOKIE_BUF_SIZE PAGE_SIZE
#define TEMP_BUF_SIZE 256

/*
    Работа со структурой task struct
    next_task, prev_stack
    создать кольцо, связанный список
    вывести информацию о процессе
    в режиме ядра

    включить просмотр структур task struct

    начало init_tusk
    до тех пор пока не встретим init_task
*/

ssize_t fortune_read(struct file *file, char *buf, size_t count, loff_t *f_pos);
ssize_t fortune_write(struct file *file, const char *buf, size_t count, loff_t *f_pos);

int fortune_init(void);
void fortune_exit(void);

struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = fortune_read,
    .write = fortune_write,
};

static char *cookie_buf;
static struct proc_dir_entry *proc_entry;
static unsigned read_index;
static unsigned write_index;

char temp[TEMP_BUF_SIZE];

struct task_struct *task = &init_task;

int len, t_len;

ssize_t fortune_read(struct file *file, char *buf, size_t count, loff_t *f_pos)
{
    if (*f_pos > 0)
        return 0;

    if (read_index >= write_index)
        read_index = 0;

    len = 0;
    t_len = 0;

    if (write_index > 0)
    {
        len = sprintf(temp, "%s\n", &cookie_buf[read_index]);

        copy_to_user(buf, temp, len);
        buf += len;
        read_index += len;
    }

    do {
        t_len = sprintf(temp, "+_+ название: %s [%d]\t\tродитель название: %s [%d]\n", task->comm, task->pid, task->parent->comm, task->parent->pid);

        copy_to_user(buf, temp, t_len);
        buf += t_len;
        len += t_len;
    } while((task = next_task(task)) != &init_task);

    t_len = sprintf(temp, "+_+ Current task is %s [%d]\n", current->comm, current->pid);
    copy_to_user(buf, temp, t_len);
    buf += t_len;
    len += t_len;

    *f_pos += len;

    return len;
}

ssize_t fortune_write(struct file *file, const char *buf, size_t count, loff_t *f_pos)
{
    int space_available = (COOKIE_BUF_SIZE - write_index) + 1;

    if (count > space_available)
    {
        printk(KERN_INFO "+_+ cookie pot is full\n");
        return -ENOSPC;
    }

    if (copy_from_user(&cookie_buf[write_index], buf, count))
        return -EFAULT;

    write_index += count;
    cookie_buf[write_index - 1] = 0;

    return count;
}

int fortune_init(void)
{
    cookie_buf = (char *) vmalloc(COOKIE_BUF_SIZE);

    if (!cookie_buf)
    {
        printk(KERN_INFO "+_+ not enough memory for the cookie pot\n");
        return -ENOMEM;
    }

    memset(cookie_buf, 0, COOKIE_BUF_SIZE);
    proc_entry = proc_create("fortune", 0644, NULL, &fops);

    if (!proc_entry)
    {
        vfree(cookie_buf);
        printk(KERN_INFO "+_+ Couldn't create proc entry\n");
        return -ENOMEM;
    }

    read_index = 0;
    write_index = 0;

    proc_mkdir("my_dir_in_proc", NULL);
    proc_symlink("my_symbolic_in_proc", NULL, "/proc/fortune");

    printk(KERN_INFO "+_+ fortune module loaded.\n");
    return 0;
}

void fortune_exit(void)
{
    remove_proc_entry("fortune", NULL);

    if (cookie_buf)
        vfree(cookie_buf);

    printk(KERN_INFO "+_+ fortune module unloaded.\n");
}

module_init(fortune_init);
module_exit(fortune_exit);
