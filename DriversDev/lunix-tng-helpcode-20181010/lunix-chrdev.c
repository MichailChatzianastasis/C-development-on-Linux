/*
 * lunix-chrdev.c
 *
 * Implementation of character devices
 * for Lunix:TNG
 *
 * < Michail Chatzianastasis , Dimitris Katsiros >
 *allagh
 */

#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mmzone.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>

#include "lunix.h"
#include "lunix-chrdev.h"
#include "lunix-lookup.h"

/*
 * Global data
 */
struct cdev lunix_chrdev_cdev;

/*
 * Just a quick [unlocked] check to see if the cached
 * chrdev state needs to be updated from sensor measurements.
 */
static int lunix_chrdev_state_needs_refresh(struct lunix_chrdev_state_struct *state)
{
	//elegxume an oi metrhseis einai freskes
	struct lunix_sensor_struct *sensor;

	WARN_ON ( !(sensor = state->sensor));
	/* ? */

	/* The following return is bogus, just for the stub to compile */
	return 0; /* ? */
}

/*
 * Updates the cached state of a character device
 * based on sensor data. Must be called with the
 * character device state lock held.
 */
static int lunix_chrdev_state_update(struct lunix_chrdev_state_struct *state)
{
	struct lunix_sensor_struct *sensor;
	sensor=state->sensor;


	//spinlocks
	if(lunix_chrdev_state_needs_refresh()== 0){
		//return 0;
		//debug("leaving\n");
	}
	size_t buf_size = 20;
	//memcpy(state->buf_data,/* metrhsh sensora */,buf_size);
	// sigoura irthan freska dedomena , ananeose ta kai ananeose tin ora
//	lunix_sensor_update(sensor,);
//	state->buf_timestamp =
	return 1;
	//spinlock
	/*
	 * Grab the raw data quickly, hold the
	 * spinlock for as little as possible.
	 */
	/* ? */
	/* Why use spinlocks? See LDD	lunix_sensor_update(sensor,);
3, p. 119 */

	/*
	 * Any new data available?
	 */
	/* ? */

	/*
	 * Now we can take our time to format them,
	 * holding only the private state semaphore
	 */

	/* ? */

	debug("leaving\n");
	return 0;
}

/*************************************
 * Implementation of file operations
 * for the Lunix character device
 *************************************/

static int lunix_chrdev_open(struct inode *inode, struct file *filp)
{
	/* Declarations */
	/* ? */
	int ret,minor_n;

	debug("entering\n");
	ret = -ENODEV;
	if ((ret = nonseekable_open(inode, filp)) < 0)
		goto out;

	/*
	 * Associate this open file with the relevant sensor based on
	 * the minor number of the device node [/dev/sensor<NO>-<TYPE>]
	 */
	 minor_n = MINOR(inode-> i_rdev);
	 int sensor_number = minor_n/8;
	 int msr_number = minor_n%8;
	 printk("o sensor einai o %d , kai h metrhsh eiani h %d/n",sensor_number,msr_number);

	/* Allocate a new Lunix character device private state structure */
	/* ? */

	/*struct cdev* new_lunix;
	new_lunix= inode->i_cdev;
	filp->private_data=new_lunix;*/

	struct lunix_chrdev_state_struct* lunix_state;
	lunix_state = kmalloc(sizeof(struct lunix_chrdev_state_struct),GFP_KERNEL);
	lunix_state->minor_n = minor_n;
	lunix_state->f_pos = kmalloc(sizeof(loff_t),GFP_KERNEL);
  *(lunix_state->f_pos) =1 ;
	filp->private_data = lunix_state;
	lunix_state->buf_timestamp=0;



out:
	debug("leaving, with ret = %d\n", ret);
	return ret;
}

static int lunix_chrdev_release(struct inode *inode, struct file *filp)
{
	/* ? */

	struct lunix_chrdev_state_struct *lunix_state;
	lunix_state=filp->private_data;
	kfree(lunix_state->f_pos);
	kfree(lunix_state);
	printk("Device file closed,released memory");
	return 0;
}

static long lunix_chrdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	/* Why? */

	return -EINVAL;
}




