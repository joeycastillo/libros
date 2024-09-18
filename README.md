# Libros - WIP firmware for the Open Book

For ESP32-S3 book, you must flash [babel.bin](https://www.oddlyspecificobjects.com/projects/openbook/babel.bin) using the following command: 

```bash
esptool.py --port PORT write_flash 0x340000 babel.bin 
```

# Build instructions for ESP-32

1. Open project root directory with VS-Code
2. If needed, install PlatformIO IDE extension in VS-Code
3. Select PlatformIO icon from icon bar
4. Under Project tasks, select: `esp32-s3-devkitc-1`->`General`->`Build`

Expect to see:
```
Retrieving maximum program size .pio/build/esp32-s3-devkitc-1/firmware.elf
Checking size .pio/build/esp32-s3-devkitc-1/firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [=         ]   8.1% (used 26384 bytes from 327680 bytes)
Flash: [==        ]  18.8% (used 629181 bytes from 3342336 bytes)
======================== [SUCCESS] Took 2.79 seconds ========================

Environment         Status    Duration
------------------  --------  ------------
esp32-s3-devkitc-1  SUCCESS   00:00:02.789
======================== 1 succeeded in 00:00:02.789 ========================
```

You can do the same from command line:
`platformio run -e esp32-s3-devkitc-1`

TODO: rest of the README
