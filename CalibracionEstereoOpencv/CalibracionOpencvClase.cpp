#include "stdafx.h"
#include "CalibracionOpencvClase.h"
#include "EstereoCalibrate.h"

using namespace std;




CalibracionOpencvClase::CalibracionOpencvClase(std::vector<string>archivosRGB,std::vector<string> archivosIR,string archivoSettings)
{
	this->archivosRGB=archivosRGB;
	this->archivosIR=archivosIR;
	this->archivoSettings=archivoSettings;

	/*EstereoCalibrate *estereo=new EstereoCalibrate(archivosRGB.capacity(),9,6,
		"ficheroResultados.xml",1920,1080,512,424,30);
*/
}


CalibracionOpencvClase::~CalibracionOpencvClase(void)
{
}



