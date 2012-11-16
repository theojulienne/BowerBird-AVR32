/**
 * \file
 *
 * \brief USB Device Communication Device Class (Audio) interface definitions.
 *
 * Copyright (c) 2009-2011 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 *    AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _UDI_Audio_H_
#define _UDI_Audio_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_audio.h"
#include "udd.h"
#include "udc_desc.h"
#include "udi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup udi_group
 * \defgroup udi_audio_group UDI for Audio
 *
 * @{
 */

#define AUDIO_CLASS                           0x01
#define AUDIOCONTROL_SUBCLASS                      0x01
#define AUDIOSTREAMING_SUBCLASS                    0x02
#define MIDISTREAMING_SUBCLASS                     0x03

// Standard Audio control (AC) interface descriptor
#define  AC_INTERFACE_NB               2
#define  AC_ALTERNATE                  0
#define  AC_NB_ENDPOINT                0        //! No endpoint for AC interface
#define  AC_INTERFACE_CLASS            AUDIO_CLASS   //! Audio Class
#define  AC_INTERFACE_SUB_CLASS        AUDIOCONTROL_SUBCLASS //! Audio_control sub class
#define  AC_INTERFACE_PROTOCOL         NO_PROTOCOL
#define  AC_INTERFACE_INDEX            0

//** AC interface descriptor Audio specific
#define  NB_OF_STREAMING_INTERFACE     0x02

// Audio Streaming (AS) interface descriptor for micro
#define  STD_AS_IN_INTERFACE_NB              0x03                             // Index of Std AS Interface
//Alternate O Audio Streaming (AS) interface descriptor
#define  ALT0_AS_IN_INTERFACE_INDEX          0x00                             // Index of Std AS interface Alt0
#define  ALT0_AS_IN_NB_ENDPOINT              0x00                             // Nb od endpoints for alt0 interface
#define  ALT0_AS_IN_INTERFACE_CLASS          AUDIO_CLASS                      // Audio class
#define  ALT0_AS_IN_INTERFACE_SUB_CLASS      AUDIOSTREAMING_SUBCLASS          // Audio streamn sub class
#define  ALT0_AS_IN_INTERFACE_PROTOCOL       NO_PROTOCOL                      // Unused
//Alternate 1 Audio Streaming (AS) interface descriptor
#define  ALT1_AS_IN_INTERFACE_INDEX          0x01                             // Index of Std AS interface Alt1
#define  ALT1_AS_IN_NB_ENDPOINT              0x01                             // Nb od endpoints for alt1 interface
#define  ALT1_AS_IN_INTERFACE_CLASS          AUDIO_CLASS                      // Audio class
#define  ALT1_AS_IN_INTERFACE_SUB_CLASS      AUDIOSTREAMING_SUBCLASS          // Audio streamn sub class
#define  ALT1_AS_IN_INTERFACE_PROTOCOL       NO_PROTOCOL                      // Unused
//AS general Interface descriptor
#define  AS_IN_TERMINAL_LINK                 MICRO_OUTPUT_TERMINAL_ID         // Unit Id of the output terminal
#define  AS_IN_DELAY                         0x01                             // Interface delay
#define  AS_IN_FORMAT_TAG                    AUDIO_FORMAT_TI_PCM              // PCM Format
// Format type
#define  IN_FORMAT_TYPE                      AUDIO_FORMAT_TYPE_I              // Format TypeI
#define  IN_FORMAT_NB_CHANNELS               0x04                             // One Channel
#define  IN_FORMAT_FRAME_SIZE                0x02                             // Two bytes per audio sample
#define  IN_FORMAT_BIT_RESOLUTION            0x10                             // 16 bits per sample
#define  IN_FORMAT_SAMPLE_FREQ_NB            0x01                             // One frequency supported
/*#if (BOARD!=EVK1105) || (DEFAULT_DACS!=AUDIO_MIXER_DAC_AIC23B)
# define  IN_FORMAT_LSBYTE_SAMPLE_FREQ        0x1F40                           // 0x001F40=8 kHz Sample frequency
#else
# define  IN_FORMAT_LSBYTE_SAMPLE_FREQ        0xAC44                           // 0x00AC44=44.1 kHz Sample frequency
#endif*/
#define  IN_FORMAT_LSBYTE_SAMPLE_FREQ        (16000)
#define  IN_FORMAT_MSBYTE_SAMPLE_FREQ        0x00                             // MsByte of 0x001F40
 //Audio endpoint specific descriptor field
