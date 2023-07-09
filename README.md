# nowhere-status
This project started out of boredom. As most programming projects do.
A simple status bar command I use for sway. It's mildly broken but it's a neat project.
Currently weather module is broken, not the actual module but the logic that goes behind it is.
I am considering making the weather module a completely different binary. 
Essentially you would run `make weather` and it would make the nowhere-status binary with weather support built in.

## Build instructions
In the base directory of the repository just run
```bash
make
```

## Usage
Usage: `nowhere-status [options]`
```
    -o, --offline                   does not retrieve weather info
    
    -l STRING, --location=STRING    specify where weather is pulled from
    
    -v, --version                   prints version and exits
    
    -h, --help                      prints this message and exits
```

## Example usage
Once make is invoked a build directory will be created with all the object files. This directory will contain the nowhere-status binary. This example shows usage of a couple available flags. This is executed in a terminal however at the current moment this is a swaybar exclusive application. It will just output raw json.
```bash
./build/nowhere-status --offline
```
