/*++

Copyright © 2001-2021 Future Technology Devices International Limited

THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

FTDI DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON FTDI PARTS.

FTDI DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.


Module Name:

ftd2xx.h

Abstract:

Native USB device driver for FTDI FT232x, FT245x, FT2232x, FT4232x, FT2233H and FT4233H devices
FTD2XX library definitions

Environment:

kernel & user mode


--*/

/**
 * note: this file has been significantly stripped down to only include what
 * mspdebug cares about, to only compile on windows, and to make the function
 * definitions function pointer types, instead.
 */


#ifndef FTD2XX_H
#define FTD2XX_H

#ifdef _WIN32
// Compiling on Windows
#include <windows.h>

typedef PVOID	FT_HANDLE;
typedef ULONG	FT_STATUS;

//
// Device status
//
enum {
	FT_OK,
	FT_INVALID_HANDLE,
	FT_DEVICE_NOT_FOUND,
	FT_DEVICE_NOT_OPENED,
	FT_IO_ERROR,
	FT_INSUFFICIENT_RESOURCES,
	FT_INVALID_PARAMETER,
	FT_INVALID_BAUD_RATE,

	FT_DEVICE_NOT_OPENED_FOR_ERASE,
	FT_DEVICE_NOT_OPENED_FOR_WRITE,
	FT_FAILED_TO_WRITE_DEVICE,
	FT_EEPROM_READ_FAILED,
	FT_EEPROM_WRITE_FAILED,
	FT_EEPROM_ERASE_FAILED,
	FT_EEPROM_NOT_PRESENT,
	FT_EEPROM_NOT_PROGRAMMED,
	FT_INVALID_ARGS,
	FT_NOT_SUPPORTED,
	FT_OTHER_ERROR,
	FT_DEVICE_LIST_NOT_READY,
};


#define FT_SUCCESS(status) ((status) == FT_OK)

//
// FT_OpenEx Flags
//

#define FT_OPEN_BY_SERIAL_NUMBER	1
#define FT_OPEN_BY_DESCRIPTION		2
#define FT_OPEN_BY_LOCATION			4

#define FT_OPEN_MASK (FT_OPEN_BY_SERIAL_NUMBER | \
                      FT_OPEN_BY_DESCRIPTION | \
                      FT_OPEN_BY_LOCATION)

//
// FT_ListDevices Flags (used in conjunction with FT_OpenEx Flags
//

#define FT_LIST_NUMBER_ONLY			0x80000000
#define FT_LIST_BY_INDEX			0x40000000
#define FT_LIST_ALL					0x20000000

#define FT_LIST_MASK (FT_LIST_NUMBER_ONLY|FT_LIST_BY_INDEX|FT_LIST_ALL)

//
// Baud Rates
//

#define FT_BAUD_300			300
#define FT_BAUD_600			600
#define FT_BAUD_1200		1200
#define FT_BAUD_2400		2400
#define FT_BAUD_4800		4800
#define FT_BAUD_9600		9600
#define FT_BAUD_14400		14400
#define FT_BAUD_19200		19200
#define FT_BAUD_38400		38400
#define FT_BAUD_57600		57600
#define FT_BAUD_115200		115200
#define FT_BAUD_230400		230400
#define FT_BAUD_460800		460800
#define FT_BAUD_921600		921600

//
// Word Lengths
//

#define FT_BITS_8			(UCHAR) 8
#define FT_BITS_7			(UCHAR) 7

//
// Stop Bits
//

#define FT_STOP_BITS_1		(UCHAR) 0
#define FT_STOP_BITS_2		(UCHAR) 2

//
// Parity
//

#define FT_PARITY_NONE		(UCHAR) 0
#define FT_PARITY_ODD		(UCHAR) 1
#define FT_PARITY_EVEN		(UCHAR) 2
#define FT_PARITY_MARK		(UCHAR) 3
#define FT_PARITY_SPACE		(UCHAR) 4

//
// Flow Control
//

#define FT_FLOW_NONE		0x0000
#define FT_FLOW_RTS_CTS		0x0100
#define FT_FLOW_DTR_DSR		0x0200
#define FT_FLOW_XON_XOFF	0x0400

//
// Purge rx and tx buffers
//
#define FT_PURGE_RX			1
#define FT_PURGE_TX			2

//
// Events
//

typedef void(*PFT_EVENT_HANDLER)(DWORD, DWORD);

#define FT_EVENT_RXCHAR			1
#define FT_EVENT_MODEM_STATUS	2
#define FT_EVENT_LINE_STATUS	4

//
// Timeouts
//

#define FT_DEFAULT_RX_TIMEOUT	300
#define FT_DEFAULT_TX_TIMEOUT	300

//
// Device types
//

typedef ULONG	FT_DEVICE;

