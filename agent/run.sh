make
gcc -fPIC -shared -o hooking.so ../hooking/hooking_module.c -ldl
echo -e "\033[31m"Test"\033[0m"
LD_PRELOAD=./hooking.so ./monitor
./monitor