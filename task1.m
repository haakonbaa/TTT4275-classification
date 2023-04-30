% classify.m
%
% Reads the classied.bin file and saves some data

clear; clear all; clc;

if exist('OCTAVE_VERSION', 'builtin') ~= 0
    warning('off','all')
    pkg load statistics
end


%% Read the classified data


file = fopen('./output/task1_classified.bin','r');
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

%% Calculate confusion matrix

cm = zeros(10,10);
for i = 1:size(classified,1)
    r = classified(i,1)+1;
    c = classified(i,2)+1;
    cm(r,c) = cm(r,c) + 1;
end

fig = figure('visible','off');
confusionchart(cm,0:9);
set(fig, 'Position',[0 0 400 400]);
saveas(fig,'./output/confusion_matrix_nn_naive.svg');

correctClass = 0;
for i = 1:10
    correctClass = correctClass + cm(i,i);
end
fprintf("correctly classified: %d = %.2f%%\n",correctClass,100*correctClass/size(classified,1))

%% save some of the missclassified images

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

function displayImages(img1, img2, name, PADD, RATIO)
    if nargin < 5
        RATIO = 2 
    end
    if nargin < 4
        PADD = 0.1 
    end
    fig = figure('Visible','off');
    subplot(1,2,1);
    displayImage(img1);
    set(gca, 'xtick', [], 'ytick', [], 'box', 'off','Position',[PADD/RATIO PADD (1-2*PADD)/RATIO (1-2*PADD)]);
    subplot(1,2,2);
    displayImage(img2);
    set(gca, 'xtick', [], 'ytick', [], 'box', 'off','Position',[1-(1-2*PADD)/RATIO-PADD/RATIO PADD (1-2*PADD)/RATIO (1-2*PADD)]);
    set(fig, 'Position', [0 0 RATIO*400 400]);
    saveas(fig, name)
end


