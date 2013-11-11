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
#include "utilfuncs.h"
#include "vernier.h"
#endif


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	controller = nullptr;
	ui->processBtn->setEnabled(false);

	//derivatives
	ui->derivKsize->setEnabled(false);
	ui->xOrder->setEnabled(false);
	ui->yOrder->setEnabled(false);

	//
	ui->historyTab->close();

	ui->functionBox->setCurrentIndex(0);
	//ui->imgInfo->set
	connect(ui->processBtn, SIGNAL(clicked()), this, SLOT(updateImgInfo()));
	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(updateImgInfo()));
	connect(ui->imageTab, SIGNAL(tabCloseRequested(int)), this, SLOT(updateImgInfo()));
	connect(ui->imageTab, SIGNAL(currentChanged(int)), this, SLOT(updateImgInfo()));

	//
	connect(ui->actionView_Histories, SIGNAL(triggered()), this, SLOT(setUpHistoryTab()));
	connect(ui->imageTab, SIGNAL(currentChanged(int)), this, SLOT(setUpHistoryTab()));
	connect(ui->hRefreshBtn, SIGNAL(clicked()), this, SLOT(setUpHistoryTab()));
	connect(ui->actionClose_Histories_Window, SIGNAL(triggered()), ui->hCloseBtn, SIGNAL(clicked()));
	connect(ui->historyTab, SIGNAL(tabCloseRequested(int)), ui->hCloseBtn, SIGNAL(clicked()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_radioButton_1_clicked()
{
	CCvtColorController *ctrl = dynamic_cast<CCvtColorController *>(controller);
	ctrl->setCode(cv::COLOR_RGB2GRAY);
}

void MainWindow::on_radioButton_1_toggled(bool checked)
{
	//ASSERT(0);
}

void MainWindow::on_radioButton_1_pressed()
{

	//ASSERT(0);
}

void MainWindow::on_actionOpen_triggered()
{
#if _IN_VS_
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
		tr("Image Files (*.bmp *.jpg *.jpeg *.png)"));

	if (!fileName.isEmpty()) {

		ImageStorage* imgstore = ImageStorage::getInstance();
		imgstore->setInputImage(fileName);
		addTab(imgstore, ui);
		ui->processBtn->setEnabled(true);
	}
#endif
}

void MainWindow::on_actionClose_triggered()
{
	if (ui->imageTab->count() != 0)
	{
		on_imageTab_tabCloseRequested(ui->imageTab->currentIndex());
	}
}

void MainWindow::on_actionSave_triggered()
{
	ImageStorage* imgstore = ImageStorage::getInstance();
	std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();

	if (current != nullptr)
	{
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
			tr("Image Files (*.bmp *.jpg *.jpeg *.png)"));

		if (!fileName.isEmpty()) {

			/*ImageStorage* imgstore = ImageStorage::getInstance();
			std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();*/
			Ui::Image img = std::get<1>(current->second);

			std::string name = fileName.toStdString();
			cv::imwrite(name, img);
		}
	}
	else
	{
		QMessageBox::warning(this, tr("File"), tr("No image file to be saved."));
	}

}

void MainWindow::on_radioButton_2_clicked()
{
	CCvtColorController *ctrl = dynamic_cast<CCvtColorController *>(controller);
	ctrl->setCode(cv::COLOR_GRAY2RGB);
}

void MainWindow::on_radioButton_3_clicked()
{
	CCvtColorController *ctrl = dynamic_cast<CCvtColorController *>(controller);
	ctrl->setCode(cv::COLOR_RGB2XYZ);
}

void MainWindow::on_radioButton_4_clicked()
{
	CCvtColorController *ctrl = dynamic_cast<CCvtColorController *>(controller);
	ctrl->setCode(cv::COLOR_XYZ2RGB);
}

void MainWindow::on_radioButton_5_clicked()
{
	CCvtColorController *ctrl = dynamic_cast<CCvtColorController *>(controller);
	ctrl->setCode(cv::COLOR_RGB2YCrCb);
}

void MainWindow::on_radioButton_6_clicked()
{
	CCvtColorController *ctrl = dynamic_cast<CCvtColorController *>(controller);
	ctrl->setCode(cv::COLOR_YCrCb2RGB);
}

void MainWindow::on_radioButton_7_clicked()
{
	CCvtColorController *ctrl = dynamic_cast<CCvtColorController *>(controller);
	ctrl->setCode(cv::COLOR_RGB2HSV);
}

void MainWindow::on_radioButton_8_clicked()
{
	CCvtColorController *ctrl = dynamic_cast<CCvtColorController *>(controller);
	ctrl->setCode(cv::COLOR_HSV2RGB);
}

