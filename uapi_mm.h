#ifndef UAPI_MM_H
#define UAPI_MM_H

#include "memory_manager.h"

/* Initialization functions*/
void mm_init();


/* Declaration functions*/
void mm_instantiate_new_page_family(char* struct_name, __uint32_t struct_size);

#define MM_REG_STRUCT(struct_name) \
    mm_instantiate_new_page_family(#struct_name, sizeof(struct_name)) 



#endif  //END_UAPI_MM_H