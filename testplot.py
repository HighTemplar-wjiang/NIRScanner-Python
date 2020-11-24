# NIRScanner Nano Python wrapper.
# Created by Jintao Yang on 2020/11/23
#!/usr/bin/python3.8

import os
import sys
import math
import matplotlib.pyplot as plt
import numpy as np
from NIRS import NIRS
from time import sleep

def main():
    nirs = NIRS()
    nirs.display_version()

    # Set config. 
    nirs.set_pga_gain(0)
    nirs.set_config(8, NIRS.TYPES.COLUMN_TYPE, 220, 10, 900, 1700, 15, 1, "12345", "my_cfg8")
    nirs.set_hibernate(False)
    nirs.clear_error_status()
    nirs.set_lamp_on_off(0)
    nirs.set_lamp_on_off(1)
    print("Scanning...")
    sleep(1)
    nirs.scan(6)
    sleep(1)
    nirs.set_lamp_on_off(-1)
    results = nirs.get_scan_results()
    print(results)
    
    my_reference = [1033, 1184, 1308, 1443, 1621, 1886, 2131, 2403, 2684, 3046, 3319, 3564, 3823, 4147, 4411, 4679, 4917, 5272, 5504, 5717, 5924, 6153, 6302, 6398, 6493, 6596, 6654, 6691, 6729, 6758, 6764, 6787, 6782, 6789, 6776, 6749, 6727, 6697, 6685, 6652, 6640, 6606, 6574, 6536, 6504, 6455, 6415, 6371, 6314, 6253, 6224, 6171, 6098, 6041, 5985, 5945, 5862, 5815, 5769, 5716, 5672, 5638, 5605, 5577, 5557, 5553, 5536, 5541, 5545, 5534, 5560, 5569, 5614, 5658, 5694, 5735, 5784, 5850, 5904, 5973, 6017, 6094, 6143, 6197, 6276, 6361, 6426, 6506, 6561, 6629, 6685, 6715, 6761, 6781, 6804, 6830, 6832, 6834, 6836, 6819, 6827, 6800, 6785, 6764, 6751, 6716, 6685, 6658, 6637, 6595, 6550, 6510, 6471, 6407, 6369, 6319, 6271, 6181, 6116, 6024, 5898, 5664, 5460, 5263, 5193, 5156, 5208, 5253, 5304, 5367, 5400, 5429, 5435, 5447, 5417, 5416, 5395, 5359, 5344, 5311, 5287, 5281, 5219, 5208, 5143, 5119, 5063, 5009, 4961, 4903, 4863, 4802, 4758, 4687, 4633, 4578, 4534, 4459, 4386, 4322, 4048, 4188, 4130, 4070, 3995, 3925, 3846, 3779, 3715, 3626, 3560, 3493, 3421, 3332, 3265, 3195, 3130, 3040, 2970, 2909, 2859, 2776, 2699, 2632, 2578, 2498, 2433, 2376, 2310, 2238, 2163, 2115, 2039, 1968, 1898, 1843, 1798, 1723, 1668, 1597, 1566, 1489, 1429, 1381, 1306, 1225, 1141, 1060, 976, 838, 739, 621, 540, 426, 357, 296, 262, 219, 186, 150]
    reformat_intensity = []
    reformat_reference = []
    reformat_wavelength = []
    for i in range(results["valid_length"]):
        if my_reference[i] != 0 and results["intensity"][i] != 0:
            reformat_intensity.append(results["intensity"][i])
            reformat_reference.append(my_reference[i])
            reformat_wavelength.append(results["wavelength"][i])
            
    reflectance = np.array(reformat_intensity) / np.array(reformat_reference)
    absorption = -1 * np.log10(reflectance)
    
    #plt.plot(reformat_wavelength, reformat_intensity, '-r',  label='intensity')     
    #plt.plot(reformat_wavelength, reflectance, '-g',  label='reflectance')
    plt.plot(reformat_wavelength, absorption,  '-b', label='absorption')
           
    plt.xlabel("wavelength")
    plt.ylabel("intensity-ratio")
    plt.title("NIR Scan Spectrum")
    plt.legend(loc='upper right', fontsize=7) # label position
    plt.show()

if __name__ == "__main__":
    main()
