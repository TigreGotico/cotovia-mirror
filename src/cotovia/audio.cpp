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
 
/**
 * \file
 * \author fmendez
 * \brief Manejo del dispositivo de audio
 */

#include <stdio.h>
#include <stdlib.h>
#include "modos.hpp"
#include "audio.hpp"

int Audio::audio_abierto = 0;
#ifndef _WIN32
snd_pcm_t * Audio::pcm_handle;          
const char * Audio::device="plug:dmix"; 
int Audio::channels = 1;
#else
HWAVEOUT       Audio::hWaveOut;
HPSTR          Audio::lpData[N_BUF];
LPWAVEHDR      Audio::lpWaveHdr[N_BUF];
//int            BufferesLibres=N_BUF;
static HANDLE         Semaforo;
#endif


/** 
 * \author fmendez
 * \brief Abre la tarjeta de sonido para escritura (reproducción)
 * 
 * \param fs Frecuencia de muestreo
 * 
 * \return 0 en ausencia de error
 */
int Audio::AbreDispositivoAudio(unsigned int fs) {
	int err;
	if (audio_abierto) {
		audio_abierto++;
		error_audio = 0;
		return 0;
	}
#ifndef _WIN32

	if ((err = snd_pcm_open(&pcm_handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		fprintf(stderr, "Audio: Error opening PCM device %s: %s\n", device,  snd_strerror(err));
		error_audio=1;
		return(-1);
	}

	if ((err = snd_pcm_set_params(pcm_handle,SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, channels, fs, 1, TIEMPO_BUF * 1000)) < 0) {
		fprintf(stderr,"Audio: snd_pcm_set_params error: %s\n", snd_strerror(err));
		error_audio=1;
		snd_pcm_close (pcm_handle);
		return(-1);
	}


#else //WIN32

	WAVEFORMATEX   Formato;
	int i;

	if (NULL == (Semaforo = CreateSemaphore(
					NULL,   // no security attributes
					N_BUF,   // initial count
					N_BUF,   // maximum count
					NULL))  // unnamed semaphore
		 ) {
		error_audio=1;
		return -1;
	}

	Formato.wFormatTag       = WAVE_FORMAT_PCM;
	Formato.nChannels        = 1;
	Formato.nSamplesPerSec   = fs;
	Formato.nAvgBytesPerSec  = Formato.nSamplesPerSec * BYTES_MUESTRA;
	Formato.nBlockAlign      = 2;
	Formato.wBitsPerSample   = BYTES_MUESTRA * 8;
	Formato.cbSize           = 0;
	/*	if (difonemas == 1) {
			Formato.nSamplesPerSec   = FS;
			Formato.nAvgBytesPerSec  = FS*BYTES_MUESTRA;
			} // if (difonemas == 1)
			else {
			Formato.nSamplesPerSec   = locutor_actual->frecuencia_muestreo;
			Formato.nAvgBytesPerSec  = locutor_actual->frecuencia_muestreo*BYTES_MUESTRA;
			} // else
			if (ocho_khz) {
			Formato.nSamplesPerSec/=2;
			Formato.nAvgBytesPerSec/=2;
			}
			*/
	if (waveOutOpen((LPHWAVEOUT) &hWaveOut,WAVE_MAPPER,(LPWAVEFORMATEX) &Formato,
				(DWORD) waveOutProc, 0, CALLBACK_FUNCTION)){
		error_audio=1;
		return -1;
	}
	/*se preparan los N_BUF buferes */

	int tam_buf = fs * BYTES_MUESTRA * TIEMPO_BUF / 1000;

	for(i=0;i<N_BUF;i++){
		if ((lpData[i]=(HPSTR) malloc(tam_buf))== NULL) {
			error_audio=1;
			waveOutClose(hWaveOut);
			return -1;
		}
		if ((lpWaveHdr[i]=(LPWAVEHDR) malloc(sizeof(WAVEHDR)))== NULL) {
			error_audio=1;
			waveOutClose(hWaveOut);
			return -1;
		}
		lpWaveHdr[i]->lpData = lpData[i];
		lpWaveHdr[i]->dwBufferLength = tam_buf;
		lpWaveHdr[i]->dwFlags = 0;
		lpWaveHdr[i]->dwLoops = 0;
		if (waveOutPrepareHeader(hWaveOut,lpWaveHdr[i],sizeof(WAVEHDR))){
			error_audio=1;
			waveOutClose(hWaveOut);
			return -1;
		}
	}
#endif
	audio_abierto++;
	error_audio = 0;
	return 0;
}

/** 
 * \author fmendez
 * \brief Cierra el dispositivo de audio
 * 
 * \return 0 en ausencia de error
 */
int Audio::CierraDispositivoAudio(void) {

	audio_abierto--;

	if (audio_abierto){
		return 0;
	}

#ifndef _WIN32
	if (!error_audio)
		snd_pcm_close (pcm_handle);
#else

	int i=0;

	while (i<N_BUF) {
		WaitForSingleObject(Semaforo,INFINITE);
		i++;
	}
	if (error_audio)
		return 0;
	for(i=0;i<N_BUF;i++){
		waveOutUnprepareHeader(hWaveOut,lpWaveHdr[i],sizeof(WAVEHDR));
		free(lpWaveHdr[i]);
		free(lpData[i]);
	}
	waveOutClose(hWaveOut);
#endif
	return 0;
}


/** 
 * \author fmendez
 * \brief Reproduce audio
 * 
 * \param datos_audio Puntero al buffer que contiene los datos a reproducir
 * \param long_datos_audio número de bytes a escribir
 * 
 * \return 0 en ausencia de error
 */
int Audio::EscribeAudio(char *datos_audio, unsigned int long_datos_audio){

#ifdef _SIN_VOZ
	return 0;
#endif

	if (error_audio)
		return 0;

#ifndef _WIN32
	int err;

	if ((err = snd_pcm_writei (pcm_handle, datos_audio, long_datos_audio/2)) < 0) {
		snd_pcm_recover(pcm_handle,err,1);
		//fprintf (stderr, "pcm writei failed (%s)\n", snd_strerror (err));
	}

	//err= snd_pcm_writei(pcm_handle, datos_audio, long_datos_audio/2);
	snd_pcm_drain(pcm_handle);
	snd_pcm_prepare(pcm_handle);


#else //WIN32
	int i=0;
	WaitForSingleObject(Semaforo,INFINITE);
	//BufferesLibres--;
	//busco un buffer libre
	while   (lpWaveHdr[i]->dwFlags & WHDR_INQUEUE)
		i++;
	memcpy(lpWaveHdr[i]->lpData,datos_audio,long_datos_audio);
	lpWaveHdr[i]->dwBufferLength=long_datos_audio;
	waveOutWrite(hWaveOut,lpWaveHdr[i],sizeof(WAVEHDR));
	//fprintf(stderr,"Pillo buffer, quedan libres %d\n",BufferesLibres);
#endif
	return 0;
}
#ifdef _WIN32
#pragma argsused
/** 
 * \author fmendez
 * \brief Libera un semáforo cada vez que se procesa un buffer de sonido. Esta
 * función sólo se utiliza en Windows(c)
 * 
 * \param waveOut 
 * \param uMsg 
 * \param dwInstance 
 * \param dwParam1 
 * \param dwParam2 
 *
 */
void CALLBACK waveOutProc(HWAVEIN waveOut, UINT uMsg, DWORD dwInstance,
		DWORD dwParam1, DWORD dwParam2){
	if (uMsg == WOM_DONE){
		//BufferesLibres++;
		ReleaseSemaphore(Semaforo,1,NULL);
	}
}
#endif


