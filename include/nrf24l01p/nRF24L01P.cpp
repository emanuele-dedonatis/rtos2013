/**
 * File:   nRF24L01P.cpp
 *  \brief     This is the implementation of the functions defined in  nRF24L01P.h.
 *  \author    Andrea Corna
 *  \author    Lorenzo Fontana
 *  \version   1.0
 *  \date      03/01/2014
 */

#include "nRF24L01P.h"
#include <cstdio>
#include "miosix.h"

/*Pipe 0 macro*/
#define NRF24L01P_PIPE_NO_0                     0

/*Receive buffer size macro*/
#define NRF24L01P_RX_BUFFER_SIZE                32


/*Command MACRO*/
#define NRF24L01P_CMD_RD_REG                    0x00
#define NRF24L01P_CMD_WT_REG                    0x20
#define NRF24L01P_CMD_WR_TX_PAYLOAD             0xa0
#define NRF24L01P_CMD_NOP                       0xff
#define NRF24L01P_R_RX_PAY                      0x61
#define NRF24L01P_SPI_CMD_FLUSH_TX              0xe1

/*Module registers macro*/
#define NRF24L01P_REG_CONF                      0x00
#define NRF24L01P_REG_AA                        0x01
#define NRF24L01P_REG_EN_RXADDR                 0x02
#define NRF24L01P_REG_SETUP_AW                  0x03
#define NRF24L01P_REG_SETUP_RETR                0x04
#define NRF24L01P_REG_RF_CH                     0x05
#define NRF24L01P_REG_RF_SETUP                  0x06
#define NRF24L01P_REG_STATUS                    0x07
#define NRF24L01P_REG_RPD                       0x09
#define NRF24L01P_REG_RX_ADDR_P0                0x0a
#define NRF24L01P_REG_TX_ADDR                   0x10
#define NRF24L01P_REG_RX_PW_P0                  0x11
#define NRF24LO1P_REG_ADDR_BITMASK              0x1f

/*Configuration Config register*/
#define NRF24L01P_CONFIG_MASK_TX                (1<<5)
#define NRF24L01P_PRIM_RX                       (1<<0)
#define NRF24L01P_PWR_UP                        (1<<1)

/*Macro of Status Register*/
#define NRF24L01P_STATUS_TX_DS                  (1<<5)
#define NRF24L01P_STATUS_MAX_RT                 (1<<4)
#define NRF24L01P_STATUS_RX_DR                  (1<<6)
#define NRF24L01P_STATUS_RX_P_NO                (0x7<<1)

/*Configuration RF register*/
#define NRF24L01P_RX_FIFO_EMPTY                 (7<<1)
#define NRF24L01P_RF_MASK                       ((1 << 5)| (1<<3))
#define NRF24L01P_RF_250KBPS                    (1 << 5)
#define NRF24L01P_RF_1MBPS                      (0)
#define NRF24L01P_RF_2MBPS                      (1 << 3)
#define NRF24L01P_RF_SETUP_RF_PWR_MASK          (0x3<<1)
#define NRF24L01P_RF_SETUP_RF_DR_MASK           (40<<0)
#define NRF24L01P_RF_SETUP_PWR_0DBM             (0x3<<1)
#define NRF24L01P_RF_SETUP_PWR_MINUS_6DBM       (0x2<<1)
#define NRF24L01P_RF_SETUP_PWR_MINUS_12DBM      (0x1<<1)
#define NRF24L01P_RF_SETUP_PWR_MINUS_18DBM      (0x0<<1)
#define NRF24L01P_RF_DR_250KBPS                 (1<<5)
#define NRF24L01P_RF_DR_1MBPS                   (0)
#define NRF24L01P_RF_DR_2MBPS                   (1<<3)

/*Configuration CRC*/
#define NRF24L01P_CONFIG_CRC0                   (1<<2)
#define NRF24L01P_CONFIG_EN_CRC                 (1<<3)
#define NRF24L01P_CONFIG_CRC_MASK       (NRF24L01P_CONFIG_EN_CRC| NRF24L01P_CONFIG_CRC0)
#define NRF24L01P_CONFIG_CRC_NONE               (0)
#define NRF24L01P_CONFIG_CRC_8BIT       (NRF24L01P_CONFIG_EN_CRC)
#define NRF24L01P_CONFIG_CRC_16BIT      (NRF24L01P_CONFIG_EN_CRC|NRF24L01P_CONFIG_CRC0)

