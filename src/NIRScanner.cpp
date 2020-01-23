#include "NIRScanner.h"
#include "dlpspec.h"

using namespace std;


NIRScanner::NIRScanner(uScanConfig *pConfig) {

    // Init config.
    if (pConfig != nullptr) {
        this->mConfig = *pConfig;
    } else {
        this->mConfig.scanCfg.scanConfigIndex = 8;
        this->mConfig.scanCfg.scan_type = HADAMARD_TYPE;
        this->mConfig.scanCfg.num_patterns = 228;
        this->mConfig.scanCfg.num_repeats = 3;
        this->mConfig.scanCfg.wavelength_start_nm = MIN_WAVELENGTH;
        this->mConfig.scanCfg.wavelength_end_nm = MAX_WAVELENGTH;
        this->mConfig.scanCfg.width_px = 7;
    }

    // Open USB.
    USB_Init();
    if (0 != USB_Open()) {
        std::cout << "ERROR: Failed to open USB." << std::endl;
    }

    // Quary PGA gain.
    int pga_val = NNO_GetPGAGain();
    this->mPrevPGAGain = pga_val;
    std::cout << "PGA gain: " << pga_val << std::endl;

    // Fetch reference data.
    if (PASS != mEvm.FetchRefCalData()) {
        std::cout << "EVM FetchRefCalData failed." << std::endl;
    }
    if (PASS != mEvm.FetchRefCalMatrix()) {
        std::cout << "EVM FetchRefCalMatrix failed." << std::endl;
    }

    // Get reference data pointer.
    this->pRefDataBlob = this->mEvm.GetRefCalDataBlob();

    // Apply configuration.
    this->configEVM();

}


NIRScanner::~NIRScanner() {

}


int NIRScanner::readVersion()
/*
 * This function reads and displays the GUI , TIVA FW and DLPC Flash versions on the Information Tab
 * @return  0 = PASS
 *         -1 = FAIL
 *
 */
{
    char versionStr[255];
    unsigned int tiva_sw_ver;
    unsigned int dlpc_sw_ver;
    unsigned int dlpc_fw_ver;
    unsigned int speclib_ver;
    unsigned int eeprom_cfg_ver;
    unsigned int eeprom_cal_ver;
    unsigned int eeprom_refcal_ver;
    unsigned int tiva_sw_ver_major;
    unsigned int tiva_sw_ver_minor;
    unsigned int tiva_sw_ver_build;
    unsigned int speclib_ver_major;
    unsigned int speclib_ver_minor;
    unsigned int speclib_ver_build;
    unsigned int dlpc_fw_ver_major;
    unsigned int dlpc_fw_ver_minor;
    unsigned int dlpc_fw_ver_build;
    int ret_val;


    if (USB_IsConnected()) {
        uint32 device_status;

        if ((ret_val = NNO_ReadDeviceStatus(&device_status)) == PASS) {
            NNO_error_status_struct error_status;

            if (NNO_ReadErrorStatus(&error_status) == PASS) {
                std::cout << "Error Status " << error_status.status << " Press to clear" << std::endl;
            } else {
                std::cout << "Unable to read\n Error Status" << std::endl;
            }
        } else if (ret_val == NNO_CMD_BUSY) //If nano is servicing a higher priority command interface
        {
            /* Make sure NNO_GetVersion also returns NNO_CMD_BUSY because some older Tiva FW do not
             * support NNO_ReadDeviceStatus() command and their response to that command cannot be trusted
             */
            if (NNO_GetVersion(&tiva_sw_ver, &dlpc_sw_ver, &dlpc_fw_ver, &speclib_ver, &eeprom_cal_ver,
                               &eeprom_refcal_ver, &eeprom_cfg_ver) == NNO_CMD_BUSY) {
                //Set Tiva Active Status as false and BT connected as true
                std::cout << "NNO_CMD_BUSY" << std::endl;
            }
        } else {
            //Set Tiva Active Status as false
            std::cout << "Active status false" << std::endl;
        }


        if (NNO_GetVersion(&tiva_sw_ver, &dlpc_sw_ver, &dlpc_fw_ver, &speclib_ver, &eeprom_cal_ver, &eeprom_refcal_ver,
                           &eeprom_cfg_ver) == 0) {
            // Tiva version.
            tiva_sw_ver &= 0xFFFFFF;
            tiva_sw_ver_major = tiva_sw_ver >> 16;
            tiva_sw_ver_minor = (tiva_sw_ver << 16) >> 24;
            tiva_sw_ver_build = (tiva_sw_ver << 24) >> 24;

            sprintf(versionStr, "Software version: %d.%d.%d", tiva_sw_ver_major, tiva_sw_ver_minor, tiva_sw_ver_build);
            std::cout << versionStr << std::endl;

            // DLPC version.
            dlpc_fw_ver_major = dlpc_fw_ver >> 24;
            dlpc_fw_ver_minor = (dlpc_fw_ver << 8) >> 24;
            dlpc_fw_ver_build = (dlpc_fw_ver << 16) >> 16;
            sprintf(versionStr, "Firmware version: %d.%d.%d", dlpc_fw_ver_major, dlpc_fw_ver_minor, dlpc_fw_ver_build);
            std::cout << versionStr << std::endl;

            // EEPROM cal version.
            sprintf(versionStr, "EEPROM version: %d/", eeprom_cal_ver);
            std::cout << versionStr;

            // EEPROM refcal version.
            sprintf(versionStr, "%d/", eeprom_refcal_ver);
            std::cout << versionStr;

            // EEPROM config version.
            sprintf(versionStr, "%d", eeprom_cfg_ver);
            std::cout << versionStr << std::endl;

            // Speclib verion.
            speclib_ver &= 0xFFFFFF;
            speclib_ver_major = speclib_ver >> 16;
            speclib_ver_minor = (speclib_ver << 16) >> 24;
            speclib_ver_build = (speclib_ver << 24) >> 24;

            sprintf(versionStr, "Spectrum lib version: %d.%d.%d", speclib_ver_major, speclib_ver_minor,
                    speclib_ver_build);
            std::cout << versionStr << std::endl;

            return PASS;
        } else {
            return FAIL;    //Read failed
        }
    } else //if USB is not connected
    {
        //Set Tiva Active Status as false
        std::cout << "USB not connected." << std::endl;

    }
    return FAIL; //USB is not connected.
}