void MainWindow::on_radioButton_9_clicked()
{
	CCvtColorController *ctrl = dynamic_cast<CCvtColorController *>(controller);
	ctrl->setCode(cv::COLOR_RGB2YUV);
}

void MainWindow::on_radioButton_10_clicked()
{
	CCvtColorController *ctrl = dynamic_cast<CCvtColorController *>(controller);
	ctrl->setCode(cv::COLOR_YUV2RGB);
}

void MainWindow::on_radioButton_13_clicked()
{
	CBlurController *ctrl = dynamic_cast<CBlurController *>(controller);
	ctrl->setSmoothMethod(blur);
}

void MainWindow::on_radioButton_15_clicked()
{
	CBlurController *ctrl = dynamic_cast<CBlurController *>(controller);
	ctrl->setSmoothMethod(gaussianBlur);
}

void MainWindow::on_radioButton_14_clicked()
{
	CBlurController *ctrl = dynamic_cast<CBlurController *>(controller);
	ctrl->setSmoothMethod(medianBlur);
}

void MainWindow::on_radioButton_16_clicked()
{
	CBlurController *ctrl = dynamic_cast<CBlurController *>(controller);
	ctrl->setSmoothMethod(bilateralFilter);
}

void MainWindow::on_spinBox_6_editingFinished()
{
	/*CBlurController *ctrl = dynamic_cast<CBlurController *>(controller);
	if (!ctrl->setKernelsize(ui->spinBox_6->value()))
	{
	QMessageBox msb;
	msb.setText("Error! Kernel size should be odd! Please re enter the kernelsize...");
	msb.exec();
	}*/
}

void MainWindow::on_spinBox_6_valueChanged(const QString &arg1)
{
	ui->blurOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);
}

void MainWindow::on_spinBox_6_valueChanged(int arg1)
{

}

void MainWindow::on_radioButtonSobel_clicked()
{
	CDerivativesController *ctrl = dynamic_cast<CDerivativesController *>(controller);
	ctrl->setMethod(derivatives_sobel);
	ui->derivKsize->setEnabled(false);
	ui->xOrder->setEnabled(true);
	ui->yOrder->setEnabled(true);
	ui->doubleSpinBoxTh1->setEnabled(false);
	ui->doubleSpinBoxTh2->setEnabled(false);
}

void MainWindow::on_radioButtonLaplacian_clicked()
{
	CDerivativesController *ctrl = dynamic_cast<CDerivativesController *>(controller);
	ctrl->setMethod(derivatives_laplacian);
	ui->derivKsize->setEnabled(true);
	ui->xOrder->setEnabled(false);
	ui->yOrder->setEnabled(false);
	ui->doubleSpinBoxTh1->setEnabled(false);
	ui->doubleSpinBoxTh2->setEnabled(false);
}

void MainWindow::on_spinBox_editingFinished()
{

}

void MainWindow::on_spinBox_valueChanged(const QString &arg1)
{

}

void MainWindow::on_spinBox_valueChanged(int arg1)
{

}

void MainWindow::on_spinBox_2_editingFinished()
{

}

void MainWindow::on_spinBox_2_valueChanged(const QString &arg1)
{

}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{

}

void MainWindow::on_spinBox_3_editingFinished()
{

}

void MainWindow::on_spinBox_3_valueChanged(int arg1)
{

}

void MainWindow::on_spinBox_3_valueChanged(const QString &arg1)
{

}

void MainWindow::on_binaryB_clicked()
{
	CThresholdController *ctrl = dynamic_cast<CThresholdController *>(controller);
	ctrl->setType(BINARY);
	ui->spinBoxMaxVal->setEnabled(true);
	ui->spinBoxThresh->setEnabled(true);
}

void MainWindow::on_binaryinvB_clicked()
{
	CThresholdController *ctrl = dynamic_cast<CThresholdController *>(controller);
	ctrl->setType(BINARY_INV);
	ui->spinBoxMaxVal->setEnabled(true);
	ui->spinBoxThresh->setEnabled(true);
}

void MainWindow::on_truncB_clicked()
{
	CThresholdController *ctrl = dynamic_cast<CThresholdController *>(controller);
	ctrl->setType(TRUNC);
	ui->spinBoxMaxVal->setEnabled(false);
	ui->spinBoxThresh->setEnabled(true);
}

void MainWindow::on_tozeroB_clicked()
{
	CThresholdController *ctrl = dynamic_cast<CThresholdController *>(controller);
	ctrl->setType(TOZERO);
	ui->spinBoxMaxVal->setEnabled(false);
	ui->spinBoxThresh->setEnabled(true);
}

void MainWindow::on_tozeroinvB_clicked()
{
	CThresholdController *ctrl = dynamic_cast<CThresholdController *>(controller);
	ctrl->setType(TOZERO_INV);
	ui->spinBoxMaxVal->setEnabled(false);
	ui->spinBoxThresh->setEnabled(true);
}

