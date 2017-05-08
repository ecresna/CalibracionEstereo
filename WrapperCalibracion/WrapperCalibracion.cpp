// Archivo DLL principal.

#include "stdafx.h"
#include <msclr\marshal_cppstd.h>

#include "WrapperCalibracion.h"
using namespace std;
using WrapperCalibracion::ClaseIntermedia;

//constructor que hace la calibración completa desde cero.
ClaseIntermedia::ClaseIntermedia(array<System::String^>^ archivosRGB, array<System::String^>^ archivosIR,int cols,int rows,int cuadrosCol,int cuadrosFil,float tamCuadro,System::String^ camara)
{

	this->archivosIR=archivosIR;
	this->archivosRGB=archivosRGB;

	//averiguar tamaño de imagen
	msclr::interop::marshal_context context;
	std::string nombreImagen=context.marshal_as<std::string>(archivosRGB[0]);
	Mat imagen=imread(nombreImagen,1);

	this->rows=imagen.rows;
	this->cols=imagen.cols;
	
	this->camara=camara;
	
	tamanyo=archivosRGB->Length;
    calCompleta=true;

    cv::namedWindow("Imagen RGB", CV_WINDOW_KEEPRATIO);
    cv::moveWindow("Imagen RGB",0,0);

	cv::namedWindow("Imagen IR", CV_WINDOW_KEEPRATIO);
    cv::moveWindow("Imagen IR",600,0);

	cv::resizeWindow("Imagen RGB",1000,562);
	cv::resizeWindow("Imagen IR",1000,562);

	conversorAopencv(cuadrosCol,cuadrosFil,tamCuadro);	

}

// para hacer la Calibracion Individual
void ClaseIntermedia::inicializacion(array<System::String^>^ archivosRGB, int cols, int rows,int cuadrosCol, int cuadrosFil,System::String^ nombreCal,System::String^ nombreDesdist,System::String^ camara,int breescalado,double cyRGB, double cxIR)
{
	
	this->archivosRGB=archivosRGB;
	this->camara=(camara);
	this->cyRGB=cyRGB;
	this->cxIR=cxIR;
	this->breescaladoClase=breescalado;

	if (nombreDesdist!="")
	{
		carpetaDesdist=nombreDesdist;
	}
	else {carpetaDesdist="";}
	
	tamanyo=archivosRGB->Length;
    calCompleta=true;

    cv::namedWindow("Imagen RGB", CV_WINDOW_KEEPRATIO);
    cv::moveWindow("Imagen RGB",0,0);
	
	cv::resizeWindow("Imagen RGB",1000,562);

	//averiguar tamaño de imagen
	msclr::interop::marshal_context context;
	std::string nombreImagen=context.marshal_as<std::string>(archivosRGB[0]);
	Mat imagen=imread(nombreImagen,1);
	

	this->cols=imagen.cols;
	this->rows=imagen.rows;

	conversorAopencvCalibracion(cuadrosCol,cuadrosFil,nombreCal);
}
ClaseIntermedia::ClaseIntermedia()
{	

}