#define  AUDIO_EP_IN_ATRIBUTES               0x00                             // No sampling freq, no pitch, no pading
#define  AUDIO_EP_IN_DELAY_UNIT              0x00                             // Unused
#define  AUDIO_EP_IN_LOCK_DELAY              0x0000                           // Unused

// Audio Streaming (AS) interface descriptor for speaker
#define  STD_AS_OUT_INTERFACE_NB             0x01                             // Index of Std AS Interface

//* AC interface descriptor for micro
// Input Terminal descriptor
#define  MICRO_INPUT_TERMINAL_ID             0x04
#define  MICRO_INPUT_TERMINAL_TYPE           AUDIO_TE_TYPE_INPUT_MICROPHONE   // Terminal is microphone
#define  MICRO_INPUT_TERMINAL_ASSOCIATION    0x00                             // No association
#define  MICRO_INPUT_TERMINAL_NB_CHANNELS    0x01                             // One channel for input terminal
#define  MICRO_INPUT_TERMINAL_CHANNEL_CONF   AUDIO_CHANNEL_NO_POSITION        // Mono sets no position
#define  MICRO_INPUT_TERMINAL_CH_NAME_ID     0x00                             // No channel name
#define  MICRO_INPUT_TERMINAL_NAME_ID        0x00                             // No terminal name
// Feature Unit Control descriptor
#define  MICRO_FEATURE_UNIT_ID               0x05
#define  MICRO_FEATURE_UNIT_SOURCE_ID        MICRO_INPUT_TERMINAL_ID
#define  MICRO_FEATURE_UNIT_CTRL_CH_MASTER   (AUDIO_FU_CONTROL_CS_MUTE|AUDIO_FU_CONTROL_CS_VOLUME) // Mute + Volume control on master channel
#define  MICRO_FEATURE_UNIT_CTRL_CH_1        0x00                             // No control on channel 1
// Output Terminal descriptor
#define  MICRO_OUTPUT_TERMINAL_ID            0x06
#define  MICRO_OUTPUT_TERMINAL_TYPE          AUDIO_TE_TYPE_USB_STREAMING      // USB Streaming
#define  MICRO_OUTPUT_TERMINAL_ASSOCIATION   0x00                             // No association
#define  MICRO_OUTPUT_TERMINAL_SOURCE_ID     MICRO_FEATURE_UNIT_ID            // From Feature Unit Terminal
#define  MICRO_OUTPUT_TERMINAL_NAME_ID       0x00                             // No terminal name

#define  USB_ENDPOINT_IN                     0x80

#include <usb_protocol.h>
#include <avr32/io.h>
#define AVR32_USBB_UECFGX_EPTYPE_ISOCHRONOUS                AVR32_USBB_UECFG0_EPTYPE_ISOCHRONOUS
#define TYPE_ISOCHRONOUS AVR32_USBB_UECFGX_EPTYPE_ISOCHRONOUS

// USB Endpoint for Audio stream Microphone
#define  ENDPOINT_NB_IN                (EP_AUDIO_IN | USB_ENDPOINT_IN)
#define  EP_ATTRIBUTES_IN              TYPE_ISOCHRONOUS
#define  EP_SIZE_IN                    (128)
#define  EP_INTERVAL_IN                0x01     // One packet per frame

/**
 * \name Interface Descriptor
 *
 * The following structures provide the interface descriptor.
 * It must be implemented in USB configuration descriptor.
 */
//@{

typedef struct __attribute__((__packed__)) {
	uint8_t    bLength;             //!< Size of this descriptor in bytes
	uint8_t    bDescriptorType;     //!< CS interface
	uint8_t    bDescritorSubtype;   //!< HEADER Subtype
	uint16_t   bcdADC;              //!< Revision of class spec
	uint16_t   wTotalLength;        //!< Total size of class specific descriptor
	uint8_t    bInCollection;       //!< Number of streaming interface
	uint8_t    baInterface1;        //!< Interface number of the first AudioStreaming or MIDIStreaming interface in the Collection.
	uint8_t    baInterface2;        //!< Interface number of the second AudioStreaming or MIDIStreaming interface in the Collection.
} usb_audio_ac_iface_desc_t;

typedef struct __attribute__((__packed__)) {
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    bDescriptorSubType;
	uint8_t    bTerminalLink;
	uint8_t    bDelay;
	uint16_t   wFormatTag;
} usb_audio_as_iface_desc_t;

