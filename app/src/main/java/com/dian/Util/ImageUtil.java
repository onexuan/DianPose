package com.dian.Util;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PointF;
import android.graphics.PorterDuff;
import android.graphics.RectF;
import android.media.FaceDetector;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;

public class ImageUtil {


    public static Bitmap drawFacePoints(Bitmap bitmap, float[] points, int color, boolean copy, float scale) {
//        if (copy) {
//            bitmap2 = bitmap.copy(Bitmap.Config.ARGB_8888, true);
//        }
//        else {
//            bitmap2 = Bitmap.createBitmap(bitmap.getWidth(), bitmap.getHeight(), Bitmap.Config.ARGB_8888);
//        }
        bitmap.setHasAlpha(true);
        Canvas canvas = new Canvas(bitmap);
        Paint paint = new Paint();
        paint.setColor(color);
        for (int i = 0; i < 34; i += 2) {
            canvas.drawCircle(points[i] * scale, points[i + 1] * scale, 5, paint);
        }
        canvas.save();


        return bitmap;
    }

    public static Bitmap drawFacePoints(Bitmap bitmap, float[] points, boolean copy, float scale) {
        return drawFacePoints(bitmap, points, Color.YELLOW, copy, scale);
    }

    public static float[] remapPoints(float[] points, float scale, float xOffset, float yOffset) {
        for (int i = 0; i < 212; i += 2) {
                /* x */
                points[i] = points[i] * scale + xOffset;
                points[i + 1] = points[i + 1] * scale + yOffset;
        }
        return points;
    }



    public static Bitmap drawRect(Bitmap bitmap, RectF[] rectFS, int color, int strokeWidth) {
        if (rectFS == null) {
            return null;
        }
        Bitmap resultBitmap = Bitmap.createBitmap(bitmap.getWidth(), bitmap.getHeight(), Bitmap.Config.ARGB_8888);
//        Bitmap resultBitmap = bitmap.copy(bitmap.getConfig(), true);
        resultBitmap.setHasAlpha(true);
        Canvas canvas = new Canvas(resultBitmap);
        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
        Paint paint = new Paint();
        paint.setColor(color);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(strokeWidth);
        int i = 0;
        for (RectF rectF: rectFS) {
            canvas.drawRect(rectF, paint);
            i++;
        }
        if (i == 0) {
            return null;
        }
        canvas.save();
        return resultBitmap;
    }


    public static Bitmap drawRect(Bitmap bitmap, RectF[] rectFS, int color, int strokeWidth, int originWidth) {
        if (rectFS == null) {
            return null;
        }
        float scale = (float) originWidth / bitmap.getWidth();
        Log.d("boxnitmap", "" + scale);
        Bitmap resultBitmap = Bitmap.createBitmap((int)(bitmap.getWidth() * scale),
                (int)(bitmap.getHeight() * scale), Bitmap.Config.ARGB_8888);
        resultBitmap.setHasAlpha(true);
        Canvas canvas = new Canvas(resultBitmap);
        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
        Paint paint = new Paint();
        paint.setColor(color);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(strokeWidth);
        int i = 0;
        for (RectF rectF: rectFS) {
            canvas.drawRect(rectF, paint);
            i++;
        }
        if (i == 0) {
            return null;
        }
        canvas.save();
        return resultBitmap;
    }



    public static Bitmap drawFaceRect(Bitmap bitmap, FaceDetector.Face[] faces, int color, int strokeWidth) {
        Bitmap resultBitmap = Bitmap.createBitmap(bitmap.getWidth(), bitmap.getHeight(), Bitmap.Config.ARGB_8888);
        resultBitmap.setHasAlpha(true);
        Canvas canvas = new Canvas(resultBitmap);
        canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
        Paint paint = new Paint();
        paint.setColor(color);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(strokeWidth);
        for (FaceDetector.Face face: faces) {

            if (face != null) {
                RectF r = getFaceRect(face);
                Log.d("FaceDetector", r.top + "/" + r.bottom + "/" + r.left + "/" + r.right);
                canvas.drawRect(r, paint);
            }
        }
        canvas.save();
        return resultBitmap;
    }

    public static RectF getFaceRect(FaceDetector.Face face) {
        PointF pf = new PointF();
        face.getMidPoint(pf);
        RectF r = new RectF();
        int prefix = 400;
        float eyeDistance = (float) (face.eyesDistance() * 1.5);
        r.left = pf.x - eyeDistance;
        r.right = pf.x + eyeDistance;
        r.top = pf.y - eyeDistance - prefix;
        r.bottom = pf.y + eyeDistance - prefix;

        r.top += eyeDistance * 0.3;
        r.bottom += eyeDistance * 0.3;

        if (r.left < 0) {
            r.right -= r.left;
            r.left = 0;
        }
        if (r.top < 0) {
            r.bottom -= r.top;
            r.top = 0;
        }

        return r;
    }


    public static Bitmap scaleBitmap(Bitmap origin, int newWidth, int newHeight) {
        if (origin == null) {
            return null;
        }
        int height = origin.getHeight();
        int width = origin.getWidth();
        float scaleWidth = ((float) newWidth) / width;
        float scaleHeight = ((float) newHeight) / height;
        Matrix matrix = new Matrix();
        matrix.postScale(scaleWidth, scaleHeight);// 使用后乘
        Bitmap newBM = Bitmap.createBitmap(origin, 0, 0, width, height, matrix, false);
        return newBM;
    }

