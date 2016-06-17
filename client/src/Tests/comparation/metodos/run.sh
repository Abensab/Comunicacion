#!/bin/bash

make
echo "SignalSize;FilterSize;BlockSize;Convolution;ConvolutionLoopUnroll8;ConvolutionOverlapADD;ConvolutionOverlapADD8;ConvolutionARM;ConvolutionARMOverlapADD;ConvolutionFFT;ConvolutionOverlapSave;ConvolutionFFTW;ConvolutionOverlapSaveFFTW;"

filter=64

for value in 64 128 256 512 1024 2048
do
	./convolution.o $value $filter $value
	./convolutionOverlapAdd.o $value $filter $value
	./convolutionOverlapAdd8.o $value $filter $value
	./convolutionNeon.o $value $filter $value
	./convolutionOverlapAddNEON.o $value $filter $value
	./convolutionFFT.o $value $filter $value
	./convolutionOverlapSave.o $value $filter $value
	./convolutionFFTW.o $value $filter $value
	./convolutionOverlapSaveFFTW.o $value $filter $value
	echo ""
done

filter=512

for value in 512 1024 2048
do
	./convolution.o $value $filter $value
	./convolutionOverlapAdd.o $value $filter $value
	./convolutionOverlapAdd8.o $value $filter $value
	./convolutionNeon.o $value $filter $value
	./convolutionOverlapAddNEON.o $value $filter $value
	./convolutionFFT.o $value $filter $value
	./convolutionOverlapSave.o $value $filter $value
	./convolutionFFTW.o $value $filter $value
	./convolutionOverlapSaveFFTW.o $value $filter $value
	echo ""
done

filter=2048

for value in 2048
do
	./convolution.o $value $filter $value
	./convolutionOverlapAdd.o $value $filter $value
	./convolutionOverlapAdd8.o $value $filter $value
	./convolutionNeon.o $value $filter $value
	./convolutionOverlapAddNEON.o $value $filter $value
	./convolutionFFT.o $value $filter $value
	./convolutionOverlapSave.o $value $filter $value
	./convolutionFFTW.o $value $filter $value
	./convolutionOverlapSaveFFTW.o $value $filter $value
	echo ""
done

make clean