typedef struct __attribute__((__packed__)) {
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    bDescriptorSubType;
	uint8_t    bFormatType;
	uint8_t    bNrChannels;
	uint8_t    bSubFrameSize;
	uint8_t    bBitResolution;
	uint8_t    bSampleFreqType;
	uint16_t   wLsbyteiSamFreq;
	uint8_t    bMsbyteiSamFreq;
} usb_format_type_microphone_t;

typedef struct __attribute__((__packed__)) {
	uint8_t    bLength;             //!< Size of this descriptor in bytes
	uint8_t    bDescriptorType;     //!< ENDPOINT descriptor type
	uint8_t    bEndpointAddress;    //!< Address of the endpoint
	uint8_t    bmAttributes;        //!< Endpoint's attributes
	uint16_t   wMaxPacketSize;      //!< Maximum packet size for this EP
	uint8_t    bInterval;           //!< Interval for polling EP in ms
	uint8_t    bRefresh;
	uint8_t    bSynAddress;
} usb_endpoint_audio_desc_t;

typedef struct __attribute__((__packed__)) {
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    bDescriptorSubType;
	uint8_t    bmAttributes;
	uint8_t    bLockDelayUnits;
	uint16_t   wLockDelay;
} usb_endpoint_audio_specific_t;


typedef struct __attribute__((__packed__)) {
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    bDescriptorSubType;
	uint8_t    bTerminalID;
	uint16_t   wTerminalType;
	uint8_t    bAssocTerminal;
	uint8_t    bNrChannels;
	uint16_t   wChannelConfig;
	uint8_t    iChannelNames;
	uint8_t    iTerminal;
} usb_audio_in_ter_descriptor_t;

typedef struct __attribute__((__packed__)) {
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    bDescriptorSubType;
	uint8_t    bUnitID;
	uint8_t    bSourceID;
	uint8_t    bControSize;
	uint8_t    bmaControlMaster;
	uint8_t    bmaControlCh1;
} usb_audio_feature_unit_descriptor_micro_t;

typedef struct __attribute__((__packed__)) {
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    bDescriptorSubType;
	uint8_t    bTerminalID;
	uint16_t   wTerminalType;
	uint8_t    bAssocTerminal;
	uint8_t    bSourceID;
	uint8_t    iTerminal;
} usb_audio_out_ter_descriptor_t;

/**
 * \brief Control Audio Interface
 *
 * The AudioControl (AC) interface descriptors contain all relevant 
 * information to fully characterize the corresponding audio function.
 */
typedef struct __attribute__((__packed__)) {
	//** The first interface is the Control Audio Interface
	
	//! The AudioControl (AC) interface descriptors contain all relevant information to fully characterize the corresponding audio function.
	usb_iface_desc_t ac_iface;
	//! Class-Specific AC Interface Descriptor
	usb_audio_ac_iface_desc_t ac_class_iface;
	
	//usb_audio_in_ter_descriptor_t
	usb_audio_in_ter_descriptor_t in_ter_desc;
	//usb_audio_feature_unit_descriptor_micro_t
	usb_audio_feature_unit_descriptor_micro_t unit_desc_micro;
	//usb_audio_out_ter_descriptor_t
	usb_audio_out_ter_descriptor_t out_ter_desc;
	
	//! Standard AS Interface Descriptor
	usb_iface_desc_t as_iface_0;
	usb_iface_desc_t as_iface_1;
	//! Class-Specific AS Interface Descriptor
	usb_audio_as_iface_desc_t as_class_iface;
	//! USB Format Type Microphone
	usb_format_type_microphone_t microphone_format;
	//! Endpoint Audio Descriptor
	usb_endpoint_audio_desc_t endpoint;
	//! Endpoint Audio Specific
	usb_endpoint_audio_specific_t endpoint_specific;
} udi_audio_desc_t;



/**
 * \brief Data Class interface descriptor
 *
 * Interface descriptor with associated endpoint descriptors for the
 * Audio Data Class interface.
 */
/*typedef struct __attribute__((__packed__)) {
	//! Standard interface descriptor
	usb_iface_desc_t iface;
	//! Data IN/OUT endpoint descriptors
	usb_ep_desc_t ep_in;
	usb_ep_desc_t ep_out;
} udi_audio_data_desc_t;*/


