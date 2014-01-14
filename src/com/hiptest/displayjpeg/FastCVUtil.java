/*==============================================================================
            Copyright (c) 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.hiptest.displayjpeg;

public class FastCVUtil{	  
	   /**
	    * Initializes native processing
	    */
	   public native static void init();	 

	   /**
	    * Retrieves FastCV processing timing from native layer. 
	    *  
	    * @return FastCV processing timing, filtered, in ms. 
	    */
	   public native static float getFastCVProcessTime();
}
