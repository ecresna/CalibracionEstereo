using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Forms;
using System.IO;
using System.Xml;
using System.Globalization;

using WrapperCalibracion;
using ns;


namespace CalibracionEstereo
{
    /// <summary>
    /// Lógica de interacción para MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        // variables carpetas y ficheros
        string path_carpeta_RGB = @"C:\IMAGENES\";
        string path_carpeta_IR = @"C:\IMAGENES\";
        string path_carpeta_XML_RGB = @"C:\BOOTH\Calib\Geo\RGB";
        string path_carpeta_XML_IR = @"C:\BOOTH\Calib\Geo\IR";
        string path_carpeta_Calibracion = @"C:\IMAGENES\";/*@"C:\IMAGENES\estereo\estereo\";*/
        string path_carpeta_camaras = @"camaras.xml";

        string nombre_carpeta_RGB;
        string nombre_carpeta_IR;
        string nombre_carpeta_Desdistorsion;//carpeta nueva para la calibracion única 
        string nombre_carpeta_CalibracionRGB;//carpeta nueva para la calibracion única 
        string nombre_carpeta_DesdistorsionRGB;//carpeta nueva para la calibracion única 
        string nombre_carpeta_CalibracionIR;//carpeta nueva para la calibracion única 
        string nombre_carpeta_DesdistorsionIR;//carpeta nueva para la calibracion única 
        string path_carpeta_salida;
        string nombre_archivo_xml_RGB;
        string nombre_archivo_xml_IR;
        string[] filePathsRGB, filePathsIR, filePathsCalibracion;

        //calculo calibracon
        bool archivoElegido = false;
        bool archivoCalibracionElegido = false;
        bool archivoSplinesElegido = false;
        bool elegidaCarpetasalida = false;
        bool sincamaras = false;
        bool elegidoArchivoSettings = false;
        bool archivoDesdistorsionElegido=false;

        //
        bool tipoReescaladoElegido = false;

        //matrices importar
        bool archivoXML_RGB = false;
        bool archivoXML_IR = false;


        //para leer los XML
        double puntoFocalXRGB, puntoFocalXIR, puntoFocalYRGB, puntoFocalYIR;
        double puntoXRGB, puntoXIR;
        double puntoYRGB, puntoYIR;

        //comprobador camaras
        string camaraSeleccionada;
        int numCamaraSeleccionada;
        //string[] modCamara = { "359821734147", "045846133747", "066417433747", "073733533547", "137277134947", "145250440247", "306939533947", "353464333947", "359821734147", "200175634547" };

        double distancia;

        //tamaño de reescalado de las imagenes
        //por defecto
         const int colsRGB = 1920;
         const int rowsRGB = 1080;
        

         const int colsIR = 512;
         const int rowsIR = 424;

         int cols, rows;
         
        float tamCuadro;
        int breescalado = 0;

        string tipoCalibracion;

        WrapperCalibracion.ClaseIntermedia intermedia;



        public MainWindow()
        {
            InitializeComponent();
            //List<string> modCamaras = new System.Collections.Generic.List<string>(modCamara);
            //modCamaras.ForEach(item => comboCamaras.Items.Add(item));
            rellenaCombofilas();
            rellenaComboCamaras();
            
           // comboReescalado.SelectedIndex = 0;
            textBox_tamaño_cuadros.Text = "24,5"; // cuadro de 30 mm
            tamCuadro =(float) Convert.ToDouble(textBox_tamaño_cuadros.Text);
            leerXMLCamaras(path_carpeta_camaras);
            //comboCamaras.bi
            intermedia = new ClaseIntermedia();

        }

        private void BotonRGB_Click(object sender, RoutedEventArgs e)
        {
            if (camaraSeleccionada != null)
            {
                // Displays an OpenFileDialog so the user can select a Cursor.
                FolderBrowserDialog dialogoEntrada = new FolderBrowserDialog();
                dialogoEntrada.RootFolder = Environment.SpecialFolder.MyComputer;
                dialogoEntrada.SelectedPath = path_carpeta_RGB;


                //Muestra el diálogo
                //Si se selecciona un fichero .byte
                //se guarda el nombre para hacer un bucle
                if (dialogoEntrada.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    //  nombre_archivo_entrada=openFileDialog1.FileName; // el fichero elegido con el path
                    nombre_carpeta_RGB = dialogoEntrada.SelectedPath; // solo el fichero elegido con su extension
                    label_RGB.Content = nombre_carpeta_RGB;
                    archivoElegido = true;

                }
            }
            else
            {
                System.Windows.MessageBox.Show("eliga primero cámara");
            }
        }

      