/*Configuration SETUP AW register*/
#define NRF24L01P_SETUP_AW_AW_MASK              (0x3<<0)
#define NRF24L01P_SETUP_AW_AW_3BYTE             (0x1<<0)
#define NRF24L01P_SETUP_AW_AW_4BYTE             (0x2<<0)
#define NRF24L01P_SETUP_AW_AW_5BYTE             (0x3<<0)

/*Time macro*/
#define NRF24L01P_TPD2STBY                      2000  //2mS
#define NRF24L01P_TPECE2CSN                     4     //4uS
#define NRF24L01P_TPECETR                       10
#define NRF24L01P_TPRCV                         130

/*CRC Configuration*/
#define NRF24L01P_CRC_NONE                      0
#define NRF24L01P_CRC_8_BIT                     8
#define NRF24L01P_CRC_16_BIT                    16

/*Output Power configuration*/
#define NRF24L01P_TX_PWR_ZERO_DB                 0
#define NRF24L01P_TX_PWR_MINUS_6_DB             -6
#define NRF24L01P_TX_PWR_MINUS_12_DB            -12
#define NRF24L01P_TX_PWR_MINUS_18_DB            -18

/*Data rate configuration*/
#define NRF24L01P_DATARATE_250KBPS              250
#define NRF24L01P_DATARATE_1MBPS                1000
#define NRF24L01P_DATARATE_2MBPS                2000

/*Tx & Rx Fifo Size*/
#define NRF24L01P_TX_FIFO_SIZE                  32
#define NRF24L01P_RX_FIFO_SIZE                  32

/*Frequency Configuration*/
#define NRF24L01P_MIN_RF_FREQUENCY              2400
#define NRF24L01P_MAX_RF_FREQUENCY              2525

/*Macro disable auto ack*/
#define NRF24L01P_EN_AA_NONE                    0

/*Macro disable auto retransmit*/
#define NRF24L01P_SETUP_RETR_NONE               0

/*Macro default address and width*/
#define NRF24L01P_ADDRESS_DEFAULT       ((unsigned long long) 0xE7E7E7E7E7 )
#define NRF24L01P_ADDRESS_DEFAULT_WIDTH         5

#define NRF24L01P_RX_PW_Px_MASK                 0x3F
    

using namespace miosix;


/*Spi Gpio*/
typedef Gpio<GPIOB_BASE,11> CE;
typedef Gpio<GPIOB_BASE,12> CS;
typedef Gpio<GPIOB_BASE,13> SCK;
typedef Gpio<GPIOB_BASE,14> MISO;
typedef Gpio<GPIOB_BASE,15> MOSI;
typedef Gpio<GPIOA_BASE,1> IRQ;


nRF24L01P::nRF24L01P() {
    
    spi = new spi_driver();
    setup_Gpio();
    reset_module();
    power_down();
    clear_pending_interrupt();
    set_crc_width(NRF24L01P_CRC_8_BIT);
    disable_auto_ack();
    disable_auto_retransmit();
    disable_tx_interrupt();
    set_transfer_size(32);
    set_frequency(2510);
    set_air_data_rate(NRF24L01P_DATARATE_1MBPS);
    printf("Status %d\n",get_register_status());
    printf("Output power %d\n",get_output_power());
    printf("Air data rate %d\n",get_air_data_rate());
    printf("Crc %d\n",get_crc_width());
    printf("Channel %d\n", get_register(NRF24L01P_REG_RF_CH));
    printf("SETUP_AW %d\n",get_register(NRF24L01P_REG_SETUP_AW));
    printf("tx register 0x%010llX\n",get_tx_address());
    printf("rx address 0x%010llX\n",get_rx_address_pipe0());

        
}

nRF24L01P::nRF24L01P(const nRF24L01P& orig) {
}

nRF24L01P::~nRF24L01P() {
     
}
    /*clear every pending interrupt bits*/
