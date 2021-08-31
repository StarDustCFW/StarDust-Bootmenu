![License badge](https://img.shields.io/badge/license-GPLv3-blue.svg)
[![Homebrew Store](https://img.shields.io/badge/Homebrew%20Switch-store-%23ff4554.svg)](https://www.switchbru.com/appstore/#/app/argon-nx)

# StarDust Bootmenu

Is not recomended use this out of StarDustCFWpack

## What StarDust Boot is?
![picture](sd-card-example/screenshot.png)

- **Custom backgrounds** can be added by placing `background.bmp` file inside `StarDust` directory. **The background must be smaller or equal than 720x1280**.
- **Custom titles** can be added by placing `title.bmp` file inside `StarDust` directory. **There is not a specific size for title**.
- Take **screenshots** to share your StarDust Boot gui.
- FULL touch support. Thanks to @pixel-stuck


## About BMP format

The only format supported is **BMP 32 bit ARGB color**.
The color used for transparency is **#1D1919**.

Find these logos [here](img/example-logos) and the background and title [here](sd-card-example/).

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
* __xalgovia__ and __Retrogamer 74__ for the splash and logos. Also thanks them to use ArgonNX.
* __langerhans__ and  __stuck-pixel__ for their implementation of touch support.
* __D3fau4__ for touch support testing.
* [pwall2222](https://github.com/pwall2222) Code Support
