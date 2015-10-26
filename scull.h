#ifndef _SCULL_H_
#define _SCULL_H_

#undef SDEBUG

#ifdef SCULL_DEBUG
#ifdef __KERNEL__
/* kernel space */
#define SDEBUG(fmt, args...) printk(KERN_DEBUG fmt, ## args)
#else
#define SDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#endif
#else
#define SDEBUG(fmt, args...) /* nothing */
#endif

#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0
#endif

#ifndef SCULL_NR_DEVS
#define SCULL_NR_DEVS 4
#endif

#ifndef SCULL_BLKSIZE
#define SCULL_BLKSIZE 4000
#endif

#ifndef SCULL_BLKNR
#define SCULL_BLKNR 1000
#endif

#define nullptr 0x0

struct scull_data {
	void **data;			/* Pointer to actual data block */
	struct scull_data *next;
};

struct scull_dev {
	struct scull_data *data;	/* Pointer to first scull data */
	int blk_size;			/* Size of each block in data */
	int blk_nr;			/* No of blocks in data */
	unsigned long data_size;	/* Amount of data stored here */
	struct cdev cdev;		/* Char device struct used by kernel */
};

extern int scull_major;		/* Major number for the device */
extern int scull_nr_devs;	/* Number of devices */
extern int scull_blk_size;	/* Size of each data block */
extern int scull_blk_nr;	/* Number of data blocks */

#endif /* _SCULL_H_ */
