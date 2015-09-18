% Theory
% 
% Overlap Save Method
% 
% In this method, the size of the input data blocks is N=L+M-1 and the DFTs 
% and the IDFTs are of length L. Each Data Block consists of the last M-1 
% data points of the previous block followed by L new data points to form a 
% data sequence of length N=L+M-1.An N point DFT  is computed for each data 
% block. The impulse response of the FIR filter is increased in length by 
% appending L-1 zeros and an N-point DFT of the sequence is computed once 
% and stored. The multiplication of the N-point DFTs for the mth block of 
% data yields 
%                         Ym(k)=h(k)Xm(k). 
% Since the data record is of length N, the first M-1 points of Ym(n)are 
% corrupted by aliasing and must be discarded. The last L points of Ym(n) 
% are exactly the same as the result  from linear convolution. To avoid 
% loss of data due to aliasing, the last M-1  points of each data record 
% are saved and these points become the first M-1 data points of the 
% subsequent record. To begin the processing, the first M-1 point of the 
% first record is set to zero. The resulting data sequence from the IDFT 
% are given where the first M-1 points are discarded due to aliasing and 
% the remaining L points constitute the desired result from the linear 
% convolution. This segmentation of the input data and the fitting of the 
% output data blocks together form the output sequence. 

clc;
clear all;
%x=input('Enter 1st sequence X(n)= ');
%h=input('Enter 2nd sequence H(n)= ');
x = [3,4,5,6,7,8,9,10];
h = [2,1.-1,-2];

%L=input('Enter length of each block L = ');
L = 2;


% % Code to plot X(n)
% subplot (2,2,1);
% stem(x);
% stem(x,'blue');
% xlabel ('n---->');
% ylabel ('Amplitude ---->');
% title('X(n)');
% 
% %Code to plot H(n)
% subplot (2,2,2);
% stem(h);
% stem(h,'black');
% xlabel ('n---->');
% ylabel ('Amplitude ---->');
% title(' H(n)');

% Code to perform Convolution using Overlap Save Method
M=length(h);
lx=length(x);

r=rem(lx,L);

x1=[x zeros(1,L-r)];

nr=(length(x1))/L;


h1=[h zeros(1,L-1)];

for k=1:nr
    Ma(k,:)=x1(((k-1)*L+1):k*L);
    if k==1
        Ma1(k,:)=[zeros(1,M-1) Ma(k,:)];
    else 
        Ma1(k,:)=[Ma(k-1,(L-M+2):L) Ma(k,:)];        
    end
     Ma2(k,:)=ifft(fft(Ma1(k,:)).*fft(h1));
end
% 
% Ma3=Ma2(:,M:(L+M-1));
% y1=Ma3';
% y=y1(:)'







% % Representation of the Convoled Signal
% subplot (2,2,3:4);
% stem(y,'red');
% xlabel ('n---->');
% ylabel ('Amplitude ---->');
% title ('Convolved Signal');
% 
% % Add title to the Overall Plot
% ha = axes ('Position',[0 0 1 1],'Xlim',[0 1],'Ylim',[0 1],'Box','off','Visible','off','Units','normalized', 'clipping' , 'off');
% text (0.5, 1,'\bf Convolution using Overlap Save Method ','HorizontalAlignment','center','VerticalAlignment', 'top')