#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "lodepng.h"

#define INPUT_NAME "vis.png"

#define OUTPUT_NAME "ear.dfield"
#define OUTPUT_WIDTH 64
#define OUTPUT_HEIGHT 64

#define MAX_DISTANCE 512
#define PNG_TRESHOLD 127

unsigned char output[2 + OUTPUT_WIDTH * OUTPUT_HEIGHT];
double buffer[OUTPUT_WIDTH * OUTPUT_HEIGHT];

unsigned char *input;
unsigned input_width, input_height;
double max_distance = -MAX_DISTANCE;
double min_distance = MAX_DISTANCE;

void set_pixel(int x, int y)
{
	int source_is_inside, target_is_inside, cx, cy, ix, iy, dx, dy, im;
	int minx, miny, maxx, maxy;
	double min, distance;

	cx = (x * input_width) / OUTPUT_WIDTH;
	cy = (y * input_height) / OUTPUT_HEIGHT;

	min = MAX_DISTANCE;

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
	
	source_is_inside = input[(cx + cy * input_width) << 2] > PNG_TRESHOLD;
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

		if(min > max_distance){
			max_distance = min;
		}

		buffer[x + y * OUTPUT_WIDTH] = min;
		printf(" ");
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

		if(-min < min_distance){
			min_distance = -min;
		}

		buffer[x + y * OUTPUT_WIDTH] = -min;
		printf(".");
	}
}

int main(int argc, char** argv)
{
	unsigned char *debug_png;
	FILE *file;
	int i, pixels, ox, oy;
	double raw_pixel;
	size_t png_i;
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

	printf("%f %f\n", max_distance, min_distance);

	pixels = OUTPUT_WIDTH * OUTPUT_HEIGHT;
	debug_png = (unsigned char*)malloc(pixels * 4);
	for(i = 0; i < pixels; i++){
		raw_pixel = buffer[i];	
		raw_pixel -= min_distance;
		pixel = 255 - (raw_pixel / (max_distance - min_distance)) * 255;

		output[i + 2] = pixel;

		png_i = i << 2;
		debug_png[png_i + 0] = pixel;
		debug_png[png_i + 1] = pixel;
		debug_png[png_i + 2] = pixel;
		debug_png[png_i + 3] = 255;
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
