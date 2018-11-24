#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>		  /* for put_user */
#include <linux/device.h>         // Header to support the kernel Driver Model

#define  CLASS_NAME  "charDesencr"        ///< The device class -- this is a character device driver

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "desencriptador"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80					/* Max length of the message from the device */


MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Jeremias Benitez, Tomas Sarquis");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Un desencriptador de mensajes simple");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

static int majorNumber;		/* majorNumber number assigned to our device driver */
static int Device_Open = 0;	/* Is device open? Used to prevent multiple access to device */
static int timesOpened = 0;
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;

static struct class*  charEncryptorClass  = NULL; ///< The device-driver class struct pointer
static struct device* charEncryptorDevice = NULL; ///< The device-driver device struct pointer

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

int init_module(void)
{
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber<0)
		return majorNumber;

	// Registro la device class
	charEncryptorClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(charEncryptorClass))
	{                // Checkeo errores
		unregister_chrdev(majorNumber, DEVICE_NAME);
		return PTR_ERR(charEncryptorClass);
	}

	// Registro el device driver
	charEncryptorDevice = device_create(charEncryptorClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(charEncryptorDevice))
	{               // Checkeo errores
		class_destroy(charEncryptorClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		return PTR_ERR(charEncryptorDevice);
	}
	return 0;
}

void cleanup_module(void)
{

	device_destroy(charEncryptorClass, MKDEV(majorNumber, 0));     // remove the device
	class_unregister(charEncryptorClass);                          // unregister the device class
	class_destroy(charEncryptorClass);                             // remove the device class
	unregister_chrdev(majorNumber, DEVICE_NAME);             	   // unregister the major number
}

static int device_open(struct inode *inode, struct file *file)
{
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	timesOpened++;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;		/* We're now ready for our next caller */

	/*
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module.
	 */
	module_put(THIS_MODULE);

	return 0;
}

static ssize_t device_read(struct file *filp,
						   char *buffer,	// buffer a llenar con datos
						   size_t length,	// longitud del buffer
						   loff_t * offset){

	int error_count = 0;
   	// copy_to_user has the format ( * to, *from, size) and returns 0 on success
   	error_count = copy_to_user(buffer, msg, length);

   	return error_count;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	static int i;
	sprintf(msg, "%s", buff);   // Guardo el mensaje recibido (en buff), en el string msg

	//Encripto el mensaje
    for (i = 0; i < len; i++){
        msg[i] = msg[i] - 1;
    }

    //Hago que el puntero apunte al mensaje para cuando lo lea
    msg_Ptr = msg;
	return 0;
}