void NIRScanner::configEVM(uScanConfig *pConfig) {
/* 
 * Config NRIScan Nano. If pConfig is not provided, using the config stored in the instance..
 * 
 * 
*/
    if (pConfig != nullptr) {
        this->mConfig = *pConfig;
    }

    int numpat = this->mEvm.ApplyScanCfgtoDevice(&this->mConfig);
    if (numpat != mConfig.scanCfg.num_patterns) {
        std::cout << "Apply scan config returned " << numpat << std::endl;
        return;
    }

}

void NIRScanner::setConfig(uint16_t scanConfigIndex,  // < Unique ID per spectrometer which is modified when the config is changed. Can be used to determine whether a cached version of the config is valid per spectrometer SN.
                           uint8_t scan_type,  // 0: COLUMN_TYPE, 1: HADAMARD_TYPE, 2: SLEW_TYPE.
                           uint16_t num_patterns, // Number of desired points in the spectrum.
                           uint16_t num_repeats, // Number of times to repeat the scan on the spectromter before averaging the scans together and returning the results. This can be used to increase integration time.
                           uint16_t wavelength_start_nm, // Minimum wavelength to start the scan from, in nm.
                           uint16_t wavelength_end_nm, // Maximum wavelength to end the scan at, in nm.
                           uint8_t width_px // Pixel width of the patterns. Increasing this will increase SNR, but reduce resolution.
                           ) {
    this->mConfig.scanCfg.scanConfigIndex = scanConfigIndex;
    this->mConfig.scanCfg.scan_type = scan_type;
    this->mConfig.scanCfg.num_patterns = num_patterns;
    this->mConfig.scanCfg.num_repeats = num_repeats;
    this->mConfig.scanCfg.wavelength_start_nm = wavelength_start_nm;
    this->mConfig.scanCfg.wavelength_end_nm = wavelength_end_nm;
    this->mConfig.scanCfg.width_px = width_px;

    this->configEVM();
}

void NIRScanner::setLampOnOff(bool newValue)
/* 
* Set the lamp always on or off. 
*/
{
    if(newValue) {
        // Disable controlling the lamp when scanning.
        NNO_SetScanControlsDLPCOnOff(false);
        // Enable DLPC.
        NNO_DLPCEnable(true, true);
        // Save current PGA gain and set it to 64.
        int currentPGAGain = NNO_GetPGAGain();
        if(currentPGAGain < 0) {
            // Failed.
            this->mPrevPGAGain = 1;
        }
        else {
            this->mPrevPGAGain = currentPGAGain;
        }
        NNO_SetFixedPGAGain(true, 64);
    } 
    else {
        // Enable control when scanning.
        NNO_SetScanControlsDLPCOnOff(true);
        // Disable DLPC.
        NNO_DLPCEnable(false, false);
        // Set PGA Gain to auto.
        NNO_SetFixedPGAGain(false, this->mPrevPGAGain);
    }
}


