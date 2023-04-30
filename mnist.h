#ifndef _MNIST
#define _MNIST

#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>


#define IMAGE_SIZE 28*28
#define NUM_CLASSES 10


struct Image {
    int label;
    uint8_t pixels[IMAGE_SIZE];
};

struct Images {
    struct Image* images;
    size_t numImages;
};

union uint32 {
    uint8_t bytes[4];
    uint32_t value;
};

size_t imageDistance(struct Image* image1, struct Image* image2);

void imagePrint(struct Image* image);

struct Images* load_images(char* imageFile, char* labelFile);

// ---------- Implementation ---------------

// imageDistance returns the distance between two images.
// 
// The distance is the sum of the squared difference between
// corresponding pixels in the two images.
size_t imageDistance(struct Image* image1, struct Image* image2) {
    size_t distance = 0;
    for (int i = 0; i < IMAGE_SIZE; i++) {
        int v = (int)image1->pixels[i] - (int)image2->pixels[i];
        distance += (size_t)(v * v);
    }
    return distance;
}

// imagePrint prints an image to stdout.
void imagePrint(struct Image* image) {
    printf("Label: %d -----------\n", image->label);
    for (int i = 0; i < IMAGE_SIZE; i++) {
        if (i % 28 == 0) {
            printf("\n");
        }
        if (image->pixels[i] > 0) {
            printf("%02x", image->pixels[i]);
        } else {
            printf("  ");
        }
    }
    printf("\n");

}

struct Images* load_images(char* imageFile, char* labelFile) {
    struct Images* resImages = (struct Images*)malloc(sizeof(struct Images));

    FILE* file = fopen(imageFile, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", imageFile);
        return NULL;
    }

    // Magic number
    for (int i = 0; i < 4; i++) {
        fgetc(file);
    }

    // Number of images
    union uint32 numImages;
    for (int i = 0; i < 4; i++) {
        int j = 3 - i;
        numImages.bytes[j] = fgetc(file);
    }

    // 2x number of rows 
    for (int i = 0; i < 8; i++) {
        fgetc(file);
    }

    // load images
    struct Image* images = (struct Image*)calloc(numImages.value, sizeof(struct Image));
    for (int imagei = 0; imagei < numImages.value; imagei++) {
        for (int i = 0; i < IMAGE_SIZE; i++) {
            images[imagei].pixels[i] = (uint8_t)fgetc(file);
        }
    }

    fclose(file);

    FILE* file_labels = fopen(labelFile, "rb");
    if (file_labels == NULL) {
        fprintf(stderr, "Error opening file %s\n", labelFile);
        return NULL;
    }

    // Magic number and number of bytes

    for (int i = 0; i < 8; i++) {
        fgetc(file_labels);
    }

    for (int imagei = 0; imagei < numImages.value; imagei++) {
        images[imagei].label = fgetc(file_labels);
    }

    fclose(file_labels);

    resImages->numImages = numImages.value;
    resImages->images = images;

    return resImages;
}

#endif