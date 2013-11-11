#pragma once
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "debug.h"
#include "qfile.h"
#include "qfiledialog.h"
#include "qmessagebox.h"

#include "imagestorage.h"

#define _IN_VS_ 1

#if _IN_VS_
#include <string.h>
#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "modules.h"

#endif

void addTab(ImageStorage* imgstore, Ui::MainWindow *ui);
void addTab(std::string tabName, ImageStorage* imgstore, Ui::MainWindow *ui);
void displayImg(cv::Mat _image, QLabel *label);



