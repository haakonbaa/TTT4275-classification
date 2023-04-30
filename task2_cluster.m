clear; clear all; clc;

%% Open files for writing

file_img = fopen('./input/train_cluster_images.bin','w');
fwrite(file_img,[0 0 8 3],'uint8','ieee-be'); % magic number
fwrite(file_img,64*10,'uint32','ieee-be'); % #images
fwrite(file_img,[28 28],'uint32','ieee-be'); % rows, cols

file_lab = fopen('./input/train_cluster_labels.bin','w');
fwrite(file_lab,[0 0 8 1],'uint8','ieee-be'); % magic number
fwrite(file_lab,64*10,'uint32','ieee-be'); % #images

%% load and cluster data

data = load('./input/data_all.mat')
[numofclass, b] = groupcounts(data.trainlab);
    

for index = 0:9
    class_indexes = find(data.trainlab == index);
    class_data = data.trainv(class_indexes,:);
    [~, C] = kmeans(class_data, 64);
    for i=1:64
        fwrite(file_img,C(i,:),'uint8','ieee-be');
        fwrite(file_lab,index,'uint8','ieee-be');
    end
    fprintf('class %d done\n',index);
end

fclose(file_img);
fclose(file_lab);