void nRF24L01P::clear_pending_interrupt(){
    set_register(NRF24L01P_REG_STATUS, NRF24L01P_STATUS_TX_DS | NRF24L01P_STATUS_MAX_RT |
                                NRF24L01P_STATUS_RX_DR);
}


void nRF24L01P::disable_tx_interrupt(){
    int current_config = get_register(NRF24L01P_REG_CONF); 
    current_config |= NRF24L01P_CONFIG_MASK_TX;
    set_register(NRF24L01P_REG_CONF,current_config);

}



void nRF24L01P::set_receive_mode(){
    
    if (mode==NRF24L01P_POWER_DOWN_MODE){
        power_up();
    }
    int cur_config = get_register(NRF24L01P_REG_CONF);
    cur_config |= NRF24L01P_PRIM_RX;
    set_register(NRF24L01P_REG_CONF,cur_config);
    if (CE::value()==0){
    CE_enable();
     }
   
    usleep(NRF24L01P_TPRCV);    
    
    mode = NRF24L01P_RX_MODE;
   
}

/**
 * power up the module
 */
void nRF24L01P::power_up() {
    //I get the current config and I add the power up bit then I write it back 
    int current_config = get_register(NRF24L01P_REG_CONF); 
    current_config |= NRF24L01P_PWR_UP;
    set_register(NRF24L01P_REG_CONF,current_config);
    usleep(NRF24L01P_TPD2STBY);
    mode=NRF24L01P_STANDBY_MODE;
    flush_tx();
}

void nRF24L01P::power_down() {
    int current_config = get_register(NRF24L01P_REG_CONF);
    current_config &= ~NRF24L01P_PWR_UP;
    set_register(NRF24L01P_REG_CONF,current_config);
    usleep(NRF24L01P_TPD2STBY);
    mode = NRF24L01P_POWER_DOWN_MODE;
}



void nRF24L01P::set_transmit_mode(){
    if (mode==NRF24L01P_POWER_DOWN_MODE){
        power_up();
    }
    int cur_config = get_register(NRF24L01P_REG_CONF);
    cur_config &= ~NRF24L01P_PRIM_RX;
    set_register(NRF24L01P_REG_CONF,cur_config);
    
        CE_enable();
    
    mode = NRF24L01P_TX_MODE;
    
}

int nRF24L01P::transmit(int count, char* data){
    if( count < 0)
        return 0;
    if( count > NRF24L01P_TX_FIFO_SIZE)
        count = NRF24L01P_TX_FIFO_SIZE;
    CE_disable();
    set_register(NRF24L01P_REG_STATUS, NRF24L01P_STATUS_TX_DS); /*clear bit interrupt data sent tx fifo*/
    CS::low();
    spi->write(NRF24L01P_CMD_WR_TX_PAYLOAD); //command to start write from payload TX
    for( int i=0; i<count; i++){
        spi->write(*data++);
    }
    CS::high();
    set_transmit_mode();
    CE::high();
    usleep(NRF24L01P_TPECETR);  
    CE_disable();
    while( !( get_register_status() & NRF24L01P_STATUS_TX_DS)){
        
    } //polling waiting for transfer complete
    set_register(NRF24L01P_REG_STATUS, NRF24L01P_STATUS_TX_DS); /*clear bit data sent tx fifo*/
    set_receive_mode();
    return count;
    
}

int nRF24L01P::receive(char *data,int count){
    if(mode!=NRF24L01P_RX_MODE){
        printf("Before receive setup module in receive_mode\n");
        return -1;
    }
   if (count<=0) {
        return 0;
    }
    if (count>NRF24L01P_RX_BUFFER_SIZE){
        count= NRF24L01P_RX_BUFFER_SIZE;
    }
    
   CS::low();
   spi->write(NRF24L01P_R_RX_PAY);
        
   for(int i=0;i<count;i++){
        *data = spi->read();
         data++;
   }
   CS::low();
   return count;
  
}

void nRF24L01P::CE_restore(int old_ce){
    old_ce ? CE::high():CE::low();    
    //sleep to apply ce value change
    usleep(NRF24L01P_TPECE2CSN);    
}

void nRF24L01P::CE_enable(){
    CE::high();
    //sleep to apply ce value change
    usleep(NRF24L01P_TPECE2CSN);       
}

