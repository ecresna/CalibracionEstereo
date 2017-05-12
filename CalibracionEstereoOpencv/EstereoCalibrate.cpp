#include "stdafx.h"


#include "EstereoCalibrate.h"
#include <iostream>

using namespace cv;
using namespace std;

//tipos de matrices 
#define MATRIZ_BN CV_8UC1
#define MATRIZ_COLOR CV_8UC3
// modos de video de la uEye
#define MODOUEYE8BN IS_CM_MONO8 
#define MODOUEYE8RGB IS_CM_BGR8_PACKED
//total de cámaras
#define NUMTOTALCAMS 2
//tipo de formato de video
#define FORMATOVIDEO 25 //cámara color

//tamaños 
#define COLORANCHO 1024 
#define COLORALTO   1280

#define VERTICAL false // se muestran la imágenes en vertical
#define PARESTEREO true // se muestran las imágenes estéreo

#define MANUAL true
#define TIPOCAMARA 0 //HORUS
//#define TIPOCAMARA 1 //WALLY

void recortarImagen(Mat &imagen, Mat &imagenRecortada, int ancho, int alto,Size tamanyo);

EstereoCalibrate::EstereoCalibrate(int numBoards,int board_w,int board_h,char* ficheroGuardaResultados,int colorancho,int coloralto,float tamCuad)
{
   
	this->colorancho=colorancho;
	this->coloralto=coloralto;
    int board_n = board_w*board_h;
	board_sz=Size(board_w,board_h);
    this->numBoards=numBoards;
    carga=false;
    cargaPuntos=false;
	tamanyoMatrizRGB=Size(colorancho,coloralto);
	tamanyoMatrizIR=Size(colorancho,coloralto);
    
    fguardaResultados=ficheroGuardaResultados;
    //tamaño del cuadro del damero de calbración (por defecto) en mm
    tamcuadDamero=tamCuad;
    
    for (int j=0; j<board_n; j++)
    {
        obj.push_back(Point3f((j/board_w)*tamcuadDamero, (j%board_w)*tamcuadDamero, 0.0f));
        
         
    }   
         

	// Se inician matrices
	//img1	 ->IMG IZQUIERDA
	//img2	 ->IMG DERECHA
	//gray1	 ->IMG IZQUIERDA
	//gray2	 ->IMG DERECHA
	
    Mat img1, img2, gray1, gray2;

	// exito en encontrar todos los puntos en un tablero
	success = 0;
	exitos=0;
	 
	// se ha encontrado los puntos? Inicialización
	//found1 ->	IZQUIERDA
	//found2 ->	DERECHA
    found1 = false;
    found2 = false;
    encontrado1=false;
    encontrado2=false;

    
    //Marca el final del bucle de adquisición
    finBucle=false;
    
    
    
     //Matrices de la calibracion estereo
    // CM1->IZQUIERDA
    //CM2->DERECHA
    CM1 = Mat(3, 3, CV_64FC1);
    CM2 = Mat(3, 3, CV_64FC1);
 
    
    errorRep=0;
    cont=0;
    
    //variable para tracking
    tracking=false;
    
    //sensibilidad esquinas
    bsensibilidadEsquinas=true;
    
    //por defecto se usa la opción 1 en el tipo de calibrado
    opcionCalibrado=1;
    
  
    
    //matriz de la trnasformacion previa de la distorsión
    ImIR  = Mat(colorancho,coloralto, CV_8UC3);//se crea en el struct imEstereo 
    ImRGB  = Mat(colorancho,coloralto, CV_8UC3);//se crea en el struct imEstereo

	//error tableros
	indicesResultado.clear();
	
	
	
	
}

EstereoCalibrate::EstereoCalibrate(int numBoards,int board_w,int board_h,int colorancho,int coloralto,float tamCuad,string carpetaDesdist)
{
   
	this->colorancho=colorancho;
	this->coloralto=coloralto;
    int board_n = board_w*board_h;
	board_sz=Size(board_w,board_h);
    this->numBoards=numBoards;
    carga=false;
    cargaPuntos=false;
	tamanyoMatrizRGB=Size(colorancho,coloralto);
	tamanyoMatrizIR=Size(anchoIR,altoIR);

	if (carpetaDesdist!="")
	{
		carpetaSalida=carpetaDesdist;
	}
	else{
		carpetaSalida="";
	}
    //tamaño del cuadro del damero de calbración (por defecto) en mm
    tamcuadDamero=tamCuad;
    
    for (int j=0; j<board_n; j++)
    {
        obj.push_back(Point3f((j/board_w)*tamcuadDamero, (j%board_w)*tamcuadDamero, 0.0f));
        
         
    }   
         

	// Se inician matrices
	//img1	 ->IMG IZQUIERDA
	//img2	 ->IMG DERECHA
	//gray1	 ->IMG IZQUIERDA
	//gray2	 ->IMG DERECHA
	
    Mat img1, img2, gray1, gray2;

	// exito en encontrar todos los puntos en un tablero
	success = 0;
	exitos=0;
	 
	// se ha encontrado los puntos? Inicialización
	//found1 ->	IZQUIERDA
	//found2 ->	DERECHA
    found1 = false;
    found2 = false;
    encontrado1=false;
    encontrado2=false;

    
    //Marca el final del bucle de adquisición
    finBucle=false;
    
    
    
     //Matrices de la calibracion estereo
    // CM1->IZQUIERDA
    //CM2->DERECHA
    CM1 = Mat(3, 3, CV_64FC1);
    CM2 = Mat(3, 3, CV_64FC1);
 
    
    errorRep=0;
    cont=0;
    
    //variable para tracking
    tracking=false;
    
    //sensibilidad esquinas
    bsensibilidadEsquinas=true;
    
    //por defecto se usa la opción 1 en el tipo de calibrado
    opcionCalibrado=1;
    
  
    
    //matriz de la trnasformacion previa de la distorsión
   
    ImRGB  = Mat(colorancho,coloralto, CV_8UC3);//se crea en el struct imEstereo

	
	
	
	
	
}

//void EstereoCalibrate::ImageToGrey(Mat &imgRGB,Mat &imgIR,Mat &grayRGB,Mat &grayIR)
//{
//	cvtColor(imgRGB, grayRGB, CV_BGR2GRAY,0);	  //IZQUIERDA
//    cvtColor(imgIR, grayIR, CV_BGR2GRAY,0);	  //DERECHA
//      
//}

void EstereoCalibrate::ImageToGrey(Mat &imgRGB,Mat &grayRGB)
{
	cvtColor(imgRGB, grayRGB, CV_BGR2GRAY,0);	  //IZQUIERDA
         
}

