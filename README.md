# NIRScanner-Python
This is an unofficial Python library for NIRScanner Nano produced by Texas Instruments.

# Prerequisite
- Operating System
  - Only tested on Linux (Ubuntu & Noobs for Pi)
- Dependencies
  - Python 2.7 or Python 3.5 above 
  - Development packages for libudev, libusb. Debian/Ubuntu systems can run:
    ```bash
    sudo apt-get install libudev-dev libusb-1.0-0-dev
    ```

# Quick Start
## Compile
Already compiled for Debian/Ubuntu systems.
```console
$ cd NIRScanner-Python
```
For compiling Python 2 library:
```console
NIRScanner-Python$ ./src/scripts/compile_py2.sh
```
For compiling Python 3 library:
```console
NIRScanner-Python$ ./src/scripts/compile_py3.sh
```
## Deploy
You need **_NIRScanner.so** and **NIRS.py** to be in your project path, where **_NIRScanner.so** is the compiled Python libray, and **NIRS.py** is the Python Class wrapper.
### Python 2
Use _NIRScanner.so.**2**
```console
NIRScanner-Python$ cp ./lib/_NIRScanner.so.2 <your-project-path>/_NIRScanner.so
NIRScanner-Python$ cp ./lib/NIRS.py <your-project-path>
```
### Python 3
Use _NIRScanner.so.**3**
```console
NIRScanner-Python$ cp ./lib/_NIRScanner.so.3 <your-project-path>/_NIRScanner.so
NIRScanner-Python$ cp ./lib/NIRS.py <your-project-path>
```

### Raspberry Pi with Python 2
Use _NIRScanner.so.**2.pi**
```console
NIRScanner-Python$ cp ./lib/_NIRScanner.so.2.pi <your-project-path>/_NIRScanner.so
NIRScanner-Python$ cp ./lib/NIRS.py <your-project-path>
```

## Run
```python
from NIRS import NIRS
nirs = NIRS()
results = nirs.scan()
```
That's it.

# FAQ
TBD

# License
This repository uses source codes from DLP NIRscan Nano GUI and DLP Spectrum Library, I didn't find any lisence within those repositories, please email me if there is.