enum {
	FT_DEVICE_BM,
	FT_DEVICE_AM,
	FT_DEVICE_100AX,
	FT_DEVICE_UNKNOWN,
	FT_DEVICE_2232C,
	FT_DEVICE_232R,
	FT_DEVICE_2232H,
	FT_DEVICE_4232H,
	FT_DEVICE_232H,
	FT_DEVICE_X_SERIES,
	FT_DEVICE_4222H_0,
	FT_DEVICE_4222H_1_2,
	FT_DEVICE_4222H_3,
	FT_DEVICE_4222_PROG,
	FT_DEVICE_900,
	FT_DEVICE_930,
	FT_DEVICE_UMFTPD3A,
	FT_DEVICE_2233HP,
	FT_DEVICE_4233HP,
	FT_DEVICE_2232HP,
	FT_DEVICE_4232HP,
	FT_DEVICE_233HP,
	FT_DEVICE_232HP,
	FT_DEVICE_2232HA,
	FT_DEVICE_4232HA,
	FT_DEVICE_232RN,
};

//
// Bit Modes
//

#define FT_BITMODE_RESET					0x00
#define FT_BITMODE_ASYNC_BITBANG			0x01
#define FT_BITMODE_MPSSE					0x02
#define FT_BITMODE_SYNC_BITBANG				0x04
#define FT_BITMODE_MCU_HOST					0x08
#define FT_BITMODE_FAST_SERIAL				0x10
#define FT_BITMODE_CBUS_BITBANG				0x20
#define FT_BITMODE_SYNC_FIFO				0x40

// Driver types
#define FT_DRIVER_TYPE_D2XX		0
#define FT_DRIVER_TYPE_VCP		1



