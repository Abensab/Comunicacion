function [ resultConv ] = convolution1( sound, filter )
%convolution1 made to compare with the original.
%   This is a function that dose the same thing as conv

soundLength = length(sound);
filterLength = length(filter);

resutlLength = soundLength + filterLength - 1;
resultConv = zeros(1, resutlLength );


for i = 1:soundLength
    for j = 1:filterLength
        resultConv(i+j-1) = resultConv(i+j-1)+sound(i)*filter(j);
    end
end

end



