# Classification

Run the classification script with
```bash
gcc classify.c -O6 -o main && ./main
```
Remember ```-O6``` for **blazingly** fast execution :). This will create a binary file containing necessary data to make figures and such. For more information see the [classify.c](./classify.c) On my computer this takes less than 2 minutes.

run **classify.m** with [octave](https://octave.org/) or, if you prefer MATLAB.
```bash
octave classify.m
# or
matlab -nodisplay -nosplash -nodesktop -r "run('./classify.m');exit;"
```