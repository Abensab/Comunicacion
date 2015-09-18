load H1.mat
filtro = H_1';

[y,Fs] = audioread('/home/myhay/ClionProjects/Comunicacion/bin/sound/001_piano.wav','native');
yy=int32(y);
bb=yy.*(2^16);

sonido = double(bb);

tic
resultConv = conv(sonido, filtro);
toc

%tic
%resultConv2 = convolution1(sonido, filtro);
%toc

tic
resultConv3 = overlapSaveFFT(sonido', filtro', 512);
toc



% for i = 1: length(resultConv)
%     if (resultConv(i) ~= resultConv2(i))
%         fprintf( 'resultConv[%d] = %d \n', i, resultConv(i) )
%         fprintf( 'resultConv2[%d] = %d \n\n', i, resultConv2(i) )
%     
%     end
% end


% plot(resultConv);
% 
% hold on;
% 
% pause
% 
% plot(resultConv2,'r');
% 
% hold on;
% 
% pause
% 
% plot(resultConv3,'g');