void MainWindow::on_lineEdit_editingFinished()
{

}

void MainWindow::on_lineEdit_returnPressed()
{

}

void MainWindow::on_lineEdit_textEdited(const QString &arg1)
{

}

void MainWindow::on_lineEdit_2_editingFinished()
{

}

void MainWindow::on_lineEdit_2_returnPressed()
{

}

void MainWindow::on_lineEdit_2_textEdited(const QString &arg1)
{

}

void MainWindow::on_radioButtonSobel_2_clicked()
{

}

void MainWindow::on_radioButtonLaplacian_2_clicked()
{

}

void MainWindow::on_spinBox_4_editingFinished()
{

}

void MainWindow::on_spinBox_4_valueChanged(const QString &arg1)
{

}

void MainWindow::on_spinBox_4_valueChanged(int arg1)
{

}

void MainWindow::on_radioButtonSobel_3_clicked()
{

}

void MainWindow::on_radioButtonLaplacian_3_clicked()
{

}

void MainWindow::on_radioButton_clicked()
{

}

void MainWindow::on_radioButton_11_clicked()
{

}

void MainWindow::on_radioButton_12_clicked()
{

}

void MainWindow::on_spinBox_5_editingFinished()
{

}

void MainWindow::on_spinBox_5_valueChanged(const QString &arg1)
{

}

void MainWindow::on_spinBox_5_valueChanged(int arg1)
{

}

void MainWindow::on_processBtn_clicked()
{
	try
	{
		// set up input img
		ImageStorage* imgstore = ImageStorage::getInstance();
		std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();

		if (current == nullptr)
		{
			return;
		}

		if (controller)
		{
			int stat = controller->setInputImage(current->first, std::get<1>(current->second));
			//QMessageBox mbox;

			switch (stat)
			{
			case success:
				break;
			case not3Channels:
				QMessageBox::warning(this, tr("Error"), tr("Input Image has no 3 channels"));
				break;
			case notSingleChannel:
				QMessageBox::warning(this, tr("Error"), tr("Input Image is not single-channel"));
				break;
			default:
				break;
			}

			if (stat == success)
			{
				/* newly added 2013.8.10 */
				std::string subClassName = typeid(*controller).name();
				if (subClassName == "class CSisrController")
				{
					if (ui->goBtn->isEnabled())
					{
						ui->goBtn->clicked();
					}
				}
				else
				{
					// do ... 
					controller->process();
					cv::Mat results = controller->getLastResult();

					//int c = results.channels();

					std::string s_name = current->first;
					s_name.append("_label");

					QString qname = QString::fromStdString(s_name);
					QWidget * t = ui->imageTab->currentWidget();
					QString s = t->objectName();
					QLabel * label = ui->imageTab->currentWidget()->findChild<QLabel *>(qname);
					ASSERT(label != nullptr);

					displayImg(results, label);

					std::get<1>(current->second) = results.clone();
					std::get<2>(current->second).push_back(results.clone());
				}
			}
			else
			{
				/*	mbox.setText("Error! Input image is broken!");
				mbox.exec();*/
			}

		}
		else
		{
			QMessageBox::warning(this, tr("Error"), tr("Please select parameters first!"));
		}
	}
	catch(CSisr::SisrExcept& e)
	{
		QMessageBox::warning(this, tr("Exception"),
			"Currently there are no over-complete dics loaded...\nPlease load config file or train the dics");
	}
	catch(cv::Exception& e)
	{
		QMessageBox::warning(this, tr("Exception"), tr(e.what()));
	}
	catch(...)
	{
		QMessageBox::warning(this, tr("Exception"), tr("Unknown exception occurs..."));
	}

}

void MainWindow::on_previousBtn_clicked()
{

}

void MainWindow::on_nextBtn_clicked()
{

}

void MainWindow::on_imageTab_currentChanged(int index)
{
	if (ui->imageTab->currentWidget() != nullptr)
	{
		std::string s = ui->imageTab->currentWidget()->objectName().toStdString();

		ImageStorage* imgstore = ImageStorage::getInstance();

		imgstore->modifyCurrent(s);
	}
}

void MainWindow::on_imageTab_tabCloseRequested(int index)
{
	ImageStorage* imgstore = ImageStorage::getInstance();

	std::string s = ui->imageTab->widget(index)->objectName().toStdString();

	if (!imgstore->isEmpty())
	{
		imgstore->remove(s);
	}

	//ui->imageTab->removeTab(index);
	/* removeTab does't delete the 'Tab'actrually */
	ui->imageTab->widget(index)->deleteLater();

	if (imgstore->isEmpty())
	{	
		ui->processBtn->setEnabled(false);
	}
	/*ui->imageTab->deleteLater();*/
}

