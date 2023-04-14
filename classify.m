% classify.m
%
% Reads the classied.bin file and prints some data

clear; clear all; clc;

if exist('OCTAVE_VERSION', 'builtin') ~= 0
    warning('off','all')
    pkg load statistics
end

%% Load data

data = load('./MNIST/data_all.mat');
whos -file ./MNIST/data_all.mat;

%% Read the classified data

file = fopen('classified.bin');
num = fread(file, 1, 'int32', 'ieee-le');

classified = zeros(num,2);
for i = 1:num
    classified(i,1) = fread(file,1,'uchar','ieee-le');
    classified(i,2) = fread(file,1,'uchar','ieee-le');
end
fclose(file);

%% Calculate confusion matrix
cm = zeros(10,10);
for i = 1:size(classified,1)
    r = classified(i,1)+1;
    c = classified(i,2)+1;
    cm(r,c) = cm(r,c) + 1;
end

display(cm)
correctClass = 0;
for i = 1:10
    correctClass = correctClass + cm(i,i);
end
fprintf("correctly classified: %d = %.2f%%\n",correctClass,100*correctClass/size(classified,1))
return

% save all missclassified images
for i = 1:size(classified,1)
    train_lab = classified(i,1);
    class_lab = classified(i,2);
    if train_lab ~= class_lab
        name = sprintf("images/miss/%d%d.png",train_lab, class_lab)
        fig = figure('Visible','off');
        displayImage(data.testv(i,:));
        set(gca, 'xtick', [], 'ytick', [], 'box', 'off','Position',[0.05 0.05 0.9 0.9]);
        set(fig, 'Position', [0 0 400 400]);
        saveas(fig, name)
    end
end
displayImages(data.testv(1,:),data.testv(2,:),'test.png', 0.05, 2)