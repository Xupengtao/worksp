#include<iostream>
#include<string>
using namespace std;
 
#define FrameSize 176*144  //视频帧大小
#define FrameNum 300       //yuv帧数
#define ET_SIZE 300
char errorText[ET_SIZE];
 
void errorMsg(const char *msg)
{
	printf("error:%s\n", msg);
#ifdef _WIN32
	system("pause");
#endif
	exit(-1);
}
 
void readSequence(char *fileName, unsigned char **frames)
{
	FILE *fp = fopen(fileName, "rb");
	if (fp == NULL)
	{
		sprintf(errorText, "File %s doesn't exist\n", fileName);
		errorMsg(errorText);
	}
 
	int uvSize = FrameSize / 2;            //H.264编解码获得的关键帧都是彩色序列，这里主要是提取亮度分量Y。
	unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char) * uvSize);
	for (int i = 0; i < FrameNum; i++) {
		//read y
		if (fread(frames[i], 1, FrameSize, fp) != FrameSize) {  //每一个keyFrames[i]分配的是 (unsigned char) * FrameSize的大小，所以fread要读的每个数据项的字节数为1，可以表示每个像素点亮度范围0~255
			sprintf(errorText, "Input sequence %s is not enough", fileName);
			errorMsg(errorText);
		}
 
		//read u,v
		fread(buf, 1, uvSize, fp);
	}
	free(buf);
	fclose(fp);
}
 
void write_bmpheader(unsigned char *bitmap, int offset, int bytes, int value) {
	int i;
	for (i = 0; i < bytes; i++)
		bitmap[offset + i] = (value >> (i << 3)) & 0xFF;
}
 
unsigned char *convertToBmp(unsigned char *inputImg, int width, int height, int *ouputSize) {
	/*create a bmp format file*/
	int bitmap_x = (int)ceil((double)width * 3 / 4) * 4;
	unsigned char *bitmap = (unsigned char*)malloc(sizeof(unsigned char)*height*bitmap_x + 54);
 
	bitmap[0] = 'B';
	bitmap[1] = 'M';
	write_bmpheader(bitmap, 2, 4, height*bitmap_x + 54); //whole file size
	write_bmpheader(bitmap, 0xA, 4, 54); //offset before bitmap raw data
	write_bmpheader(bitmap, 0xE, 4, 40); //length of bitmap info header
	write_bmpheader(bitmap, 0x12, 4, width); //width
	write_bmpheader(bitmap, 0x16, 4, height); //height
	write_bmpheader(bitmap, 0x1A, 2, 1);
	write_bmpheader(bitmap, 0x1C, 2, 24); //bit per pixel
	write_bmpheader(bitmap, 0x1E, 4, 0); //compression
	write_bmpheader(bitmap, 0x22, 4, height*bitmap_x); //size of bitmap raw data
	for (int i = 0x26; i < 0x36; i++)
		bitmap[i] = 0;
 
	int k = 54;
	for (int i = height - 1; i >= 0; i--) {
		int j;
		for (j = 0; j < width; j++) {
			int index = i*width + j;
			for (int l = 0; l < 3; l++)
				bitmap[k++] = inputImg[index];
		}
		j *= 3;
		while (j < bitmap_x) {
			bitmap[k++] = 0;
			j++;
		}
	}
 
	*ouputSize = k;
	return bitmap;
}
 
void saveToBmp(unsigned char *inputImg, int width, int height, char *outputFileName) {
	int size;
	unsigned char *bmp = convertToBmp(inputImg, width, height, &size);
	FILE *fp = fopen(outputFileName, "wb+");
	if (fp == NULL) {
		sprintf(errorText, "Could not open file: %s", outputFileName);
		errorMsg(errorText);
	}
	fwrite(bmp, 1, size, fp);
	fclose(fp);
	free(bmp);
}
 
int main() {
	int width = 176, height = 144;
	unsigned char **oriFrames;
	oriFrames = (unsigned char**)malloc(sizeof(unsigned char*) * FrameNum);
	for (int i = 0; i < FrameNum; i++) {
		oriFrames[i] = (unsigned char*)malloc(sizeof(unsigned char) * FrameSize);
	}
	readSequence("foreman_qcif_300.yuv", oriFrames);
	
	char imgName[30];
	for (int i = 0; i < FrameNum; i++) {
		sprintf(imgName,"./picture/ReconsFrame%d.bmp", i);
		//矩阵oriFrames[i]可以是任何你想保存为图片的像素矩阵，这里是yuv视频图像每一帧的像素数据
		saveToBmp(oriFrames[i], width, height, imgName);  
	}
}