# NIRScanner Python Wrapper
# Created by Weiwei Jiang
# Fix by Jintao Yang on 2020/11/24
#!/usr/bin/python3.8

import os, sys
sys.path.append(os.path.join(os.path.dirname(__file__), "./"))

import atexit
from _NIRScanner import *

import ctypes


class NIRS:

    class TYPES:
        COLUMN_TYPE = 0
        HADAMARD_TYPE = 1
        SLEW_TYPE = 2

    def __init__(self):
        self.nirs_obj = new_NIRScanner()
        atexit.register(self._cleanup)

    def _cleanup(self):
        print("Cleanning up NIRS instance.")
        delete_NIRScanner(self.nirs_obj)

    def scan_snr(self, scan_type="hadamard"):
        if scan_type == "hadamard":
            hadamard_flag = True
        elif scan_type == "column":
            hadamard_flag = False
        else:
            print("Unknow scan type {}.".format(scan_type))
        results_str = NIRScanner_scanSNR(self.nirs_obj, hadamard_flag)

        # Convert to Python object and return. 
        return eval(results_str)

    def scan(self, num_repeats=1):
        NIRScanner_scan(self.nirs_obj, False, num_repeats)

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

    def set_hibernate(self, new_value: bool):
        return NIRScanner_setHibernate(self.nirs_obj, new_value)

    def set_config(self, scanConfigIndex=8, scan_type=1, num_patterns=228, num_repeats=6, 
                   wavelength_start_nm=900, wavelength_end_nm=1700, width_px=7, exposure_time = 0, ScanConfig_serial_number = "", config_name = "cfg8"):
        return NIRScanner_setConfig(self.nirs_obj, scanConfigIndex, scan_type, num_patterns, num_repeats, 
                                    wavelength_start_nm, wavelength_end_nm, width_px, exposure_time, ScanConfig_serial_number, config_name)
    
    def set_pga_gain(self, new_value):
        return NIRScanner_setPGAGain(self.nirs_obj, new_value)

    def set_lamp_on_off(self, new_value):
        return NIRScanner_setLampOnOff(self.nirs_obj, new_value)
    
    def clear_error_status(self):
        return NIRScanner_resetErrorStatus(self.nirs_obj)

    def sync_device_date_time(self, year = 1970, month = 1, day = 1, wday = 0, hour = 0, min = 0, sec = 0):
        return NIRScanner_syncDeviceDateTime(self.nirs_obj, year, month, day, wday, hour, min, sec)

if __name__ == "__main__":
    import time

    nirs = NIRS()
    nirs.display_version()

    # Set config. 
    nirs.set_config(8, NIRS.TYPES.HADAMARD_TYPE, 228, 6, 900, 1700, 7)

    # Turn on the lamp.
    nirs.set_lamp_on_off(1)
    time.sleep(3)

    # Scan.
    nirs.scan()
    results = nirs.get_scan_results()
    nirs.scan()
    results = nirs.get_scan_results()

    # Turn lamp off.
    nirs.set_lamp_on_off(-1)

    pass
