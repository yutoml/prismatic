#include <QFileDialog>
#include <QString>
#include "prismmainwindow.h"
#include "ui_prismmainwindow.h"
#include <fstream>
#include <iostream>
#include "PRISM_entry.h"
#include <sstream>

PRISMMainWindow::PRISMMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PRISMMainWindow)
{
    ui->setupUi(this);
	this->meta = new PRISM::Metadata<double>;
    this->setWindowTitle("PRISM");
	{
		std::stringstream ss;
		ss << this->meta->interpolationFactor;
		this->ui->lineedit_f->setText(QString::fromStdString(ss.str()));
	}
	this->ui->lineedit_atomsfile->setText(QString::fromStdString(this->meta->filename_atoms));
	this->ui->lineedit_outputfile->setText(QString::fromStdString(this->meta->filename_output));
	connect(this->ui->lineedit_f,SIGNAL(editingFinished()),this,SLOT(setInterpolationFactor()));
	connect(this->ui->lineedit_atomsfile,SIGNAL(editingFinished()),this,SLOT(setFilenameAtoms_fromLineEdit()));
	connect(this->ui->lineedit_outputfile,SIGNAL(editingFinished()),this,SLOT(setFilenameOutput_fromLineEdit()));
	connect(this->ui->btn_atomsfile_browse, SIGNAL(pressed()), this, SLOT(setFilenameAtoms_fromDialog()));
	connect(this->ui->btn_outputfile_browse, SIGNAL(pressed()), this, SLOT(setFilenameOutput_fromDialog()));
	connect(this->ui->btn_go,SIGNAL(pressed()),this,SLOT(launch()));
    connect(this->ui->spinBox_numGPUs, SIGNAL(valueChanged(int)), this, SLOT(setNumGPUs(const int&)));
    connect(this->ui->spinBox_numThreads, SIGNAL(valueChanged(int)), this, SLOT(setNumThreads(const int&)));
    connect(this->ui->spinBox_numFP, SIGNAL(valueChanged(int)), this, SLOT(setNumFP(const int&)));
    connect(this->ui->lineEdit_alphaBeamMax, SIGNAL(editingFinished()), this, SLOT(setAlphaBeamMax_fromLineEdit()));
    connect(this->ui->lineedit_pixelSize, SIGNAL(editingFinished()), this, SLOT(setPixelSize_fromLineEdit()));
    connect(this->ui->lineEdit_potbound, SIGNAL(editingFinished()), this, SLOT(setPotBound_fromLineEdit()));
    connect(this->ui->lineEdit_sliceThickness, SIGNAL(editingFinished()), this, SLOT(setSliceThickness_fromLineEdit()));
    connect(this->ui->lineEdit_cellDimX, SIGNAL(editingFinished()), this, SLOT(setCellDimX_fromLineEdit()));
    connect(this->ui->lineEdit_cellDimY, SIGNAL(editingFinished()), this, SLOT(setCellDimY_fromLineEdit()));
    connect(this->ui->lineEdit_cellDimZ, SIGNAL(editingFinished()), this, SLOT(setCellDimZ_fromLineEdit()));
    connect(this->ui->lineEdit_E0, SIGNAL(editingFinished()), this, SLOT(setE0_fromLineEdit()));

}

void PRISMMainWindow::setInterpolationFactor(){
	bool flag;
	const size_t& new_f = this->ui->lineedit_f->text().toUInt(&flag);
	if (flag){
		std::cout << "Setting interpolation factor to " << new_f<< std::endl;
		this->meta->interpolationFactor = new_f;
	} else{
		std::cout << "Invalid interpolation factor input: " <<  this->ui->lineedit_f->text().toStdString() << std::endl;
	}
}

void PRISMMainWindow::setFilenameAtoms_fromDialog(){
	QString filename;
	filename = QFileDialog::getOpenFileName(this, tr("ExistingFile"), filename);
	this->ui->lineedit_atomsfile->setText(filename);
	this->setFilenameAtoms(filename.toStdString());
}

void PRISMMainWindow::setFilenameOutput_fromDialog(){
	QString filename;
	filename = QFileDialog::getOpenFileName(this, tr("AnyFile"), filename);
	this->ui->lineedit_outputfile->setText(filename);
	this->setFilenameOutput(filename.toStdString());
}

