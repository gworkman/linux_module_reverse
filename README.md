# Description

This kernel module parses a binary string to integer and reverses the bits of the string.

# Installation

On a linux machine with the correct build tools and linux header files, run `make` to build the module.

Then, run `sudo insmod reverse.ko`.

After the module is loaded, check the logs with `dmesg` to get the major number. Then, create the interface file with `sudo mknod /dev/reverse MAJOR_NUBMER 0`, where `MAJOR_NUMBER` was the number from the logs previously.

To remove the module, run `sudo rmmod reverse.ko`

# Usage

To use the function of the module, write some valid data to the `/dev/reverse` file. Valid data may only contain the characters `0`, `1`, and `\n`.

The reversed output of the module will be available in the `dmesg` logs.

# Future improvements

As it currently stands, this is not a very useful kernel module. Some easy future improvements:

- Return the last reversed integer during a read to the file, such as `cat /dev/reverse`
- Automatically create or close the interface file when the module is loaded or removed
- Improve the string to integer parsing. Allow for different integer formats (decimal, hexadecimal, octal, binary)

