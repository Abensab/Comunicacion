%p=[0 ones(1,10) zeros(1,5)]; % corresponde para n=-1 hasta n=14
%x=p; 
%v=p;
%y= conv(x,v);
%n=-2:25;
%stem(n,y(1:length(n)))

x = [3,4,5];
y = [2,1];

plot(x);

hold on;

resultConv = conv(x,y);

plot(resultConv,'r')

convolution1(x,y)
conv(x,y)