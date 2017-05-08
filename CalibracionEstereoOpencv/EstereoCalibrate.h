#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"  
//#include <algorithm>
//#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>


//#include <opencv2/objdetect/objdetect.hpp>

#include <stdio.h>



using namespace cv;


class EstereoCalibrate
{
public:
	
	~EstereoCalibrate(void);
	EstereoCalibrate(int numBoards,int board_w,int board_h,char* ficheroGuardaResultados,int colorancho,int coloralto,float tamCuad);
	EstereoCalibrate(int numBoards,int board_w,int board_h,int colorancho,int coloralto,float tamCuad,string carpetaDesdist);
	public:
//	void ImageToGrey(Mat &img1,Mat &img2,Mat &grey1,Mat &grey2);
	void ImageToGrey(Mat &img1,Mat &grey1);
	bool EncuentraEsquinas(Mat &RGB_gray,Mat &RGB,Mat &IR_gray,Mat &IR);
	bool EncuentraEsquinas(Mat &grayRGB,Mat &RGB);
	void CalculaSubPix (Mat &img1,vector<Point2f> &corners);
	void almacenaEsquinas(bool RGBtrue);
	// cal estereo
	bool calibracionEstereo(Size tamanyoMatriz);
	//cal camara individual
	bool EstereoCalibrate::calibracionCamara(Mat &img1);
	// ERROR!!!
	double EstereoCalibrate::computeReprojectionErrors(const vector<vector<Point3f> >& objectPoints,
	const vector<vector<Point2f> >& imagePoints,
	const vector<Mat>& rvecs, const vector<Mat>& tvecs,
	const Mat& cameraMatrix, const Mat& distCoeffs,
	vector<float>& perViewErrors);

	//coprobacion grafica resultado!!!!!!
	void comprobacionGraficaResultado(Size imageSize,std::vector<string> &view,vector<int> indicesBuenos,vector<int> indicesResultado,double cyRGB, int reescalado);
	//set matrices externas para el calibrado con dos camaras distiontas!!!!
	void setMatrices(string matrizRGB,string matrizIR);
	void comprobacion(Mat &imag1_,Mat &imag2_,int indice);
	void liberaTodo(void); 
	public:
	int leerCal(char* archivoCal,Mat &ImapxIzqMat,Mat &ImapyIzqMat,Mat &ImapxDerMat,Mat &ImapyDerMat);
	//int leerCal(char* archivoCal);
	bool getestadoEncontrado1(void);
	void setestadoEncontrado1(bool estado);
	void borraVectores(void);
	int salvarCalibracion(string fguardaResultados, bool estereo,int reescalado,string tipoCalibracion)  ;
	int salvarPuntosCal(char* fguardaResultados,FileStorage &fichero)  ;
	int leerPuntosCal(char* fguardaResultados, FileStorage &fichero)  ;
	void cambiaTamanyoTablero(int col, int fil);
	void calculoError();
	double getErrorRepro(void);
	//prueba para tracking
	void iniciaMatTracking(int columnas,int filas,double tamdamero);
	void CalculaTracking(Mat &img1,std::vector<cv::Point2f> puntosBlobs);
	void setTracking(bool valor);
	void limpiaVector(vector<Point2d>  &vectorLimpia)	 ;
	void comprobacionDamero(vector<Point2f> &copia,vector<Point2f> &original);
	vector<cv::KeyPoint>  EstereoCalibrate::getPointsBlobs();
	//prueba
	void copiaPuntosaFile(FileStorage &fs, const string name, vector<vector<Point2f>> &vov)				 ;
	void leePuntosdeFile(FileStorage &fns, string name, vector<vector<Point2f>> &vov,vector<Point2f> &esquinas,vector<vector<Point3f>> &object);
	void lanzaCopiaPuntos(char* fguardaResultados,char* fguardaResultados2);
	void lanzaLecturaPuntos(char* fguardaResultados);
	void setCargaPuntosCal(bool valor);
	void setCargaCal(bool valor);
	void setOpcionCalibrado(int valor);
	void setTamCuadDamero(int valor);
	int  getTamCuadDamero(void);
	string getnumSerial(int numCam);
	void setnumSerial(int numCam,string valor)	;
    void setmapeado(Mat &imIzquierda,Mat &imIzquierdaTransf, double coeff1)	;
    void setSensibilidadEsquinas(bool valor);
    
   // int cargaFichero(char* archivoCal)   ;
	int cargaFichero(string archivoCalRGB,string archivoCalIR)   ;

