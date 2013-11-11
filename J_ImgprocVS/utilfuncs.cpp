#include "utilfuncs.h"

void addTab(ImageStorage* imgstore, Ui::MainWindow *ui)
{
	QWidget *tab = new QWidget();
	QString qname = QString::fromStdString(imgstore->getCurrent()->first);
	tab->setObjectName(qname);
	QHBoxLayout *horizontalLayout = new QHBoxLayout(tab);
	horizontalLayout->setSpacing(6);
	horizontalLayout->setContentsMargins(11, 11, 11, 11);
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	QScrollArea * scrollArea = new QScrollArea(tab);
	scrollArea->setObjectName(QStringLiteral("scrollArea"));
	scrollArea->setWidgetResizable(true);
	QWidget * scrollAreaWidgetContents = new QWidget();
	scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
	scrollAreaWidgetContents->setGeometry(QRect(0, 0, 492, 447));
	QHBoxLayout* horizontalLayout_13 = new QHBoxLayout(scrollAreaWidgetContents);
	horizontalLayout->setSpacing(6);
	horizontalLayout_13->setContentsMargins(11, 11, 11, 11);
	horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
	QLabel* label = new QLabel(scrollAreaWidgetContents);
	std::string stmp = imgstore->getCurrent()->first;
	stmp.append("_label");
	QString qname2 = QString::fromStdString(stmp);
	label->setObjectName(qname2);
	QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy2.setHorizontalStretch(0);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
	label->setSizePolicy(sizePolicy2);

	horizontalLayout_13->addWidget(label);
	scrollArea->setWidget(scrollAreaWidgetContents);
	horizontalLayout->addWidget(scrollArea);
	ui->imageTab->addTab(tab, QString());

	std::string tmp1 = imgstore->getCurrent()->first.c_str();
	const char* tmp2 = tmp1.c_str();
	ui->imageTab->setTabText(ui->imageTab->indexOf(tab), QApplication::translate("MainWindow", tmp2, 0));
	ui->imageTab->setCurrentIndex(ui->imageTab->indexOf(tab));


	//-------------------------

	//---------------------
	//display image

	cv::Mat image = std::get<1>(imgstore->getCurrent()->second);  
	QImage img;  
	if(image.channels() == 3)    // RGB image  
	{  
		//cvtColor(image, image, CV_BGR2RGB);  
		img = QImage((const uchar*)(image.data),  //(const unsigned char*)  
			image.cols, image.rows,  
			image.cols*image.channels(),
			QImage::Format_RGB888);  
	}else                     // gray image  
	{  
		img = QImage((const uchar*)(image.data),  
			image.cols, image.rows,  
			image.cols*image.channels(),
			QImage::Format_Indexed8);  
	}  

	if (!img.isNull())
	{
		label->setPixmap(QPixmap::fromImage(img));  
		label->resize(label->pixmap()->size());  
		label->setAlignment(Qt::AlignCenter);
		label->show();
	}
	else
	{
		ASSERT(0);
	}
	  

	//---------------------
}

void addTab(std::string tabName, ImageStorage* imgstore, Ui::MainWindow *ui)
{
	/*ui->processBtn->setEnabled(true);
	ui->previousBtn->setEnabled(true);*/

	QWidget *tab = new QWidget();
	QString qname = QString::fromStdString(tabName);
	tab->setObjectName(qname);
	QHBoxLayout *horizontalLayout = new QHBoxLayout(tab);
	horizontalLayout->setSpacing(6);
	horizontalLayout->setContentsMargins(11, 11, 11, 11);
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	QScrollArea * scrollArea = new QScrollArea(tab);
	scrollArea->setObjectName(QStringLiteral("scrollArea"));
	scrollArea->setWidgetResizable(true);
	QWidget * scrollAreaWidgetContents = new QWidget();
	scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
	scrollAreaWidgetContents->setGeometry(QRect(0, 0, 492, 447));
	QHBoxLayout* horizontalLayout_13 = new QHBoxLayout(scrollAreaWidgetContents);
	horizontalLayout->setSpacing(6);
	horizontalLayout_13->setContentsMargins(11, 11, 11, 11);
	horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
	QLabel* label = new QLabel(scrollAreaWidgetContents);
	std::string stmp = tabName;
	stmp.append("_label");
	QString qname2 = QString::fromStdString(stmp);
	label->setObjectName(qname2);
	QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy2.setHorizontalStretch(0);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
	label->setSizePolicy(sizePolicy2);

	horizontalLayout_13->addWidget(label);
	scrollArea->setWidget(scrollAreaWidgetContents);
	horizontalLayout->addWidget(scrollArea);
	ui->imageTab->addTab(tab, QString());

	std::string tmp1 = tabName.c_str();
	const char* tmp2 = tmp1.c_str();
	ui->imageTab->setTabText(ui->imageTab->indexOf(tab), QApplication::translate("MainWindow", tmp2, 0));
	ui->imageTab->setCurrentIndex(ui->imageTab->indexOf(tab));


	//-------------------------

	//---------------------
	//display image

	cv::Mat image = std::get<1>(imgstore->getCurrent()->second);  
	QImage img;  
	if(image.channels() == 3)    // RGB image  
	{  
		//cvtColor(image, image, CV_BGR2RGB);  
		img = QImage((const uchar*)(image.data),  //(const unsigned char*)  
			image.cols, image.rows,  
			image.cols*image.channels(),
			QImage::Format_RGB888);  
	}else                     // gray image  
	{  
		img = QImage((const uchar*)(image.data),  
			image.cols, image.rows,  
			image.cols*image.channels(),
			QImage::Format_Indexed8);  
	}  

	if (!img.isNull())
	{
		label->setPixmap(QPixmap::fromImage(img));  
		label->resize(label->pixmap()->size());  
		label->setAlignment(Qt::AlignCenter);
		label->show();
	}
	else
	{
		ASSERT(0);
	}
	  

	//---------------------
}

void displayImg(cv::Mat _image, QLabel *label)
{
	cv::Mat image = _image;  
		QImage img;  
		if(image.channels() == 3)    // RGB image  
		{  
			//cvtColor(image, image, CV_BGR2RGB);  
			//already convert to RGB after loading to memory...
			img = QImage((const uchar*)(image.data),  //(const unsigned char*)  
				image.cols, image.rows, 
				image.cols*image.channels(),
				QImage::Format_RGB888);  
		}else                     // gray image  
		{  
			img = QImage((const uchar*)(image.data),  
				image.cols, image.rows,  
				image.cols*image.channels(),
				QImage::Format_Indexed8);  
		}  

		label->setPixmap(QPixmap::fromImage(img));  
		label->resize(label->pixmap()->size());  
		label->setAlignment(Qt::AlignCenter);
		label->show();  
}