void nRF24L01P::CE_disable(){
    CE::low();
}


void nRF24L01P::set_register(int addr_register,int data_register){
        int old_ce =CE::value();  //save the CE value    
        CE_disable(); //in order to change value of register the module has to be in StandBy1 mode
        CS::low();
        spi->write(NRF24L01P_CMD_WT_REG |(addr_register & NRF24LO1P_REG_ADDR_BITMASK)); //command to write the at correct address of register
        spi->write(data_register & NRF24L01P_CMD_NOP);    //data used to set the register
        CS::high();
        CE_restore(old_ce);

}

int  nRF24L01P::get_register(int reg){
    int command = NRF24L01P_CMD_RD_REG | (reg & NRF24LO1P_REG_ADDR_BITMASK);
    int result;
    CS::low();
    spi->write(command);   
    result = spi->read();
    CS::high();
    return result;   
}

bool nRF24L01P::packet_in_pipe0(){
    int pipe = NRF24L01P_PIPE_NO_0;
    int status=get_register_status();
    if((status & NRF24L01P_STATUS_RX_DR)&&((status & NRF24L01P_STATUS_RX_P_NO)>>1)==(pipe & 0x7)){
        return true;
    }
    return false;
}


int nRF24L01P::get_register_status(){
    CS::low();
    int status = spi->read();    //the module send status bit every time is sent a command
    CS::high();
    return status;
}


void nRF24L01P::setup_Gpio(){
    
    MISO::mode(Mode::ALTERNATE);
    MISO::alternateFunction(5);
    MOSI::mode(Mode::ALTERNATE); 
    MOSI::alternateFunction(5);
    IRQ::mode(Mode::INPUT_PULL_UP);
    SCK::mode(Mode::ALTERNATE);
    SCK::alternateFunction(5);
    CS::mode(Mode::OUTPUT);
    CS::high();
    CE::mode(Mode::OUTPUT);
    CE::high();
    
}

void nRF24L01P::set_frequency(int frequency){
   if ((frequency < NRF24L01P_MIN_RF_FREQUENCY) | (frequency > NRF24L01P_MAX_RF_FREQUENCY)){
        printf("Error frequency module %d\n",frequency);
        return;
    }
    int channel = (frequency - NRF24L01P_MIN_RF_FREQUENCY) & 0x7F;  /*from manual RF_freq = frequency - NRF24L01P_MIN_RF_FREQUENCY)*/
    set_register(NRF24L01P_REG_RF_CH, channel);
}

int nRF24L01P::get_frequency(){
    int freq = get_register(NRF24L01P_REG_RF_CH);
    return (freq + NRF24L01P_MIN_RF_FREQUENCY);

}

void nRF24L01P::set_power_output(int power){
    
    int rf_config = get_register(NRF24L01P_REG_RF_SETUP) & ~NRF24L01P_RF_SETUP_RF_PWR_MASK; /*get rf config except for the power bits*/
    switch (power){                                     /*set the power*/
        case NRF24L01P_TX_PWR_ZERO_DB:
            rf_config |= NRF24L01P_RF_SETUP_PWR_0DBM;
            break;
        case NRF24L01P_TX_PWR_MINUS_6_DB:
            rf_config |= NRF24L01P_RF_SETUP_PWR_MINUS_6DBM;
            break;
        case NRF24L01P_TX_PWR_MINUS_12_DB:
            rf_config |= NRF24L01P_RF_SETUP_PWR_MINUS_12DBM;
            break;
        case NRF24L01P_TX_PWR_MINUS_18_DB:
            rf_config |= NRF24L01P_RF_SETUP_PWR_MINUS_18DBM;
            break;
        default:
            printf("Error power module %d\n",power);
            return;
    }
    set_register(NRF24L01P_REG_RF_SETUP, rf_config);    /*set the rf setup register*/
}

