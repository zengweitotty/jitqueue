/*
    file Name:      jitqueue.c
    Author:         zengweitotty
    version:        V1.0
    Data:           2013/04/01
    Email:          zengweitotty@gmail.com
    Description     delay using work_queue,
*/
#include <linux/init.h> //using for module_init and some related
#include <linux/kernel.h>   //using for printk and some related
#include <linux/module.h>
#include <linux/hardirq.h>  //using for in_interrupt
#include <linux/proc_fs.h>  //using for create_proc_read_entry
#include <linux/moduleparam.h>  //using for module_param
#include <linux/slab.h> //using for kmalloc
#include <linux/errno.h>    //using for Error Number
#include <linux/jiffies.h>  //using for jiffies
#include <linux/wait.h> //using for wait
#include <linux/sched.h>    //using for schedule
#include <linux/workqueue.h>	//using for work queue

struct jitqueue_data{
	struct delayed_work work0;
	struct delayed_work work1;
	char* buf;
	int len;
	unsigned long prevjiffies;
	int loop;
	wait_queue_head_t wait;
};

//struct workqueue_struct *workqueue = NULL;
struct jitqueue_data *jitData;

static void jitqueue_work_fn(struct work_struct *work){
	struct delayed_work *delayed_work_temp = container_of(work,struct delayed_work,work);
	struct jitqueue_data *jitDatatemp = container_of(delayed_work_temp,struct jitqueue_data,work0);
	jitDatatemp->prevjiffies = jiffies;
	//if(jitDatatemp->loop-- > 0){
	//	INIT_DELAYED_WORK(&jitDatatemp->work1,jitqueue_work_fn);
		jitDatatemp->buf += sprintf(jitDatatemp->buf,"The previous jiffies is %9li\t.and is in interrupt %d",jitDatatemp->prevjiffies,in_interrupt()?1:0);
	//	if(queue_delayed_work(workqueue,&jitDatatemp->work1,HZ) == 1){
	//	    printk(KERN_INFO "[jitqueue/jitqueue_work_fn] Success to join the work queue\n");
	//	}else{
	//	    printk(KERN_INFO "[jitqueue/jitqueue_work_fn] May be work has joined the work queue\n");
	//	}
	//}else{
		//wake_up_interruptible(&jitDatatemp->wait);		
	//}
	printk(KERN_INFO "[jitqueue/jitqueue_work_fn] work queue's work is running\n");		
}

static int jitqueue_fn(char* buf,char** start,off_t offset,int len,int *eof,void* data){
	jitData->loop = 2;
	jitData->buf = buf;
	jitData->prevjiffies = jiffies;
	INIT_DELAYED_WORK(&jitData->work0,jitqueue_work_fn);
	jitData->buf += sprintf(jitData->buf,"The previous jiffies is %9li\t.and is in interrupt %d",jitData->prevjiffies,in_interrupt()?1:0);
	schedule_delayed_work(&jitData->work0,HZ);
	//if(queue_delayed_work(workqueue,&jitData->work0,HZ) == 1){
	//	printk(KERN_INFO "[jitqueue/jitqueue_fn] Success to join the work queue\n");		
	//}else{
	//	printk(KERN_INFO "[jitqueue/jitqueue_fn] May be work has joined the work queue\n");		
	//}

	//wait_event_interruptible(jitData->wait,!jitData->loop);
	//if(signal_pending(current))
	//	return -ERESTARTSYS;
//	if(!cancel_delayed_work(&jitData->work0)){
//		printk(KERN_INFO "[jitqueue/jitqueue_fn] May the work queue run in some CPU\n");
//		return -EFAULT;
//	}
//	if(!cancel_delayed_work(&jitData->work1)){
//		printk(KERN_INFO "[jitqueue/jitqueue_fn] May the work queue run in some CPU\n");
//		return -EFAULT;
//	}
//	flush_workqueue(jitData->workqueue);
	*eof = 1;
	jitData->len = jitData->buf - buf;
	return jitData->len;
}

static int __init jitqueue_init(void){
	jitData = kmalloc(sizeof(struct jitqueue_data),GFP_KERNEL);
	if(!jitData){
		printk(KERN_INFO "[jitqueue/jitqueue_init] Can not malloc struct jitData\n");
		return -ENOMEM;
	}
	memset(jitData,0,sizeof(struct jitqueue_data));
	//workqueue = create_singlethread_workqueue("jit_queue");
	//if(!workqueue){
	//	kfree(jitData);
	//	return -ENOMEM;		
	//}
	init_waitqueue_head(&jitData->wait);
	create_proc_read_entry("jitqueue",0,NULL,jitqueue_fn,NULL);
	printk(KERN_INFO "[jitqueue/jitqueue_init] Success initialize proc\n");
	return 0;
}

static void __exit jitqueue_exit(void){
	//if(!cancel_delayed_work(&jitData->work)){
	//	printk(KERN_INFO "[jitqueue/jitqueue_fn] May the work queue run in some CPU\n");
		//return -EFAULT;
	//}
	//flush_workqueue(jitData->workqueue);
	//destroy_workqueue(workqueue);
	flush_scheduled_work();
	remove_proc_entry("jitqueue",NULL);
	kfree(jitData);
	printk(KERN_INFO "[jitqueue/jitqueue_exit] Success exit!\n");
}

module_init(jitqueue_init);
module_exit(jitqueue_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zengweitotty");
MODULE_DESCRIPTION("using workqueue to delay");

