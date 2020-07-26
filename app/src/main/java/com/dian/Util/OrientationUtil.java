package com.dian.Util;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.util.Log;
import android.view.OrientationEventListener;

public class OrientationUtil {
    public static final int ORIENTATION_PORTRAIT = 0;
    public static final int ORIENTATION_LANDSCAPE = 1;
    public static final int ORIENTATION_PORTRAIT_R = 2;
    public static final int ORIENTATION_LANDSCAPE_R = 3;
    public static int mOrientation = 0;

    public static void start(Context context) {
        MyOrientoinListener listener = new MyOrientoinListener(context);
        listener.enable();
    }

    static class MyOrientoinListener extends OrientationEventListener {

        MyOrientoinListener(Context context) {
            super(context);
        }

        public MyOrientoinListener(Context context, int rate) {
            super(context, rate);
        }

        @Override
        public void onOrientationChanged(int orientation) {
            if ((orientation >= 0 && orientation < 45) || (orientation >= 315)) {
                mOrientation = ORIENTATION_PORTRAIT;
            }
            else if (orientation >= 45 && orientation < 135) {
                mOrientation = ORIENTATION_LANDSCAPE;
            }
            else if (orientation >= 135 && orientation < 225) {
                mOrientation = ORIENTATION_PORTRAIT_R;
            }
            else if (orientation >= 225) {
                mOrientation = ORIENTATION_LANDSCAPE_R;
            }
        }
    }
}