bool EstereoCalibrate::EncuentraEsquinas(Mat &grayRGB,Mat &RGB,Mat &grayIR,Mat &IR)
{      
		
		  cout<<"entra  findChessboardCorners"<<endl;
	
				//corners1 ->IZQUIERDA
		
				//DAMERO
				this->found1 = findChessboardCorners(grayRGB, board_sz, cornersRGB,0/* CALIB_CB_FAST_CHECK+*/ /*CALIB_CB_ADAPTIVE_THRESH*/ /*+*//* CALIB_CB_NORMALIZE_IMAGE+*/ /*CV_CALIB_CB_FILTER_QUADS*/);	
			
				if (found1)	
				{ 			
					drawChessboardCorners(RGB, board_sz, cornersRGB,  this->found1); //IZQUIERDA
					CalculaSubPix(grayRGB,cornersRGB);
					
				}
				//esto es una prueba para el gitdbdfbsdfbdfbdfbdfb

				this->found2 = findChessboardCorners(grayIR, board_sz, cornersIR, 0/*CALIB_CB_FAST_CHECK+*/ /*CALIB_CB_ADAPTIVE_THRESH*/ /*+*/ /*CALIB_CB_NORMALIZE_IMAGE+*/ /*CV_CALIB_CB_FILTER_QUADS*/);	
				if (found2)	
				{ 			
					drawChessboardCorners(IR, board_sz, cornersIR,  this->found2); //IZQUIERDA
					CalculaSubPix(grayIR,cornersIR);
				}		

				if (found1 && found2)
				{
					indicesResultado.push_back(1); 
						return true;}
				else{
					indicesResultado.push_back(0);
					return false;}
		
}

bool EstereoCalibrate::EncuentraEsquinas(Mat &grayRGB,Mat &RGB)
{
	try
	{
				
			   //DAMERO
				this->found1 = findChessboardCorners(grayRGB, board_sz, cornersRGB,0/* CALIB_CB_FAST_CHECK+*/ /*CALIB_CB_ADAPTIVE_THRESH*/ /*+*/ /*CALIB_CB_NORMALIZE_IMAGE+*//* CV_CALIB_CB_FILTER_QUADS*/);	
			
				if (found1)	
				{ 			
					drawChessboardCorners(RGB, board_sz, cornersRGB,  this->found1); //IZQUIERDA
					CalculaSubPix(grayRGB,cornersRGB);
					indicesResultado.push_back(1);
					
				}	   
				else
				{
					indicesResultado.push_back(0);
				}
				return found1;
	}
	catch(cv::Exception &error)
	{
		throw error;
		//cout<<"Error en FindChessboardCorners"<<endl;
	}
	catch(...)
	{
		throw "Error en FindChessboardCorners";
	}
			
}
	
		  
 

void EstereoCalibrate::CalculaSubPix ( Mat &imgRGB,vector<Point2f> &corners)
{  


				Mat viewGray;
		
			//	cvtColor(imgRGB, viewGray, COLOR_BGR2GRAY);
						cornerSubPix(imgRGB, corners, Size(5, 5),
							Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.001));

		
				      
           
				            
}

void EstereoCalibrate::almacenaEsquinas(bool RGBtrue)
{	   
					       
            imagePointsRGB.push_back(cornersRGB);   //IZQUIERDA

			//	cout<<corners1<<endl;
			//	cout<<imagePointsRGB[0]<<endl;
			if (RGBtrue)
			{
				imagePointsIR.push_back(cornersIR);	//DERECHA
			}
			object_points.push_back(obj);   
			//cout<<object_points[0]<<endl;
            success++; 
            exitos++; 
        
} 

 
int EstereoCalibrate::salvarCalibracion(string fguardaResultados,bool estereo,int reescalado,string tipoCalibracion)
{
     //    this->fguardaResultados=fguardaResultados;
	int ancho, alto,anchoBruto,altoBruto;
	float desfase;
	FileStorage fsLectura;

	//if (estereo)
	//{
	//	string camara=fguardaResultados.substr(46,12);
	//	string total="C:\\BOOTH\\Calib\\Geo\\RGB\\calibracionCal_RGB_"+camara+"_reescalada.xml";
	//	// lee archivo de calibracion previo
	//	fsLectura=FileStorage(total, FileStorage::READ);
	//	if(fsLectura.isOpened())
	//	{
	//			fsLectura["Desfase"] >> desfase;
	//			fsLectura["AnchoImRecortada"] >> ancho;
	//			fsLectura["AltoImRecortada"] >> alto;
	//			fsLectura["AnchoImRecortadaBruto"] >> anchoBruto;
	//			fsLectura["AltoImRecortadaBruto"] >> altoBruto;
	//		
	//		
	//			}
	//		else
	//			{
	//				errorLectura=-1;
	//				return errorLectura;
	//			}
	//}

    //archivo para guardar resultados
        fs1=FileStorage(fguardaResultados, FileStorage::WRITE);
        if(!fs1.isOpened())
		{
			errorLectura=-1;
			return errorLectura;
		}
		if (estereo)
		{
			fs1 << "M1" << CM1;
			fs1 << "M2" << CM2;
			fs1 << "D1" << D1;
			fs1 << "D2" << D2;
			fs1 << "R" << R;
			fs1 << "T" << T;	
			
		
			//tamanyo del cuadro del damero
			fs1 << "TamCuadDamero" <<  getTamCuadDamero();

			if (fsLectura.isOpened())
			{
				fs1 << "Desfase"<< desfase;
				fs1 << "AnchoImRecortada"<<ancho;
				fs1 << "AltoImRecortada"<<alto;
				fs1 << "AnchoImRecortadaBruto"<<anchoBruto;
				fs1 << "AltoImRecortadaBruto"<<altoBruto;
			}

		}
		//si no es estéreo
		else{
			if (reescalado==1)
			{
				//cambiaCxCy(CM1, tipoCalibracion);
			}
			if (tipoCalibracion=="Cal_RGB")
			{
				fs1 << "TipoCalibracion"<<"RGB";
			}
			else
			{
				fs1 << "TipoCalibracion"<<"IR";
			}
			fs1 << "M1" << CM1;
			fs1 << "D1" << D1;
			fs1 << "Desfasey"<< this->desfasey;
 			fs1 << "Desfasex"<< this->desfasex;
			fs1 << "AnchoImRecortada"<<this->tamImrecortadaAncho;
			fs1 << "AltoImRecortada"<<this->tamImrecortadaAlto;
			fs1 << "AnchoImRecortadaBruto"<<this->tamImrecortadaAnchoBruto;
			fs1 << "AltoImRecortadaBruto"<<this->tamImrecortadaAltoBruto;

		}
		
		
		//se libera el archivo
		fs1.release();	
		fsLectura.release();
		return errorLectura;
 
      
	
}
void EstereoCalibrate::lanzaCopiaPuntos(char* fguardaResultados,char* fguardaResultados2)
{
		char nombreDer[250]="";
		strcpy(nombreDer,fguardaResultados);
		char nombreIzq[250]="";
		strcpy(nombreIzq,fguardaResultados);
		string ficheroDer,ficheroIzq;
		stringstream stringDer,stringIzq;		
		
		char *p1=strstr(nombreDer,".xml");
		strncpy(p1,"_puntos_der.xml",15);
		char *p2=strstr(nombreIzq,".xml");
		strncpy(p2,"_puntos_izq.xml",15);
		
		stringDer<<nombreDer;
		stringIzq<<nombreIzq;

		ficheroDer=stringDer.str();
		ficheroIzq=stringIzq.str();
		cout<<"*********************Lanza copia puntos!!"<<endl;
		cout<<ficheroDer<<endl;
		cout<<ficheroIzq<<endl;
		//crea el objeto Filestorage
	    salvarPuntosCal(nombreDer,fPuntosDer);
	    salvarPuntosCal(nombreIzq,fPuntosIzq);
	    // copia puntos al archivo
		copiaPuntosaFile(fPuntosDer,ficheroDer,imagePointsIR);
		copiaPuntosaFile(fPuntosIzq,ficheroIzq,imagePointsRGB);
		cout<<"puntos copiados"<<endl;
		
	//	cout<<   imagePointsIR[0].at(0).x;
}

