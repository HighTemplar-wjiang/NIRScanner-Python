//
// Created by akg on 18/06/03.
//

#ifndef NIRSCANNER_H
#define NIRSCANNER_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>
#include "API.h"
#include "usb.h"
#include "dlpspec.h"
#include "evm.h"
#include "NNOStatusDefs.h"

using namespace std;

class NIRScanner {
private:
    EVM mEvm;
    uScanConfig mConfig;
    void *pRefDataBlob;
    scanResults mScanResults;
    scanResults mReferenceResults;

public:
    NIRScanner(uScanConfig* pConfig = nullptr);
    ~NIRScanner();

    int readVersion();
    void setConfig(uint16_t scanConfigIndex, uint8_t scan_type, uint16_t num_patterns, uint16_t num_repeats, 
                   uint16_t wavelength_start_nm, uint16_t wavelength_end_nm, uint8_t width_px); 
    void configEVM(uScanConfig* pConfig = nullptr);
    void scan(bool saveDataFlag = false, int numRepeats = 1);
    string getScanData();
    int setHibernate(bool newValue);


private:
    int _performScanReadData(bool storeInSD, uint16 numRepeats, void *pData, int *pBytesRead);
    int _interpretData(void *pData);
};

#endif //NIRSCANNER_H