//Constructor para la calibración estéreo que importa ya los archivos de calibracion
ClaseIntermedia::ClaseIntermedia(array<System::String^>^ archivosRGB, array<System::String^>^ archivosIR,System::String^ archivoXML_RGB,System::String^ archivoXML_IR,int cols,int rows,int cuadrosCol,int cuadrosFil,float tamCuadro, int breescalado,System::String^ camara,double cyRGB, double cxIR)
{

	
	this->archivosIR=archivosIR;
	this->archivosRGB=archivosRGB;

	//averiguar tamaño de imagen
	msclr::interop::marshal_context context;
	std::string nombreImagen=context.marshal_as<std::string>(archivosRGB[0]);
	Mat imagen=imread(nombreImagen,1);

	this->rows=imagen.rows;
	this->cols=imagen.cols;


	breescaladoClase=breescalado;
	this->cyRGB=cyRGB;
	this->cxIR=cxIR;

	this->camara=(camara);
	
	tamanyo=archivosRGB->Length;
	this->archivoXML_RGB=archivoXML_RGB;
	this->archivoXML_IR=archivoXML_IR;
	
	calCompleta=false;

	cv::namedWindow("Imagen RGB", CV_WINDOW_KEEPRATIO);
    cv::moveWindow("Imagen RGB",0,0);

	cv::namedWindow("Imagen IR", CV_WINDOW_KEEPRATIO);
    cv::moveWindow("Imagen IR",600,0);

	cv::resizeWindow("Imagen RGB",1000,562);
	cv::resizeWindow("Imagen IR",1000,562);

	//función para hacer la calibración usando calibraciones ya hechas
	conversorAopencvXML(cuadrosCol,cuadrosFil,tamCuadro);

}
void ClaseIntermedia::setMatricesCamara(double puntoFocalXRGB, double puntoFocalXIR,double puntoFocalYRGB, double puntoFocalYIR,double puntoXRGB, double puntoXIR, double puntoYRGB, double puntoYIR)
{
	 //para leer los XML
         this->puntoFocalXRGB=puntoFocalXRGB;
		 this->puntoFocalXIR=puntoFocalXIR;
		 this->puntoFocalYRGB=puntoFocalYRGB;
		 this->puntoFocalYIR=puntoFocalYIR;
         this->puntoXRGB=puntoXRGB;
		 this->puntoXIR=puntoXIR;
         this->puntoYRGB=puntoYRGB;
		 this->puntoYIR=puntoYIR;
}

void ClaseIntermedia::conversorAopencvCalibracion(int cuadrosCol, int cuadrosFil,System::String^ nombreCal)
{
	

	msclr::interop::marshal_context context;

	std::vector<string> archivosRGBunmanaged;//vectores no manejados
	

	for (int i=0;i<tamanyo;i++)
	{
		std::string standardStringRGB = context.marshal_as<std::string>(archivosRGB[i]);
		archivosRGBunmanaged.push_back(standardStringRGB);
		
	}
	std::string nombreCalibracion=context.marshal_as<std::string>(nombreCal);
	std::string stringcarpetaDesdist = context.marshal_as<std::string>(carpetaDesdist);

	calEstereo=new EstereoCalibrate(tamanyo,cuadrosCol,cuadrosFil,cols,rows,18.5,stringcarpetaDesdist);

	/*archivoPruebaIR.release();
	archivoPruebaRGB.release();*/
	std::string nombreCam=context.marshal_as<std::string>(this->camara);

	calEstereo->setnumSerial(1,nombreCam);
	calEstereo->setnumSerial(2,nombreCam);
	inicializacion();

	programaPrincipal(archivosRGBunmanaged,nombreCalibracion);

	

}

void ClaseIntermedia::conversorAopencv(int cuadrosCol,int cuadrosFil,float tamCuadro)
{
	

	msclr::interop::marshal_context context;

	std::vector<string> archivosRGBunmanaged;//vectores no manejados
	std::vector<string> archivosIRunmanaged;//vectores no manejados

	for (int i=0;i<tamanyo;i++)
	{
		std::string standardStringRGB = context.marshal_as<std::string>(archivosRGB[i]);
		std::string standardStringIR = context.marshal_as<std::string>(archivosIR[i]);
		archivosRGBunmanaged.push_back(standardStringRGB);
		archivosIRunmanaged.push_back(standardStringIR);
	}
	//calculamos el tamaño

	/*Mat archivoPruebaRGB=imread(archivosRGBunmanaged[0]);
	Mat archivoPruebaIR=imread(archivosIRunmanaged[0]);*/

	
	calEstereo=new EstereoCalibrate(tamanyo,cuadrosCol,cuadrosFil,
		"ficheroResultados.xml",this->cols,this->rows,tamCuadro);

	/*archivoPruebaIR.release();
	archivoPruebaRGB.release();*/

	inicializacion();
	std::string nombreCam=context.marshal_as<std::string>(this->camara);

	calEstereo->setnumSerial(1,nombreCam);
	calEstereo->setnumSerial(2,nombreCam);
	programaPrincipal(archivosRGBunmanaged,archivosIRunmanaged);

	

}

