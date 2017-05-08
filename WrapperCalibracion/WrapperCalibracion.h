// WrapperCalibracion.h

#pragma once
#include "CalibracionOpencvClase.h"
#include "EstereoCalibrate.h"

using namespace System;
using namespace std;

namespace WrapperCalibracion {

	public ref class ClaseIntermedia
	{
		// TODO: agregar aquí los métodos de la clase.

	public:
		ClaseIntermedia(array<System::String^>^ archivosRGB, array<System::String^>^ archivosIR,int cols,int rows,int cuadrosCol,int cuadrosFil,float tamCuadro,System::String^ camara);
		ClaseIntermedia();
		ClaseIntermedia(array<System::String^>^ archivosRGB, array<System::String^>^ archivosIR,System::String^ archivoXML_RGB,System::String^ archivoXML_IR,int cols, int rows, int cuadrosCol,int cuadrosFil,float tamCuadro,int breescalado,System::String^ camara,double cyRGB, double cxIR);
		void inicializacion(array<System::String^>^ archivosRGB, int cols, int rows,int cuadrosCol, int cuadrosFil,System::String^ nombreCal,System::String^ nombreDesdist,System::String^ camara,int breescalado,double cyRGB, double cxIR);
		void setMatricesCamara(double puntoFocalXRGB, double puntoFocalXIR,double puntoFocalYRGB, double puntoFocalYIR,double puntoXRGB, double puntoXIR, double puntoYRGB, double puntoYIR);
	private:

		array<System::String^>^ archivosRGB;
		array<System::String^>^ archivosIR;
		void conversorAopencv(int cuadrosCol,int cuadrosFil,float tamCuadro);
		void conversorAopencvCalibracion(int cuadrosCol,int cuadrosFil,System::String^ nombreCal);
		void conversorAopencvXML(int cuadrosCol,int cuadrosFil,float tamcuadro);
		void inicializacion();
		void programaPrincipal(std::vector<string> imagenesRGB,std::vector<string> imagenesIR);
		void programaPrincipal(std::vector<string> imagenesRGB,std::string nombreCal);
		void recortarImagen(Mat &imagen, Mat &imagenRecortada, int ancho, int alto,Size tamanyo);

		System::String^ archivoXML_RGB;
		System::String^ archivoXML_IR;
		System::String^ carpetaDesdist;
		bool calCompleta;

		EstereoCalibrate *calEstereo;
		int tamanyo;
		int cols;
	    int rows;
		int breescaladoClase;

		System::String^ camara;
		int anchoImagenRecortada;
		int altoImagenRecortada;

		double cyRGB,cxIR;

		 //para leer los XML
        double puntoFocalXRGB, puntoFocalXIR, puntoFocalYRGB, puntoFocalYIR;
        double puntoXRGB, puntoXIR;
        double puntoYRGB, puntoYIR;
		 		
		

		
		
		
		
	};
}
