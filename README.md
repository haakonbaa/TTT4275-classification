# Classification

Compile the classification code with

```bash
gcc classify.c -O6 -o classify
```

Remember ```-O6``` for **blazingly** fast execution :). If you want, you can run the classification program with:

```
./classify input/test_images.bin input/test_labels.bin input/train_images.bin input/train_labels.bin output_data/task1_classified.bin

./classify input/test_images.bin input/test_labels.bin input/train_cluster_images.bin input/train_cluster_labels.bin output/task2_classified_nn.bin
```
but the data already exists in the ```output_data``` directory.
Running the classification program will create a binary file containing necessary data to make figures and such. For more information see the [classify.c](./classify.c) file. On my computer this takes less than 2 minutes.