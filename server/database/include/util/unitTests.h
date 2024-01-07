#ifndef LAB1_TESTS_H
#define LAB1_TESTS_H
#include "../../include/file/fileApi.h"
#include "stdio.h"
#include "../../include/file/tableBlocks.h"
#include <string.h>
#include "../../include/query/query.h"
#include "../../include/util/util.h"
#include "../../include/platformic/cutfile.h"


#ifdef _WIN32
#define FILE_NAME "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\file.bin"
#define FILE_NAME_1 "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\testInsert.bin"
#define FILE_NAME_2 "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\data.bin"
#else
#define FILE_NAME "/home/iwaa0303/CLionProjects/llp_3/server/database/test/file.bin"
#define FILE_NAME_1 "/home/iwaa0303/CLionProjects/llp_3/server/database/test/testInsert.bin"
#define FILE_NAME_2 "/home/iwaa0303/CLionProjects/llp_3/server/database/test/data.bin"
#endif

void test1(FILE* file);

void test2(FILE* file);

void test3(FILE* file);

void test4(void);

void test5(FILE* file);

void test6(void);

void test7(void);

void test8(void);

void test9(void);

void test10(FILE* file);

void test11(void);

void test12(void);

void test13(void );

void testInsert(void );

#endif //LAB1_TESTS_H