    public static Bitmap scaleBitmap(Bitmap origin, int newWidth) {
        if (origin == null) {
            return null;
        }
        int height = origin.getHeight();
        int width = origin.getWidth();
        float scaleWidth = ((float) newWidth) / width;
        Matrix matrix = new Matrix();
        matrix.postScale(scaleWidth, scaleWidth);// 使用后乘
        Bitmap newBM = Bitmap.createBitmap(origin, 0, 0, width, height, matrix, false);
        return newBM;
    }


    public static Bitmap cropBitmap(Bitmap origin, RectF rectF) {
        if (origin == null) {
            return null;
        }
//        float contrast = (float) ((64 + 64) / 128.0);
//        ColorMatrix cMatrix = new ColorMatrix();
//        cMatrix.set(new float[] { contrast, 0, 0, 0, 0, 0,
//                contrast, 0, 0, 0,// 改变对比度
//                0, 0, contrast, 0, 0, 0, 0, 0, 1, 0 });
//
//
//        Paint paint = new Paint();
//        paint.setColorFilter(new ColorMatrixColorFilter(cMatrix));
//
//        Canvas canvas = new Canvas(origin);
//        // 在Canvas上绘制一个已经存在的Bitmap。这样，dstBitmap就和srcBitmap一摸一样了
//        canvas.drawBitmap(origin, 0, 0, paint);

        /* check width */
        if (rectF.right > origin.getWidth()) {
            rectF.right = origin.getWidth();
        }
        if (rectF.bottom > origin.getHeight()) {
            rectF.bottom = origin.getHeight();
        }
        if (rectF.left < 0) {
            rectF.left = 0;
        }
        if (rectF.top < 0) {
            rectF.top = 0;
        }
        if (rectF.width() <= 0 || rectF.height() <= 0) {
            return origin;
        }
//        Log.d("CropBitmap", "" + rectF.width() + "," + rectF.height() + ":" + rectF.left + "," + rectF.top);
        return Bitmap.createBitmap(origin, (int)rectF.left, (int)rectF.top, (int)rectF.width(), (int)rectF.height(), null, false);
    }


    public static byte[] getPixelsRGBA(Bitmap image) {
        // calculate how many bytes our image consists of
        int bytes = image.getByteCount();
        ByteBuffer buffer = ByteBuffer.allocate(bytes); // Create a new buffer
        image.copyPixelsToBuffer(buffer); // Move the byte data to the buffer
        byte[] temp = buffer.array(); // Get the underlying array containing the

        return temp;
    }

    public static byte[] getPixelsRGB(Bitmap bitmap, int imgWidth, int imgHeight) {
        int[] colorValues = new int[bitmap.getWidth() * bitmap.getHeight()];
        byte[] bytes = new byte[imgWidth * imgHeight * 3];
//        ByteBuffer byteBuffer = ByteBuffer.wrap(bytes, 0, imgWidth * imgHeight * 3);
        if (bitmap != null) {
            bitmap.getPixels(colorValues, 0, bitmap.getWidth(), 0, 0, bitmap.getWidth(), bitmap.getHeight());
//            bitmap.recycle();
        }
        // Convert the image to floating point.

        for (int i = 0; i < imgWidth * imgHeight; i++) {
            int pixelValue = colorValues[i];
            byte r = (byte)((pixelValue >> 16) & 0xFF);
            byte g = (byte)((pixelValue >> 8) & 0xFF);
            byte b = (byte)(pixelValue & 0xFF);
//            byteBuffer.put((byte)((pixelValue >> 16) & 0xFF));
//            byteBuffer.put((byte)((pixelValue >> 8) & 0xFF));
//            byteBuffer.put((byte)(pixelValue & 0xFF));
            bytes[i * 3] = r;
            bytes[i * 3 + 1] = g;
            bytes[i * 3 + 2] = b;
        }

        return bytes;
    }

    public static byte[] getPixelsGray(Bitmap image, int imgWidth, int imgHeight) {
        int alpha = 0xFF << 24;
        image = image.copy(Bitmap.Config.RGB_565, true);
        int[] intValues = new int[imgWidth * imgHeight];
        image.getPixels(intValues, 0, imgWidth, 0, 0, imgWidth, imgHeight);
        // Convert the image to floating point.
        byte[] bytes = new byte[imgWidth * imgHeight];

        for (int i = 0; i < imgWidth * imgHeight; i++) {
            int pixelValue = intValues[i];
            byte r = (byte)((pixelValue >> 16) & 0xFF);
            byte g = (byte)((pixelValue >> 8) & 0xFF);
            byte b = (byte)(pixelValue & 0xFF);
            int grey = (int) ((float) r * 0.3 + (float) g * 0.59 + (float) b * 0.11);
            grey = alpha | (grey << 16) | (grey << 8) | grey;
            bytes[i] = (byte)grey;
        }

        return bytes;
    }

    public static float[] getMacePixels(Bitmap bitmap) {
        float[] floatValues = new float[112 * 112 * 3];
        int[] colorValues = new int[112 * 112];
        FloatBuffer floatBuffer = FloatBuffer.wrap(floatValues, 0, 112 * 112 *3);
        if (bitmap != null) {
            bitmap.getPixels(colorValues, 0, bitmap.getWidth(), 0, 0, bitmap.getWidth(), bitmap.getHeight());
//            bitmap.recycle();
        }
        for (int i = 0; i < colorValues.length; i++) {
            int value = colorValues[i];
            floatBuffer.put((((value >> 16) & 0xFF) - 127.5f) * 0.0078125f);
            floatBuffer.put((((value >> 8) & 0xFF) - 127.5f) * 0.0078125f);
            floatBuffer.put(((value & 0xFF) - 127.5f) * 0.0078125f);
        }
        return floatBuffer.array();
    }


}
