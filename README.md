# Classification

Compile the classification code with

```bash
gcc -O6 classify.c -I. -o classify
```

Remember ```-O6``` for **blazingly** fast execution :). If you want, you can run the classification program. Below are some examples. 

```
./classify input/test_images.bin input/test_labels.bin \
    input/train_images.bin input/train_labels.bin \
    ./output/nnW.bin -k=1

./classify input/test_images.bin input/test_labels.bin \
    input/cluster/img0.bin input/cluster/lab0.bin \
    ./output/7nnC0.bin -k=7
```
The data is already stored in the ```output``` directory, so running the classifier is not really necessary. Running the classification program will create a binary file containing necessary data to make figures and such. For more information see the [classify.c](./classify.c) file. On my computer this takes less than 2 minutes.

The files ```confusion.m```, ```miss.m``` and ```task2_cluster.m``` should run out of the box and generate some plots in the ```./output/``` directory. I have not made a script to generate all the data at once because the script would take many minutes to run.