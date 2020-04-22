package com.coperand.awpanel;

import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.DividerItemDecoration;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import com.coperand.awpanel.MainActivity.Macro;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.util.ArrayList;


public class MacrosActivity extends AppCompatActivity {

    private String ip;
    private ArrayList<Macro> macList;
    private DatagramSocket socket = null;
    private AsyncTask task;
    MacroDataAdapter adapter;

    public void setMacList(ArrayList<Macro> macList) {
        this.macList = macList;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_macros);

        if(savedInstanceState != null)
        {
            macList = (ArrayList<Macro>)savedInstanceState.getSerializable("macList");
            ip = savedInstanceState.getString("ip");
        }

        Toolbar toolbar = findViewById(R.id.macro_toolbar);
        setSupportActionBar(toolbar);

        if(socket == null)
        {
            try {
                socket = new DatagramSocket();
            } catch (SocketException e) {
                e.printStackTrace();
            }
        }

        macList = (ArrayList<Macro>)getIntent().getSerializableExtra("macros");
        ip = getIntent().getStringExtra("ip");

        task = new RecvTask().executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);

        RecyclerView recyclerView = (RecyclerView) findViewById(R.id.macro_list);
        adapter = new MacroDataAdapter(this, macList);
        DividerItemDecoration dividerItemDecoration = new DividerItemDecoration(recyclerView.getContext(), LinearLayoutManager.VERTICAL);
        recyclerView.addItemDecoration(dividerItemDecoration);
        recyclerView.setAdapter(adapter);
    }

    @Override
    public void onSaveInstanceState(Bundle savedInstanceState)
    {
        super.onSaveInstanceState(savedInstanceState);
        savedInstanceState.putSerializable("macList", macList);
        savedInstanceState.putString("ip", ip);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        task.cancel(true);
        socket.close();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.toolbar_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if(item.getItemId() == R.id.action_refresh)
        {
            if (checkWiFi()) {
                macList.clear();
                new SendTask().execute(ip);
            } else
                Toast.makeText(getApplicationContext(), R.string.wifi_disabled, Toast.LENGTH_SHORT).show();
        }
        return true;
    }

    public boolean checkWiFi()
    {
        WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
        return wifiManager.isWifiEnabled();
    }

    public void sendCode(int position)
    {
        new SendCodeTask().execute(ip, String.valueOf(macList.get(position).code));
    }

    private class SendCodeTask extends AsyncTask<String, Void, Integer> {
        @Override
        protected Integer doInBackground(String... data)
        {
            InetAddress addr;
            String ip = data[0];
            int code = Integer.parseInt(data[1]);

            byte[] bytes = new byte[2];
            bytes[0] = 0x3;
            bytes[1] = (byte) code;

            try{
                addr = InetAddress.getByName(ip);

                DatagramPacket dp = new DatagramPacket(bytes, bytes.length, addr, 15623);
                socket.send(dp);
            }
            catch (Exception e)
            {
                return -1;
            }

            return 0;
        }

        @Override
        protected void onPostExecute(Integer result)
        {
            if(result != 0)
                Toast.makeText(getApplicationContext(), R.string.send_error, Toast.LENGTH_SHORT).show();
        }
    }

    private class SendTask extends AsyncTask<String, Void, Integer> {
        @Override
        protected Integer doInBackground(String... data)
        {
            InetAddress addr;
            String ip = data[0];

            byte[] bytes = new byte[1];
            bytes[0] = 0x01;

            try{
                addr = InetAddress.getByName(ip);
                DatagramPacket dp = new DatagramPacket(bytes, bytes.length, addr, 15623);
                socket.send(dp);
            }
            catch (Exception e)
            {
                return -1;
            }

            return 0;
        }

        @Override
        protected void onPostExecute(Integer result)
        {
            if(result != 0)
                Toast.makeText(getApplicationContext(), R.string.send_error, Toast.LENGTH_SHORT).show();
        }
    }

    class RecvTask extends AsyncTask<Void, Void, Void>
    {
        @Override
        protected Void doInBackground(Void... params)
        {

            while(true)
            {
                try {
                    byte[] recv_buf = new byte[1024];
                    DatagramPacket packet = new DatagramPacket(recv_buf, recv_buf.length);
                    socket.receive(packet);

                    InetAddress addr = packet.getAddress();
                    byte[] data = packet.getData();
                    int len = packet.getLength();

                    if(len <  1 || data[0] != 0x2)
                        continue;

                    sendToGui(data, len, addr.toString());
                }
                catch (Exception ex)
                {
                    ex.printStackTrace();
                }
            }
        }

        private void sendToGui(byte[] data_a, int len_a, String ip_a)
        {
            final byte[] data = data_a;
            final int len = len_a;
            final String ip = ip_a.substring(1);

            final ArrayList<Macro> macList = new ArrayList<Macro>();
            for (int j = 2; j < len;)
            {
                int dlen = data[j];
                Integer i = (int) data[j - 1];
                String str = "";
                for(int k = j + 1; k <= j + dlen; k++)
                {
                    char c = (char) data[k];
                    str = str.concat(String.valueOf(c));
                }
                Macro macro = new Macro(i, str);
                macList.add(macro);

                j += dlen + 2;
            }

            runOnUiThread(new Runnable() {
                @Override
                public void run()
                {
                    adapter.updateList(macList);
                }
            });
        }

    }
}
