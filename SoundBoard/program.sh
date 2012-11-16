set -e

make

echo 'Erasing chip...'
dfu-programmer at32uc3a3256 erase

echo 'Flashing chip...'
dfu-programmer at32uc3a3256 flash soundBoard.hex --suppress-bootloader-mem

echo 'Running code...'
dfu-programmer at32uc3a3256 start