void EstereoCalibrate::lanzaLecturaPuntos(char* fguardaResultados)
{
		char nombreDer[250]="";
		strcpy(nombreDer,fguardaResultados);
		char nombreIzq[250]="";
		strcpy(nombreIzq,fguardaResultados);
		string ficheroDer,ficheroIzq;
		stringstream stringDer,stringIzq;		
		
		char *p1=strstr(nombreDer,".xml");
		strncpy(p1,"_puntos_der.xml",15);
		char *p2=strstr(nombreIzq,".xml");
		strncpy(p2,"_puntos_izq.xml",15);
		
		stringDer<<nombreDer;
		stringIzq<<nombreIzq;

		ficheroDer=stringDer.str();
		ficheroIzq=stringIzq.str();
		cout<<ficheroDer<<endl;
		cout<<ficheroIzq<<endl;
		
		//crea el objeto Filestorage de lectura
	    leerPuntosCal(nombreDer,fPuntosDer);
	    leerPuntosCal(nombreIzq,fPuntosIzq);
	    
		cout<<"Filestorage de lectura hecho"<<endl;
		leePuntosdeFile(fPuntosDer,ficheroDer,imagePointsGuardado2,cornersLectura2,object_points);
		object_points.clear();
		leePuntosdeFile(fPuntosIzq,ficheroIzq,imagePointsGuardado1,cornersLectura1,object_points);
		cout<<"puntos leidos"<<endl;
}

// Función que crea un objeto Filestorage
int EstereoCalibrate::salvarPuntosCal(char* fguardaResultados,FileStorage &fichero)
{
	this->fguardaResultados=fguardaResultados;

	

	//archivo para guardar resultados
	fichero=FileStorage(fguardaResultados, FileStorage::WRITE);
	
	if(!fichero.isOpened())
	{
	errorLectura=-1;
	return errorLectura;
	}

	return errorLectura;
 
      
	
}

//Función que crea un objeto FileNode
int EstereoCalibrate::leerPuntosCal(char* fguardaResultados, FileStorage &fichero)
{

fichero=FileStorage(fguardaResultados,FileStorage::READ);

	

 
      
return 1;	
}
bool EstereoCalibrate::calibracionCamara(Mat &imgRGB)	//(izquierda,derecha)
{
	
    // si las matrices de calibrado vienen de un archivo (carga)
    // no es necesario hacer el calibrado estereo ( ya está hecho)
			std::cout<<"calibrado normal empieza"<<endl;
			vector<Mat> rvecs, tvecs;
			vector<Mat> rvecs2, tvecs2;
			CvSize tamRGB=CvSize(imgRGB.size());	
					
				
					D1 = Mat::zeros(8, 1, CV_64F);
					
	                D2 = Mat::eye(8, 1, CV_64F);
					CM1 = Mat::eye(3, 3, CV_64F);
                    CM2 = Mat::eye(3, 3, CV_64F);
					
									     
			        
					switch(opcionCalibrado)
			        {			        
						
							case 1:	 		        					        
							{													   	
							    std::cout<<"calibrado  opcion 1"<<endl;
							    std::cout<<imgRGB.size()<<endl;	

								//no se calculan las distorsiones tangenciales
								calibrateCamera(object_points, imagePointsRGB,tamRGB,
								CM1, D1, this->rvecs, this->tvecs,0
								/*CALIB_FIX_PRINCIPAL_POINT*//*+CV_CALIB_ZERO_TANGENT_DIST*/,TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30,DBL_EPSILON));	
								
								
								std::cout<<"*******FIN calibrado opcion1"<<endl;
								//cout<< CM1<<endl<<CM2<<endl;
								std::cout<<this->rvecs[0]<<endl<<this->tvecs[0]<<endl;			
								

								break;
							 }
							 
							 case 2:
								 {
								 std::cout<<"calibrado  opcion2"<<endl;
								 //Ahora no es un calibrado estereo, ¡Solo tenemos una cámara!!!
								 calibrateCamera(object_points, imagePointsRGB,imgRGB.size(),
								CM1, D1, this->rvecs, this->tvecs,0/*
								CALIB_FIX_ASPECT_RATIO  + 
								CALIB_ZERO_TANGENT_DIST +
								CALIB_SAME_FOCAL_LENGTH +
								CALIB_FIX_K3 + CALIB_FIX_K4 + CALIB_FIX_K5,TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 100, 1e-5)*/
								);

								


								std::cout<<"FIN calibrado con intrinsic guess"<<endl;
							
								break;
							 }						
							 
								
					}

					// Si todo ha funcionado , vamos a calcular el error de am,bas calibraciones

					//Camara 1
					vector<float> reprojErrs1,reprojErrs2;
					double errorGordo=computeReprojectionErrors(object_points,imagePointsRGB,this->rvecs,this->tvecs,CM1,D1,reprojErrs1);
					
					// Escribe errores individuales en disco
					escribirErrores(tipoCal,reprojErrs1,errorGordo);

					cout<<"CM1"<<endl;
					cout<<CM1<<endl;
					cout<<"CM2"<<endl;
					cout<<CM2<<endl;  
					cout<<"D1"<<endl;
					cout<<D1<<endl;
					
				
					
			
	
return 1;

}

