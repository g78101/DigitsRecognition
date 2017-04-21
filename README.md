# Introduction

以 "hand-write-digit-recognition-with-opencv" 為基礎進行延伸

連結：https://github.com/wzyuliyang/hand-write-digit-recognition-with-opencv

提供訓練跟辨識功能

辨識:

    直接放入待辨識圖片的檔案位置即可。

訓練(半人工):

    1.將想訓練的圖片整理好(image-1,image-2,.....)，並變更起始圖片的編號跟圖片總數的#define。
    2.打開#define saveFile，執行程式。
    3.分割好的圖片會存在$(PROJECT_DIR)/DR_TrainOrPredict/training裡面(請以人工校正，目前大約有9成是對的)。
    4.打開trainging.txt與changeFileName.sh對照preValue、lastEndNumber更改值，並執行changeFileName.sh。
    5.執行$(PROJECT_DIR)/DR_TrainOrPredict/t10k-images的createpath.py。
    6.打開#define Training，重新執行程式。

# How to build

將$(PROJECT_DIR)變更為專案的路徑，$(IMAGE_DIR)變更為圖片所在的資料夾路徑