//función para hacer la calibración Estéreo usando calibraciones ya hechas
void ClaseIntermedia::conversorAopencvXML(int cuadrosCol,int cuadrosFil,float tamCuadro)
{
	
	
	msclr::interop::marshal_context context;
	std::vector<string> archivosRGBunmanaged;//vectores no manejados
	std::vector<string> archivosIRunmanaged;//vectores no manejados

	std::string archivoXML_RGB=context.marshal_as<std::string>(this->archivoXML_RGB);
	std::string archivoXML_IR=context.marshal_as<std::string>(this->archivoXML_IR);

	calEstereo=new EstereoCalibrate(tamanyo,cuadrosCol,cuadrosFil,
		"ficheroResultados.xml",this->cols,1080,tamCuadro);

	// carga archivos 
	calEstereo->cargaFichero(archivoXML_RGB,archivoXML_IR);

	for (int i=0;i<tamanyo;i++)
	{
		std::string standardStringRGB = context.marshal_as<std::string>(archivosRGB[i]);
		std::string standardStringIR = context.marshal_as<std::string>(archivosIR[i]);
		archivosRGBunmanaged.push_back(standardStringRGB);
		archivosIRunmanaged.push_back(standardStringIR);
	}

	string camaraS=context.marshal_as<std::string>(this->camara);
	
	
	

	
	inicializacion();

	//aqui se cambia la matriz de calibración
	
	calEstereo->setMatrices(archivoXML_RGB,archivoXML_IR);//establece las matrices CM1,CM2

	programaPrincipal(archivosRGBunmanaged,archivosIRunmanaged);

	try{
	//	calEstereo->calibracionCamara(imagenRGB_BN,imagenIR_BN);
		calEstereo->opcionCalibrado=2;
		calEstereo->calibracionEstereo(Size(anchoImagenRecortada,1080));
		
		calEstereo->salvarCalibracion("C:\\BOOTH\\Calib\\Geo\\Estereo\\calibracionEstereo_"+camaraS+".xml",true,0,"Estereo");
		Mat imagenRGB_reescalada,imagenRGB_recortada;
	   Mat imagenIR_recortada=Mat::zeros(Size(1920,1080),CV_8UC1);
	    Mat imagenIR_reescalada;
		
		double Ty=puntoFocalYRGB*(rows/puntoFocalYIR);
		double Tx=puntoFocalXRGB*(cols/puntoFocalXIR);

		double cy=puntoYIR*(Ty/rows);
		double cx=puntoXIR*(Tx/cols);

		double desfasey=puntoYRGB/cy;
		double desfasex=Tx/2-cx;

		//entra aqui si hay un reescalado
		

			for (int i=0;i<tamanyo;i++)
			{
				if (breescaladoClase==1)
				{
					std::string standardStringRGB = context.marshal_as<std::string>(archivosRGB[i]);
					std::string standardStringIR = context.marshal_as<std::string>(archivosIR[i]);
					Mat imagenRGB=imread(standardStringRGB,0);
					Mat imagenIR=imread(standardStringIR,0);

					//flip
					/*cv::flip(imagenRGB,imagenRGB,1);
					cv::flip(imagenIR,imagenIR,1);*/

			

					// Reescalado y recorte automáticos
					///////////////////////////////
					// Para IR
					///////////////////////////////
				/*	float indiceReescalado=(double)(this->cols)/(double)(imagenIR.cols);
					int recorte=abs(this->rows-(imagenIR.rows*indiceReescalado))/2;

					resize(imagenIR,imagenIR_reescalada,Size(),indiceReescalado,indiceReescalado,INTER_CUBIC);
					recortarImagen(imagenIR_reescalada,imagenIR_recortada,0,recorte,Size(this->cols,this->rows));
	*/
						//AUTO NACHO

					

					double ratioNew=(1080.0/(double)424);
				double rat=(double)512*ratioNew;
				double R1=(double)(1920.0f-rat);
				double R2=R1/(double)2.0f;
				int x=(int)R2;
				int y=0;

				resize(imagenIR,imagenIR_reescalada,Size(),ratioNew,ratioNew,INTER_LANCZOS4);				
				imagenIR_reescalada.copyTo(imagenIR_recortada(cv::Rect(x,y,imagenIR_reescalada.cols, imagenIR_reescalada.rows)));

				//	resize(imagenIR,imagenIR_recortada,Size(cols,rows),0,0,INTER_LANCZOS4);
				
				//	recortarImagen(imagenIR,imagenIR_recortada,0,(int)desfaseyNacho,Size(cols,1080));

				
					/////////////////////////////////
					// PAra RGB
					////////////////////////////////////
						
				/*	indiceReescalado=((double) (this->rows)) / ((double)imagenRGB.rows);
					double drecorte=((double)imagenRGB.cols*indiceReescalado);
					int recorte2=(int)( (drecorte-(double)(this->cols))/2);
					resize(imagenRGB,imagenRGB_reescalada,Size(),indiceReescalado,indiceReescalado,INTER_CUBIC);
					recortarImagen(imagenRGB_reescalada,imagenRGB_recortada,recorte2,0,Size(this->cols,this->rows));
	*/

					//AUTO NACHO
					//float cosa=(R-(R*ratio))*2;

					   //desfase en x	
				      double CXid=(double)((double)cols/512.0)*cxIR;
					  double desfase=((cols/2)-CXid);					  						
					  double desfaseX=(1920-cols)/2-desfase;

			//		resize(imagenRGB,imagenRGB_reescalada,Size(),1,1,INTER_CUBIC);
			//		recortarImagen(imagenRGB,imagenRGB_recortada,(int)desfaseX,0,Size(cols,1080));


				//	calEstereo->comprobacion(imagenRGB_reescalada,imagenIR_reescalada);
					calEstereo->comprobacion(imagenRGB,imagenIR_recortada,i);
					waitKey(50);
				}
				else //no se reescala
				{
					std::string standardStringRGB = context.marshal_as<std::string>(archivosRGB[i]);
					std::string standardStringIR = context.marshal_as<std::string>(archivosIR[i]);
					Mat imagenRGB=imread(standardStringRGB,0);
					Mat imagenIR=imread(standardStringIR,0);

					//flip
					/*cv::flip(imagenRGB,imagenRGB,1);
					cv::flip(imagenIR,imagenIR,1);*/
					calEstereo->comprobacion(imagenRGB,imagenIR,i);

				}
			}
	}
	catch(cv::Exception& excepcion)
	{
			throw gcnew System::Exception("Fallo al hacer la calibracion");
		
	}
	
}

	

	



