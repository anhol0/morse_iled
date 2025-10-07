# morse_iled for Lenovo ThinkPad laptops

### Lenovo ThinkPad laptops come with the red LED light on the back but no one seemed to do anything with it. This program takes control over the LED and displays  morse code in it depending on input and dictionary that user provided

## Building

### Requirements:

* ThinkPad Laptop with Linux distribution
* thinkpad_acpi module installed and running
* GNU Make
* GCC

Installation of required packages:

Debian based:

`$ sudo apt-get install make gcc firmware-linux`

Arch based:

`$ sudo pacman -S make gcc linux-firmware`

#### Make the package:

In the project directory run `$ make`

Binary *morse-iled* will be located in the `./build` directory

#### Install the program:

`$ sudo make install` with optional `PREFIX` flag which is defaulted to */usr/local/bin*

#### Uninstall the program:

`$ sudo make remove` to remove program from the location it was installed to (IF INSTALLED IN CUSTOM LOCATION USE `PREFIX` flag when running command)

## Usage:

### To start the daemon:

`$ sudo morse-iled -d`

### To run the client:

`$ morse-iled -s "MESSAGE TO DISPLAY" -f <PATH TO DICTIONARY>`

The default dictionary is located in the project's root directory and called `english.cfg`. You can provide the absolute or relative path to the dictionary but there have to be one.

## Documentation on how to write config files

**!!!At this point only English language is supported but addition for all unicode characters will be added!!!**

To write config file use the pattern:

`<letter/character> <morse code conststing of "." and "-" only>`

On every line **without empty lines**

Example:

```
a .-
b -...
c -.-.
```

And so on

## Credits
1. [GNU Project](https://www.gnu.org/) for **make** and **gcc**
2. [Linux kernel](https://github.com/torvalds/linux) for **thinkpad_acpi**