bool EstereoCalibrate::calibracionEstereo(Size tamanyoMatriz)	//(izquierda,derecha)
{	   
	    try
	   {

		   
    // si las matrices de calibrado vienen de un archivo (carga)
    // no es necesario hacer el calibrado estereo ( ya está hecho)
			cout<<"calibrado estereo empieza"<<endl;
												
			// si se quiere hacer una calibración normal de "esquinas capturadas" 			
				
					cout<<"calibrado estereo Directo"<<endl;
					/*D1 = Mat::eye(1, 5, CV_64F);
	                D2 = Mat::eye(1, 5, CV_64F);
					CM1 = Mat::eye(3, 3, CV_64F);
                    CM2 = Mat::eye(3, 3, CV_64F);*/
										     
			        
					switch(opcionCalibrado)
			        {			        
						
							case 1:	 		        					        
							{													   	
							    cout<<"calibrado estereo opcion1"<<endl;
							    							
								errorRep=stereoCalibrate(object_points,  imagePointsIR,imagePointsRGB, 
									CM2, D2, CM1, D1, tamanyoMatriz, R, T, E, F,
									TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,30,DBL_EPSILON),CALIB_FIX_PRINCIPAL_POINT+CV_CALIB_ZERO_TANGENT_DIST);
								
								cout<<"*******FIN calibrado estereo opcion1"<<endl;
								cout<< CM1<<endl<<CM2<<endl;
								cout<< R<<endl<<T<<endl;
								break;
							 }
							 
							// en este caso vamos a usar las matrices de cámara ya calculadas 
							 case 2:
							 {
								cout<<"calibrado estereo opcion2----->matrices de camara incluidas"<<endl; 

								//hay ya matrices de camara???
								cout<<"matiz camara 1"<<CM1<<endl;
								cout<<"matiz camara 2"<<CM2<<endl;
								cout<<"matiz dist 1"<<D1<<endl;
								cout<<"matiz dist 2"<<D2<<endl;
								cout <<"tamaño matriz: "<<tamanyoMatriz<<endl;

								errorRep=stereoCalibrate(object_points,imagePointsIR, imagePointsRGB, 
								CM2, D2, CM1, D1, tamanyoMatriz, R, T, E, F,
								TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,50,DBL_EPSILON),CV_CALIB_FIX_INTRINSIC/*+CV_CALIB_FIX_ASPECT_RATIO*/);
								cout<<"FIN calibrado estereo opcion 2"<<endl;
								
								break;
							 }							 
								
					}			
			
		   cout<<"R"<<endl;
			cout<<R<<endl;
			cout<<"T"<<endl;
			cout<<T<<endl;  
			
			cout<<"ErrorEstereo: "<<errorRep<<endl;
			// Visualizar el error
			Mat matError=Mat::zeros(400,400,CV_8UC3);
			int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
			cv::Point textOrg(100, 200);
			string errorString=to_string(errorRep);
			putText(matError,errorString,textOrg,fontFace,2.0,Scalar(255,255,255),3,8);
			cv::namedWindow("Error!", CV_WINDOW_KEEPRATIO);

			imshow("Error!",matError);



			
		//	if (tamanyoMatriz.width!=512) //lo que significa que SI hay reescalado
			{
				//estas funciones SI que requieren que las imágenes estereo sean iguales
				stereoRectify(CM2, D2, CM1, D1, tamanyoMatriz, R, T, R1, R2, P1, P2, Q,CV_CALIB_ZERO_DISPARITY ,0/*0.025*/,tamanyoMatriz,&validRoi[0], &validRoi[1]);   

				//   // MAtrices de los "mapeados" para aplicar a las imagenes y rectificar distorsión y 
				//   // el desalineamiento estereo
				initUndistortRectifyMap(CM1, D1, R1, P1, tamanyoMatriz, CV_32FC1, map2x, map2y);	//IZQUIERDA
				initUndistortRectifyMap(CM2, D2, R2, P2, tamanyoMatriz, CV_32FC1, map1x, map1y);	//DERECHA
			}
			
		   
	}
	catch( cv::Exception& excepcion )
	{
		const char* err_msg = excepcion.what();
		std::cout << "Ha habido un fallo en la calibracion: " << err_msg << std::endl;
		throw err_msg;
	}
  
    
    calibrado=true;
    
    return calibrado;
}

void EstereoCalibrate::comprobacion(Mat &imagRGB,Mat &imagIR,int indice)
{		
cout<<"**********carga: "<<carga<<endl;
			  			      

    remap(imagIR, imagIR, map1x, map1y, INTER_LINEAR, BORDER_CONSTANT, Scalar());	//DERECHA
    remap(imagRGB, imagRGB, map2x, map2y, INTER_LINEAR, BORDER_CONSTANT, Scalar());	//IZQUIERDA
    
	imwrite("C:\\IMAGENES\\estereo\\RESULTADOS_ESTEREO\\imagenes\\imderecha_corregir_cal_"+to_string(indice)+".jpg",imagRGB);
	imwrite("C:\\IMAGENES\\estereo\\RESULTADOS_ESTEREO\\imagenes\\imizquierda_corregir_cal_"+to_string(indice)+".jpg",imagIR);	 

		 Mat canvas;
    double sf;
    int w, h;
	bool isVerticalStereo=false;
	Mat cimg,cimg2;

   //estereo horizontal
    Size imageSize;
	imageSize=imagRGB.size();
    sf = 600./MAX(imageSize.width, imageSize.height);
    w = cvRound(imageSize.width*sf);
    h = cvRound(imageSize.height*sf);
    canvas.create(h, w*2, CV_8UC3);
   

    
    
       
        cvtColor(imagRGB, cimg, COLOR_GRAY2BGR);
	//cimg=imagRGB.clone();
        Mat canvasPart =canvas(Rect(w*0, 0, w, h)); 
        resize(cimg, canvasPart, canvasPart.size(), 0, 0, CV_INTER_AREA);
        if( true )
        {
            Rect vroi(cvRound(validRoi[0].x*sf), cvRound(validRoi[0].y*sf),
                      cvRound(validRoi[0].width*sf), cvRound(validRoi[0].height*sf));
            rectangle(canvasPart, vroi, Scalar(0,0,255), 3, 8);
        }

		 cvtColor(imagIR, cimg2, COLOR_GRAY2BGR);
		//cimg=imagIR.clone();
         Mat canvasPart2 =canvas(Rect(w*1, 0, w, h)); 
        resize(cimg2, canvasPart2, canvasPart2.size(), 0, 0, CV_INTER_AREA);
        if( true )
        {
            Rect vroi(cvRound(validRoi[1].x*sf), cvRound(validRoi[1].y*sf),
                      cvRound(validRoi[1].width*sf), cvRound(validRoi[1].height*sf));
            rectangle(canvasPart2, vroi, Scalar(0,0,255), 3, 8);
        }
    

    if( !isVerticalStereo )
        for( int j = 0; j < canvas.rows; j += 16 )
            line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
    else
        for(int  j = 0; j < canvas.cols; j += 16 )
            line(canvas, Point(j, 0), Point(j, canvas.rows), Scalar(0, 255, 0), 1, 8);
    imshow("rectified", canvas);
	waitKey(10);

	
    

	  
}
void EstereoCalibrate::borraVectores()
{	 
	//if(imagePointsRGB.size()>0 && imagePointsIR.size()>0 && object_points.size()>0)
	{
	imagePointsRGB.clear();
	imagePointsIR.clear();
	object_points.clear();	
	exitos=0;
	cout<<"ha borrado los vectores"<<endl;
	}
	//else
	//{cout<<"no ha borrado los vectores!!!!!!!!!!!!"<<endl;}
	
	
}
void EstereoCalibrate::liberaTodo()
{

	imagePointsRGB.clear();
	imagePointsIR.clear();
	object_points.clear();
    CM1.release();
    CM2.release();
    D1.release();
    D2.release();
    R.release();
    T.release();
    E.release();
    F.release();
    
    map1x.release();
    map1y.release();
    map2x.release();
    map2y.release();
    imgU1.release();
    imgU2.release();
    
     //MAtrices de la rectificacion estereo
    R1.release();
    R2.release();
    P1.release();
    P2.release();
    Q.release();
    

}

bool EstereoCalibrate::getestadoEncontrado1(void)
{
	return encontrado1;
}



void EstereoCalibrate::setestadoEncontrado1(bool estado)
{
	encontrado1=estado;
}

