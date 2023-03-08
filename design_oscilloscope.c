
#include "firebird_simulation.h"
#include "u8glib.h"
#include "adc.h"
#include <util/delay.h>

int buffer_input_data[192];
int V_max, V_min, V_mid;
float V_pp;
int start_wave_idx, end_wave_idx;
float ms_per_div = 0.5;
float volts_per_div = 0.625;
float freq;
int pixel_loc[96];
int hold_flag = 0;
int vert_disp = 0;

void dso_switches_input_channel_config(void)
{

	div_hold_switch_ddr_reg &= ~(1<<div_add_sw_pin);
	div_hold_switch_ddr_reg &= ~(1<<div_sub_sw_pin);
	div_hold_switch_ddr_reg &= ~(1<<hold_sw_pin);
	
	div_hold_switch_port_reg |= (1<<div_add_sw_pin);
	div_hold_switch_port_reg |= (1<<div_sub_sw_pin);
	div_hold_switch_port_reg |= (1<<hold_sw_pin);

	up_down_switch_ddr_reg &= ~(1<<up_sw_pin);
	up_down_switch_ddr_reg &= ~(1<<down_sw_pin);

	up_down_switch_port_reg |= (1<<up_sw_pin);
	up_down_switch_port_reg |= (1<<down_sw_pin);

	dso_input_channel_ddr_reg &= ~(1<<dso_input_channel_pin);

	dso_input_channel_port_reg &= ~(1<<dso_input_channel_pin);
}

void dso_switches_interrupt_config(void)
{

	cli();	// Disable interrupts globally

	EICRA_reg |= (1<<div_add_sw_ISC_bit1);
	EICRA_reg &= ~(1<<div_add_sw_ISC_bit0);
	EICRA_reg |= (1<<div_sub_sw_ISC_bit1);
	EICRA_reg &= ~(1<<div_sub_sw_ISC_bit0);

	EICRA_reg &= ~(1<<hold_sw_ISC_bit1);
	EICRA_reg &= ~(1<<hold_sw_ISC_bit0);

	EICRB_reg |= (1<<up_sw_ISC_bit1); 
	EICRB_reg &= ~(1<<up_sw_ISC_bit0);
	EICRB_reg |= (1<<down_sw_ISC_bit1);
	EICRB_reg &= ~(1<<down_sw_ISC_bit0);

	// Enable INT1, INT2, INT3, INT4 and INT5 interrupts
	EIMSK_reg |= (1<<div_add_sw_pin_int);
	EIMSK_reg |= (1<<div_sub_sw_pin_int);
	EIMSK_reg |= (1<<hold_sw_pin_int);
	EIMSK_reg |= (1<<up_sw_pin_int);
	EIMSK_reg |= (1<<down_sw_pin_int);


	sei();	// Enable interrupts gloabally
}
void dso_init(void)
{

	dso_switches_input_channel_config();
	dso_switches_interrupt_config();
	adc_init();
	glcd_setup();
}
ISR(div_add_sw_pin_int_vect)
{
	ms_per_div = ms_per_div + 0.5;
	if (ms_per_div >= 2)
	ms_per_div = 2;
}
ISR(div_sub_sw_pin_int_vect)
{
	ms_per_div = ms_per_div - 0.5;
	if (vert_disp <= 0.125)
	ms_per_div = 0.125;
}
ISR(hold_sw_pin_int_vect)
{
	hold_flag ^= hold_flag;
	
}
ISR(up_sw_pin_int_vect)
{
	vert_disp--;
	if (vert_disp <= -31)
	vert_disp = -30;
	
}
ISR(down_sw_pin_int_vect)
{
	vert_disp++;
	if (vert_disp >= 31)
	vert_disp = 30;
}
void dso_start_screen(void)
{
	setFont(u8g_font_5x7);
	firstPage();
	do
	{
		drawStr( 32, 32, "DSO START" );
	}
	while( nextPage() );
}
void sample_input_wave(void)
{

	for (int i = 0; i < 192; i++)
	{
		buffer_input_data[i] = convert_analog_channel_data(dso_input_channel_pin);
		
	}
}

