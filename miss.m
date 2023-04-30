% Reads a summary of classified images generated
% from classify.c and outputs the misclassified
% images as .svg's in the output directory

clear; clear all; clc;

%% Read the classified data

file = fopen('./output/nnW.bin','r');
if file == -1
    display('Could not read')
    return
end
num = fread(file, 1, 'int32', 'ieee-le');
classified = zeros(num,2);
for i = 1:num
    classified(i,1) = fread(file,1,'uchar','ieee-le');
    classified(i,2) = fread(file,1,'uchar','ieee-le');
end
fclose(file);

%% save all of the missclassified images

data = load('./input/data_all.mat');

for i = 1:size(classified,1)
    train_lab = classified(i,1);
    class_lab = classified(i,2);
    if train_lab ~= class_lab
        name = sprintf("./output/miss/%dis%dclass_as%d.svg",i,train_lab, class_lab);
        fig = figure('Visible','off');
        displayImage(data.testv(i,:));
        set(gca, 'xtick', [], 'ytick', [], 'box', 'off','Position',[0.05 0.05 0.9 0.9]);
        set(fig, 'Position', [400 400 400 400]);
        saveas(fig, name)
    end
end

function displayImage(dimg)
    img = zeros(28,28);
    img(:) = dimg;
    image(img');
end