void MainWindow::on_functionBox_currentChanged(int index)
{
#if _IN_VS_
	//ASSERT(0);
	switch (index)
	{
	case 0:
		controller = CCvtColorController::getInstance();
		break;
	case 1:
		controller = CBlurController::getInstance();
		break;
	case 2:
		controller = CDerivativesController::getInstance();
		break;
	case 3:
		controller = CThresholdController::getInstance();
		break;
	case 4:
		controller = CPyramidsController::getInstance();
		break;
	case 5:
		controller = CMorphTransController::getInstance();
		break;
	case 6:
		controller = CSisrController::getInstance();
		//ui->processBtn->setEnabled(false);
		break;
	case 7:
		controller = nullptr;
		break;
	case 8:
		controller = CHistController::getInstance();
		break;
	default:
		break;
	}
#endif
}

void MainWindow::updateImgInfo()
{
	ImageStorage* imgstore = ImageStorage::getInstance();
	std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();

	if (!imgstore->isEmpty())
	{
		//std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();
		if (current != nullptr)
		{
			Ui::Image img =  std::get<1>(current->second);

			QString imgname = QString::fromStdString(current->first);
			QString info = QString("Name: %1\nChannels: %2\nDepth: %3\nSize: %4 x %5\n"
				).arg(imgname).arg(img.channels()).arg(img.depth()).arg(img.rows).arg(img.cols);

			QColor c("blue");
			ui->imgInfo->setTextColor(c);
			ui->imgInfo->setText(info);
		}
		else
		{
			ui->imgInfo->setText("");
		}
	}
	else
	{
		ui->imgInfo->setText("");
	}

	QCoreApplication::processEvents();
}

void MainWindow::on_blurOkBtn_clicked()
{
	CBlurController *ctrl = dynamic_cast<CBlurController *>(controller);
	if (!ctrl->setKernelsize(ui->spinBox_6->value()))
	{
		QMessageBox msb;
		msb.setText("Error! Kernel size should be odd! Please re enter the kernelsize...");
		msb.exec();
	}
	else
	{
		ui->blurOkBtn->setText("Submitted");
		ui->processBtn->setEnabled(true);
	}
}

void MainWindow::on_actionView_Histories_triggered()
{
	ui->historyTab->show();
}

void MainWindow::on_hCloseBtn_clicked()
{
	ui->historyTab->close();
}

/* */
void MainWindow::setUpHistoryTab()
{
	ui->labelHistory->clear();
	if (! ui->historyTab->isHidden())
	{
		ui->hNextBtn->setEnabled(false);
		ui->hPrevBtn->setEnabled(false);
		ui->hSaveBtn->setEnabled(false);

		/* get a copy of history */
		ImageStorage* imgstore = ImageStorage::getInstance();
		std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();

		/* -- */
		if (/*!imgstore->isEmpty() &&*/ current != nullptr)
		{
			Ui::Images* histories = &(std::get<2>(current->second)); 

			Vernier * vnr = Vernier::getInstance();

			if (vnr->init(histories->size()))
			{
				ui->hSaveBtn->setEnabled(true);
				// to be done...
				displayImg((*histories)[histories->size() - 1], ui->labelHistory);
				//
				if (vnr->getCurrentIdx() > 0/*< vnr->getLength() - 1*/)
				{
					ui->hPrevBtn->setEnabled(true);
				}
			}
		}
	}
	else
	{
		ASSERT(0);
	}

}
void MainWindow::on_hPrevBtn_clicked()
{
	ui->hNextBtn->setEnabled(true);
	Vernier * vnr = Vernier::getInstance();
	//vnr->increase();

	vnr->decrease();

	/* get a copy of history */
	ImageStorage* imgstore = ImageStorage::getInstance();
	std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();
	Ui::Images* histories = &(std::get<2>(current->second)); 
	int index = vnr->getCurrentIdx();
	displayImg((*histories)[index], ui->labelHistory);

	//if (vnr->getCurrentIdx() == vnr->getLength() - 1)
	if (vnr->getCurrentIdx() == 0)
	{
		ui->hPrevBtn->setEnabled(false);
	}

	QCoreApplication::processEvents();
}

void MainWindow::on_hNextBtn_clicked()
{
	ui->hPrevBtn->setEnabled(true);
	Vernier * vnr = Vernier::getInstance();
	//vnr->decrease();
	vnr->increase();

	/* get a copy of history */
	ImageStorage* imgstore = ImageStorage::getInstance();
	std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();
	Ui::Images* histories = &(std::get<2>(current->second)); 
	int index = vnr->getCurrentIdx();
	displayImg((*histories)[index], ui->labelHistory);

	//if (vnr->getCurrentIdx() == 0)
	if (vnr->getCurrentIdx() == vnr->getLength() - 1)
	{
		ui->hNextBtn->setEnabled(false);
	}

	QCoreApplication::processEvents();
}