//int EstereoCalibrate::cargaFichero(char* archivoCal)
int EstereoCalibrate::cargaFichero(string archivoCalRGB,string archivoCalIR)
{

	cout<<"calibrado estereo indirecto"<<endl;
				    this->D1 = Mat::eye(1, 5, CV_64F);
	                this->D2 = Mat::eye(1, 5, CV_64F);
					this->CM1 = Mat::eye(3, 3, CV_64F);
                    this->CM2 = Mat::eye(3, 3, CV_64F);

	FileStorage archivocargarCalibracionRGB(archivoCalRGB, CV_STORAGE_READ);
		if(!archivocargarCalibracionRGB.isOpened())
		{
			errorLectura=-1;
			return errorLectura;
		}

	
		
		carga=true;

	

		std::cout<<"sigue..."<<endl;
		Rect roi1, roi2;
	//	Mat M1, D1, M2, D2;
		archivocargarCalibracionRGB["M1"] >> CM1;		
		
		archivocargarCalibracionRGB["D1"] >> D1;
		archivocargarCalibracionRGB["Desfasex"] >> desfasex;
		archivocargarCalibracionRGB["Desfasey"] >> desfasey;

		archivocargarCalibracionRGB.release();

		FileStorage archivocargarCalibracionIR(archivoCalIR, CV_STORAGE_READ);
		if(!archivocargarCalibracionIR.isOpened())
		{
			errorLectura=-1;
			return errorLectura;
		}
		
		archivocargarCalibracionIR["M1"] >> CM2;
		
		archivocargarCalibracionIR["D1"] >> D2;		


		cout <<"matriz CM1 orig: "<<CM1<<" " <<"matriz CM2 orig: "<<CM2<<endl;
		
		    
		archivocargarCalibracionIR.release();
	

}

void EstereoCalibrate::cambiaCxCy(Mat &CM1,string tipoCal)
{
	if (tipoCal=="Cal_RGB")
	{
		CM1.at<double>(0,2)=CM1.at<double>(0,2)- this->desfasex;
	}
	else
	{
		CM1.at<double>(1,2)=CM1.at<double>(1,2)- this->desfasey;
	}
}

int EstereoCalibrate::leerCal(char* archivoCal,Mat &ImapxIzqMat,Mat &ImapyIzqMat,Mat &ImapxDerMat,Mat &ImapyDerMat)
//int EstereoCalibrate::leerCal(char* archivoCal)
{
	 ///PELIGRO!!!!!!!!
	 // NADA DE VALORES LITERALES ^^
	 cout<<"tamaño imagenes: "<<colorancho<<", "<<coloralto<<endl;
	// CvSize tamanyoDisparidad={colorancho,coloralto}/*ImapxIzqMat.size()*/;
	 CvSize tamanyoDisparidad={coloralto, colorancho}/*ImapxIzqMat.size()*/;
	 
	// cargaFichero(archivoCal);
		
	//	stereoRectify (CM1, D1, CM2, D2, tamanyoDisparidad, R, T, R1, R2, P1, P2,Q,0/*CV_CALIB_ZERO_DISPARITY*/,0/*0.05*/);


////		Mat map11, map12, map21, map22;
		initUndistortRectifyMap(CM1, D1, R1, P1, tamanyoDisparidad, CV_16SC2,ImapxIzqMat, ImapyIzqMat);
		initUndistortRectifyMap(CM2, D2, R2, P2, tamanyoDisparidad, CV_16SC2,ImapxDerMat, ImapyDerMat);	  	
			
	
		cout<<"lectura calibracion hecha"<<endl;
		return errorLectura;
}

void EstereoCalibrate::cambiaTamanyoTablero(int col,int fil)
	{
	  columnas=col;
	  filas=fil;
 //Inicializamos vectores y contenedores
    board_sz = Size(columnas, filas);
    }
    
  
    
 
    
    //// CALIBRATION QUALITY CHECK
//// because the output fundamental matrix implicitly
//// includes all the output information,
//// we can check the quality of calibration using the
//// epipolar geometry constraint: m2^t*F*m1=0
void EstereoCalibrate::calculoError()
{			
//int N=54;	
//int i;	
//int n=columnas*filas;	  
//	//vector<CvPoint2D32f> points[2];    
//	vector<CvPoint3D32f> lines[2];
//	//imagePointsRGB.resize(N);
//	//imagePointsIR.resize(N);
//    /*points[0].resize(N);
//    points[1].resize(N);*/	
//   int hola=imagePointsRGB.capacity();
//    Mat _imagePointsRGB = Mat(1, N, CV_32FC2, imagePointsRGB );
//    Mat _imagePointsIR = Mat(1, N, CV_32FC2, imagePointsIR );
//    lines[0].resize(N);
//    lines[1].resize(N);
//    Mat _L1 = Mat(1, N, CV_32FC3, &lines[0][0]);
//    Mat _L2 = Mat(1, N, CV_32FC3, &lines[1][0]);
//	
////Always work in undistorted space
//    undistortPoints( _imagePointsRGB, _imagePointsRGB,
//        CM1, D1, 0, 0 );
//    undistortPoints( _imagePointsIR, _imagePointsIR,
//       CM2, D2, 0, 0 );
// /*   computeCorrespondEpilines( _imagePointsRGB, 1, F, _L1 );
//    computeCorrespondEpilines( _imagePointsIR, 2, F, _L2 );*/
//    double avgErr = 0;
//  // cout<<imagePointsRGB[i].at(0).x;
//   /* for( i = 0; i < N; i++ )
//    {
//	    cout<<imagePointsRGB[i].at(0).x;
//        double err = fabs((imagePointsRGB[i].at(0).x * lines[1][i].x) + (imagePointsRGB[i].at(1).y * lines[1][i].y + lines[1][i].z))  ;
//        double error2 =fabs((imagePointsIR[i].at(0).x * lines[0][i].x) + (imagePointsIR[i].at(1).y * lines[0][i].y + lines[0][i].z));
//        avgErr += (err+error2);
//    }
//   cout<<"avg err = "<<avgErr/(numBoards*n)<<endl ;*/
}

double EstereoCalibrate::getErrorRepro(void)
{
	return errorRep;
}

void EstereoCalibrate::setTracking(bool valor)
{
	tracking=valor;
}

void EstereoCalibrate::limpiaVector(vector<Point2d> &vectorLimpia)
{
for (int i=0;i<vectorLimpia.size();i++){
	vectorLimpia.at(i)=Point(0,0);   }
}

void EstereoCalibrate::comprobacionDamero(vector<Point2f> &copia,vector<Point2f> &original)
{	
	//bool is_equal=true;
	//for (int i=0;i<copia.size();i++)
	//{
	//	if(cvRound(copia[i].x)==cvRound(original[i].x) && cvRound(copia[i].y)==cvRound(original[i].y )  )
	//	{
	//		is_equal=true;
	//		//cout<<"Todo normal "<<endl;
	//		//break;
	//	}
	//	else
	//	{
	//		is_equal=false;
	//		cout<<"Hay un fallo!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl; 
	//		cout<<cvRound(copia[i].x)<<" "<<cvRound(original[i].x)   ;
	//		cout<<cvRound(copia[i].y)<<" "<<cvRound(original[i].y)   ;
	//		break; 
	//	}
	//}
	
	
	
}

