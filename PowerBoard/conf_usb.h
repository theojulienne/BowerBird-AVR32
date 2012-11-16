#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include <compiler.h>

// usb device definitions
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_AVR_CDC
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL AVR"
#define  USB_DEVICE_PRODUCT_NAME          "CDC"

#define  USB_DEVICE_ATTR                  \
	(USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)


// USB Device Callbacks definitions
#define  UDC_VBUS_EVENT(b_vbus_high)      stdio_usb_vbus_event(b_vbus_high)
#define  UDC_SOF_EVENT()
#define  UDC_SUSPEND_EVENT()
#define  UDC_RESUME_EVENT()

// cdc interface callback definitions
#define  UDI_CDC_ENABLE_EXT()             stdio_usb_enable()
#define  UDI_CDC_DISABLE_EXT()            stdio_usb_disable()
#define  UDI_CDC_RX_NOTIFY()              
#define  UDI_CDC_SET_CODING_EXT(cfg)      
#define  UDI_CDC_SET_DTR_EXT(set)         
#define  UDI_CDC_SET_RTS_EXT(set)

// default options for cdc, not that useful because we're not using a real usart
#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8

//! The includes of classes and other headers must be done at the end of this file to avoid compile error
#include <udi_cdc_conf.h>
#include <stdio_usb.h>

#endif // _CONF_USB_H_