void MainWindow::on_hSaveBtn_clicked()
{
	ui->hPrevBtn->setEnabled(true);
	Vernier * vnr = Vernier::getInstance();
	/* get pointer of histories */
	ImageStorage* imgstore = ImageStorage::getInstance();
	std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();
	Ui::Images* histories = &(std::get<2>(current->second)); 
	int index = vnr->getCurrentIdx();
	Ui::Image img = (*histories)[index];
	//displayImg((*histories)[index], ui->labelHistory);

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
		tr("Image Files (*.bmp *.jpg *.jpeg *.png)"));

	if (!fileName.isEmpty())
	{
		std::string name = fileName.toStdString();
		cv::imwrite(name, img);
	}

}

void MainWindow::on_hRefreshBtn_clicked()
{

}

void MainWindow::on_actionClose_Histories_Window_triggered()
{

}

void MainWindow::on_xOrder_valueChanged(int arg1)
{
	ui->derOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);

	if (ui->xOrder->value() == 0)
	{
		ui->yOrder->setValue(1);
	}
	else if (ui->xOrder->value() == 1)
	{
		ui->yOrder->setValue(0);
	}
}

void MainWindow::on_yOrder_valueChanged(int arg1)
{
	ui->derOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);

	if (ui->yOrder->value() == 0)
	{
		ui->xOrder->setValue(1);
	}
	else if (ui->yOrder->value() == 1)
	{
		ui->xOrder->setValue(0);
	}
}

void MainWindow::on_derivKsize_valueChanged(int arg1)
{
	ui->derOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);
}

void MainWindow::on_derOkBtn_clicked()
{
	CDerivativesController *ctrl = dynamic_cast<CDerivativesController *>(controller);

	ctrl->setKsize(ui->derivKsize->value());
	ctrl->setXorder(ui->xOrder->value());
	ctrl->setYorder(ui->yOrder->value());
	ctrl->setLowThresh(ui->doubleSpinBoxTh1->value());
	ctrl->setHighThresh(ui->doubleSpinBoxTh2->value());

	ui->derOkBtn->setText("Submitted");
	ui->processBtn->setEnabled(true);
}

void MainWindow::on_spinBoxMaxVal_valueChanged(int arg1)
{
	ui->threshOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);
}

void MainWindow::on_spinBoxThresh_valueChanged(int arg1)
{
	ui->threshOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);
}

void MainWindow::on_threshOkBtn_clicked()
{
	CThresholdController *ctrl = dynamic_cast<CThresholdController *>(controller);

	ctrl->setMaxval(ui->spinBoxMaxVal->value());
	ctrl->setThresh(ui->spinBoxThresh->value());

	ui->threshOkBtn->setText("Submitted");
	ui->processBtn->setEnabled(true);
}

void MainWindow::on_pyrGausianB_clicked()
{
	CPyramidsController *ctrl = dynamic_cast<CPyramidsController *>(controller);
	ctrl->setMethod(pyr_gaussian);

	QList<QAbstractButton *> btns = ui->pyrInerBtnG->buttons();
	std::for_each(btns.begin(), btns.end(),
		[] (QAbstractButton* elem){
			elem->setEnabled(false);
	});
	ui->doubleSpinBoxXscale->setEnabled(false);
	ui->doubleSpinBoxYscale->setEnabled(false);
}

void MainWindow::on_pyrLaplacianB_clicked()
{
	CPyramidsController *ctrl = dynamic_cast<CPyramidsController *>(controller);
	ctrl->setMethod(pyr_laplacian);

	QList<QAbstractButton *> btns = ui->pyrInerBtnG->buttons();
	std::for_each(btns.begin(), btns.end(),
		[] (QAbstractButton* elem){
			elem->setEnabled(false);
	});
	ui->doubleSpinBoxXscale->setEnabled(false);
	ui->doubleSpinBoxYscale->setEnabled(false);
}

void MainWindow::on_pyrimadOkBtn_clicked()
{
	CPyramidsController *ctrl = dynamic_cast<CPyramidsController *>(controller);
	//ctrl->setScale(ui->spinBoxPyrimad->value());
	ctrl->setXscale(ui->doubleSpinBoxXscale->value());
	ctrl->setYscale(ui->doubleSpinBoxYscale->value());
	ui->pyrimadOkBtn->setText("Submitted");
	ui->processBtn->setEnabled(true);
}



