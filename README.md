![License badge](https://img.shields.io/badge/license-GPLv3-blue.svg)
[![Homebrew Store](https://img.shields.io/badge/Homebrew%20Switch-store-%23ff4554.svg)](https://www.switchbru.com/appstore/#/app/argon-nx)

# StarDust Bootmenu

Is not recomended use this out of StarDustCFWpack

## What StarDust Boot is?
![picture](https://raw.githubusercontent.com/Kronos2308/argon-nx/master/sd-card-example/old-stuf/screenshot.png)


## Features

- **Autolaunch/autochainload** the payload named `payload.bin` inside `StarDust Boot` directory in your sd card root.
- If `StarDust/payload.bin` is not found or `VOLUME_DOWN_BUTTON` is held during StarDust Boot injection, StarDust Boot will list all the payloads located at `StarDust/payloads`, so you can select one of them to launch it.
- **Customize payloads' logos**. **Logos must be smaller or equal than 200x200**. Example:
```
StarDust
  ├───logos
  │     default.bmp       # Default logo (logo for all payloads)
  │     Atmosphere.bmp # Logo for fusee-primary.bin payload
  │
  ├───payloads
  │     Atmosphere.bin
  │     ReiNX.bin         # Will be rendered using default logo
  │     
  └───playback
      hekate
```

- **Custom backgrounds** can be added by placing `background.bmp` file inside `StarDust` directory. **The background must be smaller or equal than 720x1280**.
- **Custom titles** can be added by placing `title.bmp` file inside `StarDust` directory. **There is not a specific size for title**.
- Take **screenshots** to share your StarDust Boot gui.
- FULL touch support. Thanks to @pixel-stuck


## About BMP format

The only format supported is **BMP 32 bit ARGB color**.
The color used for transparency is **#1D1919**.

## GUI

This image was captured with the **screenshot** feature.

Find these logos [here](img/example-logos) and the background and title [here](sd-card-example/).

The layout of files on the sd card for this image:
```
StarDust
├─── payloads
│       Atmosphere.bin
│       ReiNX.bin
│       fusee-gelee.bin
│       hekate.bin
│       SXOS.bin
│
├─── logos
|       Atmosphere.bmp
|       Reinx.bmp
|       hekate.bmp
|       SXOS.bmp
|
├─── background.bmp
└─── title.bmp
```

## Improve performance

StarDust Boot can use **minerva dram training** to improve performance.
The use of minerva is optional but recommended. To use minerva just place the compiled `minerva.bso` inside `StarDust/sys`. The directory `StarDust/sys` with minerva, is included in `sd-files.zip` in the release section.

## Compatibility

This payload will launch any CFW or payload. 

## Roadmap

1. Kind of config file
2. Change to prettier font



## Credits

* __devkitPro__ for the [devkitA64](https://devkitpro.org/) toolchain.
* __naehrwert__ and __st4rk__ for the original [hekate](https://github.com/nwert/hekate) project and its hwinit code base.
* __CTCaer__ for the continued [hekate](https://github.com/CTCaer/hekate) and his **minerva** project.
* __xalgovia__ and __Retrogamer 74__ for the splash and logos. Also thanks them to use StarDust Boot in RetroReloaded.
* __langerhans__ and  __stuck-pixel__ for their implementation of touch support.
* __D3fau4__ for touch support testing.
