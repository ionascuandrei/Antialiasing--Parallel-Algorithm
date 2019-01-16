#include "main2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int num_threads;
int resolution;

void initialize(image *im) {
	im->resolution = resolution;
	unsigned char** image;
	image = malloc(im->resolution * sizeof(char*));
	for (int i = 0; i < im->resolution; i++) {
		image[i] = malloc(im->resolution * sizeof(char));
	}
	im->image = image;
}

void *threadFunction(void *var) {
	int thread_id = ((threads_info*)var)->tid;
	image *frame = ((threads_info*)var)->img;
	unsigned char **image = frame->image;

	int startLine = thread_id * floor(frame->resolution / num_threads);
	int endLine = startLine + floor(frame->resolution / num_threads);

	if (thread_id == num_threads - 1) {
		endLine = frame->resolution;
	}

	for (int x = startLine; x < endLine; x++) {
		for (int y = 0; y < frame->resolution; y++) {
			float result = (abs(-1 * ((float)x + 0.5) + 2 * 
				((float)y +0.5) + 0) * 100 / frame->resolution) / sqrt(5);
			if ( result <= 3) {
				image[(frame->resolution - y - 1)][x] = 0;
			}
			else {
				image[(frame->resolution - y - 1)][x] = 255;
			}
		}
	}

	return NULL;
}

void render(image *im) {
	pthread_t tid[num_threads];
	threads_info thread_id[num_threads];
	for (int i = 0; i < num_threads; i++) {
		thread_id[i].tid = i;
		thread_id[i].img = im;
	}
	for (int i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}
	for (int i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}
}

void writeData(const char * fileName, image *img) {
	FILE *out;
	out = fopen(fileName, "wb");

	fwrite("P5\n", 3, sizeof(char), out);
	fprintf(out, "%d %d\n", img->resolution, img->resolution);
	fwrite("255\n", 4, sizeof(char), out);

	for (int i = 0; i < img->resolution; i++) {		
		fwrite(img->image[i], 1, img->resolution, out);
		free(img->image[i]);
	}
	free(img->image);
	fclose(out);
}