        private void BotonByte_Click(object sender, RoutedEventArgs e)
        {
            if (camaraSeleccionada != null)
            {
                // Displays an OpenFileDialog so the user can select a Cursor.
                FolderBrowserDialog dialogoEntrada = new FolderBrowserDialog();
             //   dialogoEntrada.SelectedPath = path_carpeta_IR;
                dialogoEntrada.RootFolder = Environment.SpecialFolder.MyComputer;
                dialogoEntrada.SelectedPath = path_carpeta_IR;


                //Muestra el diálogo
                //Si se selecciona un fichero .byte
                //se guarda el nombre para hacer un bucle
                if (dialogoEntrada.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    //  nombre_archivo_entrada=openFileDialog1.FileName; // el fichero elegido con el path
                    nombre_carpeta_IR = dialogoEntrada.SelectedPath; // solo el fichero elegido con su extension
                    label_IR.Content = nombre_carpeta_IR;
                    elegidaCarpetasalida = true;

                }
            }
            else
            {
                System.Windows.MessageBox.Show("eliga primero cámara");
            }
        }

        // Si se pulsa el boton de calibracion
        private void Boton_calibracion_Click(object sender, RoutedEventArgs e)
        {

            if (camaraSeleccionada != null && tipoReescaladoElegido)
            {
                seleccionTipoCalibracion(0);//RGB
                // Displays an OpenFileDialog so the user can select a Cursor.
                FolderBrowserDialog dialogoEntrada = new FolderBrowserDialog();
                dialogoEntrada.RootFolder = Environment.SpecialFolder.MyComputer;
                dialogoEntrada.SelectedPath = path_carpeta_Calibracion + camaraSeleccionada.Substring(0, 4);


                //Muestra el diálogo
                //Si se selecciona un fichero .byte
                //se guarda el nombre para hacer un bucle
                if (dialogoEntrada.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    //  nombre_archivo_entrada=openFileDialog1.FileName; // el fichero elegido con el path
                    nombre_carpeta_CalibracionRGB = dialogoEntrada.SelectedPath; // solo el fichero elegido con su extension
                    label_calibracion.Content = nombre_carpeta_CalibracionRGB;
                    archivoCalibracionElegido = true;
                    //desdist
                    if (breescalado == 0)
                    {
                        //existe esa carpeta??Si no, se crea.                        
                        nombre_carpeta_Desdistorsion = nombre_carpeta_CalibracionRGB + "\\desdist";
                        if (System.IO.Directory.Exists(nombre_carpeta_Desdistorsion))
                        {
                            nombre_carpeta_DesdistorsionRGB = nombre_carpeta_CalibracionRGB + "\\desdist";
                            label_desdistorsion.Content = nombre_carpeta_DesdistorsionRGB;
                        }
                        else // si no se crea 
                        {
                            System.IO.Directory.CreateDirectory(nombre_carpeta_Desdistorsion);
                            nombre_carpeta_DesdistorsionRGB = nombre_carpeta_Desdistorsion;
                            label_desdistorsion.Content = nombre_carpeta_DesdistorsionRGB;
                        }

                        
                    }
                    else
                    {
                        //existe esa carpeta??Si no, se crea.  
                        nombre_carpeta_Desdistorsion = nombre_carpeta_CalibracionRGB+"\\desdist2";
                        if (System.IO.Directory.Exists(nombre_carpeta_Desdistorsion))
                        {
                            nombre_carpeta_DesdistorsionRGB = nombre_carpeta_Desdistorsion;
                            label_desdistorsion.Content = nombre_carpeta_DesdistorsionRGB;
                        }
                        else // si no se crea 
                        {
                            System.IO.Directory.CreateDirectory(nombre_carpeta_Desdistorsion);
                            nombre_carpeta_DesdistorsionRGB = nombre_carpeta_Desdistorsion;
                            label_desdistorsion.Content = nombre_carpeta_DesdistorsionRGB;
                        }
                        

                    }

                }
            }
            else
            {
                System.Windows.MessageBox.Show("elija primero cámara o tipo de reescalado");
            }
        }