void MainWindow::on_spinBoxPyrimad_valueChanged(int arg1)
{
	ui->pyrimadOkBtn->setText("Submit");
}

void MainWindow::on_pyrResizeB_clicked()
{
	CPyramidsController *ctrl = dynamic_cast<CPyramidsController *>(controller);
	ctrl->setMethod(pyr_resize);

	QList<QAbstractButton *> btns = ui->pyrInerBtnG->buttons();
	std::for_each(btns.begin(), btns.end(),
		[] (QAbstractButton* elem){
			elem->setEnabled(true);
	});
	ui->doubleSpinBoxXscale->setEnabled(true);
	ui->doubleSpinBoxYscale->setEnabled(true);
}

void MainWindow::on_nearestB_clicked()
{
	CPyramidsController *ctrl = dynamic_cast<CPyramidsController *>(controller);
	ctrl->setInterpolateMethod(cv::INTER_NEAREST);
}

void MainWindow::on_linearB_clicked()
{
	CPyramidsController *ctrl = dynamic_cast<CPyramidsController *>(controller);
	ctrl->setInterpolateMethod(cv::INTER_LINEAR);
}

void MainWindow::on_areaB_clicked()
{
	CPyramidsController *ctrl = dynamic_cast<CPyramidsController *>(controller);
	ctrl->setInterpolateMethod(cv::INTER_AREA);
}

void MainWindow::on_cubicB_clicked()
{
	CPyramidsController *ctrl = dynamic_cast<CPyramidsController *>(controller);
	ctrl->setInterpolateMethod(cv::INTER_CUBIC);
}

void MainWindow::on_lanczosB_clicked()
{
	CPyramidsController *ctrl = dynamic_cast<CPyramidsController *>(controller);
	ctrl->setInterpolateMethod(cv::INTER_LANCZOS4);
}

void MainWindow::on_doubleSpinBoxXscale_valueChanged(double arg1)
{
	ui->pyrimadOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);
}

void MainWindow::on_doubleSpinBoxYscale_valueChanged(double arg1)
{
	ui->pyrimadOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);
}

void MainWindow::on_erodingB_clicked()
{
	CMorphTransController *ctrl = dynamic_cast<CMorphTransController *>(controller);
	ctrl->setMorphTranType(eroding);
}

void MainWindow::on_dilatingB_clicked()
{
	CMorphTransController *ctrl = dynamic_cast<CMorphTransController *>(controller);
	ctrl->setMorphTranType(dilating);
}

void MainWindow::on_radioButtonRect_clicked()
{
	CMorphTransController *ctrl = dynamic_cast<CMorphTransController *>(controller);
	ctrl->setElemType(0);
}

void MainWindow::on_radioButtonCross_clicked()
{
	CMorphTransController *ctrl = dynamic_cast<CMorphTransController *>(controller);
	ctrl->setElemType(1);
}

void MainWindow::on_radioButtonEllipse_clicked()
{
	CMorphTransController *ctrl = dynamic_cast<CMorphTransController *>(controller);
	ctrl->setElemType(2);
}

void MainWindow::on_spinBoxMorph_valueChanged(int arg1)
{
	ui->morphOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);
}

void MainWindow::on_morphOkBtn_clicked()
{
	CMorphTransController *ctrl = dynamic_cast<CMorphTransController *>(controller);
	ctrl->setElemSize(ui->spinBoxMorph->value());
	ui->morphOkBtn->setText("Submitted");
	ui->processBtn->setEnabled(true);
}

void MainWindow::on_learnDicBtn_clicked()
{
	ui->spinBoxSisrIteration->setEnabled(true);
	ui->spinBoxSisrDictsize->setEnabled(true);
	ui->spinBoxSisrOverlap->setEnabled(true);
	ui->spinBoxSisrScale->setEnabled(true);
	ui->spinBoxSisrSparsity->setEnabled(true);
	ui->spinBoxSisrWindow->setEnabled(true);
	ui->loadPicsBtn->setEnabled(true);
	ui->learnDicOkBtn->setEnabled(false);
	ui->saveCfBtn->setEnabled(false);
	ui->loadCfBtn->setEnabled(false);
	ui->goBtn->setEnabled(false);
	ui->showDiffBtn->setEnabled(false);
	ui->sisrSubmitBtn->setEnabled(true);

}

void MainWindow::on_scaleUpBtn_clicked()
{
	ui->spinBoxSisrIteration->setEnabled(false);
	ui->spinBoxSisrDictsize->setEnabled(false);
	ui->spinBoxSisrOverlap->setEnabled(false);
	ui->spinBoxSisrScale->setEnabled(false);
	ui->spinBoxSisrSparsity->setEnabled(false);
	ui->spinBoxSisrWindow->setEnabled(false);
	ui->loadPicsBtn->setEnabled(false);
	ui->learnDicOkBtn->setEnabled(false);
	ui->saveCfBtn->setEnabled(false);
	ui->loadCfBtn->setEnabled(true);
	ui->goBtn->setEnabled(true);
	ui->showDiffBtn->setEnabled(false);
	ui->sisrSubmitBtn->setEnabled(false);
}

