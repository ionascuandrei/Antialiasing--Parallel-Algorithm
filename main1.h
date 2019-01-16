#ifndef MAIN_H
#define MAIN_H

typedef struct {
    int type;
    int width;
    int height;
    int maxval;
	unsigned char** image;
}image;

typedef struct {
	int tid;
	image *in;
	image *out;
}threads_info;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */
