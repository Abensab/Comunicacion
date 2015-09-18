load H1.mat
h = H_1;

[y,Fs] = audioread('/home/myhay/ClionProjects/Comunicacion/bin/sound/001_piano.wav','native');
yy=int32(y);
x=yy'.*(2^16);


%% Implementation of Overlap save and Overlap Add methods for an FIR digital
%  filter with Impulse response h(n) to a given input x(n)

%% Take the transfer function sequence and Input sequence
%h= input('Enter the transfer function of the digital filter:  ');

len_h=length(h);

%x= input('Enter the input sequence:  ');
len_x=length(x);

mem= input('Enter the size of memory ');

if(mem<len_h)
    error('The memory size should at least be equal to or greater than the size of the filter response');
end
block=mem-length(h)+1;
if(mod(len_x,block)~=0)
    x=[x zeros(1,(block-mod(len_x,block)))]; %Padding the input sequence with appropriate zeros
%    display('Padded X(n) is');
%    x
end
%% Splitting the input sequence into blocks of processing size
len_x=length(x);
no_of_blocks = ceil(len_x/block);
index=0;
split=zeros(no_of_blocks,block);
for i=1:no_of_blocks
    for j=1:block
        index=index+1;
        split(i,j)=x(index);
    end
end
%display('X(n) split into smaller blocks of processing size is ');
%split
h_new=[h zeros(1,mem-length(h))];
%display('h(n) padded for performing circular convolution is'); %Circular convolution needs both sequences to be of equal lengths
%h_new

%% Overlap Save Method
display('OVERLAP SAVE METHOD PROCESSING.............');
tic
overlap_block=zeros(no_of_blocks,mem);
for i=1:no_of_blocks
    if(i==1)
        temp= [zeros(1,len_h-1) split(i,:)];
        overlap_block(i,:)= temp;
    else
        temp=overlap_block(i-1,:);
        overlap_block(i,:)=[temp(1,mem-len_h+2:end) split(i,:)];
    end
end
%display('Blocks of x(n) padded with appropriate members from the original array are');
%overlap_block

for i=1:no_of_blocks
    conv_prod(i,:)=cconv(overlap_block(i,:),h_new,mem);
end
%display('Circular convolution operation on each block results as');
%conv_prod
conv_result=zeros(no_of_blocks,mem-len_h+1);
for i=1: no_of_blocks
    temp=conv_prod(i,:);
    conv_result(i,:)=temp(1,len_h:end);
end
%display('Discarding the first m-1 terms');
%conv_result
t=conv_result';
result=t(:);
%display('Convolution Result from Overlap Save Method is:    ');
%display(result);
toc
%% Overlap Add Method
display('OVERLAP ADD METHOD PROCESSING............');
tic
add_block=zeros(no_of_blocks, mem);
for i=1:no_of_blocks
    add_block(i,:)=[split(i,:) zeros(1,len_h-1)];
end
%display('Blocks of x(n) padded with zeros in the last m-1 positions are: ');
%add_block
for i=1:no_of_blocks
    conv_add(i,:)=cconv(add_block(i,:),h_new,mem);
end
%display('Circular convolution operation on each block results as: ');
%conv_add
%display('Adding the first m-1 terms of next block to the last m-1 terms of previous block');
adding=zeros(no_of_blocks, mem);
if(no_of_blocks>1)
    
    for i=1:no_of_blocks-1
        temp=conv_add(i+1,:);
        adding(i,:)=[conv_add(i,1:end-len_h+1) (conv_add(i,end-len_h+2:end)+temp(1,1:len_h-1))];
    end
        adding(no_of_blocks,:)=[conv_add(no_of_blocks,:)];
else
   adding=conv_add;
end
%adding
result=zeros(1,len_x+len_h-1);
for i=1:no_of_blocks
    if(i==1)
        result=adding(i,:);
        if(no_of_blocks==1)
            result=[adding(i,1:end-len_h+1)];
        end
    elseif(i==no_of_blocks)
        result=[result adding(i,len_h:end-len_h+1)];
    else
        result=[result adding(i,len_h:end)];
    end
end
%display('Result of Convolution from Overlap Add Method is: ')
%result'
toc 