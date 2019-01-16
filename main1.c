#include "main1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

pthread_barrier_t barrier;
pthread_mutex_t mutex;

int num_threads;
int resize_factor;


void readInput(const char * fileName, image *img) {
	FILE *in;
	in = fopen(fileName, "rb");
	char  type[3];

	//Reading image structure
	fgets(type, 4, in);
	if (strcmp(type, "P5\n") == 0) {
		img->type = 5;
	}
	else {
		img->type = 6;
	}

	char buffer[255];
	fgets(buffer, 255, in);
	img->width = atoi(buffer);
	strcpy(buffer, strchr(buffer, ' '));
	img->height = atoi(buffer);

	fgets(buffer, 255, in);
	img->maxval = atoi(buffer);

	//Reading the image pixels
	unsigned char** image;
	if (img->type == 5) {
		//P5
		image = malloc(img->height * sizeof(char*));
		for (int i = 0; i < img->height; i++) {
			image[i] = malloc(img->width * sizeof(char));
			fread(image[i], 1, img->width, in);
		}
	}
	else {
		//P6
		image = malloc(img->height * sizeof(char*));
		for (int i = 0; i < img->height; i++) {
			image[i] = malloc(3 * img->width * sizeof(char));
			fread(image[i], 3, img->width, in);
		}
	}
	img->image = image;
	fclose(in);
}

void writeData(const char * fileName, image *img) {
	FILE *out;
	out = fopen(fileName, "wb");

	if (img->type == 6) {
		fwrite("P6\n", 3, sizeof(char), out);
	}
	else {
		fwrite("P5\n", 3, sizeof(char), out);
	}
	
	fprintf(out, "%d %d\n", img->width, img->height);
	fprintf(out, "%d\n", img->maxval);

	//If we have RGB pixels or only 1 pixel
	int times = 0;
	if (img->type == 5) {
		times = 1;
	}
	else {
		times = 3;
	}

	for (int i = 0; i < img->height; i++) {
		fwrite(img->image[i], 1, times * img->width, out);
		free(img->image[i]);
	}
	free(img->image);
	fclose(out);
}

