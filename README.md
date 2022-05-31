# RISC-V Machine Timer Interruptions Demo

## Building
If necessary, modify the compiler toolchain path in `cross_file.txt`.
```
$ meson build --cross-file cross_file.txt
$ ninja -C build
```

## Running
```
$ qemu-system-riscv64 -machine sifive_u -smp 5 -nographic -bios none -kernel build/main
```
- Use `C-x a` to stop the emulator.

## Debugging
```
$ qemu-system-riscv64 -machine sifive_u -smp 5 -nographic -bios none -kernel build/main -gdb tcp::1235 -S
```
And on another terminal:
```
$ gdb-multiarch build/main -x gdb_init
```

## Credits
- A great deal of the code comes from [michaeljclark/riscv-probe](https://github.com/michaeljclark/riscv-probe) and we include their license.
- Some hints were taken from [repnop/vanadinite](https://github.com/repnop/vanadinite/).
