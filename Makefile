CONFIG_MODULE_SIG = n
TARGET_MODULE := fibdrv2

obj-m := $(TARGET_MODULE).o
fibdrv2-objs := fibdrv.o bnum.o
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) client client_bn_500 client_bn client_time kernel_fib
	$(MAKE) -C $(KDIR) M=$(PWD) modules

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

clean_png:
	$(RM) *.png

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) client out client_bn client_bn_500 client_time kernel_fib
load:
	sudo insmod $(TARGET_MODULE).ko
unload:
	sudo rmmod $(TARGET_MODULE) || true >/dev/null

client_bn_500: client_bn_500.c
	$(CC) -o $@ $^

client: client.c
	$(CC) -o $@ $^

client_bn: client_bn.c
	$(CC) -o $@ $^
	
kernel_fib: kernel_fib.c
	$(CC) -o $@ $^ -lm

client_time: client_time.c
	$(CC) -o $@ $^ -lm

PRINTF = env printf
PASS_COLOR = \e[32;01m
NO_COLOR = \e[0m
pass = $(PRINTF) "$(PASS_COLOR)$1 Passed [-]$(NO_COLOR)\n"

check: all
	$(MAKE) unload
	$(MAKE) load
	sudo ./client > out
	$(MAKE) unload
	@diff -u out scripts/expected.txt && $(call pass)
	@scripts/verify.py

check_bn: all
	$(MAKE) unload
	$(MAKE) load
	$(MAKE) client_bn
	sudo ./client_bn > out
	$(MAKE) unload

check_bn_500: all
	$(MAKE) unload
	$(MAKE) load
	sudo ./client_bn_500 > out
	$(MAKE) unload
	@diff -u out scripts/expected_500.txt
