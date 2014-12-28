#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/random.h>

MODULE_AUTHOR("hiroki.kana@gmail.com");
MODULE_DESCRIPTION("return random characters");
MODULE_LICENSE("GPL");

static int access_num;
static spinlock_t randchar_spin_lock;
static char* module_name = "[randchar]";
static char* devname = "randchar";
static int devmajor = 77;

static int randchar_open( struct inode* inode, struct file* filep )
{
	printk( KERN_INFO "%s:open() called\n", module_name );
	
	spin_lock(&randchar_spin_lock);

	if ( access_num ) {
		spin_unlock (&randchar_spin_lock);
		return -EBUSY;
	}
	
	access_num++;
	spin_unlock(&randchar_spin_lock);
	
	return 0;
} 

static int randchar_release( struct inode* inode, struct file* filep )
{
	printk( KERN_INFO "%s:close() called\n", module_name );

	spin_lock(&randchar_spin_lock);
	access_num--;
	spin_unlock(&randchar_spin_lock);

	return 0;
}

static ssize_t randchar_read( struct file* filep, char* buf, size_t count, loff_t* pos) 
{
	unsigned int i;
	char* alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	get_random_bytes(&i, 1);
	if (copy_to_user(buf, &alpha[i % strlen(alpha)], 1) ) {
		printk(KERN_INFO "%s:copy_to_user failed\n", module_name);
		return -EFAULT;
	}
	return 1;
}


static struct file_operations randchar_fops = 
{
	owner :THIS_MODULE,
	read :randchar_read,
	open :randchar_open,
	release :randchar_release,
};

static int randchar_init( void )
{
	if ( register_chrdev(devmajor, devname, &randchar_fops )) {
		printk( KERN_INFO "%s:register randchar failed\n", module_name);
		return -EBUSY;
	}
	spin_lock_init(&randchar_spin_lock);
	printk(KERN_INFO "%s: loaded into kernel\n", module_name);
	return 0;
}

static void randchar_cleanup(void) 
{
/*
	if (unregister_chrdev(devmajor, devname)) {
		printk( KERN_INFO "%s: unregister randchar failed\n", module_name);
	}
*/
	unregister_chrdev(devmajor, devname);

	printk( KERN_INFO "%s: removed fron kernel\n", module_name);
}	
module_init(randchar_init);
module_exit(randchar_cleanup);	
