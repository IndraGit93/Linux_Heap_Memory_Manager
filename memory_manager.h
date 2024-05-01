#include <stdint.h>

#define MAX_STRUCT_NAME_SIZE 50

/*Forward Declaration*/
struct vm_page_family_;

typedef struct vm_page_{
    struct vm_page_ *next;
    struct vm_page_ *prev;
    struct vm_page_family_ *pg_family; /*back pointer*/
    uint32_t page_index;
    uint32_t page_size;
  //  block_meta_data_t block_meta_data;
    char page_memory[0];
} vm_page_t;


typedef struct vm_page_family{
    char struct_name[MAX_STRUCT_NAME_SIZE];
    uint32_t struct_size;
    vm_page_t* first_page;

}vm_page_family_t;

typedef struct vm_page_for_families{
    vm_page_family_t vm_page_family[0];
    struct vm_page_for_families* next;
}vm_page_for_families_t;


vm_page_for_families_t* first_vm_page_for_families;

#define MAX_FAMILIES_PER_VM_PAGE\
    ((SYSTEM_PAGE_SIZE) - sizeof(vm_page_family_t *))/sizeof(vm_page_family_t)
    

#define ITERATE_PAGE_FAMILIES_BEGIN(vm_page_for_family_ptr, curr)\
{ \
    uint32_t count =0; \
    for (curr = (vm_page_family_t*)&vm_page_for_family_ptr->vm_page_family[0]; \
        curr->struct_size && count < MAX_FAMILIES_PER_VM_PAGE; \
        curr++,count++) { \

#define ITERATE_PAGE_FAMILIES_END(vm_page_for_familiy_ptr, curr) }}
