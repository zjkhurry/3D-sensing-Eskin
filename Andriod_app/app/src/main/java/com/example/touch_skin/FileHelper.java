package com.example.touch_skin;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Environment;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

public class FileHelper {

    private Context mContext;
    private Object TimeUtil;

    public FileHelper(Context mContext) {
        this.mContext = mContext;
    }

    /**
     * 定义文件保存的方法，写入到文件中，所以是输出流
     */
    public void save(String FileName, String x, String y, String z, String ecg, String red, String ir) {
        String content = "x：" + x + ",y：" + y + ",z:" + z + ",ecg:" + ecg + ",red:" + red + ",ir:" + ir;
        FileOutputStream fos = null;
        try {
            // Context.MODE_PRIVATE私有权限，Context.MODE_APPEND追加写入到已有内容的后面
            fos = mContext.openFileOutput(FileName, Context.MODE_APPEND);
            fos.write(content.getBytes());
            fos.write("\r\n".getBytes());//写入换行
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * 定义文件读取的方法
     */
    public String read(String filename) throws IOException {
        FileInputStream fis = mContext.openFileInput(filename);
        byte[] buff = new byte[1024];
        StringBuilder sb = new StringBuilder("");
        int len = 0;
        while ((len = fis.read(buff)) > 0) {
            sb.append(new String(buff, 0, len));
        }
        fis.close();
        return sb.toString();
    }

    /**
     * get file name such as 20171031.txt
     *
     * @return
     */
    public String newFileName() {
        String path = null;
        try {
            path = Environment.getExternalStorageDirectory().getCanonicalPath() + "/"
                    + "/Crash/";
            File file = new File(path);
            if (!file.exists()) {
                file.mkdirs();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        StringBuilder sb = new StringBuilder();
        @SuppressLint("SimpleDateFormat") SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.CHINA);
        String now = sdf.format(new Date());
        sb.append("TIME:").append(now);
        return sb.toString();
    }
}
