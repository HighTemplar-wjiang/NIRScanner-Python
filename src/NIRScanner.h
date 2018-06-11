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
    void ConfigEVM(uScanConfig* pConfig = nullptr);
    void scan(bool saveDataFlag = false);
    string getScanData();


private:
    int _PerformScanReadData(bool storeInSD, uint16 numRepeats, void *pData, int *pBytesRead);
    int _InterpretData(void *pData);
};

#endif //NIRSCANNER_H
