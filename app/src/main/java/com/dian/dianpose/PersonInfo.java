package com.dian.dianpose;

public class PersonInfo {
    public int x1;
    public int y1;
    public int x2;
    public int y2;
    public int height;
    public int width;

    public int[] keypoints;
    public float[] keypoints_scores;


    PersonInfo(int x, int y, int x_, int y_) {
        x1 = x;
        y1 = y;
        x2 = x_;
        y2 = y_;
        height = y2 - y1;
        width = x2 - x1;
        keypoints = new int[17 * 2];
        keypoints_scores = new float[17];

    }

    PersonInfo(int x,int y,int x_,int y_,int[] keypoint,float[] keypointsScores) {
        x1 = x;
        y1 = y;
        x2 = x_;
        y2 = y_;
        height = y2 - y1;
        width = x2 - x1;
        keypoints = keypoint;
        keypoints_scores = keypointsScores;

    }

}
