#pragma once

#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\highgui\highgui.hpp>

#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <fstream>


using namespace cv;


class CalibracionOpencvClase
{
public:
	CalibracionOpencvClase(std::vector<string>archivosRGB,std::vector<string> archivosIR,string archivoSettings);
	~CalibracionOpencvClase(void);
	

private:
	std::vector<string>archivosRGB;
	std::vector<string>archivosIR;
	string archivoSettings;
};

