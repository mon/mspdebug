// A dual-mode FTDI driver with a Windows focus that first attempts to use the
// proprietary D2XX driver, if it has been installed automatically by Windows
// Update. If not Windows or d2xx failed to open a device, fallback to the
// libusb backed libftdi. This allows admin-less, config-less flashing, assuming
// the d2xx driver has been configured to autoload via the FTDI EEPROM.

#ifndef FTDI_DUAL_H
#define FTDI_DUAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

struct ftdi_dual_device;

// anything returning an int is expected to be < 0 on error or >= 0 on success
struct ftdi_dual_vtbl {
    void (*close)(struct ftdi_dual_device *handle);
    const char* (*get_error_string)(struct ftdi_dual_device *handle);

    int (*enable_bitbang)(struct ftdi_dual_device *handle, unsigned char bitmask);
    int (*disable_bitbang)(struct ftdi_dual_device *handle, unsigned char bitmask);
    int (*set_baudrate)(struct ftdi_dual_device *handle, int baudrate);

    int (*write_data)(struct ftdi_dual_device *handle, const uint8_t *buf, size_t len);
    int (*read_pins)(struct ftdi_dual_device *handle, uint8_t *pins);
};

struct ftdi_dual_device {
    struct ftdi_dual_vtbl *vtbl;
    // rest of struct contains the variant's internal state
};

struct ftdi_dual_device* ftdi_dual_open(const uint16_t *vid, const uint16_t *pid);

// avoids having to specify the handle twice

static inline void ftdi_dual_close(struct ftdi_dual_device *handle) {
    handle->vtbl->close(handle);
}
static inline const char* ftdi_dual_get_error_string(struct ftdi_dual_device *handle) {
    return handle->vtbl->get_error_string(handle);
}
static inline int ftdi_dual_enable_bitbang(struct ftdi_dual_device *handle, unsigned char bitmask) {
    return handle->vtbl->enable_bitbang(handle, bitmask);
}
static inline int ftdi_dual_disable_bitbang(struct ftdi_dual_device *handle, unsigned char bitmask) {
    return handle->vtbl->disable_bitbang(handle, bitmask);
}
static inline int ftdi_dual_set_baudrate(struct ftdi_dual_device *handle, int baudrate) {
    return handle->vtbl->set_baudrate(handle, baudrate);
}
static inline int ftdi_dual_write_data(struct ftdi_dual_device *handle, const uint8_t *buf, size_t len) {
    return handle->vtbl->write_data(handle, buf, len);
}
static inline int ftdi_dual_read_pins(struct ftdi_dual_device *handle, uint8_t *pins) {
    return handle->vtbl->read_pins(handle, pins);
}

#endif // FTDI_DUAL_H
