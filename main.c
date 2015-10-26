#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/cdev.h>	/* cdev_init(), cdev_del() */
#include <linux/slab.h> /* kmalloc(), kfree() */
#include <linux/fs.h>	/* file_operations, file, register_chrdev_region etc) */

#include "scull.h"

int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS;
int scull_blk_size = SCULL_BLKSIZE;
int scull_blk_nr = SCULL_BLKNR;

/*
 * module_param is used to get parameters which calling
 * insmod or modprobe
 */
module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_blk_size, int, S_IRUGO);
module_param(scull_blk_nr, int, S_IRUGO);

MODULE_AUTHOR("Chidambaram Ramanathan");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev **scull_devices;

int scull_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev;

	/*
	 * We have a pointer to i_cdev, which is the cdev on which open
	 * was made. During cdev_init, it would have registered the
	 * pointer in the inode. Now, some process has opened our device
	 * file and the driver is notified of it.
	 * container_of, will eventually give us the pointer to scull_dev
	 * given the pointer to cdev, which is a member of scull_dev.
	 */
	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev;

	return 0;	/* Success */
}

int scull_release(struct inode *inode, struct file *filp)
{
	return 0;
}

struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.llseek = nullptr,
	.read = nullptr,
	.write = nullptr,
	.open = scull_open,
	.release = scull_release,
};

void scull_setup_cdev(struct scull_dev *dev, int id)
{
	int err;
	int devno = MKDEV(scull_major, scull_minor + id);

	SDEBUG("scull_setup_cdev entered\n");

	cdev_init(&(dev->cdev), &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	err = cdev_add(&dev->cdev, devno, 1);

	if (err)
		SDEBUG("Error %d adding scull%d\n",err,id);

	SDEBUG("cdev_add succeeded\n");
}

void scull_exit_module(void)
{
	int i;
	dev_t devno = MKDEV(scull_major, scull_minor);

	if (scull_devices) {
		for (i=0; i < scull_nr_devs; i++) {
			cdev_del(&scull_devices[i]->cdev);
		}
	}

	unregister_chrdev_region(devno, scull_nr_devs);
}

int scull_init_module(void)
{
	dev_t dev = 0;
	int i, result;

	SDEBUG("Entered init function\n");

	if (scull_major) {
		/*
		 * dev_t has 12 bits major no, 20 bits minor no.
		 * MKDEV, makes a _u32 number by doing bit arithmetic
		 * 	so that major and minor numbers are in dev_t.
		 */
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	} else {
		result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs,
					      "scull");
		SDEBUG("alloc_chrdev_region returned %d\n",result);

		/*
		 * MAJOR gets the major number part in dev_t
		 */
		scull_major = MAJOR(dev);
	}

	SDEBUG("Got major number %d \n",scull_major);

	if (result < 0) {
		SDEBUG("Can't get major number\n");
		goto fail;
	}

	scull_devices = (struct scull_dev **)kmalloc(scull_nr_devs
				* sizeof(struct scull_dev**), GFP_KERNEL);

	if (!scull_devices) {
		result = -ENOMEM;
		goto fail;
	}

	SDEBUG("About to memset\n");
	memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev**));

	for (i=0; i < scull_nr_devs; i++) {
		scull_devices[i] = (struct scull_dev *)kmalloc(
				sizeof(struct scull_dev), GFP_KERNEL);
	}

	/* Initialize each devices */
	for (i=0; i < scull_nr_devs; i++) {
		SDEBUG("About to set up scull_devices\n");
		scull_devices[i]->blk_size = scull_blk_size;
		scull_devices[i]->blk_nr = scull_blk_nr;
		SDEBUG("About to call scull_setup\n");
		scull_setup_cdev(scull_devices[i], i);
	}

	return 0; /* success */

fail:
	scull_exit_module();
	return result;
}

module_init(scull_init_module);
module_exit(scull_exit_module);
