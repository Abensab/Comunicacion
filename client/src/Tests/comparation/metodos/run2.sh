#!/bin/bash

make
echo "SignalSize;FilterSize;BlockSize;Convolution;ConvolutionLoopUnroll8;ConvolutionOverlapADD;ConvolutionOverlapADD8;ConvolutionARM;ConvolutionARMOverlapADD;ConvolutionFFT;ConvolutionOverlapSave;ConvolutionFFTW;ConvolutionOverlapSaveFFTW;"

for value in 64 128 256 512 1024 2048 4096 8192 16384
do
	./convolution.o $value $value $value
	./convolutionOverlapAdd.o $value $value $value
	./convolutionOverlapAdd8.o $value $value $value
	./convolutionNeon.o $value $value $value
	./convolutionOverlapAddNEON.o $value $value $value
	./convolutionFFT.o $value $value $value
	./convolutionOverlapSave.o $value $value $value
	./convolutionFFTW.o $value $value $value
	./convolutionOverlapSaveFFTW.o $value $value $value
	echo ""
done

make clean
