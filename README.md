Use this project to read, write, or execute any address you want via the format string attack. There are caveats of course. Not every address has read, write, or execute access. And the format string attack cannot work with null bytes (0x00). In the case of read/write, this means TARGET_ADDRESS needs to avoid the null byte. For execute, this means ret_addr (the return address of app_main) needs to avoid the null byte.

Use gdbinit for JTAG debugging, if you like.