void PRISMMainWindow::setFilenameAtoms_fromLineEdit(){
	const std::string& filename = this->ui->lineedit_atomsfile->text().toStdString();
	this->setFilenameAtoms(filename);
}

void PRISMMainWindow::setFilenameOutput_fromLineEdit(){
	const std::string& filename = this->ui->lineedit_outputfile->text().toStdString();
	this->setFilenameOutput(filename);
}

void PRISMMainWindow::setFilenameAtoms(const std::string& filename){
	std::cout << "Setting atoms filename to " << filename << std::endl;
	this->meta->filename_atoms = filename;
}

void PRISMMainWindow::setFilenameOutput(const std::string& filename){
	std::cout << "Setting output filename to " << filename << std::endl;
	this->meta->filename_output = filename;
}

void PRISMMainWindow::launch(){
	std::cout << "Launching PRISM calculation with the following paramters:\n";
	std::cout << "Atoms filename = " << this->meta->filename_atoms << '\n';
	std::cout << "Output filename = " << this->meta->filename_output << '\n';
	std::cout << "Interpolation factor = " << this->meta->interpolationFactor << '\n';
	std::cout << "pixelSize[0] = " << this->meta->pixelSize[0]<< '\n';
	PRISM::PRISM_entry((*this->meta));
}

void PRISMMainWindow::setNumGPUs(const int& num){
    if (num > 0){
        this->meta->NUM_GPUS = num;
        std::cout << "Setting number of GPUs to " << num << std::endl;
    }

}

void PRISMMainWindow::setNumThreads(const int& num){
    if (num > 0){
        this->meta->NUM_THREADS = num;
        std::cout << "Setting number of CPU Threads to " << num << std::endl;
    }

}

void PRISMMainWindow::setNumFP(const int& num){
    if (num > 0){
        this->meta->numFP = num;
        std::cout << "Setting number of frozen phonon configurations to " << num << std::endl;
    }

}

void PRISMMainWindow::setPixelSize_fromLineEdit(){
    double val = this->ui->lineedit_pixelSize->text().toDouble();
    if (val > 0){
        this->meta->pixelSize = std::vector<double>{val, val};
        this->meta->realspace_pixelSize = val;
        std::cout << "Setting X/Y pixel size to " << val << std::endl;
    }
}

void PRISMMainWindow::setPotBound_fromLineEdit(){
    double val = this->ui->lineEdit_potbound->text().toDouble();
    if (val > 0){
        this->meta->potBound = val;
        std::cout << "Setting potential bound to " << val << std::endl;
    }
}

void PRISMMainWindow::setAlphaBeamMax_fromLineEdit(){
    double val = this->ui->lineEdit_alphaBeamMax->text().toDouble();
    if (val > 0){
        this->meta->alphaBeamMax = val;
        std::cout << "Setting alphaBeamMax to " << val << std::endl;
    }
}

void PRISMMainWindow::setSliceThickness_fromLineEdit(){
    double val = this->ui->lineEdit_sliceThickness->text().toDouble();
    if (val > 0){
        this->meta->sliceThickness = val;
        std::cout << "Setting sliceThickness to " << val << std::endl;
    }
}

void PRISMMainWindow::setCellDimX_fromLineEdit(){
    int val = this->ui->lineEdit_cellDimX->text().toInt();
    if (val > 0){
        this->meta->cellDim[2] = (size_t)val;
        std::cout << "Setting X cell dimension to " << val << std::endl;
    }
}

void PRISMMainWindow::setCellDimY_fromLineEdit(){
    int val = this->ui->lineEdit_cellDimY->text().toInt();
    if (val > 0){
        this->meta->cellDim[1] = (size_t)val;
        std::cout << "Setting Y cell dimension to " << val << std::endl;
    }
}

void PRISMMainWindow::setCellDimZ_fromLineEdit(){
    int val = this->ui->lineEdit_cellDimZ->text().toInt();
    if (val > 0){
        this->meta->cellDim[0] = (size_t)val;
        std::cout << "Setting Z cell dimension to " << val << std::endl;
    }
}

void PRISMMainWindow::setE0_fromLineEdit(){
    int val = this->ui->lineEdit_E0->text().toDouble();
    if (val > 0){
        this->meta->E0 = val;
        std::cout << "Setting E0 to " << val << std::endl;
    }
}




PRISMMainWindow::~PRISMMainWindow()
{
    delete ui;
	delete meta;
}