%p=[0 ones(1,10) zeros(1,5)]; % corresponde para n=-1 hasta n=14
%x=p; 
%v=p;
%y= conv(x,v);
%n=-2:25;
%stem(n,y(1:length(n)))
clear

x = [3,4,5,6,7,8,9,10];
y = [2,1];

resultConv  = conv(x,y)';
resultConv2 = convolution1(x,y);
resultConv3 = overlapSavePAK(x,y,8);