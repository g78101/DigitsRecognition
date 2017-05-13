//
//  main.cpp
//  DR_TrainOrPredict
//
//  Created by Talka_Ying on 2017/4/20.
//  Copyright © 2017年 Talka_Ying. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace cv;

//#define Training
//#define saveFile

#define svmDataXml "$(PROJECT_DIR)/HOG_SVM_DATA.xml"
#define resulttxt "$(PROJECT_DIR)/DR_TrainOrPredict/t10k-images/result.txt"
#define saveFilePath "$(PROJECT_DIR)/DR_TrainOrPredict/training/%d_9999-%d[%d].bmp"
#define imageFileName "$(IMAGE_DIR)/image-%d.jpeg"

#define imageBeginNumber 1

#define imageNumber 1

void Predict(const char *svmDataXmlPath){
    String resultStr;
    char buffers [300]; int numCounts[10]={0};

    for ( int fileIndex = imageBeginNumber ; fileIndex < imageBeginNumber + imageNumber ; fileIndex++) {
        
        sprintf (buffers,imageFileName,fileIndex);
        Mat origin=imread(string(buffers));
        
        /************************************/
//        imshow("origin", origin);
        Mat gray,resultMat;

//去雜訊
        cvtColor(origin,gray,CV_RGB2GRAY);
        threshold( gray, gray, 110, 255, CV_THRESH_BINARY_INV );
        
        CvSize dst_cvsize; int scale = 3;
        dst_cvsize.width = gray.cols * scale;//目標影像的寬為源影像寬的scale倍
        dst_cvsize.height = gray.rows * scale;
        
        resize(gray, resultMat, dst_cvsize);
        erode(resultMat, resultMat, Mat());
        GaussianBlur(resultMat, resultMat, Size(3,3), 0);
        dilate(resultMat, resultMat, Mat());
 
//分群
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        
        Mat findContoursMat = resultMat.clone(), addRectangleMat = resultMat.clone();
        findContours(findContoursMat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
        vector<Rect> boundRect;
        for( int i = 0; i < (int)contours.size(); i++ )
        {
            if(contourArea(contours[i]) > 600)
            {
                Rect tempRect = boundingRect( Mat(contours[i]) );
                boundRect.push_back(tempRect);
                
                rectangle(addRectangleMat, tempRect, Scalar( 255, 255, 255 ));
            }
        }
        
        sort(boundRect.begin(), boundRect.end(), ^(Rect i,Rect j){ return i.x<j.x; });
        
        CvSVM svm;
        
        svm.load(svmDataXmlPath);//加载训练好的xml文件，这里训练的是10K个手写数字
        printf("-----------------------------\n");
        for (int imageIndex = 0; imageIndex < (int)boundRect.size() ; ++imageIndex) {
            
            int imageWidth = boundRect[imageIndex].width;
            int imageHeight = boundRect[imageIndex].height;
            Mat image = Mat(imageHeight+10, imageWidth+10, CV_8UC1);
            image.setTo(0);
            
            Mat roi = resultMat.colRange(boundRect[imageIndex].x,boundRect[imageIndex].x+boundRect[imageIndex].width)
            .rowRange(boundRect[imageIndex].y,boundRect[imageIndex].y+boundRect[imageIndex].height);
            double roiSum = sum(roi).val[0]/255;
            
            for (int row=0 ; row<roi.rows ; ++row) {
                for (int col=0 ; col<roi.cols ; ++col) {
                    int value = 0;
                    if(roi.at<uchar>(row,col) > 50 && roiSum < 500) {
                        value = 255;
                    }
                    else {
                        value = roi.at<uchar>(row,col);
                    }
                    image.at<uchar>(row+5,col+5) = value;
                }
            }
            
            IplImage Img = IplImage(image); // the same address
            IplImage* trainTempImg=cvCreateImage(cvSize(28,28),8,1);
            cvZero(trainTempImg);
            cvResize(&Img,trainTempImg);
            
//test show:
//            sprintf (buffers,"imageIndex-%d",imageIndex);
//            cvShowImage (buffers, Img);
//            waitKey();
            
//辨識
            HOGDescriptor *hog=new HOGDescriptor(cvSize(28,28),cvSize(14,14),cvSize(7,7),cvSize(7,7),9);
            vector<float>descriptors;//存放结果
            hog->compute(trainTempImg, descriptors,Size(1,1), Size(0,0)); //Hog特征计算
            
            CvMat* SVMtrainMat=cvCreateMat(1,(int)descriptors.size(),CV_32FC1);
            int n=0;
            for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
            {
                cvmSet(SVMtrainMat,0,n,*iter);
                n++;
            }
//
            int ret = svm.predict(SVMtrainMat);//检测结果
            
            resultStr.push_back(ret + '0');
            
            /************************************/
            
            numCounts[ret] += 1;
            
            image.release();
            roi.release();
            cvReleaseImage(&trainTempImg);
            cvReleaseMat( &SVMtrainMat );
            delete hog;
            
#ifdef saveFile
            sprintf (buffers,saveFilePath,ret,fileIndex,imageIndex);
            cvSaveImage(buffers, trainTempImg );
#endif
        }
        printf("\t\t%s",resultStr.c_str());
        printf("\n-----------------------------\n");
        resultStr.clear();
        
        origin.release();
        gray.release();
        findContoursMat.release();
        addRectangleMat.release();
        resultMat.release();
        
        if (imageNumber==1) {
            waitKey();
        }
    }
    if ( imageNumber !=1 ){
        printf("\n----------result-------------\n");
        for (int i=0 ; i < 10 ; ++i ) {
            printf("\t\t%d: %d\n",i,numCounts[i]);
        }
    }
}

void Train()
{
    vector<string> img_path;//输入文件名变量
    vector<int> img_catg;
    int nLine = 0;
    string buf;
    ifstream svm_data(resulttxt);//训练样本图片的路径都写在这个txt文件中，使用bat批处理文件可以得到这个txt文件
    unsigned long n;
    while( svm_data )//将训练样本文件依次读取进来
    {
        if( getline( svm_data, buf ) )
        {
            nLine ++;
            if( nLine % 2 == 0 )//注：奇数行是图片全路径，偶数行是标签
            {
                img_catg.push_back( atoi( buf.c_str() ) );//atoi将字符串转换成整型，标志(0,1，2，...，9)，注意这里至少要有两个类别，否则会出错
            }
            else
            {
                img_path.push_back( buf );//图像路径
            }
        }
    }
    svm_data.close();//关闭文件
    CvMat *data_mat, *res_mat;
    int nImgNum = nLine / 2; //nImgNum是样本数量，只有文本行数的一半，另一半是标签
    data_mat = cvCreateMat( nImgNum, 324, CV_32FC1 );  //第二个参数，即矩阵的列是由下面的descriptors的大小决定的，可以由descriptors.size()得到，且对于不同大小的输入训练图片，这个值是不同的
    cvSetZero( data_mat );
    //类型矩阵,存储每个样本的类型标志
    res_mat = cvCreateMat( nImgNum, 1, CV_32FC1 );
    cvSetZero( res_mat );
    IplImage* src;
    IplImage* trainImg=cvCreateImage(cvSize(28,28),8,3);//需要分析的图片，这里默认设定图片是28*28大小，所以上面定义了324，如果要更改图片大小，可以先用debug查看一下descriptors是多少，然后设定好再运行
    cout<<"Start Loading ..."<<endl;
    //处理HOG特征
    string::size_type i;
    for( i = 0; i != img_path.size(); i++ )
    {
        src=cvLoadImage(img_path[i].c_str(),1);
        if( src == NULL )
        {
            //            cout<<" can not load the image: "<<img_path[i].c_str()<<endl;
            continue;
        }
        //        cout<<"deal with\t"<<img_path[i].c_str()<<endl;
        cvResize(src,trainImg);
        HOGDescriptor *hog=new HOGDescriptor(cvSize(28,28),cvSize(14,14),cvSize(7,7),cvSize(7,7),9);
        vector<float>descriptors;//存放结果
        hog->compute(trainImg, descriptors,Size(1,1), Size(0,0)); //Hog特征计算
        //        cout<<"HOG dims: "<<descriptors.size()<<endl;
        n=0;
        for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
        {
            cvmSet(data_mat,i,n,*iter);//存储HOG特征
            n++;
        }
        cvmSet( res_mat, i, 0, img_catg[i] );
        //        cout<<"Done !!!: "<<img_path[i].c_str()<<" "<<img_catg[i]<<endl;
    }
    cout<<"Total Number: "<<i<<endl;
    CvSVM svm;//新建一个SVM
    CvSVMParams param;//这里是SVM训练相关参数
    CvTermCriteria criteria;
    criteria = cvTermCriteria( CV_TERMCRIT_EPS, 1000, FLT_EPSILON );
    param = CvSVMParams( CvSVM::C_SVC, CvSVM::RBF, 10.0, 0.09, 1.0, 10.0, 0.5, 1.0, NULL, criteria );
    //保存训练好的分类器
    std::cout<<"training... ... !!! \n "<<endl;
    svm.train( data_mat, res_mat, NULL, NULL, param );//训练数据
    //保存训练好的分类器
    std::cout<<"saving... ... !!! \n "<<endl;
    svm.save( svmDataXml );
    cout<<"HOG_SVM_DATA.xml is saved !!! \n exit train process"<<endl;
    cvReleaseMat( &data_mat );
    cvReleaseMat( &res_mat );
    cvReleaseImage(&trainImg);
}

int main()
{
    
#ifndef Training
    Predict(svmDataXml);
#else
    Train();
#endif

    return 0;
}




