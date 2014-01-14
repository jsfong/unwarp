/*
 * Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved 
 * Qualcomm Technologies Confidential and Proprietary
 * 
 *
 * @file Implementation of FastCV Sample native interface.
 *
 */

//==============================================================================
// Include Files
//==============================================================================
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdlib.h>
#include <android/log.h>
#include <time.h>
#include <fastcv/fastcv.h>
#include <Math.h>
#include "loadjpeg.h"#include <android/bitmap.h>

//==============================================================================
// Declarations
//==============================================================================
#define LOG_TAG    "loadjpeg.cpp"
#define DPRINTF(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define IPRINTF(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define EPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
//==============================================================================// Macro//==============================================================================#define TWO_PI (6.2831853071795864769252867665590057683943L)#define DEG_TO_RAD = 0.017453292519943295769236907684886#define MAX(x, y) (((x) > (y)) ? (x) : (y))#define MIN(x, y) (((x) < (y)) ? (x) : (y))//==============================================================================// State//==============================================================================
struct State{	State()	{		unwarpedAspectRatio = 360.0/55.0;		unwarpedW = 1280;		unwarpedH = (int)(unwarpedW/unwarpedAspectRatio);		ptrsrcxArray = NULL;		ptrsrcyArray = NULL;		dst = output;	}	float unwarpedAspectRatio;	int  unwarpedW;	int unwarpedH;	float srcxArray[];	float srcyArray[];	float* ptrsrcxArray;	float* ptrsrcyArray;	uint8_t output[];	uint8_t* dst;};static State      state;
//==============================================================================
// Function Declarations
//==============================================================================
void moeComputeInversePolarTransform(uint32_t w, uint32_t h);void remap(uint8_t* src, uint32_t w, uint32_t h, uint8_t* dst);//==============================================================================// Variable setup//==============================================================================

