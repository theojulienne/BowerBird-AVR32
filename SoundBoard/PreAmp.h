#ifndef __PREAMP_H__
#define __PREAMP_H__

int PreAmp_Init(void);
int PreAmp_read( uint8_t channel );
int PreAmp_write( uint8_t channel, uint8_t value );

#endif /* __PREAMP_H__ */