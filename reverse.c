#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define NUMBER 0b1100111
#define EXPECTED 0b1110011
#define DEVICE_NAME "reverse"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gus Workman");
MODULE_DESCRIPTION("Reverses an integer");

// Function Prototypes
int reverse(int number, size_t length);
static int reverse_open(struct inode *, struct file *);
static int reverse_release(struct inode *, struct file *);
static ssize_t reverse_read(struct file *, char *, size_t, loff_t *);
static ssize_t reverse_write(struct file *, const char *, size_t, loff_t *);

// Variables
static int major_number;
static char write_buf[32] = {0};
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = reverse_read,
    .write = reverse_write,
    .open = reverse_open,
    .release = reverse_release,
};

// Function definitions

int reverse(int number, size_t length) {
  int reversed = 0;
  int i = 0;

  for (i = 0; i < length; i++) {
    reversed = (reversed << 1) + ((number >> i) & 0x01);
  }

  return reversed;
}

static int reverse_open(struct inode *inode, struct file *file) {
  printk(KERN_INFO "Reverse open\n");
  return 0;
}
static int reverse_release(struct inode *inode, struct file *file) {
  printk(KERN_INFO "Reverse close\n");
  return 0;
}
static ssize_t reverse_read(struct file *file, char *buffer, size_t length,
                            loff_t *offset) {
  printk(KERN_INFO "Reverse read\n");
  return -EINVAL;
}
static ssize_t reverse_write(struct file *file, const char *buffer,
                             size_t length, loff_t *offset) {
  printk(KERN_INFO "Attempted to write %d bytes to file\n", length);

  int number = 0;
  int i = 0;
  int valid_bits = 0;

  if (length > 32) {
    return -EINVAL;
  }

  // this part tripped me up - I crashed the kernel the first time
  // by trying to directly access user space
  copy_from_user(write_buf, buffer, length);

  for (i = 0; i < length; i++) {
    // check if the buffer index contains ASCII "1"
    if (write_buf[i] == 0x31) {
      number = (number << 1) + 1;
      valid_bits++;

    // check if the buffer index contains ASCII "0"
    } else if (write_buf[i] == 0x30) {
      number = number << 1;
      valid_bits++;

    // Ignore ASCII "\n", other values error
    } else if (write_buf[i] != 0x0A) {
      printk(KERN_INFO "Invalid string character: %d\n", write_buf[i]);
      return -EINVAL;
    }
  }
  printk(KERN_INFO "Attempted to convert binary string to integer: 0x%x\n", number);
  printk(KERN_INFO "Reversed number from input: 0x%x\n", reverse(number, valid_bits));
  return length;
}
// Module init/exit

static int __init reverse_init(void) {
  printk(KERN_INFO "Hello, World !\n");
  printk(KERN_INFO "Original: 0x%x\n", NUMBER);
  printk(KERN_INFO "Reversed: 0x%x\n", reverse(NUMBER, 7));
  printk(KERN_INFO "Expected: 0x%x\n", EXPECTED);

  // set up the cdevice here
  major_number = register_chrdev(0, DEVICE_NAME, &fops);
  if (major_number < 0) {
    printk(KERN_INFO "Unable to register device\n");
  } else {
    printk(KERN_INFO "Registered device with major number: %d\n", major_number);
  }

  return 0;
}
static void __exit reverse_exit(void) {
  unregister_chrdev(major_number, DEVICE_NAME);
  printk(KERN_INFO "Goodbye, World !\n");
}
module_init(reverse_init);
module_exit(reverse_exit);
