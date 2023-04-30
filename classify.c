// classify.c
// This program reads the raw data in the MNIST dataset and uses a Nearest
// Neighbor classifier to classify the test images. The results are saved to
// a binary file.

#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<time.h>

// if we should print progress during classification.
#define DEBUG

// MNIST related definitions

#define IMAGE_SIZE 28*28

static const char USAGE[] = "USAGE:\n\
  ./classify test-img test-lab train-img train-lab outfile\n\
  \n\
  all files should be in the MNIST raw format\n";

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

void swap(size_t* x, int* y, int i, int j) {
    size_t temp = x[i];
    x[i] = x[j];
    x[j] = temp;

    int temp2 = y[i];
    y[i] = y[j];
    y[j] = temp2;
}

int partition(size_t* dists, int* indexes, int p, int r) {
    size_t x = dists[r];
    int i = p - 1;
    for (int j = p; j < r; j++) {
        if (dists[j] <= x){
            i++;
            swap(dists, indexes, i, j);
        }
    }
    swap(dists, indexes, i+1, r);
    return i + 1;
}

void quickSort(size_t* dists, int* indexes, int p, int r) {
    if (p < r) {
        int q = partition(dists, indexes, p, r);
        quickSort(dists, indexes, p, q - 1);
        quickSort(dists, indexes, q+1, r);
    };
}

void quickSelect(size_t* dists, int* indexes, int p, int r, int k) {
    if (p < r) {
        int q = partition(dists, indexes, p, r);
        if (q - p > k - 1) {
            quickSort(dists, indexes, p, q - 1);
        } else if (q - p < k - 1) {
            quickSort(dists, indexes, q+1, r);
        } else {
            return;
        }
    }
}

// takes the distances and labels of the k nearest neighbors and returns the
// class that the image should be classified as
int class_from_k_nearest(size_t* distances, int* labels, int K) {
    int occurs[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 9; i++) {
        occurs[labels[i]]++;
    }

    // find the most common label
    int max = 0;
    int maxIndex = 0;
    int isTied = 0;
    for (int i = 0; i < 9; i++) {
        if (occurs[i] > max) {
            max = occurs[i];
            maxIndex = i;
            isTied = 0;
        } else if (occurs[i] == max) {
            isTied = 1;
        }
    }
    
    if (!isTied) {
        return maxIndex;
    }

    return maxIndex;

    // if there is a tie, break it by finding the closest of the tied labels
    size_t minDist = SIZE_MAX;
    int minIndex = 0;
    for (int i = 0; i < 9; i++) {
        if (occurs[i] == max) {
            if (distances[i] < minDist) {
                minDist = distances[i];
                minIndex = i;
            }
        }
    }
    return minIndex;
}


int main(int argv, char** argc) {
    // parse input arguments

    char* filenames[5];
    int current_file = 0;
    int valid = 0;
    int K = 1;
    for (int current_arg = 1; current_arg < argv; current_arg++) {
        int k = 0;
        if (sscanf(argc[current_arg],"-k=%d", &k)) {
            K = k;
        } else {
            filenames[current_file] = argc[current_arg];
            current_file++;
            if (current_file == 5) {
                valid = 1;
            } else if (current_file > 5) {
                fprintf(stderr, "Too many arguments\n");
                fprintf(stderr, USAGE);
                return 1;
            }
        }
    }
    if (K <= 0) {
        fprintf(stderr,"K must be positive\n");
        fprintf(stderr, USAGE);
        return 1;
    }
    if (!valid) {
        fprintf(stderr, "Not enough arguments\n");
        fprintf(stderr, USAGE);
        return 1;
    }
    char* test_images = filenames[0];
    char* test_labels =  filenames[1];
    char* train_images = filenames[2];
    char* train_labels = filenames[3];
    char* save_file = filenames[4];
    printf("k=%d\n",K);

    // Load images

    struct Images* testImages = load_images(test_images,test_labels);
    struct Images* trainImages = load_images(train_images,train_labels);

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

#ifdef DEBUG
    printf("Classifying images... 0%%");
    fflush(stdout);
#endif

    clock_t start_time, stop_time;
    double cpu_time_used;

    start_time = clock();
    if (K == 1) {
        // for each test image, find the closest train image
        // O(n) best, worst, average
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
#ifdef DEBUG
            if (i % 10 == 9) {	
                printf("\rClassifying images... %.1f%%", ((float)(i+1)*100)/((float)testImages->numImages));	
                fflush(stdout);	
            }
#endif
        }	
    } else {
        // for each test image, find the K closest train images
        // O(n^2) worst, O(n) average
        for (size_t i = 0; i < testImages->numImages; i++) {	

            size_t* distances = (size_t*)calloc(trainImages->numImages, sizeof(size_t));
            int* labels = (int*)calloc(trainImages->numImages, sizeof(int));

            for (size_t j = 0; j < trainImages->numImages; j++) {	
                distances[j] = imageDistance(&testImages->images[i], &trainImages->images[j]);	
                labels[j] = trainImages->images[j].label;
            }	

            quickSort(distances, labels, 0, trainImages->numImages-1);
            classified[i*2+1] = class_from_k_nearest(distances, labels, K);
    	
            free(distances);
            free(labels);
#ifdef DEBUG
            if (i % 10 == 9) {	
                printf("\rClassifying images... %.1f%%", ((float)(i+1)*100)/((float)testImages->numImages));	
                fflush(stdout);	
            }	
#endif
        }	
    }

    stop_time = clock();
    cpu_time_used = ((double) (stop_time - start_time)) / CLOCKS_PER_SEC;
    printf("\nTime used %.4fs\n", cpu_time_used);

    // Saving images 

    FILE* file = fopen(save_file, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", save_file);
        return 1;
    }
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