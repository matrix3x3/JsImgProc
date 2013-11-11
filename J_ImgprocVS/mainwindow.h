#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modules.h"
#include <qthread.h>
#include "imagestorage.h"

class WorkerThread : public QThread
{
	Q_OBJECT
		void run() Q_DECL_OVERRIDE {
			//QString result;
			/* expensive or blocking operation  */
			CSisrController* ctrl = CSisrController::getInstance();
			try{
				ctrl->learnDic();
				emit resultReady(/*result*/);
			}
			catch(cv::Exception e)
			{
				emit exceptionOccurs();
			}
	}
signals:
	void resultReady(/*const QString &s*/);
	void exceptionOccurs();
};

class LoadConfThread : public QThread
{
	Q_OBJECT
		void run() Q_DECL_OVERRIDE {
			//QString result;
			/* expensive or blocking operation  */
			CSisrController* ctrl = CSisrController::getInstance();
			try{
				ctrl->loadConfs();
				emit resultReady(/*result*/);
			}
			catch(cv::Exception e)
			{
				emit exceptionOccurs();
			}
	}
signals:
	void resultReady(/*const QString &s*/);
	void exceptionOccurs();
};

class ScaleUpThread : public QThread
{
	Q_OBJECT
		void run() Q_DECL_OVERRIDE {
			//QString result;
			/* expensive or blocking operation  */
			try
			{
				ImageStorage* imgstore = ImageStorage::getInstance();
				std::pair<const std::string, Ui::SrcAndRes>* current = imgstore->getCurrent();
				std::string name = current->first;
				CSisrController* ctrl = CSisrController::getInstance();
				ctrl->process();
				emit resultReady(name);
			}
			catch(CSisr::SisrExcept e)
			{
				emit exceptionOccurs();
			}
			catch(cv::Exception e)
			{
				std::string s = e.what();
				emit scaleUpImageTooLarge();
			}
			catch(...)
			{
				emit exceptionOccurs();
			}
	}
signals:
	void resultReady(std::string name);
	void exceptionOccurs();
	void scaleUpImageTooLarge();
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_radioButton_1_clicked();

    void on_radioButton_1_toggled(bool checked);

    void on_radioButton_1_pressed();

    void on_actionOpen_triggered();

    void on_actionClose_triggered();

    void on_actionSave_triggered();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

    void on_radioButton_5_clicked();

    void on_radioButton_6_clicked();

    void on_radioButton_7_clicked();

    void on_radioButton_8_clicked();

    void on_radioButton_9_clicked();

    void on_radioButton_10_clicked();

    void on_radioButton_13_clicked();

    void on_radioButton_15_clicked();

    void on_radioButton_14_clicked();

    void on_radioButton_16_clicked();

    void on_spinBox_6_editingFinished();

    void on_spinBox_6_valueChanged(const QString &arg1);

    void on_spinBox_6_valueChanged(int arg1);

    void on_radioButtonSobel_clicked();

    void on_radioButtonLaplacian_clicked();

    void on_spinBox_editingFinished();

    void on_spinBox_valueChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_editingFinished();

