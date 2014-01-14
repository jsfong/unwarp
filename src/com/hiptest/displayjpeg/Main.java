package com.hiptest.displayjpeg;




import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class Main extends Activity implements View.OnClickListener {
	
	//Variable
	private Button bDisplayOriImage;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_load_jpeg);
		
		bDisplayOriImage = (Button) findViewById(R.id.display_ori_image);
		bDisplayOriImage.setOnClickListener(this);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.load_jpeg, menu);
		return true;
	}	
	
	

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch(v.getId())
		{
		case R.id.display_ori_image:
			Intent intent_other_test = new Intent(getBaseContext(), LoadJpeg.class);					
			startActivity(intent_other_test);	
			break;				
		}
	}
}