void *threadFunction(void *var) {
	int thread_id = ((threads_info*)var)->tid;

	image *out = ((threads_info*)var)->out;
	image *in = ((threads_info*)var)->in;

	//Spliting the pixel matrix for each thread
	int startLine = thread_id * floor(out->height / num_threads);
	int endLine = startLine + floor(out->height / num_threads);

	if (thread_id == num_threads - 1) {
		endLine = out->height;
	}
	//For RGB or simple pixel
	int step = 1;
	if (in->type == 6) {
		step = 3;
	}
	//Line & Column step for input image
	int lineIN = startLine;
	int columnIN = 0;

	for (int line = startLine; line < endLine; line++) {
		lineIN = line * resize_factor;
		for (int column = 0; column < (step * out->width); column += 1 * step) {
			if (resize_factor % 2 == 0) {
				//Resize factor is Even
				if (in->type == 5) {
					//Grayscale image
					unsigned int newPixel = 0;
					for (int i = lineIN; i < lineIN + resize_factor; i++) {
						for (int j = columnIN; j < columnIN + resize_factor; j++) {
							newPixel += in->image[i][j];
						}
					}
					newPixel /= pow(resize_factor, 2);
					out->image[line][column] = newPixel;
				}
				else {
					//Colored image
					unsigned int newPixelR = 0;
					unsigned int newPixelG = 0;
					unsigned int newPixelB = 0;
					for (int i = lineIN; i < lineIN + resize_factor; i++) {
						for (int j = columnIN; j < columnIN + (resize_factor * step); j+=3) {
							newPixelR += in->image[i][j];
							newPixelG += in->image[i][j+1];
							newPixelB += in->image[i][j+2];
						}
					}
					newPixelR /= pow(resize_factor, 2);
					newPixelG /= pow(resize_factor, 2);
					newPixelB /= pow(resize_factor, 2);
					out->image[line][column] = newPixelR;
					out->image[line][column+1] = newPixelG;
					out->image[line][column+2] = newPixelB;
				}
			}
			else {
				//Resize factor is Odd (3)
			
				if (in->type == 5) {
					//Grayscale image

					unsigned int newPixel = 0;
					newPixel += in->image[lineIN][columnIN] +
						in->image[lineIN][columnIN + 2] +
						in->image[lineIN + 2][columnIN] +
						in->image[lineIN + 2][columnIN + 2];
					newPixel += 2 * (in->image[lineIN][columnIN + 1] +
						in->image[lineIN + 1][columnIN] +
						in->image[lineIN + 1][columnIN + 2] +
						in->image[lineIN + 2][columnIN + 1]);
					newPixel += 4 * in->image[lineIN + 1][columnIN + 1];

					newPixel /= 16;
					out->image[line][column] = newPixel;
				}
				else {
					//Colored image

					unsigned int newPixelR = 0;
					unsigned int newPixelG = 0;
					unsigned int newPixelB = 0;
					newPixelR += in->image[lineIN][columnIN] + 
						in->image[lineIN][columnIN+6] +  
						in->image[lineIN + 2][columnIN] + 
						in->image[lineIN + 2][columnIN+6]; 
					newPixelR += 2 * (in->image[lineIN][columnIN+3] +  
						in->image[lineIN + 1][columnIN] +
						in->image[lineIN + 1][columnIN+6] +
						in->image[lineIN + 2][columnIN+3]);
					newPixelR += 4 * in->image[lineIN + 1][columnIN+3];

					newPixelG += in->image[lineIN][columnIN+1] + 
						in->image[lineIN][columnIN + 6 + 1] +  
						in->image[lineIN + 2][columnIN + 1] + 
						in->image[lineIN + 2][columnIN + 6 + 1]; 
					newPixelG += 2 * (in->image[lineIN][columnIN + 3 + 1] +
						in->image[lineIN + 1][columnIN + 1] +
						in->image[lineIN + 1][columnIN + 6 + 1] +
						in->image[lineIN + 2][columnIN + 3 + 1]);
					newPixelG += 4 * in->image[lineIN + 1][columnIN + 3 + 1];

					newPixelB += in->image[lineIN][columnIN + 2] +  
						in->image[lineIN][columnIN + 6 + 2] +  
						in->image[lineIN + 2][columnIN + 2] +  
						in->image[lineIN + 2][columnIN + 6 + 2]; 
					newPixelB += 2 * (in->image[lineIN][columnIN + 3 + 2] +
						in->image[lineIN + 1][columnIN + 2] +
						in->image[lineIN + 1][columnIN + 6 + 2] +
						in->image[lineIN + 2][columnIN + 3 + 2]);
					newPixelB += 4 * in->image[lineIN + 1][columnIN + 3 + 2];

					newPixelR /= 16;
					newPixelG /= 16;
					newPixelB /= 16;
					out->image[line][column] = newPixelR;
					out->image[line][column+1] = newPixelG;
					out->image[line][column+2] = newPixelB;
				}
			}
			columnIN += (resize_factor * step);
		}
		columnIN = 0;
	}
	return NULL;
}

void resize(image *in, image * out) {
	//Creating OUT image
	out->type = in->type;
	out->height = in->height / resize_factor;
	out->width = in->width / resize_factor;
	out->maxval = in->maxval;

	//Alocating memory for OUT image
	unsigned char** image;
	if (in->type == 5) {
		//P5 Grayscale image
		image = malloc(out->height * sizeof(char*));
		for (int i = 0; i < out->height; i++) {
			image[i] = malloc(out->width * sizeof(char));
		}
	}
	else {
		//P6 Colored image
		image = malloc(out->height * sizeof(char*));
		for (int i = 0; i < out->height; i++) {
			image[i] = malloc(3 * out->width * sizeof(char));
		}
	}
	out->image = image;

	pthread_t tid[num_threads];
	threads_info thread_id[num_threads];
	for (int i = 0; i < num_threads; i++) {
		thread_id[i].tid = i;
		thread_id[i].in = in;
		thread_id[i].out = out;
	}
	for (int i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}

	for (int i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

	for (int i = 0; i < in->height; i++) {
		free(in->image[i]);
	}
	free(in->image);
}
