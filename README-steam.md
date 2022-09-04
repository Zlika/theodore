# How to install the core on the Steam version of  RetroArch

# Linux

After building the core, copy the core file and the info file using :

```
cp theodore_libretro.so ~/.local/share/Steam/steamapps/common/RetroArch/cores/
wget -O ~/.local/share/Steam/steamapps/common/RetroArch/cores/theodore_libretro.info https://github.com/libretro/libretro-super/blob/master/dist/info/theodore_libretro.info
```

The core will appear in the available cores.
