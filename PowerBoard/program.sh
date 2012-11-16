echo 'Erasing chip...'
dfu-programmer at32uc3a3256 erase || exit

echo 'Flashing chip...'
dfu-programmer at32uc3a3256 flash powerBoard.hex --suppress-bootloader-mem || exit

echo 'Running code...'
dfu-programmer at32uc3a3256 start || exit
