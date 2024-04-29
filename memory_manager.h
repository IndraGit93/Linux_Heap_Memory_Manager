#include <cstdint>

#define MAX_STRUCT_NAME_SIZE 50

typedef struct vm_page_family{
    char struct_name[MAX_STRUCT_NAME_SIZE];
    uint32_t struct_size;
}vm_page_family_t;

typedef struct vm_page_for_families{
    vm_page_family_t vm_page_family[0];
}vm_page_for_families_t;

#define MAX_FAMILIES_PER_VM_PAGE \
    ((SYSTEM_PAGE_SIZE) - sizeof(vm_page_family_t *))/sizeof(vm_page_family_t); \ 
    

#define ITERATE_PAGE_FAMILIES_BEGIN(vm_page_family_ptr, curr) \
    uint32_t count =0; \
    for (curr = (vm_page_family_t*)&vm_page_family_ptr->vm_page_family[0]; \
        curr->struct_size && count < MAX_FAMILIES_PER_VM_PAGE; \
        curr++,count++) { \

#define ITERATE_PAGE_FAMILIES_END(vm_page_familiy_ptr, curr) } \
