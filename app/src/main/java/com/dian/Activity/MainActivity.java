package com.dian.Activity;

import android.Manifest;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.hardware.camera2.CameraCharacteristics;
import android.os.SystemClock;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RadioGroup;
import android.widget.TextView;

import java.io.InputStream;

import com.dian.Fragment.Camera2BasicFragment;
import com.dian.Util.OrientationUtil;
import com.dian.dianpose.R;

import pub.devrel.easypermissions.EasyPermissions;

public class MainActivity extends AppCompatActivity {
    private RadioGroup threadGroup;
    private RadioGroup archGroup;
    private Camera2BasicFragment currentFragment = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        verifyPermissions(this);

        OrientationUtil.start(this);

        threadGroup = findViewById(R.id.thread_group);
        archGroup = findViewById(R.id.arch_group);

        Button imageButton = findViewById(R.id.button_picture);
        imageButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                faceTestNCNN();
            }
        });

        Button frontButton = findViewById(R.id.button_front);
        frontButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (faceTestThread != null) {
                    faceTestThread.interrupt();
                    faceTestThread = null;
                    TextView textView = (TextView) findViewById(R.id.time_text);
                    textView.setText("");
                    ImageView faceImageView = findViewById(R.id.face_image);
                    faceImageView.setImageDrawable(null);
                }
                if (currentFragment != null) {
                    getSupportFragmentManager()
                            .beginTransaction()
                            .remove(currentFragment)
                            .commit();
                }
                currentFragment = Camera2BasicFragment.newInstance(CameraCharacteristics.LENS_FACING_FRONT,
                        archGroup.getCheckedRadioButtonId() == R.id.use_gpu, getThreadNum(threadGroup));
                getSupportFragmentManager()
                        .beginTransaction()
                        .replace(R.id.container, currentFragment)
                        .commit();
            }
        });


        Button backButton = findViewById(R.id.button_back);
        backButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (faceTestThread != null) {
                    faceTestThread.interrupt();
                    faceTestThread = null;
                    TextView textView = (TextView) findViewById(R.id.time_text);
                    textView.setText("");
                    ImageView faceImageView = findViewById(R.id.face_image);
                    faceImageView.setImageDrawable(null);
                }
                if (currentFragment != null) {
                    getSupportFragmentManager()
                            .beginTransaction()
                            .remove(currentFragment)
                            .commit();
                }
                currentFragment = Camera2BasicFragment.newInstance(CameraCharacteristics.LENS_FACING_BACK,
                        archGroup.getCheckedRadioButtonId() == R.id.use_gpu, getThreadNum(threadGroup));
                getSupportFragmentManager()
                        .beginTransaction()
                        .replace(R.id.container, currentFragment)
                        .commit();
            }
        });

    }


    public static void verifyPermissions(Activity activity) {
        String[] perms = {Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.CAMERA};
//        String[] perms = {Manifest.permission.CAMERA};
        if (!EasyPermissions.hasPermissions(activity, perms)) {
            EasyPermissions.requestPermissions(activity, "Storage permission", 101, perms);
        }
    }


    int getThreadNum(RadioGroup threadGroup) {
        int id = threadGroup.getCheckedRadioButtonId();
        switch (id) {
            case R.id.thread_1:
                return 1;
            case R.id.thread_2:
                return 2;
            case R.id.thread_4:
                return 4;
            case R.id.thread_8:
                return 8;
        }
        return 1;
    }
//    final FaceDetector faceDetector = new FaceDetector(this);

    /* test landmark without localization */
    private Thread faceTestThread = null;
    void faceTestNCNN() {
        faceTestThread = new Thread() {
            @Override
            public void run() {
                super.run();
//                try {
//                    faceDetector.setThreadNum(getThreadNum(threadGroup));
//                    int i = 0;
//                    while (!isInterrupted()) {
//                        if (i == 25) {
//                            i = 0;
//                        }
//                        i++;
//                        InputStream inputStream = getResources().getAssets().open(i + ".jpg");
//                        final Bitmap bitmap = BitmapFactory.decodeStream(inputStream);
//                        Bitmap bitmap1 = ImageUtil.scaleBitmap(bitmap, 112, 112);
//                        final long startTime2 = SystemClock.uptimeMillis();
//                        float[] points = faceDetector.detectLandmark(bitmap1);
//                        final Bitmap bitmap2 = bitmap1.copy(Bitmap.Config.RGB_565, true);
//                        Canvas canvas = new Canvas(bitmap2);
//                        Paint paint = new Paint();
//                        paint.setColor(Color.GREEN);
//                        for (int j = 0; j < 106; j++) {
//                            canvas.drawCircle((float) (points[j * 2]),
//                                    (float) (points[j * 2 + 1]), 1, paint);
//                        }
//                        canvas.save();
//                        runOnUiThread(new Runnable() {
//                            @Override
//                            public void run() {
//
//                                TextView textView = (TextView) findViewById(R.id.time_text);
//                                textView.setText(SystemClock.uptimeMillis() - startTime2 + "ms");
//
//                                ImageView faceImageView = findViewById(R.id.face_image);
//                                faceImageView.setImageBitmap(bitmap2);
//                            }
//                        });
//                        sleep(1000);
//                    }
//                }
//                catch (Exception e) {
//                    e.printStackTrace();
//                }
            }
        };
        faceTestThread.start();
    }

}
