#ifndef MAIN_H2
#define MAIN_H2

typedef struct {
	int resolution;
	unsigned char** image;
}image;

typedef struct {
	int tid;
	image *img;
}threads_info;

void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);

#endif /* HOMEWORK_H1 */