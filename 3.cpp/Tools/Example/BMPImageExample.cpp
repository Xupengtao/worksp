#include <iostream>
#include "../imageTools/BMPImage.h"

void testBMP8()
{
    BMPImage bmpImage(201,200,8);                           // 创建8位图像
    bmpImage.save("./8bit/8.bmp");                          // 保存图像
    for(uint4 i = 100;i<200;i++)                            // 修改8bit图像的某一区域
    {
        for(uint4 j = 100;j<201;j++)
        {
            bmpImage.at<uchar>(i, j) = 255;
        }
    }
    bmpImage.save("./8bit/8_1.bmp");
    BMPImage bmpImage1("./8bit/Dj.bmp");                    // 打开一张8位BMP图像
    bmpImage1.ImageStatus();
    bmpImage1.showMPGData(0,100);
    bmpImage1.save("./8bit/Dj_1.bmp");
    printf("Gray value:%d\n",bmpImage1.at<uchar>(324,157)); // 访问某一像素
    for(uint4 i = 0;i<bmpImage1.height();i++)               // 修改某一区域
    {
        for(uint4 j = 100;j<bmpImage1.width();j++)
        {
            bmpImage1.at<uchar>(i, j) = 0;                  //第二种方式
        }
    }
    bmpImage1.save("./8bit/Dj_2.bmp");
    bmpImage = bmpImage1;                                   // 赋值构造函数测试
    bmpImage.save("./8bit/copy_1.bmp");
    BMPImage bmpImage2 = bmpImage1;                         // 拷贝构造函数测试
    bmpImage2.save("./8bit/copy_2.bmp");
}

void testBMP24()
{
    BMPImage bmpImage(201,500,24);                          // 创建24位图像
    bmpImage.save("./24bit/24.bmp");                        // 保存图像
    BGR bgr{0,255,0};                                       // 修改24bit图像的某一区域
    for(uint4 i = 100;i<200;i++)
    {
        for(uint4 j = 100;j<201;j++)
        {
            bmpImage.at<BGR>(i, j) = bgr;
        }
    }
    bmpImage.save("./24bit/24_1.bmp");
    BMPImage bmpImage1("./24bit/No.bmp");                   // 打开一张24位BMP图像
    bmpImage1.ImageStatus();
    bmpImage1.showMPGData(0,100);
    bmpImage1.showColorPalette(0,100);
    bmpImage1.save("./24bit/No_1.bmp");
    BGR value = bmpImage1.at<BGR>(0, 0);                    // 访问某一像素
    printf("(B:%d,G:%d,R:%d)\n",value.b,value.g,value.r);
    for(uint4 i = 0;i<bmpImage1.height();i++)               // 修改某一区域
    {
        for(uint4 j = 100;j<bmpImage1.width();j++)
        {
            bmpImage1.at<BGR>(i, j) = bgr;                  //第二种方式
        }
    }
    bmpImage1.save("./24bit/No_2.bmp");
    bmpImage = bmpImage1;                                   // 赋值构造函数测试
    bmpImage.save("./24bit/copy_1.bmp");
    BMPImage bmpImage2 = bmpImage1;                         // 拷贝构造函数测试
    bmpImage2.save("./24bit/copy_2.bmp");
}