//! Audio communication enpoints size for all speeds
#define UDI_Audio_COMM_EP_SIZE        64
//! Audio data enpoints size for all speeds (no need to use 512B for HS)
#define UDI_Audio_DATA_EPS_SIZE       64

/**
 * \name Content of interface descriptors for single or first com port
 */
//@{
//! By default no string associated to these interfaces
#  define UDI_AUDIO_DESC {\
	.ac_iface.bLength                = sizeof(usb_iface_desc_t),\
	.ac_iface.bDescriptorType        = USB_DT_INTERFACE,\
	.ac_iface.bInterfaceNumber       = AC_INTERFACE_NB,\
	.ac_iface.bAlternateSetting      = AC_ALTERNATE,\
	.ac_iface.bNumEndpoints          = AC_NB_ENDPOINT,\
	.ac_iface.bInterfaceClass        = AC_INTERFACE_CLASS,\
	.ac_iface.bInterfaceSubClass     = AC_INTERFACE_SUB_CLASS,\
	.ac_iface.bInterfaceProtocol     = AC_INTERFACE_PROTOCOL,\
	.ac_iface.iInterface             = AC_INTERFACE_INDEX,\
\
	.ac_class_iface.bLength          = sizeof(usb_audio_ac_iface_desc_t),\
	.ac_class_iface.bDescriptorType  = DESCRIPTOR_AUDIO_INTERFACE,\
	.ac_class_iface.bDescritorSubtype= DESCRIPTOR_SUBTYPE_AUDIO_AC_HEADER,\
	.ac_class_iface.bcdADC           = LE16(AUDIO_CLASS_REVISION),\
	.ac_class_iface.wTotalLength     = LE16(sizeof(usb_audio_ac_iface_desc_t)\
	                                      + sizeof(usb_audio_in_ter_descriptor_t)\
	                                      + sizeof(usb_audio_feature_unit_descriptor_micro_t)\
	                                      + sizeof(usb_audio_out_ter_descriptor_t)),\
	.ac_class_iface.bInCollection    = NB_OF_STREAMING_INTERFACE,\
	.ac_class_iface.baInterface1     = STD_AS_IN_INTERFACE_NB,\
	.ac_class_iface.baInterface2     = STD_AS_OUT_INTERFACE_NB,\
\
	.in_ter_desc.bLength             = sizeof(usb_audio_in_ter_descriptor_t),\
	.in_ter_desc.bDescriptorType     = DESCRIPTOR_AUDIO_INTERFACE,\
	.in_ter_desc.bDescriptorSubType  = DESCRIPTOR_SUBTYPE_AUDIO_AC_INPUT_TERMINAL,\
	.in_ter_desc.bTerminalID         = MICRO_INPUT_TERMINAL_ID,\
	.in_ter_desc.wTerminalType       = LE16(MICRO_INPUT_TERMINAL_TYPE),\
	.in_ter_desc.bAssocTerminal      = MICRO_INPUT_TERMINAL_ASSOCIATION,\
	.in_ter_desc.bNrChannels         = MICRO_INPUT_TERMINAL_NB_CHANNELS,\
	.in_ter_desc.wChannelConfig      = MICRO_INPUT_TERMINAL_CHANNEL_CONF,\
	.in_ter_desc.iChannelNames       = MICRO_INPUT_TERMINAL_CH_NAME_ID,\
	.in_ter_desc.iTerminal           = MICRO_INPUT_TERMINAL_NAME_ID,\
\
	.unit_desc_micro.bLength         = sizeof(usb_audio_feature_unit_descriptor_micro_t),\
	.unit_desc_micro.bDescriptorType = DESCRIPTOR_AUDIO_INTERFACE,\
	.unit_desc_micro.bDescriptorSubType= DESCRIPTOR_SUBTYPE_AUDIO_AC_FEATURE_UNIT,\
	.unit_desc_micro.bUnitID         = MICRO_FEATURE_UNIT_ID,\
	.unit_desc_micro.bSourceID       = MICRO_FEATURE_UNIT_SOURCE_ID,\
	.unit_desc_micro.bControSize     = 0x01,\
	.unit_desc_micro.bmaControlMaster= MICRO_FEATURE_UNIT_CTRL_CH_MASTER,\
	.unit_desc_micro.bmaControlCh1   = MICRO_FEATURE_UNIT_CTRL_CH_1,\