void MainWindow::on_sisrSubmitBtn_clicked()
{
	CSisrController *ctrl = dynamic_cast<CSisrController *>(controller);
	ctrl->setDictsize(ui->spinBoxSisrDictsize->value());
	ctrl->setIter(ui->spinBoxSisrIteration->value());
	ctrl->setWindow(ui->spinBoxSisrWindow->value());

	int overlap = ui->spinBoxSisrOverlap->value();
	if (overlap > ui->spinBoxSisrWindow->value())
	{
		ctrl->setOverlap(ui->spinBoxSisrWindow->value() - 1);
	}
	else
	{
		ctrl->setOverlap(ui->spinBoxSisrOverlap->value());
	}
	ctrl->setScale((float)ui->spinBoxSisrScale->value());
	ctrl->setSparsity(ui->spinBoxSisrSparsity->value());

	ctrl->setConfsForDicLearning();

	ui->sisrSubmitBtn->setText("Submitted");
}

void MainWindow::on_spinBoxSisrScale_valueChanged(int arg1)
{
	ui->sisrSubmitBtn->setText("Submit");
}

void MainWindow::on_spinBoxSisrWindow_valueChanged(int arg1)
{
	ui->sisrSubmitBtn->setText("Submit");
}

void MainWindow::on_spinBoxSisrOverlap_valueChanged(int arg1)
{
	ui->sisrSubmitBtn->setText("Submit");
}

void MainWindow::on_spinBoxSisrSparsity_valueChanged(int arg1)
{
	ui->sisrSubmitBtn->setText("Submit");
}

void MainWindow::on_spinBoxSisrIteration_valueChanged(int arg1)
{
	ui->sisrSubmitBtn->setText("Submit");
}

void MainWindow::on_spinBoxSisrDictsize_valueChanged(int arg1)
{
	ui->sisrSubmitBtn->setText("Submit");
}

void MainWindow::on_loadPicsBtn_clicked()
{
	//QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
	//	tr("Image Files (*.bmp *.jpg *.jpeg *.png)"));

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QString());

	if (!dir.isEmpty()) {

		CSisrController *ctrl = dynamic_cast<CSisrController *>(controller);
		std::string path = dir.toStdString();
		ctrl->setPath(path);
		ctrl->loadTrainningSet();
		if (!ctrl->isTrainningSetEmpty())
		{
			ui->learnDicOkBtn->setEnabled(true);
		}
	}

}

void MainWindow::on_learnDicOkBtn_clicked()
{
	ui->learnDicOkBtn->setEnabled(false);
	ui->saveCfBtn->setEnabled(false);
	WorkerThread *workerThread = new WorkerThread();
	connect(workerThread, SIGNAL(resultReady()), this, SLOT(updateSisrButtons()));
	connect(workerThread, SIGNAL(exceptionOccurs()), this, SLOT(sisrExceptionOccurs()));

	workerThread->start();
}

void MainWindow::on_saveCfBtn_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(),
		tr("Config Files (*.xml)"));

	if (!fileName.isEmpty()) {

		CSisrController *ctrl = dynamic_cast<CSisrController *>(controller);
		ctrl->setPath(fileName.toStdString());
		ctrl->saveConfs();
	}
}

void MainWindow::updateSisrButtons()
{
	//ui->learnDicOkBtn->setEnabled(true);
	ui->saveCfBtn->setEnabled(true);
}

void MainWindow::sisrExceptionOccurs()
{
	QMessageBox::warning(this, tr("Exception"), tr("No enough memory... Please select less training pictures"));
	//ui->learnDicOkBtn->setEnabled(true);
}
void MainWindow::on_loadCfBtn_clicked()
{
	ui->goBtn->setEnabled(false);
	ui->loadCfBtn->setEnabled(false);

	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
		tr("Config Files (*.xml)"));

	if (!fileName.isEmpty()) {

		CSisrController *ctrl = dynamic_cast<CSisrController *>(controller);
		ctrl->setPath(fileName.toStdString());
		/*ctrl->loadConfs();*/
		LoadConfThread *workerThread = new LoadConfThread();
		connect(workerThread, SIGNAL(resultReady()), this, SLOT(updateGoBtn()));
		connect(workerThread, SIGNAL(exceptionOccurs()), this, SLOT(loadConfException()));
		workerThread->start();
	}
	else
	{
		ui->loadCfBtn->setEnabled(true);
		ui->goBtn->setEnabled(true);
	}
}