void testBMP32()
{
    // -----------创建一张32位的纯黑图像------
    BMPImage bmp_32_0(233,233,32);                          // 第一种方式
    BMPImage bmp_32_1;
    bmp_32_1.create(233,233,32);                            // 第二种方式
    bmp_32_1.save("./32bit/32.bmp");                        // 保存
    // ----------打开一张32位BMP图像--------------
    BMPImage bmpImage1("./32bit/32.bmp");                   // 第一种方式
    BMPImage bmpImage2;                                     // 第二种方式
    bmpImage2.open("./32bit/32.bmp");
    // -----------访问某一像素----------------
    // 访问像素之前要判断图像的位数
    if(bmpImage1.depth() == 8)                              // 单通到图像，8位图像
    {
        uchar value = bmpImage1.at<uchar>(0,0);
        printf("Gray value:%d\n",value);
    }
    else if(bmpImage1.depth() == 24)                        //三通道图像,24位图像
    {
        BGR value = bmpImage1.at<BGR>(0, 0);
        printf("(B:%d,G:%d,R:%d)\n",value.b,value.g,value.r);
    }
    else if(bmpImage1.depth() == 32)                        //BGR+Alpha通道，32位图像
    {
        BGRA value = bmpImage1.at<BGRA>(0,0);               //另外一种方式
        printf("(B:%d,G:%d,R:%d,A:%d)\n",value.b,value.g,value.r,value.a);
    }
    // ------------修改某些像素--------------
    // 同样，修改图像之前也要判断图像的位数
    if(bmpImage1.depth() == 8)                              //8位图像
    {
        for(uint4 i = 100;i<200;i++)                        // 修改8bit图像的某一区域
        {
            for(uint4 j = 100;j<201;j++)
            {
                bmpImage1.at<uchar>(i, j) = 255;
            }
        }
    }
    else if(bmpImage1.depth() == 24)                        //24位三通道图像
    {
        BGR bgr{0,255,0};
        for(uint4 i = 100;i<200;i++)
        {
            for(uint4 j = 100;j<200;j++)
            {
                bmpImage1.at<BGR>(i, j) = bgr;
            }
        }
    }
    else if (bmpImage1.depth() == 32)                       //BGR+Alpha通道
    {
        BGRA bgra{0,255,0,255};
        for(uint4 i = 100;i<200;i++)
        {
            for(uint4 j = 100;j<200;j++)
            {
                bmpImage1.at<BGRA>(i, j) = bgra;
            }
        }
    }
    bmpImage1.save("./32bit/32_1.bmp");                     // 保存
    BMPImage copy = bmpImage1;                              // 拷贝构造函数测试
    copy.save("./32bit/copy_2.bmp");
    bmp_32_0 = bmpImage1;                                   // 赋值构造函数测试
    bmp_32_0.save("./32bit/copy_1.bmp");
}

/*
 * 保存二维数组的一个例子
 */
void save2DArray()
{
    const uint row_count = 500;                             //数组行数
    const uint col_count = 1000;                            //数组列数
    int array2d[row_count][col_count];
    for (uint i = 0; i < row_count; i++)                    // 初始化数组
    {
        for (uint j = 0; j < col_count; j++)
        {
            array2d[i][j] = i;
        }
    }
    // --------归一化到0～255---------------
    int max_value = 0;                                      // 第一步，找到最大值
    for(uint i=0;i<row_count;i++)
    {
        for(uint j=0;j<col_count;j++)
        {
            max_value = (max_value>array2d[i][j])?max_value:array2d[i][j];
        }
    }
    for(uint i=0;i<row_count;i++)                           // 第二步，缩放到0~1之间，然后放大255倍
    {
        for(uint j=0;j<col_count;j++)
        {
            array2d[i][j] = (int)(array2d[i][j]/(double)max_value*255); //缩放到0~255
        }
    }
    // --------保存数组至BMP图像-------------
    BMPImage image(col_count, row_count, 24);               // col_count是图像的宽，row_count是图像的高，创建大小和数组维数相同，纯黑24位BMP图像
    BGR color{ 0,0,0};                                      // 定义颜色
    for (uint i = 0; i < row_count; i++)
    {
        for (uint j = 0; j < col_count; j++)
        {
            color.b = (uchar)array2d[i][j];                 // 保存为灰度图时，b、g、r的值应相同。
            color.g = color.b;
            color.r = color.b;
            image.at<BGR>(i, j) = color;
        }
    }
    image.save("./save2DArray.bmp");
}

int main()
{
    BMPImage bmpImage(); 
    testBMP8();
    testBMP24();
    testBMP32();
    save2DArray();
    std::cout << "Hello, World!" << std::endl;
    cin.get();
    return 0;
}