        private void Boton_calibracionIR_Click(object sender, RoutedEventArgs e)
        {
            if (camaraSeleccionada != null)
            {
                seleccionTipoCalibracion(1);//IR
                // Displays an OpenFileDialog so the user can select a Cursor.
                FolderBrowserDialog dialogoEntrada = new FolderBrowserDialog();
                dialogoEntrada.RootFolder = Environment.SpecialFolder.MyComputer;
                dialogoEntrada.SelectedPath = path_carpeta_Calibracion + camaraSeleccionada.Substring(0, 4);


                //Muestra el diálogo
                //Si se selecciona un fichero .byte
                //se guarda el nombre para hacer un bucle
                if (dialogoEntrada.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    //  nombre_archivo_entrada=openFileDialog1.FileName; // el fichero elegido con el path
                    nombre_carpeta_CalibracionIR = dialogoEntrada.SelectedPath; // solo el fichero elegido con su extension
                    label_calibracion_IR.Content = nombre_carpeta_CalibracionIR;
                    archivoCalibracionElegido = true;

                    //desdist
                    if (breescalado == 0)
                    {
                        //existe esa carpeta??Si no, se crea.
                        nombre_carpeta_Desdistorsion = nombre_carpeta_CalibracionIR + "\\desdist";
                        if (System.IO.Directory.Exists(nombre_carpeta_Desdistorsion))
                        {
                            nombre_carpeta_DesdistorsionIR = nombre_carpeta_CalibracionIR + "\\desdist";
                            label_desdistorsion_IR.Content = nombre_carpeta_DesdistorsionIR;
                        }
                        else {
                            System.IO.Directory.CreateDirectory(nombre_carpeta_Desdistorsion);
                        
                        }
                        
                    }
                    else
                    {
                        //existe esa carpeta??Si no, se crea.
                        nombre_carpeta_Desdistorsion = nombre_carpeta_CalibracionIR + "\\desdist2";
                        if (System.IO.Directory.Exists(nombre_carpeta_Desdistorsion))
                        {
                            nombre_carpeta_DesdistorsionIR = nombre_carpeta_Desdistorsion;
                            label_desdistorsion_IR.Content = nombre_carpeta_Desdistorsion;
 
                        }
                        else
                        {
                            System.IO.Directory.CreateDirectory(nombre_carpeta_Desdistorsion);

                        }
                        

                    }

                }
            }
            else {
                System.Windows.MessageBox.Show("elija primero cámara");
            }
        }
        private void ListaTareas()
        {
            if ( checkBox_Calibracion.IsChecked == true)
            {
                // 1- 1ª calibracion RGB e IR 
                checkBox_Calibracion.IsChecked = true;
                label_estado.Content = "Calibracion RGB...";
                //RGB

                seleccionTipoCalibracion(0);
                checkBoxCalibracionIndividual.IsChecked = true;
                comboReescalado.SelectedIndex = 1;
                archivoDesdistorsionElegido = true;
                nombre_carpeta_Desdistorsion = nombre_carpeta_CalibracionRGB + "\\desdist";
                nombre_carpeta_DesdistorsionRGB = nombre_carpeta_CalibracionRGB + "\\desdist";
                label_desdistorsion.Content = nombre_carpeta_DesdistorsionRGB;
                comienzo();
                //IR
                label_estado.Content = "Calibracion IR...";

                seleccionTipoCalibracion(1);
                checkBoxCalibracionIndividual.IsChecked = true;
              //  comboReescalado.SelectedIndex = 1;
                archivoDesdistorsionElegido = true;
                nombre_carpeta_Desdistorsion = nombre_carpeta_CalibracionIR + "\\desdist";
                nombre_carpeta_DesdistorsionIR = nombre_carpeta_CalibracionIR + "\\desdist";
                label_desdistorsion_IR.Content = nombre_carpeta_DesdistorsionIR;
                existenFolders(nombre_carpeta_DesdistorsionIR);
                comienzo();

                // 2- 2ª calibracion RGB e IR 
                //RGB
                //label_estado.Content = "Calibracion RGB reescalada...";
                //seleccionTipoCalibracion(0);
                //checkBoxCalibracionIndividual.IsChecked = true;
                //comboReescalado.SelectedIndex = 0;
                ////      comboReescalado.SelectedIndex = 1;
                //nombre_carpeta_CalibracionRGB = nombre_carpeta_CalibracionRGB + "\\desdist"; ;
                //nombre_carpeta_Desdistorsion = nombre_carpeta_CalibracionRGB.Substring(0, nombre_carpeta_CalibracionRGB.LastIndexOf("\\")) + "\\desdist2";
                //existenFolders(nombre_carpeta_Desdistorsion);
                ////Existen las carpetas?
                //existenFolders(nombre_carpeta_Desdistorsion);
                ////   nombre_carpeta_DesdistorsionRGB = nombre_carpeta_Desdistorsion;

                //comienzo();
                ////IR
                //label_estado.Content = "Calibracion IR reescalada...";

                //seleccionTipoCalibracion(1);
                //checkBoxCalibracionIndividual.IsChecked = true;
                //comboReescalado.SelectedIndex = 0;
                ////     comboReescalado.SelectedIndex = 1;
                //nombre_carpeta_CalibracionIR = nombre_carpeta_CalibracionIR + "\\desdist";
                //nombre_carpeta_Desdistorsion = nombre_carpeta_CalibracionIR + "2";
                //nombre_carpeta_DesdistorsionIR = nombre_carpeta_Desdistorsion;
                ////Existen las carpetas?
                //existenFolders(nombre_carpeta_Desdistorsion);
                //comienzo();

                //3ª estéreo
                nombre_carpeta_DesdistorsionRGB = nombre_carpeta_DesdistorsionRGB.Substring(0, nombre_carpeta_DesdistorsionRGB.LastIndexOf("\\"));

                label_estado.Content = "Estéreo...";
                checkboxMatricesImportadas.IsChecked = true;
              //  comboReescalado.SelectedIndex = 0;
                  comboReescalado.SelectedIndex = 1;
                nombre_carpeta_RGB = nombre_carpeta_CalibracionRGB;
                nombre_carpeta_IR = nombre_carpeta_CalibracionIR;
                archivoXML_RGB = true; archivoXML_IR = true; archivoElegido = true; elegidaCarpetasalida = true;
                nombre_archivo_xml_RGB = @"C:\BOOTH\Calib\Geo\RGB\" + "calibracionCal_RGB_" + camaraSeleccionada + "_reescalada.xml";
                nombre_archivo_xml_IR = @"C:\BOOTH\Calib\Geo\IR\" + "calibracionCal_IR_" + camaraSeleccionada + "_reescalada.xml";
                comienzo();
            }
            else 
            {
                comienzo();
            
            }
        }
        private void comienzo()
        {
            label_estado.Content = "Mira camaras";
            label_estado.Background = Brushes.Green;
            miraCamarasElegidas();//se comprueba que camaras han sido elegidas
            label_estado.Content = "Calibracion";
            label_estado.Background = Brushes.Orange;
            /////////////////////////////
            ///Calibracion Individual
            ////////////////////////////////

            if (checkBoxCalibracionIndividual.IsChecked.Value)
            {

                if (archivoCalibracionElegido)
                {
                    //*.png
                    if (importaArchivosenCarpetaCalibracion() > 1)//importa los nombres de los archivos de las carpetas seleccionadas
                    {
                        String nombre = tipoCalibracion;
                        
                        intermedia.inicializacion(filePathsCalibracion, cols, rows, comboCols.SelectedIndex + 5, comboFilas.SelectedIndex + 5, nombre, nombre_carpeta_Desdistorsion, camaraSeleccionada, breescalado, puntoYRGB, puntoXIR);
                        label_estado.Content = "FIN";
                        label_estado.Background = Brushes.Red;
                    }

                }
                else
                {
                    System.Windows.MessageBox.Show("Falta elegir una carpeta");
                }
            }



            //////////////////////////////////////////////////
            //// Calibracion desde CERO
            //////////////////////////////////////////////////

            //if (checkBoxCalculoCalibracion.IsChecked.Value)
            //{
            //    importaArchivosenCarpeta();//importa los nombres de los archivos de las carpetas seleccionadas
            //    if (archivoElegido && elegidaCarpetasalida)
            //    {
            //        WrapperCalibracion.ClaseIntermedia intermedia = new ClaseIntermedia(filePathsRGB, filePathsIR, cols, rows, comboCols.SelectedIndex + 5, comboFilas.SelectedIndex + 5, tamCuadro, camaraSeleccionada);
            //        label_estado.Content = "FIN";
            //        label_estado.Background = Brushes.Red;

            //    }
            //    else
            //    {
            //        System.Windows.MessageBox.Show("Falta elegir una carpeta");
            //    }
            //}

            /////////////////////////////////////////////////
            //Calibracion Con matrices de camara existentes
            /////////////////////////////////////////////////
            if (checkboxMatricesImportadas.IsChecked.Value)
            {
                importaArchivosenCarpeta();//importa los nombres de los archivos de las carpetas seleccionadas
                if (archivoXML_RGB && archivoXML_IR && archivoElegido && elegidaCarpetasalida)
                {
                    WrapperCalibracion.ClaseIntermedia intermedia = new ClaseIntermedia(filePathsRGB, filePathsIR, nombre_archivo_xml_RGB, nombre_archivo_xml_IR, cols, rows, comboCols.SelectedIndex + 5, comboFilas.SelectedIndex + 5, tamCuadro, breescalado, camaraSeleccionada, puntoYRGB, puntoXIR);
                    label_estado.Content = "FIN";
                    label_estado.Background = Brushes.Red;

                }
                else
                {
                    System.Windows.MessageBox.Show("Falta elegir un archivo de calibracion");
                }

            }
            if (!checkboxMatricesImportadas.IsChecked.Value && !checkBox_Calibracion.IsChecked.Value && !checkBoxCalibracionIndividual.IsChecked.Value)
            {
                System.Windows.MessageBox.Show("Falta elegir un tipo de calibracion !!");

            }
        }

