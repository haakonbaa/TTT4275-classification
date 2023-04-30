clear; clear all; clc;


%% load and cluster data
data = load('./input/data_all.mat')
[numofclass, b] = groupcounts(data.trainlab);

% generate 10 clusters for testing

for i = 0:9
    %% Open files for writing
    filename_img = sprintf('./input/cluster/img%d.bin',i); 
    file_img = fopen(filename_img,'w');
    fwrite(file_img,[0 0 8 3],'uint8','ieee-be'); % magic number
    fwrite(file_img,64*10,'uint32','ieee-be'); % #images
    fwrite(file_img,[28 28],'uint32','ieee-be'); % rows, cols
    
    filename_lab = sprintf('./input/cluster/lab%d.bin',i); 
    file_lab = fopen(filename_lab,'w');
    fwrite(file_lab,[0 0 8 1],'uint8','ieee-be'); % magic number
    fwrite(file_lab,64*10,'uint32','ieee-be'); % #images
    
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
end