int NIRScanner::_performScanReadData(bool storeInSD, uint16 numRepeats, void *pData, int *pBytesRead)
/*
 * This function asks the Nano to perform the scan and gets back the ScanData
 * @param storeInSD - I - a boolean to indicate if the current scan is to be stored in SD card
 * @param numRepeats - I - an integer indicating the number of times the scan should repeat in Nano
 * @param pData - O - The scanData is readback from Nano in this variable
 * @param pBytesRead - O - the size of the scnData read
 *
 */
{
    int size;
    int scanTimeOut;
    time_t timeScanEnd;
    time_t timeScanStart;
    time_t lastScanTimeMS;
    unsigned int devStatus;
    string scanTimeText;

    // NNO_SetFixedPGAGain(true,1);   /* Only used for testing Fixed PGA command  */
    NNO_SetScanNumRepeats(numRepeats);
    std::cout << "Scan in progress.Estimated Scan time is approximately "
              << (float) NNO_GetEstimatedScanTime() / 1000.0 << " seconds. ";

    scanTimeOut = NNO_GetEstimatedScanTime() * 3;
    timeScanStart = time(0);

    NNO_PerformScan(storeInSD);
    //Wait for scan completion
    if (NNO_ReadDeviceStatus(&devStatus) == PASS) {
        do {
            if ((devStatus & NNO_STATUS_SCAN_IN_PROGRESS) != NNO_STATUS_SCAN_IN_PROGRESS) {
                break;
            }
            timeScanEnd = time(0);
            if ((timeScanEnd - timeScanStart) >= scanTimeOut) {
                *pBytesRead = 0;
                std::cout << "Scan time out with " << timeScanEnd - timeScanStart << std::endl;
                return FAIL;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

        } while (NNO_ReadDeviceStatus(&devStatus) == PASS);
    } else {
        std::cout << "Reading device status for scan completion failed." << std::endl;
        return FAIL;
    }

    timeScanEnd = time(0);
    lastScanTimeMS = timeScanEnd - timeScanStart;
    std::cout << "Scan time was " << lastScanTimeMS << "ms" << std::endl;

    *pBytesRead = NNO_GetFileSizeToRead(NNO_FILE_SCAN_DATA);

    if ((size = NNO_GetFile((unsigned char *) pData, *pBytesRead)) != *pBytesRead) {
        *pBytesRead = size;
        std::cout << "Scan data read from device failed" << std::endl;
        return FAIL;
    }

    return PASS;
}


int NIRScanner::_interpretData(void *pData)
/**
 * This function takes scan data and reference data as serialzed blobs, interprets them
 * using spectrum library APIs and saves for future use.
 *
 * @param pScanDataBlob - I - pointer to scanData blob; if NULL it will continue to use the last set value
 * @param pRefDataBlob - I - pointer to reference data blob; if NULL it will continue to use the last set value
 */
{
    uScanData *_pRefData;
    DLPSPEC_ERR_CODE ret_val;
    void *pCopyBuff = nullptr;

    if (pData != nullptr) {
        if (dlpspec_scan_interpret(pData, SCAN_DATA_BLOB_SIZE, &this->mScanResults) != PASS)
            return FAIL;
    }

    if (this->pRefDataBlob != nullptr) {
        pCopyBuff = malloc(SCAN_DATA_BLOB_SIZE);

        if (pCopyBuff == nullptr) {
            return (ERR_DLPSPEC_INSUFFICIENT_MEM);
        }

        memcpy(pCopyBuff, this->pRefDataBlob, SCAN_DATA_BLOB_SIZE);

        // Deserialize
        ret_val = dlpspec_scan_read_data(pCopyBuff, SCAN_DATA_BLOB_SIZE);
        if (ret_val < 0) {
            free(pCopyBuff);
            pCopyBuff = nullptr;
            return ret_val;
        }

        _pRefData = (uScanData *) pCopyBuff;
        ret_val = dlpspec_scan_interpReference(this->pRefDataBlob, SCAN_DATA_BLOB_SIZE,
                                               this->mEvm.GetRefCalMatrixBlob(_pRefData->data.serial_number),
                                               REF_CAL_MATRIX_BLOB_SIZE, &this->mScanResults,
                                               &this->mReferenceResults);
        if (PASS != ret_val) {
            return FAIL;
        }
        if (pCopyBuff != nullptr) {
            free(pCopyBuff);
            pCopyBuff = nullptr;
        }
    }
    return PASS;
}


void NIRScanner::scan(bool saveDataFlag, int numRepeats)
/**
 * This is a handler function for pushButton_scan on Scan Tab clicked() event
 * This function does the following tasks
 * Checks for USB connection
 * gets the selected Scan Configuration parameters - estimates the scan time and displays
 * does the scan by calling the corresponding API functions
 * saves the scan results in .csv and .bat files in user settings directory
 * displays the spectrum - plots the scan data on the GUI
 */
{
    void *pData;
    int scanStatus;
    int fileSize;
    string scanTimeText;

    pData = (scanData *) malloc(SCAN_DATA_BLOB_SIZE);
    if (pData == nullptr) {
        std::cout << "ERROR: Out of memory" << std::endl;
        return;
    }

    // Scanning.
    scanStatus = this->_performScanReadData(NNO_DONT_STORE_SCAN_IN_SD, numRepeats, pData, &fileSize);
    if (scanStatus != PASS) {
        std::cout << "ERROR: Scan failed." << std::endl;
    }

    // Display versions.
    std::cout << "Header version: " << ((scanData *) pData)->header_version << std::endl;

    int retVal = this->_interpretData(pData);
    if (retVal != PASS) {
        std::cout << "ERROR: Interpret data failed." << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "Detector temperature: " << this->mScanResults.detector_temp_hundredths << std::endl;

        // Save data.
        if (saveDataFlag) {
            // string fileName;
            char writeBuffer[1024];
            // std::cout << "File name to save: ";
            // std::cin >> fileName;

            ofstream myfile;
            myfile.open("scandata.csv");
            if (myfile) {
                for (int i = 0; i < mConfig.scanCfg.num_patterns; ++i) {
                    sprintf(writeBuffer, "%6.2f,%5d,%.2f\n",
                            this->mScanResults.wavelength[i],
                            this->mScanResults.intensity[i],
                            (double) this->mScanResults.intensity[i] / (double) this->mReferenceResults.intensity[i]);
                    myfile << writeBuffer;
                }
                myfile.close();
            }
        }
    }
    free(pData);
}


string NIRScanner::getScanData()
/**
* Convert scanning results to string dictionary.
* This is for Python API.
*/
{
    auto _arrayToString = [](const void *const pArray, int length, char type) -> string {
        string result;

        switch (type) {
            case 'f': {
                for (int i = 0; i < length; i++) {
                    result += (to_string(((double *) pArray)[i]) + string(","));
                }
                break;
            }
            case 'i': {
                for (int i = 0; i < length; i++) {
                    result += (to_string(((int *) pArray)[i]) + string(","));
                }
                break;
            }
            default: {
                break;
            }
        }

        return result;
    };

    string scanResults;
    scanResults = string("header_version:") + to_string(this->mScanResults.header_version);
    scanResults += string("\nscan_name:") + string(this->mScanResults.scan_name);
    scanResults += string("\nscan_time:")
                   + to_string(this->mScanResults.year + 2000)
                   + to_string(this->mScanResults.month + 1)
                   + to_string(this->mScanResults.day)
                   + to_string(this->mScanResults.hour)
                   + to_string(this->mScanResults.minute)
                   + to_string(this->mScanResults.second);
    scanResults += string("\ntemperature_system:") + to_string(this->mScanResults.system_temp_hundredths);
    scanResults += string("\ntemperature_detector:") + to_string(this->mScanResults.detector_temp_hundredths);
    scanResults += string("\nhumidity:") + to_string(this->mScanResults.humidity_hundredths);
    scanResults += string("\npga:") + to_string(this->mScanResults.pga);
    scanResults += string("\nwavelength:") + _arrayToString(this->mScanResults.wavelength, this->mScanResults.length, 'f');
    scanResults += string("\nintensity:") + _arrayToString(this->mScanResults.intensity, this->mScanResults.length, 'i');
    scanResults += string("\nreference:") + _arrayToString(this->mReferenceResults.intensity, this->mReferenceResults.length, 'i');
    scanResults += string("\nvalid_length:") + to_string(this->mScanResults.length);
//    scanResults["header_version"] = to_string(this->mScanResults.header_version);
//    scanResults["scan_name"] = string(this->mScanResults.scan_name);
//    scanResults["scan_time"] = to_string(this->mScanResults.year+2000)
//                               + to_string(this->mScanResults.month+1)
//                               + to_string(this->mScanResults.day)
//                               + to_string(this->mScanResults.hour)
//                               + to_string(this->mScanResults.minute)
//                               + to_string(this->mScanResults.second);
//    scanResults["temperature_system"] = to_string(this->mScanResults.system_temp_hundredths);
//    scanResults["temperature_detector"] = to_string(this->mScanResults.detector_temp_hundredths);
//    scanResults["humidity"] = to_string(this->mScanResults.humidity_hundredths);
//    scanResults["pga"] = to_string(this->mScanResults.pga);
//    scanResults["wavelength"] = _arrayToString(this->mScanResults.wavelength, this->mScanResults.length, 'f');
//    scanResults["intensity"] = _arrayToString(this->mScanResults.intensity, this->mScanResults.length, 'i');
//    scanResults["valid_length"] = to_string(this->mScanResults.length);

    return scanResults;
}

int NIRScanner::setHibernate(bool newValue)
/**
* Enable hibernate after inactive if True, otherwise disable. 
* This is for Python API.
*/
{
    return NNO_SetHibernate(newValue);
}



