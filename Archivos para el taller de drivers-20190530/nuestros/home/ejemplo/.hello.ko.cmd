cmd_/home/ejemplo/hello.ko := ld -r -m elf_i386 -T /usr/src/linux-headers-3.2.0-23-generic-pae/scripts/module-common.lds --build-id  -o /home/ejemplo/hello.ko /home/ejemplo/hello.o /home/ejemplo/hello.mod.o