        private void Start_Click(object sender, RoutedEventArgs e)
        {
            label_estado.Content = "Comienzo";
            label_estado.Background = Brushes.Green;
            ListaTareas();
           // comienzo();
            
        }

        public void miraCamarasElegidas()
        {
            if (comboCamaras.SelectedIndex == -1)
            {
                System.Windows.Forms.MessageBox.Show("no se ha elegido ninguna camara");
                sincamaras = true;
            }

        }

        public void rellenaCombofilas()
        {
            List<string> lista = new List<string>();         

            
            for (int i = 5; i < 25; i++)
            {
                lista.Add(i.ToString());
                
            }         

            this.comboCols.ItemsSource = lista;
            this.comboFilas.ItemsSource = lista;
            comboCols.SelectedIndex = 22-5;
            comboFilas.SelectedIndex = 13-5;
           
            //comboNombre.SelectedIndex = 0;
            tamCuadro = 24.5f;//medida del damero para estereo (cuadros grandes)
        }

        public void rellenaComboCamaras()
        {
            List<string> lista = new List<string>();


           
                lista.Add("Cal_RGB");
                lista.Add("Cal_IR");
            

           // this.comboNombre.ItemsSource = lista;
        }

       

        public int importaArchivosenCarpeta()
        {
            NumericComparer ns = new NumericComparer();
            //para RGB
           filePathsRGB = Directory.GetFiles(nombre_carpeta_RGB, "*.png");
           Array.Sort(filePathsRGB, ns);
            //para IR
           filePathsIR = Directory.GetFiles(nombre_carpeta_IR, "*.png");
           Array.Sort(filePathsIR, ns);
           
           return filePathsIR.Length; 
 
        }

