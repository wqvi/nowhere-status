# nowhere-status
This project started out of boredom. As most programming projects do.
A simple status bar command I use for sway. It's mildly broken but it's a neat project.

## Build instructions
In the base directory of the repository just run
```bash
make
```
If you want to clean all binary files run
```bash
make clean
```

## Example usage
Once make is invoked a bin directory will be created with all the object files. This directory will contain the nowhere-status binary. This application is meant to be exclusively ran by sway's `status_command`. If ran in the terminal it will just produce raw json.
```bash
./bin/nowhere-status
```