void MainWindow::loadConfException()
{
	QMessageBox::warning(this, tr("Exception"), tr("Load Config file failed...Maybe config is broken..."));
}
void MainWindow::updateGoBtn()
{
	ui->goBtn->setEnabled(true);
	ui->loadCfBtn->setEnabled(true);
}

void MainWindow::on_goBtn_clicked()
{
	ui->goBtn->setEnabled(false);
	//ui->processBtn->setEnabled(false);
	ui->loadCfBtn->setEnabled(false);

	ImageStorage* imgstore = ImageStorage::getInstance();

	if (imgstore->isEmpty())
	{
		QMessageBox::warning(this, tr("Error"), tr("No image in memory..."));
		ui->goBtn->setEnabled(true);
		ui->loadCfBtn->setEnabled(true);
		return;
	}
	std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();
	int stat = controller->setInputImage(current->first, std::get<1>(current->second));

	if (stat == success)
	{
		//CSisrController *ctrl = dynamic_cast<CSisrController *>(controller);
		try{
			/*ctrl->process();*/
			qRegisterMetaType<std::string>("std::string");
			ScaleUpThread *workerThread = new ScaleUpThread();
			connect(workerThread, SIGNAL(resultReady(std::string)), this, SLOT(getResultofScaleUp(std::string)));
			connect(workerThread, SIGNAL(exceptionOccurs()), this, SLOT(scaleUpException()));
			connect(workerThread, SIGNAL(scaleUpImageTooLarge()), this, SLOT(scaleUpImgTooLargeException()));

			workerThread->start();
		}
		catch(cv::Exception e)
		{
			ASSERT(0);
		}
		ui->showDiffBtn->setEnabled(false);
	}
}

void MainWindow::on_showDiffBtn_clicked()
{
	CSisrController *ctrl = dynamic_cast<CSisrController *>(controller);
	cv::Mat diff = ctrl->getDiff();
	cv::namedWindow("High Frequency Information");
	cv::imshow("High Frequency Information", diff);
	cv::waitKey(80);
}

void MainWindow::getResultofScaleUp(std::string filename)
{
	ImageStorage* imgstore = ImageStorage::getInstance();

	if (imgstore->isEmpty())
	{
		QMessageBox::warning(this, tr("Error"), tr("No image..."));
		return;
	}

	std::pair<const std::string, Ui::SrcAndRes>* pos = imgstore->find(filename);

	cv::Mat results = controller->getLastResult();
	std::string s_name = pos->first;
	s_name.append("_label");

	QString qname = QString::fromStdString(s_name);
	QString pageName = QString::fromStdString(filename);
	QWidget * t = ui->imageTab->findChild<QWidget*>(pageName);
	QString s = t->objectName();
	QLabel * label = t->findChild<QLabel *>(qname);
	ASSERT(label != nullptr);

	displayImg(results, label);

	std::get<1>(pos->second) = results.clone();
	std::get<2>(pos->second).push_back(results.clone());

	ui->goBtn->setEnabled(true);
	//ui->processBtn->setEnabled(true);
	ui->showDiffBtn->setEnabled(true);
	ui->loadCfBtn->setEnabled(true);
}
void MainWindow::scaleUpException()
{
	QMessageBox::warning(this, tr("Exception"), tr("Scale-up failed...Overcomplete dics are not ready..."));
	ui->goBtn->setEnabled(true);
}

void MainWindow::scaleUpImgTooLargeException()
{
	QMessageBox::warning(this, tr("Exception"), tr("Scale-up failed...\nInput Image is too large too scale-up..."));
	ui->goBtn->setEnabled(true);
}

void MainWindow::on_radioButtonCanny_clicked()
{
	CDerivativesController *ctrl = dynamic_cast<CDerivativesController *>(controller);
	ctrl->setMethod(derivatives_canny);
	ui->derivKsize->setEnabled(false);
	ui->xOrder->setEnabled(false);
	ui->yOrder->setEnabled(false);
	ui->doubleSpinBoxTh1->setEnabled(true);
	ui->doubleSpinBoxTh2->setEnabled(true);
}

void MainWindow::on_doubleSpinBoxTh1_valueChanged(double arg1)
{
	ui->derOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);
}

void MainWindow::on_doubleSpinBoxTh2_valueChanged(double arg1)
{
	ui->derOkBtn->setText("Submit");
	ui->processBtn->setEnabled(false);
}

void MainWindow::on_radioButtonHist_clicked()
{
	CHistController *ctrl = dynamic_cast<CHistController *>(controller);
	ctrl->setMethod(equalization);
}

void MainWindow::on_historyTab_tabCloseRequested(int index)
{

}
