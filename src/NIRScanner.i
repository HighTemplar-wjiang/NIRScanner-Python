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
    void resetErrorStatus();
    void setLampOnOff(int32_t newValue);
    void setConfig(uint16_t scanConfigIndex,  // < Unique ID per spectrometer which is modified when the config is changed. Can be used to determine whether a cached version of the config is valid per spectrometer SN.
                    uint8_t scan_type,  // 0: COLUMN_TYPE, 1: HADAMARD_TYPE, 2: SLEW_TYPE.
                    uint16_t num_patterns, // Number of desired points in the spectrum.
                    uint16_t num_repeats, // Number of times to repeat the scan on the spectromter before averaging the scans together and returning the results. This can be used to increase integration time.
                    uint16_t wavelength_start_nm, // Minimum wavelength to start the scan from, in nm.
                    uint16_t wavelength_end_nm, // Maximum wavelength to end the scan at, in nm.
                    uint8_t width_px ,// Pixel width of the patterns. Increasing this will increase SNR, but reduce resolution.
					uint16_t  exposure_time, //Time for for which each pattern	in this  section will be exposed. Values should be as per EXP_TIME enum above
					const char* ScanConfig_serial_number, //Serial number of the spectrometer 
					const char* config_name//User friendly scan configuration name for display 
                    ); 
    void configEVM(uScanConfig* pConfig = nullptr);
    void setPGAGain(int32_t newValue);

    string scanSNR(bool isHadamard=true);
    void scan(bool saveDataFlag=false, int numRepeats=1);
    string getScanData();
    int setHibernate(bool newValue);

private:
    int _PerformScanReadData(bool storeInSD, uint16 numRepeats, void *pData, int *pBytesRead);
    int _InterpretData(void *pData);
};

