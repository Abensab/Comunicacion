sound = [3,4,5];
filter = [2,1];



soundLength = length(sound);
filterLength = length(filter);
resutlLength = soundLength + filterLength - 1;
resultConv = zeros(1, resutlLength );


for i = 1:soundLength
    for j = 1:filterLength
        resultConv(i+j-1) = resultConv(i+j-1)+sound(i)*filter(j);
    end
end

resultConv
conv(sound,filter)