void calc_vpp_vmid(void)
{

	V_max = 0;
	V_min = 0;

	for (int i = 0; i < 192; i++)
	{
		if (buffer_input_data[i] > V_max)
		V_max = buffer_input_data[i];
		
		if (buffer_input_data[i] < V_min)
		V_min = buffer_input_data[i];
	}

	// calculate V_mid : value in buffer array that is half of the amplitude
	V_mid = (V_max + V_min)/2;

	// calculate V_pp : value in Volts for the peak-to-peak amplitude of the input waveform
	V_pp = ((V_max - V_min) * 5.00) / 255 ;
}

void calc_start_end_wave_idx(void)
{


	start_wave_idx = 0;
	end_wave_idx = 0;

	// compute the values of start_wave_idx and end_wave_idx
	for (int i = 0; i < 97; i++)
	{
		if (buffer_input_data[i] < V_mid && buffer_input_data[i+1] >= V_mid)
		{
			start_wave_idx = i;
			break;
		}
	}

	for (int i = start_wave_idx + 1; i < 96 + start_wave_idx; i++)
	{
		if (buffer_input_data[i] < V_mid && buffer_input_data[i+1] >= V_mid)
		{
			end_wave_idx = i;
			break;
		}
	}

}

void calc_frequency(void)
{
	// << TODO >> : Complete the logic in function as expected in the comment above.
	float time = ( ( ( end_wave_idx - start_wave_idx) / 8) * ms_per_div);
	freq = 1/time;
}
void measure_parameters(void)
{
	calc_vpp_vmid();

	// calculate the index values from buffer array where the "first" wave in given waveform starts (start_wave_idx) and ends (end_wave_idx) its cycle
	calc_start_end_wave_idx();

	// calculate the frequency of input waveform in kHz unit
	calc_frequency();
}
void translate_wave_to_pixels(void)
{
	int start = 0;

	int ref_val = 32 + ( ( ( V_max + V_min ) / 2 ) >> 2);
	for (int i = 0; i < 96; i++)
	{
		pixel_loc[i] = ref_val - (buffer_input_data[i+start] >> 2);
	}
	
}

void draw_ref_frame(void)
{
	drawFrame(0, 0, 95, 63);

	// drawing the center horizontal and vertical line for reference and symmetricity
	drawLine(48, 0, 48, 63);
	drawLine(0, 32, 95, 32);

	// drawing short vertical lines or markings on center horizontal line with equal divisions for Time in msec
	for (int x = 0; x < 97; x += 8)
		drawLine(x, 31, x, 33);
	
	// drawing short horizontal lines or markings on center vertical line with equal divisions for Voltage level in Volts
	for (int y = 0 ; y < 64; y += 8)
		drawLine(47, y, 49, y);
	
	// plot small dots in all four quadrants for symmetricity in viewing the input waveform
	for (int x = 8; x < 96; x += 8)
	{
		for (int y=8; y < 64; y += 8)
			drawPixel(x,y);
	}
}
void draw_waveform(void)
{
	int y1, y2;

	for (int x = 0; x < 95; x++)
	{
		y1 = pixel_loc[x] + vert_disp;				// move the waveform up and down by +/- vert_disp
		y2 = pixel_loc[x + 1] + vert_disp;			// move the waveform up and down by +/- vert_disp

		drawLine(x, y1, x, y2);
	}
}
void display_wave_info_text_data(void)
{


	drawStr(96, 7, "ms/div");
	setPrintPos(96, 14);
	print_float(ms_per_div, 3);

	drawStr(96, 23, "V/div");
	setPrintPos(96, 30);
	print_float(volts_per_div, 3);
	
	drawStr(96, 40, "Vpp");
	setPrintPos(96, 47);
	print_float(V_pp, 3);
	drawStr(123, 47, "V");
	
	drawStr(96, 55, "f(kHz)");
	setPrintPos(96, 62);
	print_float(freq, 3);
}
void dso_display_waveform_data(void)
{
	if (hold_flag == 0){
	firstPage();
	do
	{
		draw_ref_frame();
		draw_waveform();
		display_wave_info_text_data();
	}
	while( nextPage() );
	}
}
void start_dso_operation(void)
{
	sample_input_wave();
	measure_parameters();
	translate_wave_to_pixels();
	dso_display_waveform_data();
}
int main(void)
{
	dso_init();
	dso_start_screen();

	while(1)
	{
		start_dso_operation();
	}
	return 0;
}