\
	.out_ter_desc.bLength            = sizeof(usb_audio_out_ter_descriptor_t),\
	.out_ter_desc.bDescriptorType    = DESCRIPTOR_AUDIO_INTERFACE,\
	.out_ter_desc.bDescriptorSubType = DESCRIPTOR_SUBTYPE_AUDIO_AC_OUTPUT_TERMINAL,\
	.out_ter_desc.bTerminalID        = MICRO_OUTPUT_TERMINAL_ID,\
	.out_ter_desc.wTerminalType      = LE16(MICRO_OUTPUT_TERMINAL_TYPE),\
	.out_ter_desc.bAssocTerminal     = MICRO_OUTPUT_TERMINAL_ASSOCIATION,\
	.out_ter_desc.bSourceID          = MICRO_OUTPUT_TERMINAL_SOURCE_ID,\
	.out_ter_desc.iTerminal          = MICRO_OUTPUT_TERMINAL_NAME_ID,\
\
	.as_iface_0.bLength                = sizeof(usb_iface_desc_t),\
	.as_iface_0.bDescriptorType        = USB_DT_INTERFACE,\
	.as_iface_0.bInterfaceNumber       = STD_AS_IN_INTERFACE_NB,\
	.as_iface_0.bAlternateSetting      = ALT0_AS_IN_INTERFACE_INDEX,\
	.as_iface_0.bNumEndpoints          = ALT0_AS_IN_NB_ENDPOINT,\
	.as_iface_0.bInterfaceClass        = ALT0_AS_IN_INTERFACE_CLASS,\
	.as_iface_0.bInterfaceSubClass     = ALT0_AS_IN_INTERFACE_SUB_CLASS,\
	.as_iface_0.bInterfaceProtocol     = ALT0_AS_IN_INTERFACE_PROTOCOL,\
	.as_iface_0.iInterface             = 0x00,\
\
	.as_iface_1.bLength                = sizeof(usb_iface_desc_t),\
	.as_iface_1.bDescriptorType        = USB_DT_INTERFACE,\
	.as_iface_1.bInterfaceNumber       = STD_AS_IN_INTERFACE_NB,\
	.as_iface_1.bAlternateSetting      = ALT1_AS_IN_INTERFACE_INDEX,\
	.as_iface_1.bNumEndpoints          = ALT1_AS_IN_NB_ENDPOINT,\
	.as_iface_1.bInterfaceClass        = ALT1_AS_IN_INTERFACE_CLASS,\
	.as_iface_1.bInterfaceSubClass     = ALT1_AS_IN_INTERFACE_SUB_CLASS,\
	.as_iface_1.bInterfaceProtocol     = ALT1_AS_IN_INTERFACE_PROTOCOL,\
	.as_iface_1.iInterface             = 0x00,\
\
	.as_class_iface.bLength          = sizeof(usb_audio_as_iface_desc_t),\
	.as_class_iface.bDescriptorType  = DESCRIPTOR_AUDIO_INTERFACE,\
	.as_class_iface.bDescriptorSubType= DESCRIPTOR_SUBTYPE_AUDIO_AS_GENERAL,\
	.as_class_iface.bTerminalLink    = AS_IN_TERMINAL_LINK,\
	.as_class_iface.bDelay           = AS_IN_DELAY,\
	.as_class_iface.wFormatTag       = LE16(AS_IN_FORMAT_TAG),\
\
	.microphone_format.bLength       = sizeof(usb_format_type_microphone_t),\
	.microphone_format.bDescriptorType= DESCRIPTOR_AUDIO_INTERFACE,\
	.microphone_format.bDescriptorSubType= DESCRIPTOR_SUBTYPE_AUDIO_AS_FORMAT_TYPE,\
	.microphone_format.bFormatType   = IN_FORMAT_TYPE,\
	.microphone_format.bNrChannels   = IN_FORMAT_NB_CHANNELS,\
	.microphone_format.bSubFrameSize = IN_FORMAT_FRAME_SIZE,\
	.microphone_format.bBitResolution= IN_FORMAT_BIT_RESOLUTION,\
	.microphone_format.bSampleFreqType= IN_FORMAT_SAMPLE_FREQ_NB,\
	.microphone_format.wLsbyteiSamFreq= LE16(IN_FORMAT_LSBYTE_SAMPLE_FREQ),\
	.microphone_format.bMsbyteiSamFreq= IN_FORMAT_MSBYTE_SAMPLE_FREQ,\
