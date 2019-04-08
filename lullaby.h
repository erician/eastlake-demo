#ifndef LULLABY_H_
#define LULLABY_H_

#include <sys/types.h>
#define __user 

/*
 * struct po_stat
 */
struct po_stat {
    dev_t     st_dev;         	    /* ID of device containing po*/
    mode_t    st_mode;        	    /* po type and mode */
    nlink_t   st_nlink;       	    /* Number of hard links, 暂时还不用管这个 */
    uid_t     st_uid;         		/* User ID of owner */
    gid_t     st_gid;         		/* Group ID of owner */
    off_t     st_size;        		/* Total size, in bytes */
    /* Since Linux 2.6, the kernel supports nanosecond
        precision for the following timestamp fields.
        For the details before Linux 2.6, see NOTES. */
    struct timespec st_atim;  	    /* Time of last access */
    struct timespec st_mtim;        /* Time of last modification */
    struct timespec st_ctim;  	    /* Time of last status change */

    #define st_atime st_atim.tv_sec      /* Backward compatibility */
    #define st_mtime st_mtim.tv_sec
    #define st_ctime st_ctim.tv_sec
};


/*
 * system calls in lullaby.
 * attention: we will not use these functions directly, 
 * instead we will use syscall by passing syscall number to it.
 */
long sys_po_creat(const char __user *poname, mode_t mode);
long sys_po_unlink(const char __user *poname);
long sys_po_open(const char __user *poname, int flags, mode_t mode);
long sys_po_close(unsigned int pod);
long sys_po_mmap(unsigned long addr, unsigned long len, \
                 unsigned long prot, unsigned long flags, \
                 unsigned long pod, off_t pgoff);
long sys_po_munmap(unsigned long addr, size_t len);
// sys_po_extend
// sys_po_shrink
long sys_po_stat(const char __user *poname, struct po_stat __user* statbuf);
long sys_po_fstat(unsigned int pod, struct po_stat __user* statbuf);

/*
 * user library functions in lullaby
 */

/* 
 * for a po, the return value of first call po_malloc, if success, 
 * is the with the return value of sys_po_mmap with pgoff 0, if success.  
 */
void *po_malloc(int pod, size_t size);
void po_free(int pod, void *ptr);

#endif
