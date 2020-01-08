%module NIRScanner
%include "std_string.i"
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
    void ConfigEVM(uScanConfig* pConfig = nullptr);
    void scan(bool saveDataFlag = false);
    string getScanData();
    int setHibernate(bool newValue);

private:
    int _PerformScanReadData(bool storeInSD, uint16 numRepeats, void *pData, int *pBytesRead);
    int _InterpretData(void *pData);
};

