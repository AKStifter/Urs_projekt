#define TP_INT_pin                                                                       PINB3_bit
#define MISO_pin                                                                         PINB6_bit
#define MOSI_pin                                                                         PINB5_bit
#define SCK_pin                                                                          PINB7_bit
#define TP_CS_pin                                                                        PINB4_bit

#define TP_INT_pin_dir                                                                   DDRB3_bit
#define MISO_pin_dir                                                                     DDRB6_bit
#define MOSI_pin_dir                                                                     DDRB5_bit
#define SCK_pin_dir                                                                      DDRB7_bit
#define TP_CS_pin_dir                                                                    DDRB4_bit

#define input                                                                            0
#define output                                                                           1

#define low                                                                              0
#define high                                                                             1

#define CMD_RDY                                                                          0X90
#define CMD_RDX                                                                          0XD0

#define error                                                                            50.0

#define RL_min                                                                           200.0
#define RL_max                                                                           1800.0
#define RW_min                                                                           400.0
#define RW_max                                                                           1800.0

#define res_l                                                                            319.0
#define res_w                                                                            239.0


unsigned int tpx = 0x0000;
unsigned int tpy = 0x0000;


void tp_init();
void tp_IO_init();
unsigned int TP_read();
void tp_write(unsigned char value);
void TP_read_coordinates(unsigned int *x_pos, unsigned int *y_pos);
unsigned int filter_data(unsigned int value, unsigned char axis);
float map(float value, float x_min, float x_max, float y_min, float y_max);
float constrain(float value, float value_min, float value_max);