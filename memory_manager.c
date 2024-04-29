#include <stdio.h>
#include <unistd.h>
#include <memory.h> /* For getpagesize()*/
#include <sys/mman.h> /* For using mmap()*/

static size_t SYSTEM_PAGE_SIZE = 0;

/*Function to set the system page size*/
void
mm_init(){
    SYSTEM_PAGE_SIZE = getpagesize();
}

/*Function to get Virtual Memory page from Linux kernel*/
static void*
mm_get_new_vm_page_from_kernel(int units){
    char *vm_page = (char *)mmap(
        0,
        units * SYSTEM_PAGE_SIZE,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_ANON | MAP_PRIVATE,
        0, 0 );

    if(vm_page == MAP_FAILED){
        printf("Error: VM Page allocation failed");
        return NULL;
    }
    
    memset(vm_page, 0, units*SYSTEM_PAGE_SIZE);
    return (void*)vm_page;
}

/*Function to return a page to kernel*/
static void
mm_return_vm_page_to_kernel(void* vm_page, int units){
    if(munmap(vm_page, units*SYSTEM_PAGE_SIZE )){
        printf("Error: Could not return vm page to kernel");
    }
}


int main(){

    mm_init();

    void* vm_page = mm_get_new_vm_page_from_kernel(2);

    if(vm_page){
        printf("Received 2 vm pages from kernel");
    }

    mm_return_vm_page_to_kernel(vm_page,2);

    return 0;
}