static ssize_t lunix_chrdev_read(struct file *filp, char __user *usrbuf, size_t cnt, loff_t *f_pos)
{
	ssize_t ret;
	int buf_size = 20;

	struct lunix_sensor_struct *sensor;
	struct lunix_chrdev_state_struct *state;

	state = filp->private_data;
	WARN_ON(!state);

	sensor = state->sensor;
	WARN_ON(!sensor);
	loff_t* f_pos = state->f_pos
	int index,newmetr;
	index = *f_pos - 1;
	if(*f_pos==1) {
			if(lunix_chrdev_state_update(state)==0) return 0;}
	if(*f_pos>=1 ){
		if(*f_pos -1 + cnt >= buf_size) {
			//exceeds
			//have to return cnt buf_size - (*f_pos -1) bytes
			if(copy_to_user(usrbuf,&(lunix_state->buf_data[index]),buf_size -(*f_pos-1))!=0) return -EFAULT;
			*f_pos=1;
			return (buf_size-index);
		}
		else{
		// have to return cnt bytes to user
		if(copy_to_user(usrbuf,&(lunix_state->buf_data[index]),cnt)!=0) return -EFAULT;
		*f_pos = *f_pos + cnt;
		return cnt;
	}
	}

	/* Lock? */
	/*
	 * If the cached character device state needs to be
	 * updated by actual sensor data (i.e. we need to report
	 * on a "fresh" measurement, do so
	 */
	if (*f_pos == 0) {
		while (lunix_chrdev_state_update(state) == -EAGAIN) {
			/* ? */
			/* The process needs to sleep */
			/* See LDD3, page 153 for a hint */
		}
	}

	/* End of file */
	/* ? */

	/* Determine the number of cached bytes to copy to userspace */
	/* ? */

	/* Auto-rewind on EOF mode? */
	/* ? */
out:
	/* Unlock? */
	return ret;
}

static int lunix_chrdev_mmap(struct file *filp, struct vm_area_struct *vma)
{
	return -EINVAL;
}

static struct file_operations lunix_chrdev_fops =
{
        .owner          = THIS_MODULE,
	.open           = lunix_chrdev_open,
	.release        = lunix_chrdev_release,
	.read           = lunix_chrdev_read,
	.unlocked_ioctl = lunix_chrdev_ioctl,
	.mmap           = lunix_chrdev_mmap
};

int lunix_chrdev_init(void)
{
	/*
	 * Register the character device with the kernel, asking for
	 * a range of minor numbers (number of sensors * 8 measurements / sensor)
	 * beginning with LINUX_CHRDEV_MAJOR:0
	 */
	int ret;
	dev_t dev_no;
	unsigned int lunix_minor_cnt = lunix_sensor_cnt << 3;

	debug("initializing character device\n");
	cdev_init(&lunix_chrdev_cdev, &lunix_chrdev_fops);
	lunix_chrdev_cdev.owner = THIS_MODULE;
	lunix_chrdev_cdev.ops= &lunix_chrdev_fops;
	dev_no = MKDEV(LUNIX_CHRDEV_MAJOR, 0);
	ret=register_chrdev_region(dev_no,16,"lunix_chrdev");
	/* ? */
	/* register_chrdev_region? */
	if (ret < 0) {
		debug("failed to register region, ret = %d\n", ret);
		goto out;
	}
	/* ? */
	/* cdev_add? */
	ret=cdev_add(&lunix_chrdev_cdev,dev_no,16);
	if (ret < 0) {
		debug("failed to add character device\n");
		goto out_with_chrdev_region;
	}
	debug("completed successfully\n");
	return 0;

out_with_chrdev_region:
	unregister_chrdev_region(dev_no, lunix_minor_cnt);
out:
	return ret;
}

void lunix_chrdev_destroy(void)
{
	dev_t dev_no;
	unsigned int lunix_minor_cnt = lunix_sensor_cnt << 3;

	debug("entering\n");
	dev_no = MKDEV(LUNIX_CHRDEV_MAJOR, 0);
	cdev_del(&lunix_chrdev_cdev);
	unregister_chrdev_region(dev_no, lunix_minor_cnt);
	debug("leaving\n");
}
