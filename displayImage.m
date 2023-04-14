function displayImage(dimg)
    img = zeros(28,28);
    img(:) = dimg;
    image(img');
end