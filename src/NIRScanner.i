%module NIRScanner
%include "std_string.i"
%include "stdint.i"
%{
#include "NIRScanner.h"
%}

using namespace std;

class NIRScanner {
private:
    EVM mEvm;
    uScanConfig mConfig;
    void *pScanDataBlob;
    void *pRefDataBlob;
    scanResults mScanResults;
    scanResults mReferenceResults;

public:
    NIRScanner(uScanConfig* pConfig = nullptr);
    ~NIRScanner();

    int readVersion();
    void setLampOnOff(int32_t newValue);
    void setConfig(uint16_t scanConfigIndex, uint8_t scan_type, uint16_t num_patterns, uint16_t num_repeats, 
                   uint16_t wavelength_start_nm, uint16_t wavelength_end_nm, uint8_t width_px); 
    void configEVM(uScanConfig* pConfig = nullptr);
    void scan(bool saveDataFlag = false, int numRepeats=1);
    string getScanData();
    int setHibernate(bool newValue);

private:
    int _PerformScanReadData(bool storeInSD, uint16 numRepeats, void *pData, int *pBytesRead);
    int _InterpretData(void *pData);
};

