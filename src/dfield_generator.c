#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "lodepng.h"

#define INPUT_NAME "ear.png"

#define OUTPUT_NAME "ear.dfield"
#define OUTPUT_WIDTH 64
#define OUTPUT_HEIGHT 64

#define MAX_DISTANCE 512
#define PNG_TRESHOLD 128

#define HALF_MAX_DISTANCE (MAX_DISTANCE / 2)

unsigned char output[2 + OUTPUT_WIDTH * OUTPUT_HEIGHT];
unsigned char *input;
unsigned input_width, input_height;

void set_pixel(int x, int y)
{
	int source_is_inside, target_is_inside, cx, cy, ix, iy, dx, dy, im;
	int minx, miny, maxx, maxy;
	double min, distance;

	cx = (x * input_width) / OUTPUT_WIDTH;
	cy = (y * input_height) / OUTPUT_HEIGHT;

	min = HALF_MAX_DISTANCE;

	minx = cx - MAX_DISTANCE;
	if(minx < 0){
		minx = 0;
	}
	miny = cy - MAX_DISTANCE;
	if(miny < 0){
		miny = 0;
	}
	maxx = cx + MAX_DISTANCE;
	if(maxx > (int)input_width){
		maxx = input_width;
	}
	maxy = cy + MAX_DISTANCE;
	if(maxy > (int)input_height){
		maxy = input_height;
	}
	
	source_is_inside = input[(cx + cy) << 2] > PNG_TRESHOLD;

	if(source_is_inside){
		for(iy = miny; iy < maxy; iy++){
			dy = iy - cy;	
			dy *= dy;
			im = iy * input_width;
			for(ix = minx; ix < maxx; ix++){
				target_is_inside = input[(ix + im) << 2] > PNG_TRESHOLD;
				if(target_is_inside){
					continue;
				}
				dx = ix - cx;
				distance = sqrt(dx * dx + dy);
				if(distance < min){
					min = distance;
				}
			}
		}

		min = HALF_MAX_DISTANCE + (min / MAX_DISTANCE) * 255;
	}else{
		for(iy = miny; iy < maxy; iy++){
			dy = iy - cy;	
			dy *= dy;
			im = iy * input_width;
			for(ix = minx; ix < maxx; ix++){
				target_is_inside = input[(ix + im) << 2] > PNG_TRESHOLD;
				if(!target_is_inside){
					continue;
				}
				dx = ix - cx;
				distance = sqrt(dx * dx + dy);
				if(distance < min){
					min = distance;
				}
			}
		}

		min = (min / MAX_DISTANCE) * 255;
	}

	if(min < 127){
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
#pragma omp for
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
