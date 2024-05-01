#include <stdio.h>
#include <unistd.h>
#include <memory.h> /* For getpagesize()*/
#include <sys/mman.h> /* For using mmap()*/
#include "memory_manager.h"
#include <bits/mman-linux.h>
#include <assert.h>

static size_t SYSTEM_PAGE_SIZE = 0;

/*Function to set the system page size*/
void
mm_init(){
    //SYSTEM_PAGE_SIZE = getpagesize();
     SYSTEM_PAGE_SIZE = sysconf(_SC_PAGESIZE);
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


void mm_instantiate_new_page_family(char* struct_name, __uint32_t struct_size){
    
    vm_page_family_t* vm_page_family_curr = NULL;
    vm_page_for_families_t* new_vm_page_for_families;

    if(struct_size > SYSTEM_PAGE_SIZE){
        printf("Error: %s() structure %s Size exceeds system page size", __FUNCTION__, struct_name);
    }

    if(!first_vm_page_for_families){
        first_vm_page_for_families = (vm_page_for_families_t*)mm_get_new_vm_page_from_kernel(1);
        first_vm_page_for_families->next = NULL;
        strncpy(first_vm_page_for_families->vm_page_family[0].struct_name, struct_name, MAX_STRUCT_NAME_SIZE);
        first_vm_page_for_families->vm_page_family[0].struct_size = struct_size;
        return;
    }
    __uint32_t cnt =0;
    ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_for_families, vm_page_family_curr){

        if(strncmp(vm_page_family_curr->struct_name, struct_name, MAX_STRUCT_NAME_SIZE) != 0){
            cnt++;
            continue;
        }
        assert(0);
    }ITERATE_PAGE_FAMILIES_END(first_vm_page_for_families, vm_page_family_curr);

    if(cnt == MAX_FAMILIES_PER_VM_PAGE){
        new_vm_page_for_families = (vm_page_for_families_t*)mm_get_new_vm_page_from_kernel(1);
        new_vm_page_for_families->next = first_vm_page_for_families;
        vm_page_family_curr = &new_vm_page_for_families->vm_page_family[0];
        strncpy(vm_page_family_curr->struct_name, struct_name, MAX_STRUCT_NAME_SIZE);
        vm_page_family_curr->struct_size = struct_size;
        return;
    }

    strncpy(vm_page_family_curr->struct_name, struct_name, MAX_STRUCT_NAME_SIZE);
    vm_page_family_curr->struct_size = struct_size;
    vm_page_family_curr->first_page = NULL;
        return;

}


typedef struct emp{
    int num;
    int age;
}emp_t;


int main(){

    mm_init();

    void* vm_page = mm_get_new_vm_page_from_kernel(2);

    if(vm_page){
        printf("Received 2 vm pages from kernel");
    }

    mm_return_vm_page_to_kernel(vm_page,2);


    mm_instantiate_new_page_family("emp", sizeof(emp_t));
    return 0;
}
