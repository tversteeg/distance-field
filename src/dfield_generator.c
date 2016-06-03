#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "lodepng.h"

#define INPUT_NAME "ear.png"

#define OUTPUT_NAME "ear.dfield"
#define OUTPUT_WIDTH 64
#define OUTPUT_HEIGHT 64

#define MAX_DISTANCE 200

unsigned char output[2 + OUTPUT_WIDTH * OUTPUT_HEIGHT];
unsigned char *input;
unsigned input_width, input_height;

void set_pixel(int x, int y)
{
	int cx, cy, ix, iy, dx, dy, im;
	double min, distance;

	min = MAX_DISTANCE;

	cx = (x * input_width) / OUTPUT_WIDTH;
	cy = (y * input_height) / OUTPUT_HEIGHT;

	for(iy = 0; iy < (int)input_height; iy++){
		dy = iy - cy;
		if(dy > MAX_DISTANCE){
			break;
		}else if(dy < -MAX_DISTANCE){
			continue;
		}

		dy *= dy;
		im = iy * input_width;
		for(ix = 0; ix < (int)input_width; ix++){
			if(input[(ix + im) << 2] > 10){
				continue;
			}
			dx = ix - cx;
			if(dx > MAX_DISTANCE){
				break;
			}else if(dx < -MAX_DISTANCE){
				continue;
			}

			distance = sqrt(dx * dx + dy);
			if(distance < min){
				min = distance;
			}
		}
	}

	min = (min / MAX_DISTANCE) * 255;
	if(min < 64){
		printf(".");
	}else{
		printf(" ");
	}

	output[2 + x + y * OUTPUT_WIDTH] = (unsigned char)min;
}

int main(int argc, char** argv)
{
	unsigned char *debug_png;
	FILE *file;
	int i, pixels, ox, oy;
	unsigned error;
	char pixel;

	output[0] = OUTPUT_WIDTH;
	output[1] = OUTPUT_HEIGHT;

	error = lodepng_decode32_file(&input, &input_width, &input_height, INPUT_NAME);
	if(error){
		fprintf(stderr,"Error %u: %s\n", error, lodepng_error_text(error));
		exit(1);
	}

	/* Ugly bruteforce */
	for(oy = 0; oy < OUTPUT_HEIGHT; oy++){
		for(ox = 0; ox < OUTPUT_WIDTH; ox++){
			set_pixel(ox, oy);
		}
		printf("\n");
	}

	pixels = OUTPUT_WIDTH * OUTPUT_HEIGHT * 4;
	debug_png = (unsigned char*)malloc(pixels);
	for(i = 0; i < pixels; i += 4){
		pixel = 128 + output[(i >> 2) + 2];
		debug_png[i + 0] = pixel;
		debug_png[i + 1] = pixel;
		debug_png[i + 2] = pixel;
		debug_png[i + 3] = 255;
	}
	error = lodepng_encode32_file(OUTPUT_NAME ".png", debug_png, OUTPUT_WIDTH, OUTPUT_HEIGHT);
	if(error){
		fprintf(stderr,"Error %u: %s\n", error, lodepng_error_text(error));
		exit(1);
	}

	file = fopen(OUTPUT_NAME, "wb");
	fwrite(output, sizeof(char), 2 + OUTPUT_WIDTH * OUTPUT_HEIGHT, file);
	fclose(file);

	printf("Written to %s\n", OUTPUT_NAME);
	
	return 0;
}
