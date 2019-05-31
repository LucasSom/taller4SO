//buen tutorial http://www.tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN121


#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>

/*struct cdev { 
    struct kobject kobj; 
    struct module *owner; 
    const struct file_operations *ops; 
    struct list_head list; 
    dev_t dev; 
    unsigned int count; 
};*/

/*static struct file_operations {
	struct module *owner; //this_module macro
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*open) (struct inode *, struct file *);
	ssize_t (*release) (struct inode *, struct file *);
};*/

static struct cdev cdev_nuestro;
static struct file_operations fops_nuestro;
dev_t major;
static struct class *mi_class;

ssize_t read_nuestro(struct file* archivo, char __user* buffer_usuario, size_t tamanio_buffer, loff_t* offset){

}

ssize_t write_nuestro(struct file* archivo, const char __user* buffer_usuario, size_t tamanio_buffer, loff_t* offset){

}

ssize_t open_nuestro(struct inode* inodo, struct file* archivo){

}

ssize_t close_nuestro(struct inode* inodo, struct file* archivo){

}



static int __init hello_init(void) {
	printk(KERN_ALERT "----------------holaaa, empieza el nulo----------------\n");

	//aclaramos nuestras operaciones
	fops_nuestro.owner = THIS_MODULE;
	fops_nuestro.open = open_nuestro;
	fops_nuestro.release = close_nuestro;
	fops_nuestro.read = read_nuestro;
	fops_nuestro.write = write_nuestro;

	cdev_init(&cdev_nuestro, &fops_nuestro);

	//conseguimos el major y el minor
	char* dev_nombre = "nulo";
	int region_debug = alloc_chrdev_region(&major, 0, 1, dev_nombre);

	//asignamos major y minor a nuestro dispositivo cdev_nuestro
	int add_debug = cdev_add(&cdev_nuestro, major, 1);

	//para que se creen los nodos desde el espacio de usuario
	mi_class = class_create(THIS_MODULE, dev_nombre);
	device_create(mi_class, NULL, major, NULL, dev_nombre);

	printk(KERN_ALERT "nulo termino de inicializar\n");	
	return 0;
}



static void __exit hello_exit(void) {
	printk(KERN_ALERT "Adios, nulo cruel\n");

	//liberamos los major y minor
	cdev_del(&cdev_nuestro);
	unregister_chrdev_region(major, 1);

	//pedimos la destruccion de los nodos
	device_destroy(mi_class, major);
	class_destroy(mi_class);

	printk(KERN_ALERT "----------------termino posta el nulo----------------\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("strudel digital");
MODULE_DESCRIPTION("el mejor driver de la vida");




