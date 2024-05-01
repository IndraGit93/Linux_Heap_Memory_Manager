#include <unity.h>
#include <uapi_mm.h>

typedef struct emp{
    int no;
    int age;
}emp_t;

typedef struct student{
    int no;
    int age;
    char name[20];
}student_t;


void setUp(void) {
    mm_init();
}

void tearDown(void) {

}

void test_mm_init(void) {
    
    MM_REG_STRUCT(emp_t);
    MM_REG_STRUCT(student_t);
    vm_page_family_t *reg_struct  = lookup_page_family_by_name ("student_t");
    TEST_ASSERT_EQUAL(0 , strncmp(reg_struct->struct_name, "student_t", MAX_STRUCT_NAME_SIZE));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_mm_init);
    return UNITY_END();
}