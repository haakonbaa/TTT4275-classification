// classify.c
// This program reads the raw data in the MNIST dataset and uses a Nearest
// Neighbor classifier to classify the test images. The results are saved to
// a binary file.

#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>

// MNIST related definitions

#define IMAGE_SIZE 28*28

struct Image {
    int label;
    uint8_t pixels[IMAGE_SIZE];
};

struct Images {
    struct Image* images;
    size_t numImages;
};

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

union uint32 {
    uint8_t bytes[4];
    uint32_t value;
};

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

int main() {
    struct Images* testImages = load_images("./MNIST/test_images.bin\0","./MNIST/test_labels.bin\0");
    struct Images* trainImages = load_images("./MNIST/train_images.bin\0","./MNIST/train_labels.bin\0");

    if (testImages == NULL || trainImages == NULL) {
        fprintf(stderr, "Error loading images. Exit\n");
        return 1;
    }

    // array of classified images. Structured like this:
    // [image0_label, image0_classified, image1_label, image1_classified, ...]
    int *classified = (int*)calloc(testImages->numImages, sizeof(int)*2); 
    for (size_t i = 0; i < testImages->numImages; i++) {
        classified[i*2] = testImages->images[i].label;
    }

    // classify test images
    printf("Classifying images... 0%%");
    fflush(stdout);
    for (size_t i = 0; i < testImages->numImages; i++) {
        size_t minDistance = imageDistance(&testImages->images[i], &trainImages->images[0]);
        int minDistanceLabel = 0;

        for (size_t j = 1; j < trainImages->numImages; j++) {
            size_t distance = imageDistance(&testImages->images[i], &trainImages->images[j]);
            if (distance < minDistance) {
                minDistance = distance;
                minDistanceLabel = trainImages->images[j].label;
            }
        }

        classified[i*2+1] = minDistanceLabel;
        if (i % 10 == 9) {
            printf("\rClassifying images... %.1f%%", ((float)(i+1)*100)/((float)testImages->numImages));
            fflush(stdout);
        }
    }

    // Saving images 

    FILE* file = fopen("classified.bin", "wb");
    fwrite((uint32_t*)&testImages->numImages, sizeof(uint32_t), 1, file);
    for (size_t i = 0; i < testImages->numImages; i++) {
        fwrite((uint8_t*)&classified[i*2], sizeof(uint8_t), 1, file);
        fwrite((uint8_t*)&classified[i*2+1], sizeof(uint8_t), 1, file);
    }
    fclose(file);

    // deallocate memory

    free(testImages->images);
    free(testImages);

    free(trainImages->images);
    free(trainImages);

    free(classified);

    printf("\n\x1b[32mDONE\x1b[0m\n");

    return 0;
}