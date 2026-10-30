package io.github.opaiva1721.narutodiag;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Process;
import android.util.Log;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import org.json.JSONObject;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/** Diagnostic shell only. No game loading, analytics, storage or network permissions. */
public final class MainActivity extends Activity {
    private final ExecutorService worker = Executors.newSingleThreadExecutor();
    private TextView reportView;
    private Button refresh;
    private Button share;
    private String report = "";
    private static native byte[] probeVulkan();

    @Override public void onCreate(Bundle savedState) {
        super.onCreate(savedState);
        LinearLayout content = new LinearLayout(this);
        content.setOrientation(LinearLayout.VERTICAL);
        int padding = Math.round(20 * getResources().getDisplayMetrics().density);
        content.setPadding(padding, padding, padding, padding);
        // Keep controls below system bars, including target-SDK edge-to-edge devices.
        content.setOnApplyWindowInsetsListener((v, insets) -> {
            if (Build.VERSION.SDK_INT >= 30) {
                android.graphics.Insets bars = insets.getInsets(android.view.WindowInsets.Type.systemBars());
                v.setPadding(padding + bars.left, padding + bars.top, padding + bars.right, padding + bars.bottom);
            } else {
                v.setPadding(padding + insets.getSystemWindowInsetLeft(), padding + insets.getSystemWindowInsetTop(),
                    padding + insets.getSystemWindowInsetRight(), padding + insets.getSystemWindowInsetBottom());
            }
            return insets;
        });
        TextView title = new TextView(this);
        title.setText(R.string.intro);
        title.setTextSize(21);
        content.addView(title);
        refresh = new Button(this); refresh.setText(R.string.refresh);
        content.addView(refresh);
        share = new Button(this); share.setText(R.string.share);
        share.setEnabled(false);content.addView(share);
        reportView = new TextView(this);reportView.setTextIsSelectable(true);
        ScrollView scroll = new ScrollView(this);scroll.addView(reportView);
        content.addView(scroll,new LinearLayout.LayoutParams(-1,0,1));
        setContentView(content);
        refresh.setOnClickListener(v -> collect());
        share.setOnClickListener(v -> {
            Intent send = new Intent(Intent.ACTION_SEND).setType("text/plain")
                .putExtra(Intent.EXTRA_TEXT,report);
            startActivity(Intent.createChooser(send,getString(R.string.share_title)));
        });
        collect();
    }

    private void collect() {
        refresh.setEnabled(false);share.setEnabled(false);reportView.setText(R.string.collecting);
        worker.execute(() -> {
            String value;
            try {
                ActivityManager manager = (ActivityManager)getSystemService(ACTIVITY_SERVICE);
                ActivityManager.MemoryInfo memory = new ActivityManager.MemoryInfo();
                manager.getMemoryInfo(memory);
                JSONObject result = new JSONObject();
                result.put("schema",1).put("appVersion","0.1.0-scaffold");
                result.put("model",Build.MANUFACTURER+" "+Build.MODEL);
                result.put("androidApi",Build.VERSION.SDK_INT).put("abis",Arrays.toString(Build.SUPPORTED_ABIS));
                result.put("process64Bit",Process.is64Bit());
                result.put("memoryTotalMiB",memory.totalMem/(1024*1024));
                result.put("memoryAvailableMiB",memory.availMem/(1024*1024));
                result.put("lowMemory",memory.lowMemory);
                JSONObject vk;
                try {
                    System.loadLibrary("naruto_diagnostics");
                    vk = new JSONObject(new String(probeVulkan(),StandardCharsets.UTF_8));
                } catch (LinkageError error) {
                    vk = new JSONObject().put("loaderAvailable",false).put("instanceCreated",false)
                        .put("error","Biblioteca de diagnóstico indisponível: "+error.getClass().getSimpleName());
                }
                result.put("vulkan",vk);
                result.put("gameRuntimeIntegrated",false).put("renderingTested",false);
                value=ProbeSummary.status(vk.optBoolean("loaderAvailable"),vk.optBoolean("instanceCreated"),
                    vk.optInt("physicalDeviceCount",-1))+"\n\n"+result.toString(2)
                    +"\n\nConsultar dispositivos não comprova renderização ou desempenho do jogo."
                    +"\nRAM disponível é apenas uma leitura deste momento.";
            } catch (Exception error) {
                value="Falha no diagnóstico: "+error.getClass().getSimpleName()+": "+error.getMessage();
            }
            final String completed=value;
            Log.i("NarutoDiag",completed);
            runOnUiThread(() -> {
                if (isFinishing() || isDestroyed()) return;
                report=completed;reportView.setText(completed);refresh.setEnabled(true);share.setEnabled(true);
            });
        });
    }

    @Override protected void onDestroy() {
        worker.shutdown();
        super.onDestroy();
    }
}
