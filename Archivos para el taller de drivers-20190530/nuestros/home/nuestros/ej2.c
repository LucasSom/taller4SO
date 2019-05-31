//buen tutorial http://www.tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN121


#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/random.h>

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
static int limite_de_azar;
//int Device_Open=0;

/*
ssize_t open_nuestro(struct inode* inodo, struct file* archivo){
	
	if(Device_Open){
		return -EBUSY;
	}
	++Device_Open;

	try_module_get(THIS_MODULE);

	return 0;
}
*/

ssize_t read_nuestro(struct file* archivo, char __user* buffer_usuario, size_t tamanio_buffer, loff_t* offset){

	printk(KERN_ALERT "entramos a la funcion que lee\n");	
/*
	//TODO sacar, solo para experimentar
	if(!Device_Open){
		printk(KERN_ALERT "caimos en el debug feo\n");
		open_nuestro(NULL,NULL);
	}
*/
	if(limite_de_azar==-1){
		//si nunca escribio
		printk(KERN_ALERT "queremos leer pero nunca escribimos\n");
		return -EPERM;
	}

	//ya escribio alguna vez
	unsigned int rta=0;
	get_random_bytes(&rta,sizeof(int)-1);

	rta = rta % limite_de_azar;

	//esto nos va a copiar ints o caracteres??
	char *buffer = kmalloc(tamanio_buffer,GFP_KERNEL);
	int res = snprintf(buffer,tamanio_buffer,"%u\n",rta);
	unsigned long error_code = copy_to_user(buffer_usuario,buffer,tamanio_buffer);

	if(!error_code){
		//queremos devolverle el largo de lo que le escribimos..
		return res;	
	}else{
		printk(KERN_ALERT "hubo error al hacer read\n");
		return error_code;
	}
}

/*
unsigned int pow10(unsigned int j){
	unsigned int rta=1;
	int i=0;
	for (; i < j; ++i){
		rta*=10;
	}
	return rta;
}
*/

ssize_t write_nuestro(struct file* archivo, const char __user* buffer_usuario, size_t tamanio_buffer, loff_t* offset){

	printk(KERN_ALERT "entramos a la funcion que escribe\n");
/*
	//TODO sacar, solo para experimentar
	if(!Device_Open){
		printk(KERN_ALERT "caimos en el debug feo\n");
		open_nuestro(NULL,NULL);
	}
*/
	char* nuestro_input = kmalloc(tamanio_buffer+1,GFP_KERNEL);//esta bien fags=0??
	unsigned long problemas;
	problemas = copy_from_user(nuestro_input, buffer_usuario, tamanio_buffer);

	if(problemas){
		printk(KERN_ALERT "no funco el copy from user %d\n",problemas);
	}

/*
	unsigned long rta=0;
	unsigned int i;
	for(i=1; i<=tamanio_buffer; ++i){
		//recorremos el input
		//ojo sabemos que hay un bug paja con tener un cero a la izquierda del nro
		if(nuestro_input[tamanio_buffer-i] <= '9' && nuestro_input[tamanio_buffer-i] >= '0' ){
			rta += (nuestro_input[tamanio_buffer-i])*(pow10(i));
		}else{
			//si caemos aca nos dieron algo que no es un numero
			printk(KERN_ALERT "nos dieron algo que no es un numero\n");
			return -EPERM;
		}

	}
*/

	int error=0;
	//unsigned int diez = 10;
	nuestro_input[tamanio_buffer] = 0;
	error = kstrtoint(nuestro_input,10,&limite_de_azar);

	kfree(nuestro_input);

	if(error==-EINVAL){
		printk(KERN_ALERT "hay un error che: -EINVAL\n");
		return error;
	}else{
		if (error==-ERANGE){
			printk(KERN_ALERT "hay un error che: -ERANGE\n");
			return error;
		}
	}

	return tamanio_buffer;
}




/*
ssize_t close_nuestro(struct inode* inodo, struct file* archivo){

	if (!Device_Open){
		return -EBUSY;
	}

	--Device_Open;
	module_put(THIS_MODULE);

	return 0;
}
*/


static int __init hello_init(void) {
	printk(KERN_ALERT "----------------holaaa, empieza el azar----------------\n");

	limite_de_azar=-1; //si es -1 es que todavía no nos pasaron ningun string

	//aclaramos nuestras operaciones
	fops_nuestro.owner = THIS_MODULE;
	//fops_nuestro.open = open_nuestro;
	//fops_nuestro.release = close_nuestro;
	fops_nuestro.read = read_nuestro;
	fops_nuestro.write = write_nuestro;

	cdev_init(&cdev_nuestro, &fops_nuestro);

	//conseguimos el major y el minor
	char* dev_nombre = "azar";
	int region_debug = alloc_chrdev_region(&major, 0, 1, dev_nombre);

	//asignamos major y minor a nuestro dispositivo cdev_nuestro
	int add_debug = cdev_add(&cdev_nuestro, major, 1);

	//para que se creen los nodos desde el espacio de usuario
	mi_class = class_create(THIS_MODULE, dev_nombre);
	device_create(mi_class, NULL, major, NULL, dev_nombre);

	printk(KERN_ALERT "azar termino de inicializar\n");	
	return 0;
}



static void __exit hello_exit(void) {
	printk(KERN_ALERT "Adios, azar cruel\n");

	//liberamos los major y minor
	cdev_del(&cdev_nuestro);
	unregister_chrdev_region(major, 1);

	//pedimos la destruccion de los nodos
	device_destroy(mi_class, major);
	class_destroy(mi_class);

	printk(KERN_ALERT "----------------termino posta el azar----------------\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("strudel digital");
MODULE_DESCRIPTION("el mejor driver de la vida");




