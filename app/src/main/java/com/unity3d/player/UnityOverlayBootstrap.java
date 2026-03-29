package com.unity3d.player;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.view.MotionEvent;
import android.view.View;
import android.widget.FrameLayout;

public final class UnityOverlayBootstrap {
    private static boolean started;
    private static View touchView;
    private static int lastLeft = Integer.MIN_VALUE;
    private static int lastTop = Integer.MIN_VALUE;
    private static int lastWidth = Integer.MIN_VALUE;
    private static int lastHeight = Integer.MIN_VALUE;

    private UnityOverlayBootstrap() {}

    public static void start(final Activity activity) {
        if (activity == null || started) {
            return;
        }

        started = true;
        activity.runOnUiThread(() -> attach(activity));
    }

    private static void attach(Activity activity) {
        FrameLayout root = activity.findViewById(android.R.id.content);
        if (root == null) {
            started = false;
            return;
        }

        final GLES3JNIView display = new GLES3JNIView(activity);
        touchView = new View(activity);

        FrameLayout.LayoutParams displayParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT
        );
        FrameLayout.LayoutParams touchParams = new FrameLayout.LayoutParams(0, 0);

        root.addView(display, displayParams);
        root.addView(touchView, touchParams);

        touchView.setOnTouchListener((v, event) -> {
            int action = event.getActionMasked();
            switch (action) {
                case MotionEvent.ACTION_DOWN:
                case MotionEvent.ACTION_MOVE:
                case MotionEvent.ACTION_UP:
                    GLES3JNIView.MotionEventClick(action != MotionEvent.ACTION_UP, event.getRawX(), event.getRawY());
                    return true;
                default:
                    return false;
            }
        });

        final Handler handler = new Handler(Looper.getMainLooper());
        handler.post(new Runnable() {
            @Override
            public void run() {
                if (touchView.getParent() == null) {
                    return;
                }

                try {
                    String[] rect = GLES3JNIView.getWindowRect().split("\\|");
                    if (rect.length == 4) {
                        int left = Integer.parseInt(rect[0]);
                        int top = Integer.parseInt(rect[1]);
                        int width = Integer.parseInt(rect[2]);
                        int height = Integer.parseInt(rect[3]);
                        if (left != lastLeft || top != lastTop || width != lastWidth || height != lastHeight) {
                            FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) touchView.getLayoutParams();
                            params.leftMargin = left;
                            params.topMargin = top;
                            params.width = width;
                            params.height = height;
                            touchView.setLayoutParams(params);
                            lastLeft = left;
                            lastTop = top;
                            lastWidth = width;
                            lastHeight = height;
                        }
                    }
                } catch (Exception ignored) {
                }

                handler.postDelayed(this, 33);
            }
        });
    }
}