void ClaseIntermedia::inicializacion()
{
	//calEstereo->setTamCuadDamero(30);
	calEstereo->found1=false;
	calEstereo->found2=false;
	calEstereo->exitos=0;
	calEstereo->success=0;
	calEstereo->bsensibilidadEsquinas=true;
	

}

//para la calibracion única
void ClaseIntermedia::programaPrincipal(std::vector<string> imagenesRGB,std::string nombreCal)
{
	
	
	Mat imagen_BN=Mat(Size(this->cols,this->rows),CV_8UC3);
	Mat imagen_mejorada=Mat(Size(this->cols,this->rows),CV_8UC1);
	Mat imagen_reescalada;	

	vector<int> indicesBuenos,indicesResultado;
	Mat imagen;
	Mat imagen_recortada=Mat::zeros(Size(1920,1080),CV_8UC3);

	msclr::interop::marshal_context context;
	string camaraS=context.marshal_as<std::string>(this->camara);

	calEstereo->setTipoCal(nombreCal);//tipo de calibracion (RGB/IR)

	//AUTO NACHO

	
	
			double ratio=(double)cols/(double)rows;
			int R=1080-rows;
			//float desfaseY=(R-(R*ratio))*2;
			float desfaseY=(R-(R*ratio))*2;

			//desfase en x	
			double Ty=rows;
			double Tx=cols;

			double cy=puntoYIR*(Ty/424);
			double cx=puntoXIR*(Tx/512);

			double desfasey=puntoYRGB/cy;
			double desfasex=Tx/2-cx;

			calEstereo->setDesfasey(desfasey);
			calEstereo->setDesfasex(desfasex);
			calEstereo->setAltoImRecortada(1080);
			calEstereo->setAnchoImRecortada(cols);

			calEstereo->setAltoImRecortadaBruto(rows);
			calEstereo->setAnchoImRecortadaBruto(cols);
	
	

	//bucle principal
	for (int i=0;i<tamanyo;i++)
	{
		imagen=imread(imagenesRGB[i],1);
		
			   
		
		// Reescalado y recorte automáticos

		// SI HAY REESCALADO
		if (breescaladoClase==1)
		{
			// EN EL CASO DE IR
			if (imagen.cols==512)
			{
			/*	float indiceReescalado=(double)(this->cols)/(double)(imagen.cols);
				int recorte=abs(this->rows-(imagen.rows*indiceReescalado))/2;

				resize(imagen,imagen_reescalada,Size(),indiceReescalado,indiceReescalado,INTER_CUBIC);
				recortarImagen(imagen_reescalada,imagen_recortada,0,recorte,Size(this->cols,this->rows));
	*/
				//IR
				//AUTO NACHO

				double ratioNew=(1080.0/(double)rows);
				double rat=(double)cols*ratioNew;
				double R1=(double)(1920.0f-rat);
				double R2=R1/(double)2.0f;
				int x=(int)R2;
				int y=0;
				
				resize(imagen,imagen_reescalada,Size(),ratioNew,ratioNew,INTER_LANCZOS4);				
				imagen_reescalada.copyTo(imagen_recortada(cv::Rect(x,y,imagen_reescalada.cols, imagen_reescalada.rows)));

				//recortarImagen(imagen_reescalada,imagen_recortada,0,(int)( funcion ),Size(cols,1080));
				anchoImagenRecortada=imagen_recortada.size().width;
				altoImagenRecortada=imagen_recortada.size().height;
			
			}
			// EN EL CASO DE RGB
			else
			{
				/*double indiceReescalado=0;
			
				indiceReescalado=((double) (this->rows)) / ((double)imagen.rows);
				double recorte=((double)imagen.cols*indiceReescalado);
				int recorte2=(int)( (recorte-(double)(this->cols))/2);
				resize(imagen,imagen_reescalada,Size(),indiceReescalado,indiceReescalado,INTER_CUBIC);
				recortarImagen(imagen_reescalada,imagen_recortada,recorte2,0,Size(this->cols,this->rows));
	*/
				//RGB
				//AUTO NACHO
				imagen.copyTo(imagen_recortada);
			//	resize(imagen,imagen_recortada,Size(),1,1,INTER_CUBIC);//no se reeescala
				//recortarImagen(imagen_reescalada,imagen_recortada,(int)desfaseX,0,Size(cols,1080));


			}
		}
		// SI NO HAY REESCALADO
		else
		{
			//flip
		   // cv::flip(imagen,imagen,1);
			imagen.copyTo(imagen_recortada); // se copia sin hacer ninguna operacion solo para mantener los nombres
		}
			
	//	calEstereo->ImageToGrey(imagen_reescalada,imagen_BN);
		calEstereo->ImageToGrey(imagen_recortada,imagen_BN);
	//	equalizeHist(imagen_BN,imagen_mejorada);

	//	bool encontradoRGB=calEstereo->EncuentraEsquinas(imagen_mejorada,imagen_reescalada);//
		try
		{
			bool encontradoRGB=calEstereo->EncuentraEsquinas(imagen_BN,imagen_recortada);//
		
		
			if (encontradoRGB)
			{
				//calEstereo->CalculaSubPix(imagen_BN);
				calEstereo->almacenaEsquinas(false);//con false solo almacena 1 esquina
				indicesBuenos.push_back(i);
				indicesResultado.push_back(1);
			
				cout<<"imagen RGB: "<<imagenesRGB[i]<<" Correcta "<<endl;			
			}
			else
			{	
				indicesResultado.push_back(0);
				cout<<"FALLO en "<<imagenesRGB[i]<<endl;
			//	imagenesRGB.erase(imagenesRGB.begin()+i);
			//	this->tamanyo--;
			}      
		}
		catch(cv::Exception& excepcion)
		{
			throw gcnew System::Exception("Fallo al hacer la calibracion: " );
		
		}
		
	//	imshow("Imagen RGB",imagen_reescalada);//representa la imagen en color
		imshow("Imagen RGB",imagen_recortada);//representa la imagen en color
	//	imshow("Imagen mejorada",imagen_BN);//representa la imagen en color
		waitKey(10);
		
	}
	if (indicesBuenos.size()>0)
	{
	if (calCompleta)
	{
		
		try{
			calEstereo->calibracionCamara(imagen_BN);
			calEstereo->opcionCalibrado=2;
			if (breescaladoClase==0)
			{
				if (nombreCal=="Cal_RGB"){			
				calEstereo->salvarCalibracion("C:\\BOOTH\\Calib\\Geo\\RGB\\calibracion"+nombreCal+"_"+camaraS+"_normal.xml",false,0,nombreCal);//no es estereo
				}
				else{
					calEstereo->salvarCalibracion("C:\\BOOTH\\Calib\\Geo\\IR\\calibracion"+nombreCal+"_"+camaraS+"_normal.xml",false,0,nombreCal);//no es estereo

				}
			}
			else
			{
				if (nombreCal=="Cal_RGB"){			
				calEstereo->salvarCalibracion("C:\\BOOTH\\Calib\\Geo\\RGB\\calibracion"+nombreCal+"_"+camaraS+"_reescalada.xml",false,1,nombreCal);//no es estereo
				}
				else{
					calEstereo->salvarCalibracion("C:\\BOOTH\\Calib\\Geo\\IR\\calibracion"+nombreCal+"_"+camaraS+"_reescalada.xml",false,1,nombreCal);//no es estereo

				}

			}
			msclr::interop::marshal_context context;

			
			}
		catch(cv::Exception& excepcion)
		{
				throw gcnew System::Exception("Fallo al hacer la calibracion");
		
		}
		
	}
		
	calEstereo->comprobacionGraficaResultado(Size(imagen_recortada.cols,imagen_recortada.rows),imagenesRGB,indicesBuenos,indicesResultado,cyRGB,breescaladoClase);
	}
	else
	{
		throw gcnew System::Exception("NO hay esquinas suficientes");
	}
}

