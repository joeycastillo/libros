# Libros - WIP firmware for the Open Book

For ESP32-S3 book, you must flash [babel.bin](https://www.oddlyspecificobjects.com/projects/openbook/babel.bin) using the following command: 

```bash
esptool.py --port PORT write_flash 0x340000 babel.bin 
```

TODO: rest of the README