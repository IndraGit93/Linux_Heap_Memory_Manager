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
        printf("Error: Could not return vm page to kernel\n");
    }
}


void mm_instantiate_new_page_family(char* struct_name, __uint32_t struct_size){
    vm_page_family_t* vm_page_family_curr = NULL;
    vm_page_for_families_t* new_vm_page_for_families;

    if(struct_size > SYSTEM_PAGE_SIZE){
        printf("Error: %s() structure %s Size exceeds system page size \n", __FUNCTION__, struct_name);
        return;
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

void mm_print_registered_page_families(){
    vm_page_family_t* vm_page_family_curr = NULL;
    
    if(!first_vm_page_for_families){
        printf("Error: No page family registered yet! \n");
        return;
    }
    printf("List of total structures registered with LMM:\n");
    ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_for_families, vm_page_family_curr){

        printf("struct name %s, struct size: %d \n", vm_page_family_curr->struct_name, vm_page_family_curr->struct_size);


    }ITERATE_PAGE_FAMILIES_END(first_vm_page_for_families, vm_page_family_curr);    

}

vm_page_family_t * lookup_page_family_by_name (char *struct_name){
    vm_page_family_t* vm_page_family_curr = NULL;
    ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_for_families, vm_page_family_curr){
        printf("struct name %s, struct size: %d \n", vm_page_family_curr->struct_name, vm_page_family_curr->struct_size);
        if(strncmp(vm_page_family_curr->struct_name, struct_name, MAX_STRUCT_NAME_SIZE) == 0){
            return vm_page_family_curr;
            
        }
    }ITERATE_PAGE_FAMILIES_END(first_vm_page_for_families, vm_page_family_curr);    
     printf("Error: No structure with name: %s registered with LMM \n", struct_name);
     return NULL;
}

void
mm_print_vm_page_details(vm_page_t *vm_page){

    printf("\t\t next = %p, prev = %p\n", vm_page->next, vm_page->prev);
  //  printf("\t\t page family = %s\n", vm_page->pg_family->struct_name);

    uint32_t j = 0;
    block_meta_data_t *curr;
    ITERATE_VM_PAGE_ALL_BLOCKS_BEGIN(vm_page, curr){

        printf("\t\t\t%-14p Block %-3u %s  block_size = %-6u  "
                "offset = %-6u  prev = %-14p  next = %p\n",
                curr,
                j++, curr->is_free ? "F R E E D" : "ALLOCATED",
                curr->block_size, curr->offset,
                curr->prev_block,
                curr->next_block);
    } ITERATE_VM_PAGE_ALL_BLOCKS_END(vm_page, curr);
}

static void
mm_union_free_blocks(block_meta_data_t *first,
        block_meta_data_t *second){

    assert(first->is_free == MM_TRUE &&
            second->is_free == MM_TRUE);

    first->block_size += sizeof(block_meta_data_t) +
        second->block_size;

    first->next_block = second->next_block;

    if(second->next_block)
        second->next_block->prev_block = first;
}

vm_bool_t
mm_is_vm_page_empty(vm_page_t *vm_page){

    if(vm_page->block_meta_data.next_block == NULL &&
            vm_page->block_meta_data.prev_block == NULL &&
            vm_page->block_meta_data.is_free == MM_TRUE){

        return MM_TRUE;
    }
    return MM_FALSE;
}

static inline uint32_t
mm_max_page_allocatable_memory(int units){

    return (uint32_t)
        ((SYSTEM_PAGE_SIZE * units) - offset_of(vm_page_t, page_memory));
}


vm_page_t *
allocate_vm_page(vm_page_family_t *vm_page_family){

    vm_page_t *vm_page = mm_get_new_vm_page_from_kernel(1);

    /*Initialize lower most Meta block of the VM page*/
    MARK_VM_PAGE_EMPTY(vm_page);

    vm_page->block_meta_data.block_size =
        mm_max_page_allocatable_memory(1);
    vm_page->block_meta_data.offset =
        offset_of(vm_page_t, block_meta_data);
    init_glthread(&vm_page->block_meta_data.priority_thread_glue);
    vm_page->next = NULL;
    vm_page->prev = NULL;

    /*Set the back pointer to page family*/
    vm_page->pg_family = vm_page_family;

    /*If it is a first VM data page for a given
     * page family*/
    if(!vm_page_family->first_page){
        vm_page_family->first_page = vm_page;
        return vm_page;
    }
        /* Insert new VM page to the head of the linked 
     * list*/
    vm_page->next = vm_page_family->first_page;
    vm_page_family->first_page->prev = vm_page;
    vm_page_family->first_page = vm_page;
    return vm_page;
}