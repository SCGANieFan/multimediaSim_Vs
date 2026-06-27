Mode=$1



# make PLATFORM=linux V=1 
# make PLATFORM=linux -j V=1 
# make PLATFORM=arm -j V=1 

# 用 gdb 调试
# gdb build/linux/bin/app.elf

# valgrind 查内存泄漏
# valgrind build/linux/bin/app.elf

# make PLATFORM=linux clean


if [ -z "$Mode" ]; then
    make PLATFORM=linux -j V=1 
elif [ $Mode == "clean" ]; then
    make PLATFORM=linux clean
else
    echo "Mode：$name"
fi
