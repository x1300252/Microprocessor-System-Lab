//These functions inside the asm file
extern void GPIO_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);
/**
* TODO: Show data on 7-seg via max7219_send
* Input:
*    data: decimal value
*    num_digs: number of digits will show on 7-seg
* Return:
*    0: success
*    -1: illegal data range(out of 8 digits range)
*/
int display(int data, int num_digs) {
	int i, neg = (data<0)?1:0;
	int return_val = (data > 99999999 || data < -999999) ? -1 : 0;

	for (i = 1; i <= num_digs; i++) {
		if (data > 0)
			max7219_send(i, data%10);
		else if (data < 0)
			max7219_send(i, 0-data%10);
		else if (neg == 1) {
			max7219_send(i, 10);
			neg = 0;
		}
		else
			max7219_send(i, 0);
		data /= 10;
	}
	for (; i <= 8; i++)
		max7219_send(i, 15);

	return return_val;
}

void main() {
	int student_id = 416038;
	GPIO_init();
	max7219_init();
	display(student_id, 8);
}
