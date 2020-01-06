# NIRScanner Python Wrapper
# Created by Weiwei Jiang
#

import os, sys
sys.path.append(os.path.join(os.path.dirname(__file__), "./"))

import atexit
from _NIRScanner import *


class NIRS:

    def __init__(self):
        self.nirs_obj = new_NIRScanner()
        atexit.register(self._cleanup)

    def _cleanup(self):
        print("Cleanning up NIRS instance.")
        delete_NIRScanner(self.nirs_obj)

    def scan(self):
        NIRScanner_scan(self.nirs_obj)

    def get_scan_results(self):
        results_dict = {}
        results_str = NIRScanner_getScanData(self.nirs_obj)
        results_str = results_str.split("\n")

        # Deserialization.
        for item in results_str:
            keyvalue = item.split(":")
            if len(keyvalue) == 2:
                results_dict[keyvalue[0]] = keyvalue[1]

        # Type conversion.
        if "valid_length" in results_dict:
            length = int(results_dict["valid_length"])
            results_dict["valid_length"] = length

            # Convert numerical results.
            if "wavelength" in results_dict:
                results_dict["wavelength"] = [float(item) for item in results_dict["wavelength"].split(",")[:length]]
            if "intensity" in results_dict:
                results_dict["intensity"] = [int(item) for item in results_dict["intensity"].split(",")[:length]]
            if "reference" in results_dict:
                results_dict["reference"] = [int(item) for item in results_dict["reference"].split(",")[:length]]

        if "temperature_system" in results_dict:
            results_dict["temperature_system"] = int(results_dict["temperature_system"]) / 100.0
        if "temperature_detector" in results_dict:
            results_dict["temperature_detector"] = int(results_dict["temperature_detector"]) / 100.0
        if "humidity" in results_dict:
            results_dict["humidity"] = int(results_dict["humidity"]) / 100.0
        if "pga" in results_dict:
            results_dict["pga"] = int(results_dict["pga"])

        return results_dict

    def display_version(self):
        return NIRScanner_readVersion(self.nirs_obj)


if __name__ == "__main__":
    nirs = NIRS()
    nirs.display_version()
    nirs.scan()
    results = nirs.get_scan_results()
    pass
