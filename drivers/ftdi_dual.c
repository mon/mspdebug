#include "ftdi_dual.h"
#include "output.h"
#include "util.h"

#include <ftdi.h>

#define DEFAULT_VID 0x0403
static const uint16_t default_pids[] = {
	0x6001, // FT232RL
	0x6010, // FT2232HL
	0x6011, // FT4232HL
	0x6014, // FT232HL
};

struct libftdi_device {
    struct ftdi_dual_vtbl *vtbl;
    struct ftdi_context *context;
};

static void libftdi_close(struct ftdi_dual_device *_handle) {
    struct libftdi_device *handle = (struct libftdi_device*)_handle;

    ftdi_usb_close(handle->context);
    ftdi_free(handle->context);
    free(handle);
}

static const char* libftdi_get_error_string(struct ftdi_dual_device *_handle) {
    struct libftdi_device *handle = (struct libftdi_device*)_handle;

    return ftdi_get_error_string(handle->context);
}

static int libftdi_enable_bitbang(struct ftdi_dual_device *_handle, unsigned char bitmask) {
    struct libftdi_device *handle = (struct libftdi_device*)_handle;

    return ftdi_set_bitmode(handle->context, bitmask, BITMODE_BITBANG);
}

static int libftdi_disable_bitbang(struct ftdi_dual_device *_handle, unsigned char bitmask) {
    struct libftdi_device *handle = (struct libftdi_device*)_handle;

    return ftdi_set_bitmode(handle->context, bitmask, BITMODE_RESET);
}

static int libftdi_set_baudrate(struct ftdi_dual_device *_handle, int baudrate) {
    struct libftdi_device *handle = (struct libftdi_device*)_handle;

    return ftdi_set_baudrate(handle->context, baudrate);
}

static int libftdi_write_data(struct ftdi_dual_device *_handle, const uint8_t *buf, size_t len) {
    struct libftdi_device *handle = (struct libftdi_device*)_handle;

    return ftdi_write_data(handle->context, buf, len);
}

static int libftdi_read_pins(struct ftdi_dual_device *_handle, uint8_t *pins) {
    struct libftdi_device *handle = (struct libftdi_device*)_handle;

    return ftdi_read_pins(handle->context, pins);
}

static struct ftdi_dual_vtbl libftdi_vtbl = {
    .close = libftdi_close,
    .get_error_string = libftdi_get_error_string,
    .enable_bitbang = libftdi_enable_bitbang,
    .disable_bitbang = libftdi_disable_bitbang,
    .set_baudrate = libftdi_set_baudrate,
    .write_data = libftdi_write_data,
    .read_pins = libftdi_read_pins,
};

#ifdef _WIN32

#include <Windows.h>
#include "ftd2xx.h"

struct d2xx_device {
    struct ftdi_dual_vtbl *vtbl;
    FT_HANDLE ftHandle;
    FT_STATUS last_status;
};

// the actual exports from the DLL
static struct {
    bool load_ok;

    FT_CreateDeviceInfoList_t FT_CreateDeviceInfoList;
    FT_Open_t                 FT_Open;
    FT_Close_t                FT_Close;
    FT_GetDeviceInfo_t        FT_GetDeviceInfo;
    FT_Read_t                 FT_Read;
    FT_Write_t                FT_Write;
    FT_SetBaudRate_t          FT_SetBaudRate;
    FT_SetBitMode_t           FT_SetBitMode;
    FT_GetBitMode_t           FT_GetBitMode;
} d2xx;

/// NOTE: all FTDI funcs return 0 for OK, or a positive integer for errors. To
/// conveniently match libftdi's behaviour of "negative is fail", we negate all
/// return values.

static void d2xx_close(struct ftdi_dual_device *_handle) {
    struct d2xx_device *handle = (struct d2xx_device*)_handle;

    d2xx.FT_Close(handle->ftHandle);
    free(handle);
}

