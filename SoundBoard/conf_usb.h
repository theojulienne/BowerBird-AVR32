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
#define  USB_DEVICE_PRODUCT_NAME          "BowerBird Sound Board"

// we don't have the caps mounted for hispeed usb
// #define  USB_DEVICE_HS_SUPPORT

#define  USB_DEVICE_ATTR                  \
	(USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)


// USB Device Callbacks definitions
#define  UDC_VBUS_EVENT(b_vbus_high)		main_vbus_action(b_vbus_high) //stdio_usb_vbus_event(b_vbus_high)
#define  UDC_SOF_EVENT()					main_sof_action()
#define  UDC_SUSPEND_EVENT()				main_suspend_action()
#define  UDC_RESUME_EVENT()					main_resume_action()


void main_vbus_action(bool b_high);
void main_suspend_action(void);
void main_resume_action(void);
void main_sof_action(void);

/**
 * USB Device low level configuration
 * When only one interface is used, these configurations are defined by the class module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Two interfaces for this device (CDC COM + CDC DATA + Audio)
#define  USB_DEVICE_NB_INTERFACE       4

//! 4 endpoints used by CDC and Audio interfaces
// (1 | USB_EP_DIR_IN)  // CDC Notify endpoint
// (2 | USB_EP_DIR_IN)  // CDC TX
// (3 | USB_EP_DIR_OUT) // CDC RX
// (4 | USB_EP_DIR_IN)  // Audio IN
#define  USB_DEVICE_MAX_EP             4//5
//@}



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

/**
 * USB CDC low level configuration
 * In standalone these configurations are defined by the CDC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_CDC_COMM_EP               (1 | USB_EP_DIR_IN)	// Notify endpoint
#define  UDI_CDC_DATA_EP_IN            (2 | USB_EP_DIR_IN)	// TX
#define  UDI_CDC_DATA_EP_OUT           (3 | USB_EP_DIR_OUT)	// RX

//! Interface numbers
#define  UDI_CDC_COMM_IFACE_NUMBER     0
#define  UDI_CDC_DATA_IFACE_NUMBER     1
//@}
//@}

#define EP_AUDIO_IN           4
#include "udi_audio.h"

/**
 * Description of Composite Device
 * @{
 */
//! USB Interfaces descriptor structure
#define UDI_COMPOSITE_DESC_T \
	usb_iad_desc_t       udi_cdc_iad; \
	udi_cdc_comm_desc_t  udi_cdc_comm; \
	udi_cdc_data_desc_t  udi_cdc_data; \
	usb_iad_desc_t       udi_audio_iad; \
	udi_audio_desc_t     udi_audio
	/*; \
	udi_msc_desc_t       udi_msc*/

//! USB Interfaces descriptor value for Full Speed
#define UDI_COMPOSITE_DESC_FS \
	.udi_cdc_iad   = UDI_CDC_IAD_DESC, \
	.udi_cdc_comm  = UDI_CDC_COMM_DESC, \
	.udi_cdc_data  = UDI_CDC_DATA_DESC, \
	.udi_audio_iad = UDI_AUDIO_IAD_DESC, \
	.udi_audio     = UDI_AUDIO_DESC
	/*, \
	.udi_msc       = UDI_MSC_DESC_FS*/

//! USB Interfaces descriptor value for High Speed
#define UDI_COMPOSITE_DESC_HS \
	.udi_cdc_iad   = UDI_CDC_IAD_DESC,  \
	.udi_cdc_comm  = UDI_CDC_COMM_DESC, \
	.udi_cdc_data  = UDI_CDC_DATA_DESC, \
	.udi_audio_iad = UDI_AUDIO_IAD_DESC, \
	.udi_audio     = UDI_AUDIO_DESC
	/*, \
	.udi_msc       = UDI_MSC_DESC_HS*/

//! USB Interface APIs
#define UDI_COMPOSITE_API \
	&udi_api_cdc_comm, \
	&udi_api_cdc_data, \
	&udi_api_audio_control, \
	&udi_api_audio_streaming
	/*, \
	&udi_api_msc*/
//@}



//! The includes of classes and other headers must be done at the end of this file to avoid compile error
//#include <udi_cdc_conf.h> // since we're using a composite interface, these are defined above
#include <stdio_usb.h>

#endif // _CONF_USB_H_
