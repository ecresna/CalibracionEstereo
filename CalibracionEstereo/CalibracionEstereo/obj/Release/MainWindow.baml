   M S B A M L   `   `   `  ����   CalibracionEstereo#    CalibracionEstereo.MainWindow   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml       � Title$  
MainWindow��$��564.099��$��1334.47��.��� $��0,0,40,3q� J� RenderTransformOrigin$ 	0.5,0.544&�.+��� -   $��BotonRGB��$��carpeta RGB��$
��Left=�$��44��$��	33,58,0,0q�$	��Top=�$	��151���� -    ��Name$ BotonIR��$��
Carpeta IR��$
��Left=�$��41��$��
33,117,0,0q�$	��Top=�$	��151���� -   $��	label_RGB��$��carpeta RGB��$��8(�$
��Left=�$��34��$��
268,58,0,0q�$	��Top=�$	��227���� -   $��Start��$	��Go!��$
��Left=�$��71��$��
33,334,0,0q�$	��Top=�$	��227���� -    ��Name$ label_IR��$��
Carpeta IR��$��8(�$
��Left=�$��41��$��268,117,0,0q�$	��Top=�$	��227���� -   $��comboCamaras��$
��Left=�$��35��$��799,450,0,0q�$	��Top=�$	��151���� -   $ label_estado��$��Estado Actual��$
��Left=�$��56��$��
21,433,0,0q�$	��Top=�$	��439�����	#FFD0D4D3$��20(�$
��Bold"� �� TextAlignment$ Center=��� -   $ botonArchivoSettings��$��XML RGB��$
��Left=�$��43��$��
33,185,0,0q�$	��Top=�$	��151���� -	   $ labelXML_RGB��$��8(�$��archivo calibración��$
��Left=�$��43��$��268,176,0,0q�$	��Top=�$	��193���� -
   $��XML IR��$
��Left=�$��43��$��
33,241,0,0q�$	��Top=�$	��151���� -   $ labelXML_IR��$��8(�$��archivo calibración��$
��Left=�$��43��$��268,241,0,0q�$	��Top=�$	��193���� -    ��Name$  checkboxMatricesImportadas��$6��0Calibración Estéreo con matrices ya calculadas��$
��Left=�$��21��$��	21,20,0,0q�$	��Top=�$	��296��$
��Bold"��� -   $ checkBox_Calibracion��$ ��Calibración Estéreo AUTO��$
��Left=�$��21��$��
478,11,0,0q�$	��Top=�$	��182��$
��Bold"��� -   $ Boton_calibracion��$��carpeta Calibración RGB��$
��Left=�$��44��$��
495,59,0,0q�$	��Top=�$	��151���� -   $ label_calibracion��$��carpeta Calibración��$��10(�$
��Left=�$��34��$��
676,68,0,0q�$	��Top=�$	��429���� -   $��
comboFilas��$
��Left=�$��30��$��490,347,0,0q�$	��Top=�$��58���� -   $��	comboCols��$
��Left=�$��30��$��579,347,0,0q�$	��Top=�$��57���� $��filas��$
��Bold"�$
��Left=�$��27��$��503,306,0,0q�$	��Top=�$��58���� $
��cols��$
��Bold"�$
��Left=�$��27��$��592,306,0,0q�$	��Top=�$��58���� $��archivo��$
��Bold"�$
��Left=�$��27��$��691,306,0,0q�$	��Top=�$��58���� $��tamaño cuadro
��$
��Left=�$��27��$��786,306,0,0q�$	��Top=�$��98���� -   $��comboReescalado��$
��Left=�$��30��$��503,454,0,0q�$	��Top=�$	��217��.@��� $��Ninguno��$	��100��$	��100���� $��
Auto Nacho��$	��100��$	��100���� $��
Reescalado��$
��Bold"�$
��Left=�$��27��$��503,427,0,0q�$	��Top=�$	��110���� -   $ label_desdistorsion��$��carpeta Desdistorsión��$��10(�$
��Left=�$��34��$��500,117,0,0q�$	��Top=�$	��543���� -   $ Boton_calibracion_Copy��$��carpeta Calibración IR��$
��Left=�$��44��$��495,162,0,0q�$	��Top=�$	��151���� -   $ label_calibracion_IR��$��carpeta CalibraciónIR��$��10(�$
��Left=�$��34��$��676,171,0,0q�$	��Top=�$	��434���� -   $ label_desdistorsion_IR��$��carpeta DesdistorsiónIR��$��10(�$
��Left=�$��34��$��495,220,0,0q�$	��Top=�$	��548���� $��Cámaras disponibles��$
��Bold"�$
��Left=�$��27��$��799,418,0,0q�$	��Top=�$	��151���� $��Tipo de Calibración��$
��Bold"�$
��Left=�$��1120,37,0,0q�$	��Top=��� -   $��textBox_tamaño_cuadros��$
��Left=�$��35��$��799,347,0,0q� �� TextWrapping$ NoWrap=�$�� ��$	��Top=� �� TextAlignment$ Center=�$��58��$��14(��� $
��.png��$
��Left=�$��32��$��691,345,0,0q�$	��Top=�$��58��
��������� -   $#��checkBoxCalibracionIndividual��$��Calibracion Individual��$
��Left=�$��21��$��
478,37,0,0q�$	��Top=�$	��211��$
��Bold"��� -   $��tipoCal��$��70(�$�� ��$
��Left=�$	��144��$��1110,76,0,0q�$	��Top=�$	��141��