        public int importaArchivosenCarpetaCalibracion()
        {
            if (filePathsCalibracion!=null)
            {
                int cont = 0;
                foreach (String value in filePathsCalibracion)
                {
                    filePathsCalibracion[cont] = "";
                    cont++;
                }
                filePathsCalibracion.Initialize();
            }
            //para RGB
            string modoFichero = tipoCalibracion;
            if (modoFichero == "Cal_RGB")
            {
                NumericComparer ns = new NumericComparer();

                try
                {
                    filePathsCalibracion = Directory.GetFiles(nombre_carpeta_CalibracionRGB, "*.png",SearchOption.TopDirectoryOnly);

                    Array.Sort(filePathsCalibracion, ns);
                }
                catch (Exception e)
                {

                    System.Windows.MessageBox.Show("No se ha encontrado ninguna imagen RGB", "Confirmation", MessageBoxButton.OK);
                    return 0;
                }
            }
            else {
                NumericComparer ns = new NumericComparer();
                try
                {
                    filePathsCalibracion = Directory.GetFiles(nombre_carpeta_CalibracionIR, "*.png");
                    Array.Sort(filePathsCalibracion, ns);
                }
                catch (Exception e)
                {

                    System.Windows.MessageBox.Show("No se ha encontrado ninguna imagen IR", "Confirmation", MessageBoxButton.OK);
                    return 0;
                }
 
            }
            return filePathsCalibracion.Length; 
                

        }

