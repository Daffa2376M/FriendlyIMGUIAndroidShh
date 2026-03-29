package com.mycompany.application;

import android.app.Activity;
import android.os.Build;
import android.util.DisplayMetrics;
import android.view.DisplayCutout;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowManager;

public class DeviceInfo {

    /**
     * Configura a atividade para usar toda a área da tela, incluindo o entalhe.
     *
     * @param activity A instância da atividade atual.
     */
    public static void showNotch(Activity activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            // Solicita que a janela use toda a área da tela
            Window window = activity.getWindow();
            WindowManager.LayoutParams layoutParams = window.getAttributes();
            layoutParams.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            window.setAttributes(layoutParams);

            // Configura as flags para tela cheia
            window.getDecorView().setSystemUiVisibility(
                WindowManager.LayoutParams.FLAG_FULLSCREEN |
                WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN |
                WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS
            );
        }
    }

    /**
     * Obtém informações sobre o entalhe (se houver).
     *
     * @param activity A instância da atividade atual.
     * @return Informações sobre o entalhe ou null se não houver.
     */
    public static String getNotchInfo(Activity activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            WindowInsets insets = activity.getWindow().getDecorView().getRootWindowInsets();
            if (insets != null) {
                DisplayCutout cutout = insets.getDisplayCutout();
                if (cutout != null) {
                    return "Entalhe encontrado: " +
                        "Safe Insets (Top: " + cutout.getSafeInsetTop() +
                        ", Bottom: " + cutout.getSafeInsetBottom() +
                        ", Left: " + cutout.getSafeInsetLeft() +
                        ", Right: " + cutout.getSafeInsetRight() + ")";
                }
            }
        }
        return "Nenhum entalhe encontrado.";
    }
}