// programa para Calibración Estéreo 
void ClaseIntermedia::programaPrincipal(std::vector<string> imagenesRGB,std::vector<string> imagenesIR)
{
	Mat imagenMuestraIR=imread(imagenesIR[0],0);
	Mat imagenMuestraRGB=imread(imagenesRGB[0],1);
	Mat imagenIR_BN=cv::Mat(imagenMuestraIR.size(),imagenMuestraIR.type());
	Mat imagenRGB_BN=cv::Mat(imagenMuestraRGB.size(),imagenMuestraRGB.type());
	

	vector<int> indicesBuenos, indicesResultado;
	

	Mat imagenIR_recortada=Mat::zeros(Size(1920,1080),CV_8UC3);
	
	Mat imagenRGB,imagenRGB_reescalada,imagenRGB_recortada;
	Mat imagenIR,imagenIR_reescalada;
	Mat imagenMejoradaRGB,imagenMejoradaIR;

	bool encuentraRGB;

		//AUTO NACHO

			double ratio=cols*2/rows;
			int R=rows-1080;
			float desfase=(R-(R*ratio))*2;
			calEstereo->setDesfasey(desfase);
			calEstereo->setDesfasex(0);
			calEstereo->setAltoImRecortada(1080);
			calEstereo->setAnchoImRecortada(cols);

	//bucle principal
	for (int i=0;i<tamanyo;i++)
	{
		imagenRGB=imread(imagenesRGB[i],1);
		imagenIR=imread(imagenesIR[i],1);
		
				
		if (breescaladoClase==1)
		{
		// Reescalado y recorte automáticos
		    ///////////////////////////////
		    // Para IR
		    ///////////////////////////////
		/*	float indiceReescalado=(double)(this->cols)/(double)(imagenIR.cols);
			int recorte=abs(this->rows-(imagenIR.rows*indiceReescalado))/2;

			resize(imagenIR,imagenIR_reescalada,Size(),indiceReescalado,indiceReescalado,INTER_CUBIC);
			recortarImagen(imagenIR_reescalada,imagenIR_recortada,0,recorte,Size(this->cols,this->rows));
*/

				//AUTO NACHO

		double ratioNew=(1080.0/(double)424);
				double rat=(double)512*ratioNew;
				double R1=(double)(1920.0f-rat);
				double R2=R1/(double)2.0f;
				int x=(int)R2;
				int y=0;

				resize(imagenIR,imagenIR_reescalada,Size(),ratioNew,ratioNew,INTER_LANCZOS4);				
				imagenIR_reescalada.copyTo(imagenIR_recortada(cv::Rect(x,y,imagenIR_reescalada.cols, imagenIR_reescalada.rows)));

		//	cv::resize(imagenIR,imagenIR_reescalada,Size(cols,rows),0,0,INTER_CUBIC);
				
		//	recortarImagen(imagenIR,imagenIR_recortada, 0,(int)calEstereo->desfasey,Size(cols,1080));

			anchoImagenRecortada=imagenIR_recortada.size().width;
			altoImagenRecortada=imagenIR_recortada.size().height;

		
		    /////////////////////////////////
		    // PAra RGB
			////////////////////////////////////
						
			/*indiceReescalado=((double) (this->rows)) / ((double)imagenRGB.rows);
			double drecorte=((double)imagenRGB.cols*indiceReescalado);
			int recorte2=(int)( (drecorte-(double)(this->cols))/2);
			resize(imagenRGB,imagenRGB_reescalada,Size(),indiceReescalado,indiceReescalado,INTER_CUBIC);
			recortarImagen(imagenRGB_reescalada,imagenRGB_recortada,recorte2,0,Size(this->cols,this->rows));
*/
			//AUTO NACHO
			float cosa=(R-(R*ratio))*2;

					  //desfase en x	
				      double CXid=(double)((double)cols/512.0)*cxIR;
					  double desfase=((cols/2)-CXid);					  						
					  double desfaseX=(1920-cols)/2-desfase;


		//		resize(imagenRGB,imagenRGB_reescalada,Size(),1,1,INTER_CUBIC);
		//		recortarImagen(imagenRGB,imagenRGB_recortada,(int)desfaseX,0,Size(cols,1080));
				imagenRGB.copyTo(imagenRGB_recortada);

			//recortarImagen(imagen,imagen_reescalada);
			//cv::resize(imagen,imagen_reescalada,Size(this->cols,this->rows),0,0,INTER_CUBIC);
		
		
		//transformar RGB en BN y equalizar
		calEstereo->ImageToGrey(imagenRGB_recortada,imagenRGB_BN);
		//transformar IR en BN y equalizar	
		calEstereo->ImageToGrey(imagenIR_recortada,imagenIR_BN);
		}
		else
		{
			anchoImagenRecortada=imagenRGB.size().width;
			altoImagenRecortada=imagenRGB.size().height;

			//transformar RGB en BN y equalizar
			calEstereo->ImageToGrey(imagenRGB,imagenRGB_BN);
			//transformar IR en BN y equalizar	
			calEstereo->ImageToGrey(imagenIR,imagenIR_BN);
		}

	//	equalizeHist(imagenRGB_BN,imagenMejoradaRGB);		
	//	equalizeHist(imagenIR_BN,imagenMejoradaIR);

		/*calEstereo->ImageToGrey(imagenIR,imagenIR_BN);
		equalizeHist(imagenIR_BN,imagenMejoradaIR);*/

		
		if (breescaladoClase==1){
			encuentraRGB=calEstereo->EncuentraEsquinas(imagenRGB_BN,imagenRGB_recortada,imagenIR_BN,imagenIR_recortada);}
		else{
			encuentraRGB=calEstereo->EncuentraEsquinas(imagenRGB_BN,imagenRGB,imagenIR_BN,imagenIR);}
		//bool encuentraRGB=calEstereo->EncuentraEsquinas(imagenMejoradaRGB,imagenRGB,imagenMejoradaIR,imagenIR);
		

		if (encuentraRGB)
		{
		//	calEstereo->CalculaSubPix(imagenMejoradaRGB);
		//	calEstereo->CalculaSubPix(imagenMejoradaIR);
			calEstereo->almacenaEsquinas(true);
			indicesBuenos.push_back(i);
			indicesResultado.push_back(1);
			cout<<"OK "<<i<<endl;
			cout<<"imagen RGB: "<<imagenesRGB[i]<<endl;
			cout<<"imagen IR: "<<imagenesIR[i]<<endl;
		}
		else
		{
			indicesResultado.push_back(0);
			cout<<"FALLOO!!! "<<imagenesIR[i]<<endl;
			cout<<"FALLOO!!! "<<imagenesRGB[i]<<endl;
		//	imagenesIR.erase(imagenesIR.begin()+i);
		//	imagenesRGB.erase(imagenesRGB.begin()+i);
		//	this->tamanyo--;

		}      
		if (breescaladoClase==1)
		{
			imshow("Imagen RGB",imagenRGB_recortada);
			waitKey(10);
			imshow("Imagen IR",imagenIR_recortada);
			waitKey(10);
		}
		else{
				imshow("Imagen RGB",imagenRGB);
			waitKey(10);
			imshow("Imagen IR",imagenIR);
			waitKey(10);

		}

	}
	if (calCompleta)
	{
	
		try{
			msclr::interop::marshal_context context;
	        string camaraS=context.marshal_as<std::string>(this->camara);
	
		//	calEstereo->calibracionCamara(imagenRGB_BN);
		//	calEstereo->calibracionCamara(imagenIR_BN);
			calEstereo->opcionCalibrado=1;
			calEstereo->calibracionEstereo(calEstereo->tamanyoMatrizRGB);
			calEstereo->salvarCalibracion("C:\\BOOTH\\Calib\\Geo\\Estereo\\calibracionEstereo_"+ camaraS+".xml",true,0,"estereo"); //es estéreo
			

			for (int i=0;i<tamanyo;i++)
			{
				std::string standardStringRGB = context.marshal_as<std::string>(archivosRGB[i]);
				std::string standardStringIR = context.marshal_as<std::string>(archivosIR[i]);
				Mat imagenRGB=imread(standardStringRGB,0);
				Mat imagenIR=imread(standardStringIR,0);

				//flip
				/*cv::flip(imagenRGB,imagenRGB,1);
				cv::flip(imagenIR,imagenIR,1);*/
				

				//se cambia el tamaño:
				//se reescala para la RGB
		
				Mat imagenRGB_rees;
				Mat imagenIR_rees;
				//cv::resize(imagenRGB,imagenRGB_rees,Size(this->cols,this->rows),0,0,INTER_CUBIC);
///ojo!!! hay que acabarlo!!!	recortarImagen(imagenRGB,imagenRGB_rees);

				// se reescala para la IR
				double ratioNew=(1080.0/(double)rows);
				double rat=(double)cols*ratioNew;
				double R1=(double)(1920.0f-rat);
				double R2=R1/(double)2.0f;
				int x=(int)R2;
				int y=0;

				resize(imagenIR,imagenIR_rees,Size(),ratioNew,ratioNew,INTER_LANCZOS4);				
				imagenIR_rees.copyTo(imagenIR_recortada(cv::Rect(x,y,imagenIR_rees.cols, imagenIR_rees.rows)));

			//	cv::resize(imagenIR,imagenIR_rees,Size(this->cols,this->rows),0,0,INTER_CUBIC);


				//	calEstereo->comprobacion(imagenRGB_rees,imagenIR_rees);
					calEstereo->comprobacion(imagenRGB,imagenIR,i);
					waitKey(1000);
					/*char c = (char)waitKey();
					if( c == 27 || c == 'q' || c == 'Q' )
					break;*/
			}
		}
		catch(cv::Exception& excepcion)
		{
				throw gcnew System::Exception("Fallo al hacer la calibracion");
		
		}
	//	calEstereo->salvarCalibracion("aquimismo.xml");
	}
		
 //	calEstereo->comprobacionGraficaResultado(Size(1304,1080),imagenesRGB,indicesBuenos);
	
}
void ClaseIntermedia::recortarImagen(Mat &imagen, Mat &imagenRecortada, int ancho, int alto,Size tamanyo)
{
	cv::Rect rect(ancho,alto,tamanyo.width,tamanyo.height);

	
	imagenRecortada = imagen(rect);
}



