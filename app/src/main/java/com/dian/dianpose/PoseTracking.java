package com.dian.dianpose;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;

public class PoseTracking {

    static {
        System.loadLibrary("pose");
    }

    public native static void run(byte[] data, int height, int width, long session);

    public native static long createSession(int thread);

    public native static void releaseSession(long session);

    public native static int getTrackingNum(long session);

    public native static int[] getTrackingPoseKeypointsByIndex(int index, long session);
    public native static float[] getTrackingPoseConfidenceByIndex(int index, long session);
    public native static int[] getTrackingLocationByIndex(int index, long session);

    private List<PersonInfo> persons;
    private long session;

    public PoseTracking(int thread) {
        session = createSession(thread);
        persons = new ArrayList<PersonInfo>();

    }

    public void release() {
        releaseSession(session);
    }

    public void detect(byte[] data, int height, int width) {


        run(data, height, width, session);
        int numsPerson = getTrackingNum(session);
        persons.clear();
        Log.d("numsPerson_tracking", numsPerson + "");


        for (int i = 0; i < numsPerson; i++) {
            int[] kps = getTrackingPoseKeypointsByIndex(i, session);
            float[] scores = getTrackingPoseConfidenceByIndex(i, session);
            int[] personRect = getTrackingLocationByIndex(i, session);

            PersonInfo person = new PersonInfo(personRect[0], personRect[1], personRect[2], personRect[3], kps, scores);
            persons.add(person);
        }
    }

    public List<PersonInfo> getTrackingInfo() {
        return persons;

    }

}
