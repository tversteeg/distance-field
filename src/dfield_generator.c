#include <stdio.h>
#include <math.h>

#include "../ear.bitmap"

#define OUTPUT_NAME "ear.dfield"
#define OUTPUT_WIDTH 64
#define OUTPUT_HEIGHT 64

#define MAX_DISTANCE 100

char output[2 + OUTPUT_WIDTH * OUTPUT_HEIGHT];

void set_pixel(int x, int y)
{
	int ix, iy, dx, dy, im;
	double min, distance;

	min = MAX_DISTANCE;

	printf("%lu\n", sizeof(ear_bits));

	for(iy = 0; iy < ear_height; iy++){
		dy = iy - y;
		if(dy > MAX_DISTANCE || dy < -MAX_DISTANCE){
			break;
		}
		dy *= dy;
		im = iy * ear_width;
		for(ix = 0; ix < ear_width; ix++){
			if(ear_bits[ix + im] == 0){
				continue;
			}
			dx = ix - x;
			distance = sqrt(dx * dx + dy);
			if(min > distance){
				min = distance;
			}
		}
	}

	min = (min / MAX_DISTANCE) * 255 - 128;

	output[2 + x + y * OUTPUT_WIDTH] = (char)min;
}

int main(int argc, char** argv)
{
	FILE *file;
	int ox, oy;

	output[0] = OUTPUT_WIDTH;
	output[1] = OUTPUT_HEIGHT;

	/* Ugly bruteforce */
	for(oy = 0; oy < OUTPUT_HEIGHT; oy++){
		for(ox = 0; ox < OUTPUT_WIDTH; ox++){
			set_pixel(ox, oy);
		}
	}

	file = fopen(OUTPUT_NAME, "wb");
	fwrite(output, sizeof(char), 2 + OUTPUT_WIDTH * OUTPUT_HEIGHT, file);
	fclose(file);

	printf("Written to %s\n", OUTPUT_NAME);
	
	return 0;
}