//==============================================================================
// Function Definitions
//==============================================================================
void convertRGB565ToY(  uint8_t*       src, 
		unsigned int   srcWidth,
		unsigned int   srcHeight,
		uint8_t*       dst );

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void 
JNICALL Java_com_hiptest_displayjpeg_LoadJpeg_convertToLum
(
		JNIEnv*     env,
		jobject     obj,
		jshortArray img,
		jint        w,
		jint        h
)
{ 
	jshort*           jimgData = NULL;
	jboolean          isCopy = 0;

	DPRINTF( "%s %d\n", __FILE__, __LINE__ );

	if( img != NULL )
	{
		// Get data from JNI
		jimgData = env->GetShortArrayElements( img, &isCopy );

		uint8_t* lum = (uint8_t*)fcvMemAlloc( w*h, 16 );
		convertRGB565ToY( (uint8_t*)jimgData, w, h, lum );

		fcvMemFree( lum );

		// Let JNI know we don't need data anymore. this is important!
		env->ReleaseShortArrayElements( img, jimgData, JNI_ABORT );

		DPRINTF( "%s %d Done\n", __FILE__, __LINE__ );
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void convertRGB565ToY(  uint8_t*       src, 
		unsigned int   srcWidth,
		unsigned int   srcHeight,
		uint8_t*       dst )
{
	unsigned int   size = srcWidth * srcHeight;
	uint16_t       rgb;
	uint16_t       r;
	uint16_t       g;
	uint16_t       b;

	for( unsigned int i=0; i<size; i++ ){
		rgb = ((uint16_t*)src)[i];
		r = (uint16_t)((rgb&0xF800) >> 8);
		g = (uint16_t)((rgb&0x07E0) >> 3);
		b = (uint16_t)((rgb&0x001F) << 3);

		dst[i] = (uint8_t)(((r<<1) + (g<<2) + g + b)>>3);
	}
}//------------------------------------------------------------------------------//------------------------------------------------------------------------------
JNIEXPORT voidJNICALL Java_com_hiptest_displayjpeg_LoadJpeg_test(		JNIEnv*     env,		jobject     obj,		jintArray img,		jint        w,		jint        h,		jintArray outPixel){	jint*           jimgData = NULL;	jint*			joutData = NULL;	jboolean          isCopy = 0;	DPRINTF( "Enter native test");	DPRINTF( "%s %d\n", __FILE__, __LINE__ );	if( img != NULL )	{		// Get data from JNI		jimgData = env->GetIntArrayElements( img, &isCopy );		joutData = env->GetIntArrayElements( outPixel, &isCopy );		jint length = env->GetArrayLength(img);		DPRINTF( "Native input byte array length %d", length);		//state.dst = (uint8_t*)fcvMemAlloc( w*h, 16 );		uint8_t* lum = (uint8_t*)fcvMemAlloc( w*h, 16 );		//convertRGB565ToY( (uint8_t*)jimgData, w, h, lum );		DPRINTF( "PROCESSING - compute inverse polar - START/n");		moeComputeInversePolarTransform(w, h);		DPRINTF( "PROCESSING - compute inverse polar - DONE /n");		DPRINTF( "PROCESSING - fastcv remap - START/n");		//remap( (uint8_t*) jimgData, w, h,  lum);		DPRINTF( "PROCESSING - fastcv remap - DONE/n");		//testing remap		//DPRINTF( "Remap test %d/n", state.dst[w*h*4 -1] );		/**convert native output to java layer output*/		//jintArray outJNIArray = env->NewIntArray(length);		//DPRINTF( "Created output JNI intArray");		//if (outJNIArray == NULL)		//{		//  DPRINTF( "NULL/n");		//	  return NULL;		// }		//jbyte* rawjBytes = env->GetByteArrayElements(outJNIArray, &isCopy);		//DPRINTF( "Created output JNI byteArray ptr ");		//memcpy(rawjBytes, state.dst, w*h*4);		//env->SetIntArrayRegion(outJNIArray, 0, length, (const jint*)lum);		// env->SetIntArrayRegion(outJNIArray, 0, length, jimgData);		DPRINTF( "Successful copy from dst ptr to JNI ptr");		//env->ReleaseByteArrayElements( outJNIArray, rawjBytes, JNI_ABORT );		// Let JNI know we don't need data anymore. this is important!		env->ReleaseIntArrayElements( img, jimgData, JNI_ABORT );		env->ReleaseIntArrayElements( outPixel, joutData, JNI_ABORT );		//fcvMemFree( state.dst );		fcvMemFree( lum );		DPRINTF( "%s %d Done\n", __FILE__, __LINE__ );	}}//------------------------------------------------------------------------------//------------------------------------------------------------------------------void moeComputeInversePolarTransform(uint32_t w, uint32_t h){	/**Varible*/	int srcx, srcy, srcIndex;	int dstRow, dstIndex;	float radius, angle;	int inputImageW = w;	int inputImageH = h;	float maxR  = inputImageH/2.0 * 0.72;	float minR  = inputImageH/2.0 * 0.16;	float imgCx = 1314.5; //center of x	float imgCy = 997.25; // center of y	float yWarpA =   0.1850;	float yWarpB =   0.8184;	float yWarpC =  -0.0028;	// The Bloggie has a 55-degree vertical FOV, and a 360-degree horizontal FOV.	//float unwarpedAspectRatio = 360.0/55.0;	//int  unwarpedW = 1280;//width; //width??	//int  unwarpedH = (int)(unwarpedW / unwarpedAspectRatio);	state.ptrsrcxArray = state.srcxArray;	state.ptrsrcyArray = state.srcyArray;	state.ptrsrcxArray = new float[state.unwarpedW*state.unwarpedH];	state.ptrsrcyArray = new float[state.unwarpedW*state.unwarpedH];	//float srcxArray [unwarpedW*unwarpedH];	//float srcyArray [unwarpedW*unwarpedH];	DPRINTF( "NATIVE moeComputerInversePolarTransform  unwarp image from Width and height %u %u \n", w, h );	DPRINTF( "NATIVE moeComputerInversePolarTransform  unwarp image to Width and height %u %u \n", state.unwarpedW, state.unwarpedH );	for (int dsty = 0; dsty<state.unwarpedH; dsty++)	{		float y = ((float)dsty/(float)state.unwarpedH);		float yfrac = y;//yWarpA*y*y + yWarpB*y + yWarpC;		radius = (yfrac* (maxR-minR)) + minR;		dstRow =dsty*state.unwarpedW;		//DPRINTF( "dstRow %d \n", dstRow );		for (int dstx = 0; dstx<state.unwarpedW; dstx++)		{			dstIndex = dstRow + dstx;			angle    = (0 - ((float)dstx/(float)state.unwarpedW) * TWO_PI); // + (DEG_TO_RAD * angularOffset)			state.ptrsrcxArray[dstIndex] = imgCx + radius*cos(angle);			state.ptrsrcyArray[dstIndex] = imgCy + radius*sin(angle);		}		//DPRINTF( "xo and yo in dstIndex %f %f %d \n", state.ptrsrcxArray[dstIndex] ,state.ptrsrcyArray[dstIndex], dstIndex  );	}}//------------------------------------------------------------------------------//------------------------------------------------------------------------------void remap(uint8_t* src, uint32_t w, uint32_t h, uint8_t* dst){	/**Fastcv remap RGB8888 Nearest interpolation*/	fcvRemapRGBA8888NNu8	( 		src,				//const uint8_t *__restrict  	src,			w,					//uint32_t  	srcWidth,			h,					//uint32_t  	srcHeight,			0,					//uint32_t  	srcStride,			dst,				//uint8_t *__restrict  	dst,			state.unwarpedW,	//uint32_t  	dstWidth,			state.unwarpedH,	//uint32_t  	dstHeight,			0,					//uint32_t  	dstStride,			state.ptrsrcxArray,	//const float32_t *__restrict  	mapX,			state.ptrsrcyArray, //const float32_t *__restrict  	mapY,			0					//uint32_t  	mapStride	);}//------------------------------------------------------------------------------//------------------------------------------------------------------------------JNIEXPORT void JNICALLJava_com_qualcomm_fastcorner_FastCVSample_cleanup(		JNIEnv * env,		jobject obj){	DPRINTF( "%s\n", __FUNCTION__ );	/*   if( state.xocvdata )   {      fcvMemFree(state.xocvdata);      state.xocvdata = NULL;   }	 */	delete[]state.ptrsrcxArray;	delete[]state.ptrsrcyArray;	fcvCleanUp();}//------------------------------------------------------------------------------//------------------------------------------------------------------------------JNIEXPORT voidJNICALL Java_com_hiptest_displayjpeg_LoadJpeg_unwarp(		JNIEnv*     env,		jobject     obj,		jbyteArray img,		jint        w,		jint        h,		jbyteArray outPixel){	DPRINTF( "Enter native test");	DPRINTF( "%s %d\n", __FILE__, __LINE__ );	jbyte*           jimgData = NULL;	jbyte*			joutPixel = NULL;	jboolean          isCopy = 0;	if( img != NULL )	{		//Process timing		float processTime, afterprocesstime;		//Get input data from JNI		jimgData = env->GetByteArrayElements( img, &isCopy );		jint length = env->GetArrayLength(img);		DPRINTF( "Native input byte array length %d", length);		uint8_t* pJimgData = (uint8_t*)jimgData;		if( (int)jimgData & 0xF )			DPRINTF( "Image data not 128 bit allign");		//Get output data from JNI		joutPixel = env->GetByteArrayElements(outPixel, &isCopy);		jint outlength = env->GetArrayLength(outPixel);		DPRINTF( "Native output byte array length %d", length);		uint8_t* lum = (uint8_t*)fcvMemAlloc( w*h, 16 );		lum = (uint8_t*)joutPixel;		//IMAGE PROCESSING		DPRINTF( "PROCESSING - compute inverse polar - START/n");		moeComputeInversePolarTransform(w, h);		DPRINTF( "PROCESSING - compute inverse polar - DONE /n");		DPRINTF( "PROCESSING - fastcv remap - START/n");		processTime = util.getTimeMicroSeconds();		remap( (uint8_t*) pJimgData, w, h,  lum);		afterprocesstime = util.getTimeMicroSeconds();		util.setProcessTime(afterprocesstime-processTime);		DPRINTF( "%f after process time ", (afterprocesstime-processTime)/1000);		DPRINTF( "PROCESSING - fastcv remap - DONE/n");		//Release Data		env->ReleaseByteArrayElements( img, jimgData, JNI_ABORT );		env->ReleaseByteArrayElements( outPixel, joutPixel, JNI_ABORT );		fcvMemFree( lum );		DPRINTF( "%s %d Done\n", __FILE__, __LINE__ );	}}//------------------------------------------------------------------------------//------------------------------------------------------------------------------JNIEXPORT voidJNICALL Java_com_hiptest_displayjpeg_LoadJpeg_byteArraytoBitmap(		JNIEnv * env,		jobject obj,		jobject	bitmap,		jbyteArray input){	DPRINTF( "Enter native byteArraytoBitmap");	DPRINTF( "%s %d\n", __FILE__, __LINE__ );	AndroidBitmapInfo   info;	int ret;	void* pixels;	jboolean isCopy = 0;	jbyte*           jinput = NULL;	jinput = env->GetByteArrayElements( input, &isCopy );	uint8_t*			pinput ;	pinput = (uint8_t*)jinput;	if((ret= AndroidBitmap_getInfo(env, bitmap, &info)) < 0)	{		DPRINTF("AndroidBitmap_getInfo() failed ! error=%d", ret);		return ;	}	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {		DPRINTF("Bitmap format is not RGBA_8888 !");		return ;	}	if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {		DPRINTF("AndroidBitmap_lockPixels() failed ! error=%d", ret);	}	uint8_t* dst = (uint8_t*) pixels;	uint8_t* tempPixels = new uint8_t[info.height * info.width*4];	int stride = info.stride;	int pixelsCount = info.height * info.width;	memcpy(dst, pinput, sizeof(uint8_t) * pixelsCount*4);	AndroidBitmap_unlockPixels(env, bitmap);	env->ReleaseByteArrayElements( input, jinput, JNI_ABORT );}