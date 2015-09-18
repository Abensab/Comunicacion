function [ y ] = overlapSaveFFT( x, h, L )
%overlapSaveFFT(signal, filter, block size) Convolution using Overlap Save FFT
%   Code to perform Convolution using Overlap Save Method using FFT
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
 
Ma3=Ma2(:,M:(L+M-1));

y1=Ma3';

y=y1(:)';

end