void nRF24L01P::set_air_data_rate(int rate){
    int air_config = get_register(NRF24L01P_REG_RF_SETUP) & ~NRF24L01P_RF_SETUP_RF_DR_MASK; /*get rf config except rf_dr_low and rf_dr_high*/
    switch (rate){
        case NRF24L01P_DATARATE_250KBPS:
            air_config |= NRF24L01P_RF_DR_250KBPS;
            break;
        case NRF24L01P_DATARATE_1MBPS:
            air_config |= NRF24L01P_RF_DR_1MBPS;
            break;
        case NRF24L01P_DATARATE_2MBPS:
            air_config |= NRF24L01P_RF_DR_2MBPS;
            break;
        default:
            printf("Error air data rate %d\n",rate);
            return;
    }
    set_register(NRF24L01P_REG_RF_SETUP, air_config);
}

int nRF24L01P::get_air_data_rate() {
 
    int rate = get_register(NRF24L01P_REG_RF_SETUP) & NRF24L01P_RF_SETUP_RF_DR_MASK;
 
    switch ( rate ) {
 
        case NRF24L01P_RF_250KBPS:
            return NRF24L01P_DATARATE_250KBPS;
 
        case NRF24L01P_RF_1MBPS:
            return NRF24L01P_DATARATE_1MBPS;
 
        case NRF24L01P_RF_2MBPS:
            return NRF24L01P_DATARATE_2MBPS;
 
        default:
            printf( "Unknown Air Data Rate value %d\n", rate );
            return 0;

    }
}

int nRF24L01P::get_output_power() {
 
    int power = get_register(NRF24L01P_REG_RF_SETUP) & NRF24L01P_RF_SETUP_RF_PWR_MASK;
 
    switch ( power ) {
 
        case NRF24L01P_RF_SETUP_PWR_0DBM:
            return NRF24L01P_TX_PWR_ZERO_DB;
 
        case NRF24L01P_RF_SETUP_PWR_MINUS_6DBM:
            return NRF24L01P_TX_PWR_MINUS_6_DB;
 
        case NRF24L01P_RF_SETUP_PWR_MINUS_12DBM:
            return NRF24L01P_TX_PWR_MINUS_12_DB;
 
        case NRF24L01P_RF_SETUP_PWR_MINUS_18DBM:
            return NRF24L01P_TX_PWR_MINUS_18_DB;
 
        default:
            printf( "Unknown RF Output Power value %d\n", power );
            return 0;
 
    }
}

int nRF24L01P::get_crc_width() {
 
    int crcWidth = get_register(NRF24L01P_REG_CONF) & NRF24L01P_CONFIG_CRC_MASK;
 
    switch ( crcWidth ) {
 
        case NRF24L01P_CONFIG_CRC_NONE:
            return NRF24L01P_CRC_NONE;
 
        case NRF24L01P_CONFIG_CRC_8BIT:
            return NRF24L01P_CRC_8_BIT;
 
        case NRF24L01P_CONFIG_CRC_16BIT:
            return NRF24L01P_CRC_16_BIT;
 
        default:
            printf( "Unknown CRC Width value %d\n", crcWidth );
            return 0;
 
    }
}

unsigned long long nRF24L01P::get_tx_address() {
 
    int setupAw = get_register(NRF24L01P_REG_SETUP_AW) & NRF24L01P_SETUP_AW_AW_MASK;
    int width;
    switch ( setupAw ) {
 
        case NRF24L01P_SETUP_AW_AW_3BYTE:
            width = 3;
            break;
 
        case NRF24L01P_SETUP_AW_AW_4BYTE:
            width = 4;
            break;
 
        case NRF24L01P_SETUP_AW_AW_5BYTE:
            width = 5;
            break;
 
        default:
            printf( "Unknown get_tx_address width value %d\n", setupAw );
            return 0;
 
    }
 
    int cn = (NRF24L01P_CMD_RD_REG | (NRF24L01P_REG_TX_ADDR & NRF24LO1P_REG_ADDR_BITMASK));
 
    unsigned long long address = 0;
 
    CS::low();
 
    spi->write(cn);
 
    for ( int i=0; i<width; i++ ) {
 
        //
        // LSByte first
        //
        address |= ( ( (unsigned long long)( spi->read() & 0xFF ) ) << (i*8) );
 
    }
 
    CS::high();
 
    return address;
}

