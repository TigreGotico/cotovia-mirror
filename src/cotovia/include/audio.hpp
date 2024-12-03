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
 
#ifndef AUDIO_WIN_HPP
#define AUDIO_WIN_HPP
#ifndef _WIN32
#include <alsa/asoundlib.h>
#else
#include <windows.h>
#endif
#define FS 16000
#define TIEMPO_BUF  60000   /*en milisegundos*/
#define BYTES_MUESTRA     2
#define TAM_BUF  FS * BYTES_MUESTRA * TIEMPO_BUF / 1000
#define N_BUF 4


#ifdef _WIN32
void CALLBACK waveOutProc(HWAVEIN waveOut, UINT uMsg, DWORD dwInstance,
		DWORD dwParam1, DWORD dwParam2);
#endif

class Audio {
	int error_audio;
	static int audio_abierto;

#ifndef _WIN32
	static snd_pcm_t * pcm_handle;   /// Handle for the PCM device  
	static int channels; /// number of channels for playback
	static const char * device; /// playback device
#else
	static HWAVEOUT       hWaveOut;
	static HPSTR          lpData[N_BUF];
	static LPWAVEHDR      lpWaveHdr[N_BUF];
#endif

	public:
	int AbreDispositivoAudio(unsigned int fs);
	int CierraDispositivoAudio(void);
	int EscribeAudio(char *datos_audio, unsigned int long_datos_audio);

};
#endif