void  EstereoCalibrate::copiaPuntosaFile(FileStorage &fs, const string name, vector<vector<Point2f>> &vov)
{	  
	stringstream ss;
	cout<<"archivo"<<name<<endl;
    fs << "matriz" << "{";
    cout<<"numero de matrices"<<vov.size()<<endl;
  //  fs <<"vector_1"<<vov[0];
    for (int i = 0; i < vov.size(); i++)
    {
   
		ss << i;
		string str = ss.str();
        fs << "vector"+str <<vov[i];
       /* cout<<name + "_" + str<<endl;
        vector<Point2f> tmp = vov[i];*/
       /* fs << tmp;*/
       
       ss.str("");
    }		   
    fs <<"matriz"<< "}";
    fs.release();
	
}

void EstereoCalibrate::leePuntosdeFile(FileStorage &fns, string name, vector<vector<Point2f>> &vov,vector<Point2f> &esquinas,vector<vector<Point3f>> &object)
{
   // vov.clear();
	 int cont=0;
    cout<<"paso por leepuntosdeFile 1"<<endl;
    FileNode fn = fns["matriz"];
    if (fn.empty()){
    cout<<"NODO VACIO !!"<<endl;
        return;
    }
    cout<<"paso por leepuntosdeFile"<<endl;
    cout<<"tamanyo de object: "<<object.size()<<endl;

    FileNodeIterator current = fn.begin(), it_end = fn.end(); // Go through the node
    for (; current != it_end; ++current)
    {
        vector<Point2f> tmp;
        FileNode item = *current;
        item >> tmp;
        item >>	esquinas;
        vov.push_back(tmp);
        object.push_back(obj);   
        cout<<"paso por itera "<<dec<<cont<<endl;
        cont++;
    }
    cout<<"tamanyo de object despues: "<<object.size()<<endl;
    exitos=cont;
}    
    
void EstereoCalibrate::setCargaPuntosCal(bool valor)
{
		cargaPuntos=valor;
}

void EstereoCalibrate::setCargaCal(bool valor)
{
		carga=valor;
}

void EstereoCalibrate::setOpcionCalibrado(int valor)
{
		opcionCalibrado=valor;
}

void EstereoCalibrate::setTamCuadDamero(int valor)
{
		tamcuadDamero=valor;
}

int EstereoCalibrate::getTamCuadDamero(void)
{
		return tamcuadDamero;
}

void EstereoCalibrate::setnumSerial(int numCam,string valor)
{
if (numCam==1){
		serieCam1=valor;}
		else{   serieCam2=valor;
		}
}

string EstereoCalibrate::getnumSerial(int numCam)
{
		
        if (numCam==1){
		return serieCam1;}
		else{   return serieCam2;
		}
		
}

