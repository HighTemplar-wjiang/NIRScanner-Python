# NIRScanner Nano Python wrapper.
# Created by Weiwei Jiang on 2018/06/01
#

import os
import sys
from NIRS import NIRS

def main():
    nirs = NIRS()
    print("Scanning...")
    nirs.scan()
    results = nirs.get_scan_results()
    print(results)

if __name__ == "__main__":
    main()