unsigned long long nRF24L01P::get_rx_address_pipe0() {
 
    int pipe = NRF24L01P_PIPE_NO_0;
 
    int width;
 
    int setupAw = get_register(NRF24L01P_REG_SETUP_AW) & NRF24L01P_SETUP_AW_AW_MASK;
 
        switch ( setupAw ) {
 
            case NRF24L01P_SETUP_AW_AW_3BYTE:
                width = 3;
                break;
 
            case NRF24L01P_SETUP_AW_AW_4BYTE:
                width = 4;
                break;
 
            case NRF24L01P_SETUP_AW_AW_5BYTE:
                width = 5;
                break;
 
            default:
                printf( "Unknown get_rx_address width value %d\n", setupAw );
                return 0;
 
        }
 
  
    int rxAddrPxRegister = NRF24L01P_REG_RX_ADDR_P0 + ( pipe - NRF24L01P_PIPE_NO_0 );
 
    int cn = (NRF24L01P_CMD_RD_REG | (rxAddrPxRegister & NRF24LO1P_REG_ADDR_BITMASK));
 
    unsigned long long address = 0;
 
    CS::low();
 
    spi->write(cn);
 
    for ( int i=0; i<width; i++ ) {
 
        //
        // LSByte first
        //
        address |= ( ( (unsigned long long)( spi->read() & 0xFF ) ) << (i*8) );
 
    }
 
    CS::high();
 
    return address;
 
}


void nRF24L01P::set_tx_num_bit(int number){
    int num_bit = number -2;
    set_register(NRF24L01P_REG_SETUP_AW, num_bit);
}

void nRF24L01P::flush_tx()
{
  CS::low();
  spi->write( NRF24L01P_SPI_CMD_FLUSH_TX  );  //svuoto coda TX
  CS::high();
  
}

void nRF24L01P::set_crc_width(int width) {
 
    int config = (NRF24L01P_REG_CONF) & ~NRF24L01P_CONFIG_CRC_MASK;
 
    switch ( width ) {
 
        case NRF24L01P_CRC_NONE:
            config |= NRF24L01P_CONFIG_CRC_NONE;
            break;
 
        case NRF24L01P_CRC_8_BIT:
            config |= NRF24L01P_CONFIG_CRC_8BIT;
            break;
 
        case NRF24L01P_CRC_16_BIT:
            config |= NRF24L01P_CONFIG_CRC_16BIT;
            break;
 
        default:
            printf("Invalid CRC Width setting %d\n", width );
            return;
 
    }
 
    set_register(NRF24L01P_REG_CONF, config);
 
}

void nRF24L01P::disable_auto_ack(){
    set_register(NRF24L01P_REG_AA, NRF24L01P_EN_AA_NONE);// deactivate wait for ack*/
}


void nRF24L01P::disable_auto_retransmit() {
 
    set_register(NRF24L01P_REG_SETUP_RETR, NRF24L01P_SETUP_RETR_NONE);
 
}





void nRF24L01P::set_transfer_size(int size) {
 
    if ( ( size < 0 ) || ( size > NRF24L01P_RX_FIFO_SIZE ) ) {
 
        printf( "Invalid Transfer Size setting %d\n", size );
        return;
 
    }
 
    int rxPwPxRegister = NRF24L01P_REG_RX_PW_P0;
 
    set_register(rxPwPxRegister, ( size & NRF24L01P_RX_PW_Px_MASK ) );
 
}

void nRF24L01P::reset_interrupt(){
    int status = get_register_status();
    status |= NRF24L01P_STATUS_RX_DR;
    set_register(NRF24L01P_REG_STATUS, status);
    
}

int nRF24L01P::get_rpd_status(){
    return get_register(NRF24L01P_REG_RPD) & 0x01;
}

void nRF24L01P::reset_module(){
    set_register(NRF24L01P_REG_CONF, 8); //reset config_register to 00001000
    set_register(NRF24L01P_REG_AA, 63);
    set_register(NRF24L01P_REG_EN_RXADDR, 3);
    set_register(NRF24L01P_REG_SETUP_AW,3);
    set_register(NRF24L01P_REG_SETUP_RETR,3);
    set_register(NRF24L01P_REG_RF_SETUP,14);
    set_register(NRF24L01P_REG_STATUS,14);
    set_register(NRF24L01P_REG_RPD, 0);
    set_register(NRF24L01P_REG_RX_PW_P0,0);
    
}