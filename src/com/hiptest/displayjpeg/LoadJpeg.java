package com.hiptest.displayjpeg;


import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.nio.LongBuffer;
import java.nio.ShortBuffer;




















import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;

public class LoadJpeg extends Activity{
	
	/**Logging tag */
	private static final String TAG = "LoadJpeg";
	
	/**Variable*/
	static Handler handler = new Handler(); 
	private static Bitmap bmp, bmp565, outputbmp;
	private static ImageView display;	
	
	/**Load JNI library*/
	static
	{
		// Load JNI library
	      System.loadLibrary( "loadjpeg" );
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		
		display = (ImageView)findViewById(R.id.imageView1);
		
		setContentView (R.layout.displayjpeg);
		Log.d(TAG, "onCreate");
		
		
		
		//bmp565 = Bitmap.createBitmap(bmp.getWidth(), bmp.getHeight(), Bitmap.Config.RGB_565);
		//Log.d(TAG, "create RGB565 img");
		//loadJPEG ();
		LoadJpeg.handler.post(rLoadJpeg);
			
	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		
		
		
	}
	
	Runnable rLoadJpeg = new Runnable ()
	{
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			Log.i(TAG, "load jpeg runnable");
			try {
				loadJPEG ();
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}		
			LoadJpeg.handler.removeCallbacks(rLoadJpeg);
		}
		
	};

	/**
	 * Load a JPEG into processing
	 */	
	@SuppressLint("NewApi")
	private void loadJPEG() throws FileNotFoundException {
		// TODO Auto-generated method stub
		
		Log.d(TAG, "before LoadJPEG");
		bmp = BitmapFactory.decodeResource(getResources(),R.drawable.input);		
		
		if( bmp !=null && bmp.getConfig()==Bitmap.Config.ARGB_8888)
		{
			Log.d(TAG, "Input image is valid and ARGB_8888");	
			
			/**Preparing image size*/
			int bytes = bmp.getByteCount();
			Log.d(TAG, "bmp size "+ bytes);
			float unwarpedAspectRatio = (float) (360.0/55.0);
			int unwarpedW = 1280;
			int unwarpedH = (int)(unwarpedW/unwarpedAspectRatio);
			
			
			/**Preparing INPUT - Convert bitmap --> byte array WITHOUT compression*/			
			ByteBuffer buffer = ByteBuffer.allocate(bytes);
			bmp.copyPixelsToBuffer(buffer);			
			Log.d(TAG, "byteArrayData size "+ buffer.array().length);	
			byte[]input = buffer.array();
			
			/**Preparing OUTPUT byte array - Initialize output*/
			byte[] OutputByteArray = new byte[unwarpedW*unwarpedH*4];
			Log.d(TAG, "output int [] size "+ OutputByteArray.length);
			
			/**Processing Unwrapping*/
			FastCVUtil.init();
			unwarp(input, bmp.getWidth(), bmp.getHeight(),OutputByteArray);					
			Log.d(TAG, "after process output int [] size "+ OutputByteArray.length);
			
			/**Convert OutputByteArray byte[] to bitmap*/			
			/**!!decodeByteArray is for compressed bitmap, our image is uncompress*/	
			//Bitmap outDisplay = BitmapFactory.decodeByteArray(OutputByteArray, 0, OutputByteArray.length);
			
			/**Try Native to display bitmap*/
			Bitmap.Config conf = Bitmap.Config.ARGB_8888;
			Bitmap OutputBitmap = Bitmap.createBitmap(unwarpedW, unwarpedH, conf);
			byteArraytoBitmap(OutputBitmap,OutputByteArray);	
				
			/**Store bitmap  to sdcard*/
			
			try {
				OutputStream stream = new FileOutputStream("/sdcard/unwarped.jpg");				
				OutputBitmap.compress(CompressFormat.JPEG, 100, stream);
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			Log.d(TAG, "Remap "+unwarpedW+" x "+unwarpedH+" process time (ms): "+ FastCVUtil.getFastCVProcessTime()/1000);
			
			
			
		}		
	}
	

	
	 /**
	    * Performs conversion from RGB565 to Luminance in native. 
	    * @param data ShortByte.array() data. 
	    * @param w Width of buffer 
	    * @param h Height of buffer 
	    */
	   private native void convertToLum( short[] data, int w, int h );
	   
	   private native void test(int[] ls, int w, int h, int[] pixels);	   
	  
	   private native void unwarp (byte[] input, int w, int h, byte[] output);
	   
	   private native void cleanup();
	   
	   private native void byteArraytoBitmap (Bitmap img, byte[] input);
	   
	 
	   
	   
	  
}

