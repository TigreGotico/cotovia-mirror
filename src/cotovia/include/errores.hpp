/*
Cotovia: Text-to-speech system for Galician and Spanish languages.

Developed by the Multimedia Technologies Group at the University of Vigo (Spain)
 and the Center 'Ramón Piñeiro' for Research in Humanities.
http://webs.uvigo.es/gtm_voz
http://www.cirp.es

Copyright: 1996-2012  Multimedia Technologies Group, University of Vigo, Spain
           1996-2012 Centro Ramon Pineiro para a Investigación en Humanidades,
                     Xunta de Galicia, Santiago de Compostela, Spain

License: GPL-3.0+
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 .
 This package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 .
 On Debian systems, the complete text of the GNU General
 Public License version 3 can be found in /usr/share/common-licenses/GPL-3.

______________________________________________________________________________*/
 
 
//-----------------------------------------------------------------------------
//-             Definición dos erros que devolve a DLL.                       -
//-----------------------------------------------------------------------------
#define   OK                            0  // Si se completou ben a sintesis
#define   ERROR_GENERICO            -9999  // Erro xenerico
#define   ERROR_PARAMETROS          -9998  // Erro nos parametros de chamada

//............................................................................
//.    Erros que poden suceder na apertura dos arquivos da base de voz       .
//............................................................................
#define   ERROR_FICHERO_DE_ENTRADA  -1000  // Non hai arquivo para sintetizar
#define   ERROR_FICH_SEGMENTACION   -1001  // Erro na apertura da base_voz .SEG
#define   ERROR_FICH_BINARIO        -1002  // Erro na apertura da base_voz .BIN
#define   ERROR_FICH_INDICE         -1003  // Erro na apertura da base_voz .IND

//............................................................................
//.    Erros que poden suceder nos arquivos de saida                         .
//............................................................................
#define   ERROR_CREAR_FICHERO_WAV   -1004  // Erro na apertura do arquivo wav
#define   ERROR_CREAR_FICHERO_LOG   -1005  // Erro na creacion do arquivo *.log
#define   ERROR_CREAR_FICHERO_PRO   -1006  // Erro o crear o arquivo de Prosodia
#define   ERROR_CREAR_FICHERO_LIN   -1007  // Erro o crear arquivo de linguistica
#define   ERROR_CREAR_FICHERO_TRA   -1008  // Erro o crear arquivo de transcicion 
#define   ERROR_CREAR_FICHERO_PRE   -1009  // Erro o crear arquivo de preprocesado 
#define   ERROR_CREAR_FICHERO_PRS   -1010  // Erro o crear arquivo de prosodia 

//............................................................................
//.    Erros que poden suceder por problemas coa memoria                     .
//............................................................................
#define   MEMORIA_INSUFICIENTE      -1100  // Erro o reservar memoria
#define   ERROR_SINTESIS            -1101  // Erro no proceso de sintese
#define   ERROR_SINTESIS_MEMORIA    -1102  // Erro na reserva de memoria da sintese
#define   ERROR_FRASE_VACIA         -1110  // Erro se a frase esta valeira
#define   ERROR_COPIA_STRUCTS       -1111  // Polo manexo de memoria na copia entre structs

//-----------------------------------------------------------------------------
//-        Definición dos erros propios da analise automatizada.              -
//-----------------------------------------------------------------------------
#define   ERROR_DE_SINTAXIS            -1 // Erro sintaxis na chamada do programa
#define   ERROR_APERTURA_ARQUIVO       -2 // Erro en apertura de arquivos
#define   ERROR_ASIGNACION_MEMORIA     -3 // Erro na reserva de memoria
#define   ERROR_XENERICO               -4 // Erro xenerico con mensaxe por pantalla
#define   ERROR_IRREGULARES            -5 // Erro na busca de inf nos irregulares
#define   ERROR_FECHAS               -100 // Erro: formato de data mal escrito
 