static const char* d2xx_get_error_string(struct ftdi_dual_device *_handle) {
    struct d2xx_device *handle = (struct d2xx_device*)_handle;

    switch(handle->last_status) {
        case FT_OK:
            return "FT_OK";
        case FT_INVALID_HANDLE:
            return "FT_INVALID_HANDLE";
        case FT_DEVICE_NOT_FOUND:
            return "FT_DEVICE_NOT_FOUND";
        case FT_DEVICE_NOT_OPENED:
            return "FT_DEVICE_NOT_OPENED";
        case FT_IO_ERROR:
            return "FT_IO_ERROR";
        case FT_INSUFFICIENT_RESOURCES:
            return "FT_INSUFFICIENT_RESOURCES";
        case FT_INVALID_PARAMETER:
            return "FT_INVALID_PARAMETER";
        case FT_INVALID_BAUD_RATE:
            return "FT_INVALID_BAUD_RATE";
        case FT_DEVICE_NOT_OPENED_FOR_ERASE:
            return "FT_DEVICE_NOT_OPENED_FOR_ERASE";
        case FT_DEVICE_NOT_OPENED_FOR_WRITE:
            return "FT_DEVICE_NOT_OPENED_FOR_WRITE";
        case FT_FAILED_TO_WRITE_DEVICE:
            return "FT_FAILED_TO_WRITE_DEVICE";
        case FT_EEPROM_READ_FAILED:
            return "FT_EEPROM_READ_FAILED";
        case FT_EEPROM_WRITE_FAILED:
            return "FT_EEPROM_WRITE_FAILED";
        case FT_EEPROM_ERASE_FAILED:
            return "FT_EEPROM_ERASE_FAILED";
        case FT_EEPROM_NOT_PRESENT:
            return "FT_EEPROM_NOT_PRESENT";
        case FT_EEPROM_NOT_PROGRAMMED:
            return "FT_EEPROM_NOT_PROGRAMMED";
        case FT_INVALID_ARGS:
            return "FT_INVALID_ARGS";
        case FT_NOT_SUPPORTED:
            return "FT_NOT_SUPPORTED";
        case FT_OTHER_ERROR:
            return "FT_OTHER_ERROR";
        case FT_DEVICE_LIST_NOT_READY:
            return "FT_DEVICE_LIST_NOT_READY";
        default:
            return "Unknown error";
    }
}

static int d2xx_enable_bitbang(struct ftdi_dual_device *_handle, unsigned char bitmask) {
    struct d2xx_device *handle = (struct d2xx_device*)_handle;

    handle->last_status = d2xx.FT_SetBitMode(handle->ftHandle, bitmask, FT_BITMODE_ASYNC_BITBANG);
    return -handle->last_status;
}

static int d2xx_disable_bitbang(struct ftdi_dual_device *_handle, unsigned char bitmask) {
    struct d2xx_device *handle = (struct d2xx_device*)_handle;

    handle->last_status = d2xx.FT_SetBitMode(handle->ftHandle, bitmask, FT_BITMODE_RESET);
    return -handle->last_status;
}

static int d2xx_set_baudrate(struct ftdi_dual_device *_handle, int baudrate) {
    struct d2xx_device *handle = (struct d2xx_device*)_handle;

    handle->last_status = d2xx.FT_SetBaudRate(handle->ftHandle, baudrate);
    return -handle->last_status;
}

static int d2xx_write_data(struct ftdi_dual_device *_handle, const uint8_t *buf, size_t len) {
    struct d2xx_device *handle = (struct d2xx_device*)_handle;

    DWORD bytes_written;
    handle->last_status = d2xx.FT_Write(handle->ftHandle, (LPVOID)buf, len, &bytes_written);
    return FT_SUCCESS(handle->last_status) ? bytes_written : -handle->last_status;
}

static int d2xx_read_pins(struct ftdi_dual_device *_handle, uint8_t *pins) {
    struct d2xx_device *handle = (struct d2xx_device*)_handle;

    handle->last_status = d2xx.FT_GetBitMode(handle->ftHandle, pins);
    return -handle->last_status;
}

static struct ftdi_dual_vtbl d2xx_vtbl = {
    .close = d2xx_close,
    .get_error_string = d2xx_get_error_string,
    .enable_bitbang = d2xx_enable_bitbang,
    .disable_bitbang = d2xx_disable_bitbang,
    .set_baudrate = d2xx_set_baudrate,
    .write_data = d2xx_write_data,
    .read_pins = d2xx_read_pins,
};

#define LOAD_FN(name) if(!(d2xx.name = (void*)GetProcAddress(dll, #name))) return false;

