/*
 * twi_hal.c
 *
 * Created: 2020/11/01 18:07:03
 *  Author: DevilBinder
 */ 


/* Code EXAMPLES

Print error - 

for (i = 1; i < 128; ++i){
	err = twi_wire(i,0x00,rtc_config_3,1);
	if(err == TWI_OK){
		memset(char_array,0,sizeof(char_array));
		sprintf(char_array,"%d error %d, i2c addr %02x\n\r",__LINE__,err, i);
		uart_send_string((uint8_t*)char_array);
		//while(1);
	}}
*/

#include "twi_hal0.h"

uint8_t *twi_buff;
uint8_t byteIndex;
static uint8_t twi_mode = 0;
static volatile uint8_t status = 0xF8;
static uint8_t twiReady;


ISR(TWI0_vect)
{
	//cli(); // Disable Global Interrupt
	status=TWSR0 & 0xF8; // Get TWI Status Register, mask the prescaler bits (TWPS1,TWPS0)
	switch(status) {
		case TW_SR_SLA_ACK:	// 0x60: SLA+W received, ACK returned
			break;
		case TW_SR_DATA_ACK:	// 0x80: data received, ACK returned
			twi_buff[byteIndex] = TWDR0;	// Save data to the register address
			byteIndex++;
		break;
			case TW_SR_STOP:	// 0xA0: stop or repeated start condition received while selected
			twiReady = 1;
			byteIndex = 0;
		break;
			case TW_ST_SLA_ACK:	// 0xA8: SLA+R received, ACK returned {ADDRESS REGISTER or first byte in slave mode}
			TWDR0 = twi_buff[byteIndex];
			byteIndex++;
			break;
		case TW_ST_DATA_ACK:	// 0xB8: data transmitted, ACK received
			TWDR0 = twi_buff[byteIndex];
			byteIndex++;
			break;
		case TW_ST_DATA_NACK:	// 0xC0: data transmitted, NACK received
			byteIndex = 0;
			break;
		case TW_ST_LAST_DATA:	// 0xC8: last data byte transmitted, ACK received
			break;
		case TW_BUS_ERROR:	// 0x00: illegal start or stop condition
			break;
		default:
			break;
	}
	if (twi_mode){TWCR0 |= (1<<TWINT);}// Clear TWINT Flag
	//sei(); // Enable Global Interrupt
}
uint8_t twi0DataReceived(){
	uint8_t returnData = twiReady;
	twiReady = 0;
	return returnData;
}

static uint8_t twi0_start(void){
	uint16_t i = 0;
	TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
	
	while(status != TWI_START){
		i++;
		if(i >= TWI_TIMEOUT){
			return TWI_ERROR_START;
		}
	}
	return TWI_OK;
}

static void twi0_stop(void){
	
	TWCR0 = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE);
}

static uint8_t twi0_restart(void){
	uint16_t i = 0;
	TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
	
	while(status != TWI_RSTART){
		i++;
		if(i >= TWI_TIMEOUT){
			return TWI_ERROR_START;
		}
	}
	return TWI_OK;
}

static uint8_t twi0_addr_write_ack(void){
	uint16_t i = 0;
	
	TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
	while(status != TWIT_ADDR_ACK){
		i++;
		if(i >= TWI_TIMEOUT){
			return TWI_ERROR_START;
		}
	}
	return TWI_OK;
}


static uint8_t twi0_data_write_ack(void){
	uint16_t i = 0;
	
	TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
	while(status != TWIT_DATA_ACK){
		i++;
		if(i >= TWI_TIMEOUT){
			return TWI_ERROR_START;
		}
	}
	return TWI_OK;
}


static uint8_t twi0_addr_read_ack(void){
	uint16_t i = 0;
	
	TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
	while(status != TWIR_ADDR_ACK){
		i++;
		if(i >= TWI_TIMEOUT){
			return TWI_ERROR_START;
		}
	}
	return TWI_OK;
}


static uint8_t twi0_data_read_ack(uint8_t ack){
	uint16_t i = 0;
	if(ack != 0){
		TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
		while(status != TWIR_DATA_ACK){
			i++;
			if(i >= TWI_TIMEOUT){
				return TWI_ERROR_START;
			}
		}
	}else{
		
		TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
		while(status != TWIR_DATA_NACK){
			i++;
			if(i >= TWI_TIMEOUT){
				return TWI_ERROR_START;
			}
		}
		
	}
	return TWI_OK;
}

uint8_t twi0_read(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len){
	
	uint16_t i = 0;
	uint8_t err = TWI_OK;
	
	err = twi0_start();
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	TWDR0 = (addr << 1) | 0;
	
	
	err = twi0_addr_write_ack();
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	
	TWDR0 = reg;
	err = twi0_data_write_ack();
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	
	err = twi0_restart();
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	
	TWDR0 = (addr << 1) | 1;

	err = twi0_addr_read_ack();
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	
	for(i = 0; i < (len - 1);i++){
		err = twi0_data_read_ack(1);
		if(err != TWI_OK){
			twi0_stop();
			return err;
		}
		data[i] = TWDR0;
	}
	
	err = twi0_data_read_ack(0);
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	data[i] = TWDR0;
	
	twi0_stop();
	
	return err;
}


uint8_t twi0_write(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len){
	
	uint16_t i = 0;
	uint8_t err = TWI_OK;
	
	err = twi0_start();
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	TWDR0 = (addr << 1) | 0;
	
	
	err = twi0_addr_write_ack();
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	
	TWDR0 = reg;
	err = twi0_data_write_ack();
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	
	for(i = 0; i < len;i++){
		TWDR0 = data[i];
		err = twi0_data_write_ack();
		if(err != TWI_OK){
			twi0_stop();
			return err;
		}
	}
	
	
	twi0_stop();
	
	return err;
}

uint8_t twi0_transfer(uint8_t addr,uint8_t *data,uint16_t len){
	
	uint16_t i = 0;
	uint8_t err = TWI_OK;
	
	err = twi0_start();
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	TWDR0 = (addr << 1) | 0;
	
	
	err = twi0_addr_write_ack();
	if(err != TWI_OK){
		twi0_stop();
		return err;
	}
	
	for(i = 0; i < len;i++){
		TWDR0 = data[i];
		err = twi0_data_write_ack();
		if(err != TWI_OK){
			twi0_stop();
			return err;
		}
	}
	
	
	twi0_stop();
	
	return err;
}




void twi0_init(uint32_t speed){
	twi_mode = 0;
	uint32_t gen_t = 0;
	gen_t = (((F_CPU/speed) - 16) / 2) & 0xFF;
	TWBR0 = gen_t & 0xFF;
	TWCR0 = (1 << TWEN) | (1 << TWIE);
	//PORTC |= (1 << PORTC5 | 1 << PORTC4);
	
}

void twi0_slave_init(uint8_t addr, uint8_t *data, uint32_t speed){
	twi_mode = 1;
	uint32_t gen_t = 0;
	gen_t = (((F_CPU/speed) - 16) / 2) & 0xFF;
	TWBR0 = gen_t & 0xFF;
	//PORTE |= ((1<<PINE0) | (1<<PINE1));
	TWAR0 = ((addr & 0xFE)<<1);
	TWDR0 = 0x00;			// Default Initial Value
	//// Start Slave Listening: Clear TWINT Flag, Enable ACK, Enable TWI, TWI Interrupt Enable
	TWCR0 = 0;
	TWCR0 = (1<<TWINT) | (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
	twi_buff = data;
}