#ifdef __cplusplus
extern "C" {
#endif

		typedef FT_STATUS (WINAPI *FT_Open_t)(
		int deviceNumber,
		FT_HANDLE *pHandle
		);

		typedef FT_STATUS (WINAPI *FT_OpenEx_t)(
		PVOID pArg1,
		DWORD Flags,
		FT_HANDLE *pHandle
		);

		typedef FT_STATUS (WINAPI *FT_ListDevices_t)(
		PVOID pArg1,
		PVOID pArg2,
		DWORD Flags
		);

		typedef FT_STATUS (WINAPI *FT_Close_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_Read_t)(
		FT_HANDLE ftHandle,
		LPVOID lpBuffer,
		DWORD dwBytesToRead,
		LPDWORD lpBytesReturned
		);

		typedef FT_STATUS (WINAPI *FT_Write_t)(
		FT_HANDLE ftHandle,
		LPVOID lpBuffer,
		DWORD dwBytesToWrite,
		LPDWORD lpBytesWritten
		);

		typedef FT_STATUS (WINAPI *FT_IoCtl_t)(
		FT_HANDLE ftHandle,
		DWORD dwIoControlCode,
		LPVOID lpInBuf,
		DWORD nInBufSize,
		LPVOID lpOutBuf,
		DWORD nOutBufSize,
		LPDWORD lpBytesReturned,
		LPOVERLAPPED lpOverlapped
		);

		typedef FT_STATUS (WINAPI *FT_SetBaudRate_t)(
		FT_HANDLE ftHandle,
		ULONG BaudRate
		);

		typedef FT_STATUS (WINAPI *FT_SetDivisor_t)(
		FT_HANDLE ftHandle,
		USHORT Divisor
		);

		typedef FT_STATUS (WINAPI *FT_SetDataCharacteristics_t)(
		FT_HANDLE ftHandle,
		UCHAR WordLength,
		UCHAR StopBits,
		UCHAR Parity
		);

		typedef FT_STATUS (WINAPI *FT_SetFlowControl_t)(
		FT_HANDLE ftHandle,
		USHORT FlowControl,
		UCHAR XonChar,
		UCHAR XoffChar
		);

		typedef FT_STATUS (WINAPI *FT_ResetDevice_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_SetDtr_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_ClrDtr_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_SetRts_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_ClrRts_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_GetModemStatus_t)(
		FT_HANDLE ftHandle,
		ULONG *pModemStatus
		);

		typedef FT_STATUS (WINAPI *FT_SetChars_t)(
		FT_HANDLE ftHandle,
		UCHAR EventChar,
		UCHAR EventCharEnabled,
		UCHAR ErrorChar,
		UCHAR ErrorCharEnabled
		);

		typedef FT_STATUS (WINAPI *FT_Purge_t)(
		FT_HANDLE ftHandle,
		ULONG Mask
		);

		typedef FT_STATUS (WINAPI *FT_SetTimeouts_t)(
		FT_HANDLE ftHandle,
		ULONG ReadTimeout,
		ULONG WriteTimeout
		);

		typedef FT_STATUS (WINAPI *FT_GetQueueStatus_t)(
		FT_HANDLE ftHandle,
		DWORD *dwRxBytes
		);

		typedef FT_STATUS (WINAPI *FT_SetEventNotification_t)(
		FT_HANDLE ftHandle,
		DWORD Mask,
		PVOID Param
		);

		typedef FT_STATUS (WINAPI *FT_GetStatus_t)(
		FT_HANDLE ftHandle,
		DWORD *dwRxBytes,
		DWORD *dwTxBytes,
		DWORD *dwEventDWord
		);

		typedef FT_STATUS (WINAPI *FT_SetBreakOn_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_SetBreakOff_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_SetWaitMask_t)(
		FT_HANDLE ftHandle,
		DWORD Mask
		);

		typedef FT_STATUS (WINAPI *FT_WaitOnMask_t)(
		FT_HANDLE ftHandle,
		DWORD *Mask
		);

		typedef FT_STATUS (WINAPI *FT_GetEventStatus_t)(
		FT_HANDLE ftHandle,
		DWORD *dwEventDWord
		);

		typedef FT_STATUS (WINAPI *FT_ReadEE_t)(
		FT_HANDLE ftHandle,
		DWORD dwWordOffset,
		LPWORD lpwValue
		);

		typedef FT_STATUS (WINAPI *FT_WriteEE_t)(
		FT_HANDLE ftHandle,
		DWORD dwWordOffset,
		WORD wValue
		);

		typedef FT_STATUS (WINAPI *FT_EraseEE_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_SetBitMode_t)(
		FT_HANDLE ftHandle,
		UCHAR ucMask,
		UCHAR ucEnable
		);

		typedef FT_STATUS (WINAPI *FT_GetBitMode_t)(
		FT_HANDLE ftHandle,
		PUCHAR pucMode
		);

		typedef FT_STATUS (WINAPI *FT_SetUSBParameters_t)(
		FT_HANDLE ftHandle,
		ULONG ulInTransferSize,
		ULONG ulOutTransferSize
		);

		typedef FT_STATUS (WINAPI *FT_SetDeadmanTimeout_t)(
		FT_HANDLE ftHandle,
		ULONG ulDeadmanTimeout
		);

		typedef FT_STATUS (WINAPI *FT_GetDeviceInfo_t)(
		FT_HANDLE ftHandle,
		FT_DEVICE *lpftDevice,
		LPDWORD lpdwID,
		PCHAR SerialNumber,
		PCHAR Description,
		LPVOID Dummy
		);

		typedef FT_STATUS (WINAPI *FT_StopInTask_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_RestartInTask_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_SetResetPipeRetryCount_t)(
		FT_HANDLE ftHandle,
		DWORD dwCount
		);

		typedef FT_STATUS (WINAPI *FT_ResetPort_t)(
		FT_HANDLE ftHandle
		);

		typedef FT_STATUS (WINAPI *FT_CyclePort_t)(
		FT_HANDLE ftHandle
		);

	//
	// Device information
	//

	typedef struct _ft_device_list_info_node {
		ULONG Flags;
		ULONG Type;
		ULONG ID;
		DWORD LocId;
		char SerialNumber[16];
		char Description[64];
		FT_HANDLE ftHandle;
	} FT_DEVICE_LIST_INFO_NODE;

	// Device information flags
	enum {
		FT_FLAGS_OPENED = 1,
		FT_FLAGS_HISPEED = 2
	};


		typedef FT_STATUS (WINAPI *FT_CreateDeviceInfoList_t)(
		LPDWORD lpdwNumDevs
		);

		typedef FT_STATUS (WINAPI *FT_GetDeviceInfoList_t)(
		FT_DEVICE_LIST_INFO_NODE *pDest,
		LPDWORD lpdwNumDevs
		);

		typedef FT_STATUS (WINAPI *FT_GetDeviceInfoDetail_t)(
		DWORD dwIndex,
		LPDWORD lpdwFlags,
		LPDWORD lpdwType,
		LPDWORD lpdwID,
		LPDWORD lpdwLocId,
		LPVOID lpSerialNumber,
		LPVOID lpDescription,
		FT_HANDLE *pftHandle
		);


	//
	// Version information
	//

		typedef FT_STATUS (WINAPI *FT_GetDriverVersion_t)(
		FT_HANDLE ftHandle,
		LPDWORD lpdwVersion
		);

		typedef FT_STATUS (WINAPI *FT_GetLibraryVersion_t)(
		LPDWORD lpdwVersion
		);


		typedef FT_STATUS (WINAPI *FT_Rescan_t)(
		void
		);

		typedef FT_STATUS (WINAPI *FT_Reload_t)(
		WORD wVid,
		WORD wPid
		);

		typedef FT_STATUS (WINAPI *FT_GetComPortNumber_t)(
		FT_HANDLE ftHandle,
		LPLONG	lpdwComPortNumber
		);

#ifdef __cplusplus
}
#endif

#endif // _WIN32

#endif	/* FTD2XX_H */
