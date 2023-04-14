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