        private void comboCamaras_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            camaraSeleccionada = comboCamaras.SelectedItem.ToString();
            numCamaraSeleccionada = comboCamaras.SelectedIndex;
            sincamaras = false;
        }

        //boton archivo XML RGB
        private void botonArchivoSettings_Click(object sender, RoutedEventArgs e)
        {
            // Displays an OpenFileDialog so the user can select a Cursor.
            OpenFileDialog dialogoEntrada = new OpenFileDialog();
            dialogoEntrada.Filter = "ficheros imagen|*.xml";
            dialogoEntrada.Title = "Selecciona un fichero de propiedades";
            dialogoEntrada.InitialDirectory = path_carpeta_XML_RGB;

            //Muestra el diálogo
            //Si se selecciona un fichero .byte
            //se guarda el nombre para hacer un bucle
            if (dialogoEntrada.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                //  nombre_archivo_entrada=openFileDialog1.FileName; // el fichero elegido con el path
                nombre_archivo_xml_RGB = dialogoEntrada.FileName; // el archivo con su path
                labelXML_RGB.Content = nombre_archivo_xml_RGB;

                archivoXML_RGB = true;

            }
        }

        //boton archivo XML IR
        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            // Displays an OpenFileDialog so the user can select a Cursor.
            OpenFileDialog dialogoEntrada = new OpenFileDialog();
            dialogoEntrada.Filter = "ficheros imagen|*.xml";
            dialogoEntrada.Title = "Selecciona un fichero de propiedades";
            dialogoEntrada.InitialDirectory = path_carpeta_XML_IR;

            //Muestra el diálogo
            //Si se selecciona un fichero .byte
            //se guarda el nombre para hacer un bucle
            if (dialogoEntrada.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                //  nombre_archivo_entrada=openFileDialog1.FileName; // el fichero elegido con el path
                nombre_archivo_xml_IR = dialogoEntrada.FileName; // el archivo con su path
                labelXML_IR.Content = nombre_archivo_xml_IR;

                archivoXML_IR = true;

            }

        }

       

        public void leerXMLCamaras(string path)
        {
               

            System.IO.StreamReader fileLeerCamaras =
                new System.IO.StreamReader(path);

            lecturaElementosCamaras(fileLeerCamaras);

        }

        public void lecturaElementosCamaras(StreamReader ficheroXmlCamaras)
        {
            StringBuilder output = new StringBuilder();


            String xmlStringCamaras = ficheroXmlCamaras.ReadToEnd();
            


            // Create an XmlReader
            using (XmlReader reader = XmlReader.Create(new StringReader(xmlStringCamaras)))
            {
                //RGB
                reader.ReadToFollowing("Camaras");
                reader.MoveToFirstAttribute();
                string genre = reader.Value;
                //     output.AppendLine("The genre value: " + genre);

                reader.ReadToFollowing("data");
                output.AppendLine(reader.ReadElementContentAsString());
            }
            

            //  OutputTextBlock.Text = output.ToString();
            NumberFormatInfo provider = new NumberFormatInfo();
            provider.NumberDecimalSeparator = ".";
            //    provider.NumberGroupSeparator = ".";

            string camaras = output.ToString();
           
            //separamos los valores de la matriz
            string[] matriz = camaras.Split(',');
            List<string> modCamaras = new System.Collections.Generic.List<string>(matriz);
            modCamaras.ForEach(item => comboCamaras.Items.Add(item));       


        }

        public void lecturaElementosAtributos(StreamReader ficheroXmlRGB, StreamReader ficheroXmlIR)
        {
            StringBuilder output = new StringBuilder();
            StringBuilder output2 = new StringBuilder();

            String xmlStringRGB = ficheroXmlRGB.ReadToEnd();
            String xmlStringIR = ficheroXmlIR.ReadToEnd();


            // Create an XmlReader
            using (XmlReader reader = XmlReader.Create(new StringReader(xmlStringRGB)))
            {
                //RGB
                reader.ReadToFollowing("M1");
                reader.MoveToFirstAttribute();
                string genre = reader.Value;
                //     output.AppendLine("The genre value: " + genre);

                reader.ReadToFollowing("data");
                output.AppendLine(reader.ReadElementContentAsString());
            }
            using (XmlReader reader2 = XmlReader.Create(new StringReader(xmlStringIR)))
            {


                // IR
                reader2.ReadToFollowing("M1");
                reader2.MoveToFirstAttribute();
                string genre2 = reader2.Value;
                //     output.AppendLine("The genre value: " + genre);

                reader2.ReadToFollowing("data");
                output2.AppendLine(reader2.ReadElementContentAsString());
            }

            //  OutputTextBlock.Text = output.ToString();
            NumberFormatInfo provider = new NumberFormatInfo();
            provider.NumberDecimalSeparator = ".";
            //    provider.NumberGroupSeparator = ".";

            string valoresMatriz = output.ToString();
            string valoresMatriz2 = output2.ToString();
            //separamos los valores de la matriz
            string[] matriz = valoresMatriz.Split(' ');
            string[] matriz2 = valoresMatriz2.Split(' ');

            puntoFocalXRGB = Convert.ToDouble(matriz[4], provider);//FxRGB
            puntoFocalYRGB = Convert.ToDouble(matriz[11], provider);//FyRGB
            puntoXRGB = Convert.ToDouble(matriz[6], provider);//cxRGB
            puntoYRGB = Convert.ToDouble(matriz[12], provider);//cyRGB

            puntoFocalXIR = Convert.ToDouble(matriz2[4], provider);//FxIR
            puntoFocalYIR = Convert.ToDouble(matriz2[11], provider);//FyIR
            puntoXIR = Convert.ToDouble(matriz2[6], provider);//cxIR
            puntoYIR = Convert.ToDouble(matriz2[12], provider);//cyIR
            //
            intermedia.setMatricesCamara( puntoFocalXRGB,  puntoFocalXIR, puntoFocalYRGB, puntoFocalYIR, puntoXRGB,  puntoXIR,  puntoYRGB, puntoYIR);


        }

        private void checkBox_Calibracion_Checked(object sender, RoutedEventArgs e)
        {
            if (checkBox_Calibracion.IsChecked.Value == true)
            {
                checkBoxCalibracionIndividual.IsChecked = false;
                checkboxMatricesImportadas.IsChecked = false;
               // tamCuadro = 30.0f;//medida del damero para estereo (cuadros grandes)
                comboReescalado.SelectedIndex = 0; //ahora sólo vale sin reescalado , ya loestán
            }
        }

       

        private void checkboxMatricesImportadas_Checked(object sender, RoutedEventArgs e)
        {
            if (checkboxMatricesImportadas.IsChecked.Value == true)
            {
                checkBox_Calibracion.IsChecked = false;
                checkBoxCalibracionIndividual.IsChecked = false;
             //   tamCuadro = 30.0f;//medida del damero para estereo (cuadros grandes)
//comboCols.SelectedIndex = 9 - 5;
              //  comboFilas.SelectedIndex = 6 - 5;
            }
        }

        // cambio de Calibracion RGB a IR y viceversa
       
        private void seleccionTipoCalibracion(int valor)
        {
            if (valor == 1)
            {

             //   comboCols.SelectedIndex = 9 - 5;
             //   comboFilas.SelectedIndex = 6 - 5;
                //     tamCuadro = 35.5f;//medida del damero para estereo (cuadros grandes)
                cols = colsIR;
                rows = rowsIR;
                tipoCalibracion = "Cal_IR";
                tipoCal.Content = "IR";
            }
            else
            {
             //   comboCols.SelectedIndex = 9 - 5;
             //   comboFilas.SelectedIndex = 6 - 5;
                //    tamCuadro = 24.5f;//medida del damero para estereo (cuadros grandes)
                cols = colsRGB;
                rows = rowsRGB;
                tipoCalibracion = "Cal_RGB";
                tipoCal.Content = "RGB";
            }
        }

        private void comboCols_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var indice=comboCols.SelectedIndex;
        }

        

        private void comboReescalado_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            breescalado = comboReescalado.SelectedIndex;
            tipoReescaladoElegido = true;

            if (tipoCalibracion == "Cal_RGB")
            {
                if (breescalado == 0)
                {
                    cols = colsRGB;
                    rows = rowsRGB;
                }
             
            }
            else if (tipoCalibracion == "Cal_IR")
            {
                if (breescalado == 0)
                {
                    cols = colsIR;
                    rows = rowsIR;
                }
               
            }
            

        }

    

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            if (camaraSeleccionada != null)
            {
                // Displays an OpenFileDialog so the user can select a Cursor.
                FolderBrowserDialog dialogoSalida = new FolderBrowserDialog();
                dialogoSalida.RootFolder = Environment.SpecialFolder.MyComputer;
                dialogoSalida.SelectedPath = path_carpeta_Calibracion + camaraSeleccionada.Substring(0, 4);


                //Muestra el diálogo
                //Si se selecciona un fichero .byte
                //se guarda el nombre para hacer un bucle
                if (dialogoSalida.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    //  nombre_archivo_entrada=openFileDialog1.FileName; // el fichero elegido con el path
                    nombre_carpeta_DesdistorsionRGB = dialogoSalida.SelectedPath; // solo el fichero elegido con su extension
                    label_desdistorsion.Content = nombre_carpeta_DesdistorsionRGB;
                    archivoDesdistorsionElegido = true;

                }
            }
            else
            {
                System.Windows.MessageBox.Show("elija primero cámara");
            }
        }
        private void ButtonIR_Click_2(object sender, RoutedEventArgs e)
        {
            if (camaraSeleccionada != null)
            {
                // Displays an OpenFileDialog so the user can select a Cursor.
                FolderBrowserDialog dialogoSalida = new FolderBrowserDialog();
                dialogoSalida.RootFolder = Environment.SpecialFolder.MyComputer;
                dialogoSalida.SelectedPath = path_carpeta_Calibracion + camaraSeleccionada.Substring(0, 4);


                //Muestra el diálogo
                //Si se selecciona un fichero .byte
                //se guarda el nombre para hacer un bucle
                if (dialogoSalida.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    //  nombre_archivo_entrada=openFileDialog1.FileName; // el fichero elegido con el path
                    nombre_carpeta_DesdistorsionIR = dialogoSalida.SelectedPath; // solo el fichero elegido con su extension
                    label_desdistorsion_IR.Content = nombre_carpeta_DesdistorsionIR;
                    archivoDesdistorsionElegido = true;

                }
            }
            else
            {
                System.Windows.MessageBox.Show("elija primero cámara");
            }
        }

        private void comboFilas_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        private void checkBoxCalibracionIndividual_Checked(object sender, RoutedEventArgs e)
        {
            if (checkBoxCalibracionIndividual.IsChecked.Value == true)
            {
                checkBox_Calibracion.IsChecked = false;
                checkboxMatricesImportadas.IsChecked = false;
                //   tamCuadro = 30.0f;//medida del damero para estereo (cuadros grandes)
                //   comboCols.SelectedIndex = 9 - 5;
                //   comboFilas.SelectedIndex = 6 - 5;
            }
        }
        private bool existenFolders(String folder)
        {
            if (!System.IO.Directory.Exists(folder))
            {
                System.IO.Directory.CreateDirectory(folder);
            }
            return true;
        }

        

       

      

       

       
    }
}