\
	.endpoint.bLength                = sizeof(usb_endpoint_audio_desc_t),\
	.endpoint.bDescriptorType        = USB_DT_ENDPOINT,\
	.endpoint.bEndpointAddress       = ENDPOINT_NB_IN,\
	.endpoint.bmAttributes           = EP_ATTRIBUTES_IN,\
	.endpoint.wMaxPacketSize         = LE16(EP_SIZE_IN),\
	.endpoint.bInterval              = EP_INTERVAL_IN,\
	.endpoint.bRefresh               = 0x00,\
	.endpoint.bSynAddress            = 0x00,\
\
	.endpoint_specific.bLength       = sizeof(usb_endpoint_audio_specific_t),\
	.endpoint_specific.bDescriptorType = DESCRIPTOR_AUDIO_ENDPOINT,\
	.endpoint_specific.bDescriptorSubType = DESCRIPTOR_SUBTYPE_AUDIO_ENDP_GENERAL,\
	.endpoint_specific.bmAttributes  = AUDIO_EP_IN_ATRIBUTES,\
	.endpoint_specific.bLockDelayUnits = AUDIO_EP_IN_DELAY_UNIT,\
	.endpoint_specific.wLockDelay    = LE16(AUDIO_EP_IN_LOCK_DELAY),\
}
/*
   .header.bFunctionLength       = sizeof(usb_audio_hdr_desc_t),\
   .header.bDescriptorType       = Audio_CS_INTERFACE,\
   .header.bDescriptorSubtype    = Audio_SCS_HEADER,\
   .header.bcdAudio                = LE16(0x0110),\
   .call_mgmt.bFunctionLength    = sizeof(usb_audio_call_mgmt_desc_t),\
   .call_mgmt.bDescriptorType    = Audio_CS_INTERFACE,\
   .call_mgmt.bDescriptorSubtype = Audio_SCS_CALL_MGMT,\
   .call_mgmt.bmCapabilities     = \
  			Audio_CALL_MGMT_SUPPORTED | Audio_CALL_MGMT_OVER_DCI,\
   .acm.bFunctionLength          = sizeof(usb_audio_acm_desc_t),\
   .acm.bDescriptorType          = Audio_CS_INTERFACE,\
   .acm.bDescriptorSubtype       = Audio_SCS_ACM,\
   .acm.bmCapabilities           = Audio_ACM_SUPPORT_LINE_REQUESTS,\
   .union_desc.bFunctionLength   = sizeof(usb_audio_union_desc_t),\
   .union_desc.bDescriptorType   = Audio_CS_INTERFACE,\
   .union_desc.bDescriptorSubtype= Audio_SCS_UNION,\
   .ep_notify.bLength            = sizeof(usb_ep_desc_t),\
   .ep_notify.bDescriptorType    = USB_DT_ENDPOINT,\
   .ep_notify.bmAttributes       = USB_EP_TYPE_INTERRUPT,\
   .ep_notify.wMaxPacketSize     = LE16(UDI_Audio_COMM_EP_SIZE),\
   .ep_notify.bInterval          = 0xFF,\
   .ep_notify.bEndpointAddress   = UDI_CDC_COMM_EP,\
   .call_mgmt.bDataInterface     = UDI_CDC_DATA_IFACE_NUMBER,\
   .union_desc.bMasterInterface  = UDI_CDC_COMM_IFACE_NUMBER,\
   .union_desc.bSlaveInterface0  = UDI_CDC_DATA_IFACE_NUMBER,\
   .iface.iInterface             = UDI_CDC_COMM_STRING_ID,\
   }*/
//@}


#define UDI_AUDIO_IAD_DESC {\
	.bLength                      = sizeof(usb_iad_desc_t),\
	.bDescriptorType              = USB_DT_IAD,\
	.bInterfaceCount              = 2,\
	.bFunctionClass               = 0x00,\
	.bFunctionSubClass            = 0x00,\
	.bFunctionProtocol            = 0x00,\
	.bFirstInterface              = AC_INTERFACE_NB, \
	.iFunction                    = 0,\
}

//! Global struture which contains standard UDI API for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_audio_control;
extern UDC_DESC_STORAGE udi_api_t udi_api_audio_streaming;



//void udi_audio_ctrl_signal_dcd(bool b_set);
volatile bool udi_audio_is_active( void );
void udi_audio_send_samples( void *buffer, int bufferSize );

//@}

#ifdef __cplusplus
}
#endif
#endif // _UDI_Audio_H_
