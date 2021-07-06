# NIRScanner-Python
This is an unofficial Python library for NIRScanner Nano produced by Texas Instruments.

# Prerequisite
- Operating System
  - Only tested on Linux (Ubuntu & Noobs for Pi)
  - Tested with virtual machine (VMWare Workstation Player)
- Dependencies
  - Python 2.7 or Python 3.5 above 
  - Development packages for libudev, libusb. Debian/Ubuntu systems can run:
    ```bash
    sudo apt-get install libudev-dev libusb-1.0-0-dev python-dev python3-dev
    ```

# Implemented features
- Perform a scan.
- Get scanning result.
- Config the scanning pattern.
- Set PGA gain.
- Reset error status.
- Set hibernation mode.
- Keep the lamp on / off. 

If you need / implemented a new feature, you may send me an email / pull request.

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
You need root (sudo) permission. Otherwise you may need a workaround such as in [1].
```python
from NIRS import NIRS
nirs = NIRS()
results = nirs.scan()
```
Note if you are using virtual environment or Anaconda, make sure you are using the absolute python path for sudo. 
```console
$ which python
/your/path/to/python
$ sudo /your/path/to/python your_python_code.py
```
For jupyter
```console
$ which jupyter 
/your/path/to/jupyter
$ sudo /your/path/to/jupyter lab 
```

That's it.

For more features / examples please refer to _test.ipynb_ (jupyter notebook, also don't forget sudo) and _NIRS.py_.

[1] https://stackoverflow.com/questions/3738173/why-does-pyusb-libusb-require-root-sudo-permissions-on-linux

# FAQ
TBD

# License
This repository uses source codes from DLP NIRscan Nano GUI and DLP Spectrum Library, I didn't find any lisence within those repositories, please email me if there is.