    void on_spinBox_2_valueChanged(const QString &arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_spinBox_3_editingFinished();

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_3_valueChanged(const QString &arg1);

    void on_binaryB_clicked();

    void on_binaryinvB_clicked();

    void on_truncB_clicked();

    void on_tozeroB_clicked();

    void on_tozeroinvB_clicked();

    void on_lineEdit_editingFinished();

    void on_lineEdit_returnPressed();

    void on_lineEdit_textEdited(const QString &arg1);

    void on_lineEdit_2_editingFinished();

    void on_lineEdit_2_returnPressed();

    void on_lineEdit_2_textEdited(const QString &arg1);

    void on_radioButtonSobel_2_clicked();

    void on_radioButtonLaplacian_2_clicked();

    void on_spinBox_4_editingFinished();

    void on_spinBox_4_valueChanged(const QString &arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_radioButtonSobel_3_clicked();

    void on_radioButtonLaplacian_3_clicked();

    void on_radioButton_clicked();

    void on_radioButton_11_clicked();

    void on_radioButton_12_clicked();

    void on_spinBox_5_editingFinished();

    void on_spinBox_5_valueChanged(const QString &arg1);

    void on_spinBox_5_valueChanged(int arg1);

    void on_processBtn_clicked();

    void on_previousBtn_clicked();

    void on_nextBtn_clicked();

    void on_imageTab_currentChanged(int index);

    void on_imageTab_tabCloseRequested(int index);

    void on_functionBox_currentChanged(int index);

	/* my slots */
	void updateImgInfo();
	void setUpHistoryTab();
	void updateSisrButtons();
	void sisrExceptionOccurs();
	void loadConfException();
	void updateGoBtn();
	void getResultofScaleUp(std::string filename);
	void scaleUpException();
	void scaleUpImgTooLargeException();

    void on_blurOkBtn_clicked();

    void on_actionView_Histories_triggered();

    void on_hCloseBtn_clicked();

    void on_hPrevBtn_clicked();

    void on_hNextBtn_clicked();

    void on_hSaveBtn_clicked();

    void on_hRefreshBtn_clicked();

    void on_actionClose_Histories_Window_triggered();

    void on_xOrder_valueChanged(int arg1);

    void on_yOrder_valueChanged(int arg1);

    void on_derivKsize_valueChanged(int arg1);

    void on_derOkBtn_clicked();

    void on_spinBoxMaxVal_valueChanged(int arg1);

    void on_spinBoxThresh_valueChanged(int arg1);

    void on_threshOkBtn_clicked();

    void on_pyrGausianB_clicked();

    void on_pyrLaplacianB_clicked();

    void on_pyrimadOkBtn_clicked();

    void on_spinBoxPyrimad_valueChanged(int arg1);

    void on_pyrResizeB_clicked();

    void on_nearestB_clicked();

    void on_linearB_clicked();

    void on_areaB_clicked();

    void on_cubicB_clicked();

    void on_lanczosB_clicked();

    void on_doubleSpinBoxXscale_valueChanged(double arg1);

    void on_doubleSpinBoxYscale_valueChanged(double arg1);

    void on_erodingB_clicked();

    void on_dilatingB_clicked();

    void on_radioButtonRect_clicked();

    void on_radioButtonCross_clicked();

    void on_radioButtonEllipse_clicked();

    void on_spinBoxMorph_valueChanged(int arg1);

    void on_morphOkBtn_clicked();

    void on_learnDicBtn_clicked();

    void on_scaleUpBtn_clicked();

    void on_sisrSubmitBtn_clicked();

    void on_spinBoxSisrScale_valueChanged(int arg1);

    void on_spinBoxSisrWindow_valueChanged(int arg1);

    void on_spinBoxSisrOverlap_valueChanged(int arg1);

    void on_spinBoxSisrSparsity_valueChanged(int arg1);

    void on_spinBoxSisrIteration_valueChanged(int arg1);

    void on_spinBoxSisrDictsize_valueChanged(int arg1);

    void on_loadPicsBtn_clicked();

    void on_learnDicOkBtn_clicked();

    void on_saveCfBtn_clicked();

    void on_loadCfBtn_clicked();

    void on_goBtn_clicked();

    void on_showDiffBtn_clicked();

    void on_radioButtonCanny_clicked();

    void on_doubleSpinBoxTh1_valueChanged(double arg1);

    void on_doubleSpinBoxTh2_valueChanged(double arg1);

    void on_radioButtonHist_clicked();

    void on_historyTab_tabCloseRequested(int index);

private:
    Ui::MainWindow *ui;
	IController *controller;

	//void MyObject::startWorkInAThread()
	//{
	//	WorkerThread *workerThread = new WorkerThread(this);
	//	connect(workerThread, &WorkerThread::resultReady, this, &MyObject::handleResults);
	//	connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
	//	workerThread->start();
	//}
};

#endif // MAINWINDOW_H
