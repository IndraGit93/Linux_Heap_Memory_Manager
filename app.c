#include "uapi_mm.h"

typedef struct emp{
    int no;
    int age;
}emp_t;

typedef struct student{
    int no;
    int age;
    char name[20];
}student_t;

int main(){
    mm_init();

    MM_REG_STRUCT(emp_t);

    MM_REG_STRUCT(student_t);

    mm_print_registered_page_families();
 
    vm_page_family_t *reg_struct  = lookup_page_family_by_name ("student");
    if(reg_struct){
        printf("structure found");
    }else{
        printf("Not found");
    }
    return 0;
}