% Reads a summary of classified images generated
% from classify.c and outputs the confusion matrix
% as an .svg in the output directory.

clear; clear all; clc;

for filename = {'nnW','7nnW','nnC0','7nnC0'}

%% Read the classified data
filename = filename{1}
file = fopen(sprintf('./output/%s.bin',filename),'r');
if file == -1
    fprintf('Could not read file ./output/%s.bin',filename)
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
saveas(fig,sprintf('./output/%s.svg',filename));

correctClass = 0;
for i = 1:10
    correctClass = correctClass + cm(i,i);
end
fprintf("correctly classified: %d = %.2f%%\n",correctClass,100*correctClass/size(classified,1))

end