bool load_d2xx(void) {
    // already loaded?
    if(d2xx.load_ok) {
        return true;
    }

    HMODULE dll = LoadLibraryA("ftd2xx.dll");

    if(!dll) {
        return false;
    }

    LOAD_FN(FT_CreateDeviceInfoList);
    LOAD_FN(FT_Open);
    LOAD_FN(FT_Close);
    LOAD_FN(FT_GetDeviceInfo);
    LOAD_FN(FT_Read);
    LOAD_FN(FT_Write);
    LOAD_FN(FT_SetBaudRate);
    LOAD_FN(FT_SetBitMode);
    LOAD_FN(FT_GetBitMode);

    d2xx.load_ok = true;
    return true;
}

static struct ftdi_dual_device* try_open_d2xx(const uint16_t *vid, const uint16_t *pid) {
    struct d2xx_device *handle;
    DWORD i;
	uint16_t _vid, _pid;
    FT_HANDLE ftHandle;
    FT_STATUS status = FT_DEVICE_NOT_FOUND;
    DWORD deviceID;
    DWORD dev_count;

    if(!load_d2xx()) {
        printc("Failed to load FTDI d2xx library, using libusb");
        return NULL;
    }

    if(vid == NULL && pid == NULL) {
		status = d2xx.FT_Open(0, &ftHandle);
	} else {
		// pick sane defaults if either provided
		_vid = vid ? *vid : DEFAULT_VID;
		_pid = pid ? *pid : 0x6010;
        if(!FT_SUCCESS(d2xx.FT_CreateDeviceInfoList(&dev_count))) {
            printc("d2xx: FT_CreateDeviceInfoList failed");
            return NULL;
        }

        for(i = 0; i < dev_count; i++) {
            if(!FT_SUCCESS(status = d2xx.FT_Open(i, &ftHandle))) {
                continue;
            }

            if(!FT_SUCCESS(status = d2xx.FT_GetDeviceInfo(
                    ftHandle,
                    NULL,
                    &deviceID,
                    NULL,
                    NULL,
                    NULL
                    ))) {
                printc("d2xx: FT_GetDeviceInfo failed");
                d2xx.FT_Close(ftHandle);
                continue;
            }

            if((deviceID & 0xFFFF) != _pid || ((deviceID >> 16) & 0xFFFF) != _vid) {
                d2xx.FT_Close(ftHandle);
                status = FT_DEVICE_NOT_FOUND;
                continue;
            }

            break;
        }
	}

    if(!FT_SUCCESS(status)) {
        printc("No FTDI D2XX devices found, trying libusb\n");
        return NULL;
    }

    handle = calloc(1, sizeof(*handle));
    handle->vtbl = &d2xx_vtbl;
    handle->ftHandle = ftHandle;

    printc("Opened FTDI D2XX device\n");

    return (struct ftdi_dual_device*)handle;
}

#else // _WIN32

// don't care for linux - trivial to just use libusb there
static struct ftdi_dual_device* try_open_d2xx(const uint16_t *vid, const uint16_t *pid) {
    return NULL;
}

#endif // _WIN32

static struct ftdi_dual_device* try_open_libftdi(const uint16_t *vid, const uint16_t *pid) {
	int f = -1;
	size_t i;
	uint16_t _vid, _pid;
    struct ftdi_context *context;
    struct libftdi_device *handle;
    struct ftdi_dual_device *dual_handle;

	if ((context = ftdi_new()) == NULL) {
		printc_err("ftdi_dual: ftdi_new failed\n");
		return NULL;
	}

    handle = calloc(1, sizeof(*handle));
    handle->vtbl = &libftdi_vtbl;
    handle->context = context;
    dual_handle = (struct ftdi_dual_device*)handle;

	if(vid == NULL && pid == NULL) {
		// iterate through all the default VID/PID pairs for auto-detect
		for(i = 0; i < ARRAY_LEN(default_pids) && f < 0; i++) {
			f = ftdi_usb_open(handle->context, DEFAULT_VID, default_pids[i]);
		}
	} else {
		// pick sane defaults if either provided
		_vid = vid ? *vid : DEFAULT_VID;
		_pid = pid ? *pid : 0x6010;
		f = ftdi_usb_open(handle->context, _vid, _pid);
	}

    if(f < 0) {
		printc_err("ftdi_dual: unable to open libftdi device: %s\n", libftdi_get_error_string(dual_handle));
        libftdi_close(dual_handle);
		return NULL;
	}

    return dual_handle;
}

struct ftdi_dual_device* ftdi_dual_open(const uint16_t *vid, const uint16_t *pid) {
    struct ftdi_dual_device* ret = try_open_d2xx(vid, pid);
    if(ret) {
        return ret;
    }

    return try_open_libftdi(vid, pid);
}
