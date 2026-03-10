
void Rzhl_I2C_Init();
void Rzhl_I2C_SdaDir(unsigned char dir);
void Rzhl_I2C_Start(void);
void Rzhl_I2C_Stop(void);
void Rzhl_I2C_SendByte(unsigned char Byte);
unsigned char Rzhl_I2C_ReadByte();
void Rzhl_I2C_ACK();
void Rzhl_I2C_NACK();
void Rzhl_Delay_us(unsigned int xus) ;
void Rzhl_Delay_ms(unsigned int xms) ;
