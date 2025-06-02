# 编译器设置
CC = gcc
CFLAGS = -g -Wall

# 构建目标
all: test_assign1_1

# 链接目标文件
test_assign1_1: test_assign1_1.o storage_mgr.o dberror.o
	$(CC) $(CFLAGS) -o test_assign1_1 test_assign1_1.o storage_mgr.o dberror.o

# 编译测试文件
test_assign1_1.o: test_assign1_1.c test_helper.h storage_mgr.h dberror.h
	$(CC) $(CFLAGS) -c test_assign1_1.c

# 编译存储管理器
storage_mgr.o: storage_mgr.c storage_mgr.h dberror.h
	$(CC) $(CFLAGS) -c storage_mgr.c

# 编译错误处理模块
dberror.o: dberror.c dberror.h
	$(CC) $(CFLAGS) -c dberror.c

# 清理中间文件和可执行文件
clean: 
	rm -f test_assign1_1 *.o *~

# 运行测试
run:
	./test_assign1_1 