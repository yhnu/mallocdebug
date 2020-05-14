#include <stdio.h>
#include <malloc.h>
#include <dlfcn.h>
#include "test.h"
static long m_count = 0;

static void swap_hook (void);

static void * new_malloc (size_t size, const void * caller);
static void new_free (void *ptr, const void *caller);

static void * (*old_hook_malloc)(size_t size, const void * caller);
static void (*old_hook_free)(void *ptr, const void *caller);

void (*volatile __malloc_initialize_hook) (void) = swap_hook;

static void __attribute__((constructor)) swap_hook(void)
{
	old_hook_malloc = __malloc_hook;
	__malloc_hook = new_malloc;
	old_hook_free = __free_hook;
	__free_hook = new_free;
}

struct layout
{
	void *next;
	void *return_address;
};
void * __stackdump_known_base;
static void * new_malloc(size_t size, const void *caller)
{
	void *ptr;
	__malloc_hook = old_hook_malloc;
	ptr = malloc(size);
	m_count++;
	old_hook_malloc = __malloc_hook;
    
    // printf("malloc4 (%u) \n", (unsigned int) size);
    // void * ret = __builtin_return_address(1);
    // printf("ret address [%p]\n", ret);
    // void * caller2 = __builtin_frame_address(0);
    // printf("call address [%p] caller2=%p\n", caller, caller2);

    // Dl_info dlinfo;
    // //void *ip = ret;
    // if(!dladdr(caller, &dlinfo)) {
    //     perror("addr not found\n");        
    // }

    // const char *symname = dlinfo.dli_sname;
    // int f = 0;
    // printf("% 2d: %p %s+%u (%s)\n",++f, caller,symname, 0,dlinfo.dli_fname);


    void *top_frame = __builtin_frame_address(0);
	void *top_stack = ({ void *__dummy; &__dummy; });
	unsigned int cnt = 0;
	int array_count = 10;

	struct layout *current = (struct layout *)top_frame;

	while(cnt < array_count)
	{
		if((void *)current < top_stack || (__stackdump_known_base!=NULL && (void *)current > __stackdump_known_base))
			break;
        
        Dl_info dlinfo;
        if(!dladdr(current->return_address, &dlinfo)) {
            printf("addr not found %p\n", current->return_address);
            break;
        }
        const char *symname = dlinfo.dli_sname;    
        printf("% 2d: %p caller=%p %s+%x (%s)\n",cnt, current->return_address, caller , symname, (long)current->return_address - (long)dlinfo.dli_saddr, dlinfo.dli_fname);
		
        //printf("ret_addr %d: %p\n", cnt, current->return_address);
		current = (struct layout *)current->next;

		if(((unsigned long) current) & 1)
			break;

		cnt ++;
	}

	// printf("malloc() called: size = %lu; caller: %p; ptr_res = %p; state: %ld \n", size, caller, ptr, m_count);
	__malloc_hook = new_malloc;

	return ptr;
}

static void new_free(void * ptr, const void *caller)
{
    if(ptr)
    {
        __free_hook = old_hook_free;
        free(ptr);
        m_count--;
        old_hook_free = __free_hook;
        printf("free() called: caller: %p; ptr = %p; state: %ld \n", caller, ptr, m_count);
        __free_hook = new_free;
    }	
}

void test()
{
    int a[5]={0};
    void (*malloc2)();
    void *p = dlopen("/data/local/tmp/libtest.so", RTLD_NOW);
    if(p)
    {   
        malloc2 = (void (*)())dlsym(p, "malloc10");
        malloc2();
        malloc2 = (void (*)())dlsym(p, "malloc11");
        malloc2();

        // if(malloc2)
        // {
        //     malloc2();            
        // }        
        // else
        // {
        //     //p = malloc(10);            
        //     //a[10]  = 100;
        //     printf("malloc2 failed\n");
        // }
    }   
    else
    {        
        printf("dlopen failed\n");
    }
}

int main(int argc, char** argv)
{  
    test();
    return 0;
}