	//dibujos en tracking
	void dibujaEjes(Mat &img1,std::vector<cv::Point2f> corners1,vector<Point2f>imageFramePoints2);
	void dibujaCubo(Mat &img2,vector<Point2f> corners1,vector<Point2f>imageFramePoints2);
	
	// cambios de NAcho
	void cambiaCxCy(Mat &CM1,string tipoCal);
	
	public:
	bool found1,found2,finBucle;
	private:
	bool encontrado1,encontrado2;
	public:
	// Vectores de series de puntos del tablero
    vector<vector<Point3f> > object_points;	  //vector que almacena vectores de puntos 3D (float)
    vector<vector<Point2f> > imagePointsRGB, imagePointsIR;//vector que almacena vectores de puntos 2D (float)
    vector<vector<Point2f> > imagePointsGuardado1, imagePointsGuardado2;//vector que almacena vectores de puntos 2D (float)
    vector<Point2f> cornersRGB, cornersIR, cornersLectura1,cornersLectura2;
    vector<Point3f> obj;

	vector<Mat> rvecs, tvecs;

    
    int success,numBoards;

	/// IO
    FileStorage fs1;
    FileStorage fPuntosDer,fPuntosIzq,fPuntosLecturaDer,fPuntosLecturaIzq;
    FileNode NodoPuntosLectura;
    FileStorage archivocargarCalibracion;
    Size board_sz;
    char *fguardaResultados;
	string carpetaSalida;
    
    Mat CM1, CM2;
    Mat D1, D2;
    Mat R, T, E, F;
    Mat A,B,C,D;
    
	//para estereo calibrate
    Rect validRoi[2];
    
    Mat map1x, map1y, map2x, map2y;
    Mat imgU1, imgU2;
    
     //MAtrices de la rectificacion estereo
    Mat R1, R2, P1, P2, Q, T1, T2;

	//tamaño de la matriz
	Size tamanyoMatrizRGB,tamanyoMatrizIR;
    
    int exitos;
    
    int errorLectura;
    
    //Indica si se ha cargado un fichero de datos de calibracion
    bool carga;
    bool calibrado;
    
    //tamaños de tablero
    int columnas,filas;
    
    //Error de reproyeccion
    double errorRep;
    
    //tamaño imagenes
    int  colorancho,coloralto;
	 int  anchoIR,altoIR;
    
    ///////////////////////////////////////////////
    //ATENCION PRUEBA  !!!!
    //////////////////////////////////////////////
    //MAtrices ppara tracking
    //set up matrices for storage
	Mat gray2, one;
	Mat rvec, tvec,rvec2, tvec2;
	float tx,ty,tz;	//valores posicion damero
	float rx,ry,rz; //valores rotación angulso damero
	int cont;
	
	//vectores	 	
	vector<Point2f> imagenPuntos, imageFramePoints, imageOrigin;	//1
	vector<Point2f> imagenPuntos2, imageFramePoints2, imageOrigin2;	//2
	vector<Point3f> boardPoints, framePoints;
	vector<Point3f> boardPoints2, framePoints2;
	vector<Point3f> copia;
	Size cbSize;
	Mat copia1,copia2;
	
	bool tracking;
    bool bsensibilidadEsquinas;
	//carga puntos
	bool cargaPuntos;
	
	//opciones de calibrado
	int opcionCalibrado;
	
	//tamanyo cuadro del damero
	float tamcuadDamero;
	
	//numero de serie de una camara
	const char* numSerieCam1,*numSerieCam2;
	std::string argsCam1,argsCam2;
	string serieCam1,serieCam2;
	Mat ImIR,ImRGB;
	
	
	//ORDENES DE NACHO PARA METER EN EL XML DE CALIBRADO
	int tamImrecortadaAncho,tamImrecortadaAlto,tamImrecortadaAnchoBruto,tamImrecortadaAltoBruto;
	float desfasey,desfasex;

	//setter desfase y
	void setDesfasey(int desfasey);
	//setter desfase x
	void setDesfasex(int desfasex);
	//setter aumento
	void setAltoImRecortada(int tamImrecortadaAlto);
	//setter recorte
	void setAnchoImRecortada(int tamImrecortadaAncho);

	//setter aumento Bruto
	void setAltoImRecortadaBruto(int tamImrecortadaAltoBruto);
	//setter recorte bruto
	void setAnchoImRecortadaBruto(int tamImrecortadaAnchoBruto);

	//error tablero
	string tipoCal;
	void setTipoCal(string tipoCal);
	vector<int> indicesResultado;
		
		;

	 
	
	
	
	
	

};