void EstereoCalibrate::setmapeado(Mat &imIzquierda,Mat &imIzquierdaTransf, double coeff1)
{
//	for( int j = 0; j < src.rows; j++ )
//	{ 
//		for( int i = 0; i < src.cols; i++ )
//		{
//			  map_x.at<float>(j,i) = src.cols - i ;
//              map_y.at<float>(j,i) = j ;
//		}
//	}	

	for( int filas = 0; filas < imIzquierda.rows; filas++ )
	{ 
		for( int columnas = 0; columnas < imIzquierda.cols; columnas++ )
		{
		double r2=filas*filas + columnas*columnas;
		double r4=r2*r2;
		double filasNuevas =filas*(1+coeff1*r2);
		double columnasNuevas =columnas*(1+coeff1*r2);
		
		int filasN=(int)floor(filasNuevas);
		int columnasN=(int)floor(columnasNuevas);
		
		cout<<"Filas nuevas: "<<filasN   <<endl;
		cout<<"Col nuevas: "<<columnasN   <<endl;

		imIzquierdaTransf.at<Vec3b>(filasN,columnasN)[0]=imIzquierda.at<Vec3b>(filas,columnas)[0];		
 		imIzquierdaTransf.at<Vec3b>(filasN,columnasN)[1]=imIzquierda.at<Vec3b>(filas,columnas)[1];		
 		imIzquierdaTransf.at<Vec3b>(filasN,columnasN)[2]=imIzquierda.at<Vec3b>(filas,columnas)[2];
 		}
 	}
 	
 	for( int filas = 0; filas < imIzquierda.rows; filas++ )
	{ 
		for( int columnas = 0; columnas < imIzquierda.cols; columnas++ )
		{

		imIzquierda.at<Vec3b>(filas,columnas)[0]=imIzquierdaTransf.at<Vec3b>(filas,columnas)[0];		
 		imIzquierda.at<Vec3b>(filas,columnas)[1]=imIzquierdaTransf.at<Vec3b>(filas,columnas)[1];		
 		imIzquierda.at<Vec3b>(filas,columnas)[2]=imIzquierdaTransf.at<Vec3b>(filas,columnas)[2];
 		}
 	}
 		/*imEstereo.ImatCamDerechagiroTrans.at<Vec3b>(filas,columnas)[0];		
 		imEstereo.ImatCamDerechagiroTrans.at<Vec3b>(filas,columnas)[1];		
 		imEstereo.ImatCamDerechagiroTrans.at<Vec3b>(filas,columnas)[2];	*/	
//		
}

 void EstereoCalibrate::setSensibilidadEsquinas(bool valor)
{
	bsensibilidadEsquinas =valor;
			
}

  double EstereoCalibrate::computeReprojectionErrors(const vector<vector<Point3f> >& objectPoints,
	const vector<vector<Point2f> >& imagePoints,
	const vector<Mat>& rvecs, const vector<Mat>& tvecs,
	const Mat& cameraMatrix, const Mat& distCoeffs,
	vector<float>& perViewErrors)
{
	vector<Point2f> imagePointsIR;
	int i, totalPoints = 0;
	double totalErr = 0, err;
	perViewErrors.resize(objectPoints.size());

	for (i = 0; i < (int)objectPoints.size(); ++i)
	{
		projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix,
			distCoeffs, imagePointsIR);

		//	cout<<"object points "<<object_points[i]<<endl;
		//	cout<<"rvecs "<<rvecs[i]<<endl;
		//	cout<<"tvecs "<<tvecs[i]<<endl;
		//	cout<<"imagePointsIR "<<imagePointsIR[0]<<endl;
		//	cout<<"esquinas medidas error"<<imagePoints[i][0]<<endl;

			

		err = norm(Mat(imagePoints[i]), Mat(imagePointsIR), CV_L2);

		int n = (int)objectPoints[i].size();
		perViewErrors[i] = (float)std::sqrt(err*err / n);
		totalErr += err*err;
		totalPoints += n;
	}

	return std::sqrt(totalErr / totalPoints);
}

  void EstereoCalibrate::comprobacionGraficaResultado(Size imageSize, std::vector<string> &imageList,vector<int> indicesBuenos,vector<int> indicesResultado,double cyRGB, int reescalado)
  {
	    bool found=false;
		Mat view, rview, map1, map2,R;
		Mat rvec2, tvec2,imagePointsIR;
		vector<Point2f> puntosUnDamero;
		vector<vector<Point2f> > puntosDamero;
		vector<Point2f> imagenPuntos;

		
		cout<<"matriz de camara IR "<<CM1<<endl;
			cout<<"matriz de camara RGB "<<D1<<endl;

		initUndistortRectifyMap(CM1, D1, Mat(),
			getOptimalNewCameraMatrix(CM1, D1, imageSize,0, imageSize, 0),
			imageSize, CV_32FC1, map1, map2);

		//initUndistortRectifyMap(CM1, D1, Mat(),CM1,imageSize, CV_32FC1, map1, map2);

		int contRojos,contRojosTotal=0;
	    Mat puntosRojos;
	  //  view = imread(imageList[0], 1);
		puntosRojos=Mat (imageSize,CV_8SC3);
		vector<Point2f>  imagePoints_undist,puntos_desdist,puntos_desdist_norm;
		
	
		Mat imagen_reescalada;
		Mat imagen_recortada=Mat::zeros(Size(1920,1080),CV_8UC3);
	  //  Mat imagenIR_reescalada; 



		for (int i = 0; i < indicesBuenos.size(); i++)
		{
			
			view = imread(imageList[indicesBuenos[i]], 1);
			//view = imread(imageList[i], 1);

			//flip
			//    cv::flip(view,view,1);
		
			
			// SI HAY REESCALADO
			if (reescalado==1)
			{
				//flip
			  //  cv::flip(view,view,1);//doble flip para poner la imagen derecha

				// se reescala para la IR
				///////////////////////////////////////////////////
				if (view.cols==512)
				{
			/*		float indiceReescalado=(double)(this->colorancho)/(double)(view.cols);
					int recorte=abs(this->coloralto-(view.rows*indiceReescalado))/2;

					resize(view,imagen_reescalada,Size(),indiceReescalado,indiceReescalado,INTER_CUBIC);
					recortarImagen(imagen_reescalada,imagen_recortada,0,recorte,Size(this->colorancho,this->coloralto));
			*/

				//IR
				//AUTO NACHO

				double ratioNew=(1080.0/(double)coloralto);
				double rat=(double)colorancho*ratioNew;
				double R1=(double)(1920.0f-rat);
				double R2=R1/(double)2.0f;
				int x=(int)R2;
				int y=0;
				
				resize(view,imagen_reescalada,Size(),ratioNew,ratioNew,INTER_LANCZOS4);				
				imagen_reescalada.copyTo(imagen_recortada(cv::Rect(x,y,imagen_reescalada.cols, imagen_reescalada.rows)));


				
				//resize(view,imagen_recortada,imageSize,0,0,INTER_CUBIC);				
				//recortarImagen(imagen_reescalada,imagen_recortada,0,(int)desfasey,Size(imageSize.width,1080));
			


				}
				else //RGB
				{
					view.copyTo(imagen_recortada);

					//resize(view,imagen_recortada,Size(),1,1,INTER_CUBIC);//no se reescala
					//recortarImagen(imagen_reescalada,imagen_recortada,(int)desfasex,0,Size(imageSize.width,1080));

			
				}	
			}
			//NO HAY REESCALADO
			else
			{
				view.copyTo(imagen_recortada); //no se hace ninguna operacion de reescalado solo se copia para conservar el nombre
			}

			//corrige imagen
			remap(imagen_recortada, rview, map1, map2,INTER_LINEAR);
			
												
			            //proyecta puntos 3D a 2D
			/*cout<<"CM1 "<<CM1<<endl;
			cout<<"D1 "<<D1<<endl;
			cout<<"object points "<<object_points[i]<<endl;
			cout<<"rvecs "<<rvecs[i]<<endl;
			cout<<"tvecs "<<tvecs[i]<<endl;*/
						projectPoints(object_points[i],rvecs[i], tvecs[i], CM1,D1, imagePoints_undist);
						undistortPoints(imagePoints_undist,puntos_desdist_norm,CM1,D1);
																
						contRojos=0;
						
						for (int j = 0; j < board_sz.height*board_sz.width; ++j)
						{
							//float error_x=abs(cornersRGB[j].x-imagePoints_undist_vector[j].x);
						    //float error_y=abs(cornersRGB[j].y-imagePoints_undist_vector[j].y);
							double error = norm(Mat(imagePoints_undist[j]), Mat(imagePointsRGB[i][j]), CV_L2);

						//	cout<<"inicio:"<<imagePoints_undist[j]<<endl;
						//	cout<<"desdist:"<<puntos_desdist_norm[j]<<endl;
							double puntos_desdistx=puntos_desdist_norm[j].x * CM1.at<double>(0,0)+ CM1.at<double>(0,2);
							double puntos_desdisty=puntos_desdist_norm[j].y * CM1.at<double>(1,1)+ CM1.at<double>(1,2);

							/*double puntos_desdistx=imagePoints_undist[j].x ;
							double puntos_desdisty=imagePoints_undist[j].y ;*/

							puntos_desdist.push_back(Point2f(puntos_desdistx,puntos_desdisty));
						//	cout<<"desdist:"<<puntos_desdist[j]<<endl;
							if (error<=1 )
							{
							//	circle(imagen_recortada, (Point) (puntos_desdist[j]), 1 ,CV_RGB(0,255,0),2 );//verde
								circle(imagen_recortada, (Point) (imagePoints_undist[j]), 1 ,CV_RGB(255,0,0),1 );//rojo
								circle(puntosRojos, (Point) (imagePoints_undist[j]), 1 ,CV_RGB(0,255,0),1 );//verde
																
							}
							else if (  (error>1) && (error<=2) )
								
								{
								//	circle(imagen_recortada, (Point) (puntos_desdist[j]), 1 ,CV_RGB(255,255,0),2 );//amarillo
									circle(imagen_recortada, (Point) (imagePoints_undist[j]), 1 ,CV_RGB(255,255,0),1 );//amarillo
									circle(puntosRojos, (Point) (imagePoints_undist[j]),1 ,CV_RGB(255,255,0),1 );//amarillo
								
									contRojos++;
									contRojosTotal++;
								}
							else if (  (error>2) && (error<=3) )
								
								{
								//	circle(imagen_recortada, (Point) (puntos_desdist[j]), 1 ,CV_RGB(255,0,0),2 );//rojo
									circle(imagen_recortada, (Point) (imagePoints_undist[j]), 1 ,CV_RGB(255,0,0),1 );//rojo
									circle(puntosRojos, (Point) (imagePoints_undist[j]),1 ,CV_RGB(255,0,0),1 );//rojo
							/*		cout<<error_x<<endl;
									cout<<error_y<<endl;*/
									contRojos++;
								    contRojosTotal++;
								}

							else if (  error>3 )
								
								{
									
								//	circle(imagen_recortada, (Point) (puntos_desdist[j]), 1 ,CV_RGB(153,50,204),2 );//morado
									circle(imagen_recortada, (Point) (imagePoints_undist[j]), 1 ,CV_RGB(153,50,204),1 );//morado
									circle(puntosRojos, (Point) (imagePoints_undist[j]), 1 ,CV_RGB(153,50,204),1 );//morado
								/*	cout<<error_x<<endl;
									cout<<error_y<<endl;*/
									contRojos++;
								    contRojosTotal++;
								}
							
						}
						cout<<"Fallos en imagen "<<i<<" : "<<contRojos<<endl;
			
						//dibujar circulos de la deteccion corregida de esquinas
			
							//for (int j = 0; j < s.boardSize.width*s.boardSize.height; ++j)
							//{
							//	circle(rview, (Point) (imagePoints.at(i))[j], 3 ,CV_RGB(0,0,255),2 );//azul
							//}
			
						cv::namedWindow("Imagen View", CV_WINDOW_KEEPRATIO);
						cv::namedWindow("Errores", CV_WINDOW_KEEPRATIO);
						cv::moveWindow("Imagen View",0,0);
						cv::moveWindow("Errores",820,0);
						cv::resizeWindow("Imagen View",1000,562);
						cv::resizeWindow("Errores",1000,562);
						imshow("Imagen View", imagen_recortada);
						imwrite("Imagen View_"+to_string(i)+".png", imagen_recortada);
						imshow("Errores", puntosRojos);
						waitKey(10);
						/*char c = (char)waitKey();
					
						if (c == 'ESC' || c == 'q' || c == 'Q')
							break;*/

						puntosUnDamero.clear();
						puntos_desdist.clear();
					//	viewGray.release();imagePoints_undist_corregido.release(); puntos_vector.release();
					//	imagePoints_undist_vector.clear();

				//		waitKey(0);
				
		}

		//calculo media fallos
		double mediaFallos=contRojosTotal/indicesBuenos.size();
			imwrite("imagen corregida.png",rview);
						imwrite("imagen normal.png",view);
						imwrite("puntos de error.png",puntosRojos);
		cout<<"Fallos totales: "<<mediaFallos<<" de tablero con "<<board_sz.height*board_sz.width<<" puntos"<<endl;
		//	char c = (char)waitKey();

		/////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////
		// desdistorsionar BIEN
		for (int i = 0; i < imageList.size(); i++)
		{
			view = imread(imageList[i], 1);

			//flip
		//	cv::flip(view,view,1);	

			if (reescalado==1)
			{
				//flip
			  //  cv::flip(view,view,1);//doble flip para poner la imagen derecha
				// se reescala para la RGB
				///////////////////////////////////////////////////
				if (view.cols==512)
				{
					/*float indiceReescalado=(double)(this->colorancho)/(double)(view.cols);
					int recorte=abs(this->coloralto-(view.rows*indiceReescalado))/2;

				resize(view,imagen_reescalada,Size(),indiceReescalado,indiceReescalado,INTER_CUBIC);
				recortarImagen(imagen_reescalada,imagen_recortada,0,recorte,Size(this->colorancho,this->coloralto));*/

					//IR
				//AUTO NACHO
			

				double ratioNew=(1080.0/(double)coloralto);
				double rat=(double)colorancho*ratioNew;
				double R1=(double)(1920.0f-rat);
				double R2=R1/(double)2.0f;
				int x=(int)R2;
				int y=0;
				
				resize(view,imagen_reescalada,Size(),ratioNew,ratioNew,INTER_LANCZOS4);				
				imagen_reescalada.copyTo(imagen_recortada(cv::Rect(x,y,imagen_reescalada.cols, imagen_reescalada.rows)));


			//	resize(view,imagen_recortada,imageSize,0,0,INTER_CUBIC);				
			//	recortarImagen(imagen_reescalada,imagen_recortada,0,(int)( (R-(R*ratio))*2 ),Size(imageSize.width,1080));
			


				}
				else
				{
					//RGB
					view.copyTo(imagen_recortada); //no se hace ninguna operacion de reescalado solo se copia para conservar el nombre
			
				}	
			}
			else
			{
				view.copyTo(imagen_recortada); //no se hace ninguna operacion de reescalado solo se copia para conservar el nombre
			}

			//corrige imagen
			remap(imagen_recortada, rview, map1, map2, INTER_LINEAR);
			
			if (carpetaSalida!="")
			{	
				
				std::size_t encontrado_nombreFichero=imageList[i].find_last_of("/\\");
				
				cout<<"path: "<<imageList[i].substr(0,encontrado_nombreFichero);
				cout<<"fichero: "<<imageList[i].substr(encontrado_nombreFichero+1);
				string pathFichero=imageList[i].substr(0,encontrado_nombreFichero);
				string nombreFichero=imageList[i].substr(encontrado_nombreFichero+1);
				string nombreFichero2=nombreFichero.substr(0,12);//tamaño serie camara
				//suma
				string nombreTotal=carpetaSalida+"\\"+nombreFichero2+"_undist_"+to_string(i)+".png";

				imwrite(nombreTotal,rview);
			}
		}

		////////////////////////////////////////////////////
		////////////////////////////////////////////////////////7
		//FIN DESDISTORSION BIEN


  }

  void EstereoCalibrate::setMatrices(string matrizRGB,string matrizIR)
  {
	  cargaFichero(matrizRGB,matrizIR);
	 
  }
  void recortarImagen(Mat &imagen, Mat &imagenRecortada, int ancho, int alto,Size tamanyo)
{
	cv::Rect rect(ancho,alto,tamanyo.width,tamanyo.height);

	
	imagenRecortada = imagen(rect);
}

  int  EstereoCalibrate::escribirErrores(string tipoCal,vector<float> reprojErrs1,double errorGordo)
  {
					FileStorage fs;
					string tipoCalibracion;
					if (tipoCal=="Cal_IR")
					{
						tipoCalibracion="C:\\BOOTH\\Calib\\Geo\\IR";
					}
					else
					{
						tipoCalibracion="C:\\BOOTH\\Calib\\Geo\\RGB";
					}
					fs.open(tipoCalibracion+ "\\errores_"+serieCam1+".yml",FileStorage::WRITE);
					if(!fs.isOpened())
		{
			errorLectura=-1;
			return errorLectura;
		}
					fs<<"Errores Individuales"<<1;
					int contador=0;
					for(int j=0;j<indicesResultado.size();j++)
					{
						if (indicesResultado[j]==1)
						{
							fs<<"tablero "+to_string(j)<<(reprojErrs1[contador]);
							contador++;
						}
						else{
							fs<<"tablero "+to_string(j)<< "NULO";
							
						}
						
					}
					fs<<"Error gordo"<<errorGordo;
					fs.release();
  }

  //setter desfase
	void EstereoCalibrate::setDesfasey(int desfasey){this->desfasey=desfasey;}

	void EstereoCalibrate::setDesfasex(int desfasex){this->desfasex=desfasex;}
	//setter aumento
	void EstereoCalibrate::setAltoImRecortada(int alto){this->tamImrecortadaAlto=alto;}
	//setter recorte
	void EstereoCalibrate::setAnchoImRecortada(int ancho){this->tamImrecortadaAncho=ancho;}
	//settter ancho Bruto
	void EstereoCalibrate::setAnchoImRecortadaBruto(int ancho){this->tamImrecortadaAnchoBruto=ancho;}
	//settter alto Bruto
	void EstereoCalibrate::setAltoImRecortadaBruto(int alto){this->tamImrecortadaAltoBruto=alto;}
	void EstereoCalibrate::setTipoCal(string tipoCal){
	this->tipoCal=tipoCal;}





  