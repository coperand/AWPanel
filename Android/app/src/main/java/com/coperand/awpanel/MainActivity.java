package com.coperand.awpanel;

import android.net.DhcpInfo;
import android.net.wifi.WifiManager;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DividerItemDecoration;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;

import android.os.AsyncTask;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import java.io.Serializable;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity
{
    private DatagramSocket socket = null;
    private ArrayList<Pc> pc = null;
    private PcDataAdapter adapter;
    private AsyncTask task;

    public ArrayList<Pc> getPc()
    {
        return pc;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        if(socket == null)
        {
            try {
                socket = new DatagramSocket();
            } catch (SocketException e) {
                e.printStackTrace();
            }
        }

        if(pc == null)
        {
            pc = new ArrayList<Pc>();
        }

        task = new RecvTask().executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);

        RecyclerView recyclerView = (RecyclerView) findViewById(R.id.pc_list);
        adapter = new PcDataAdapter(this);
        DividerItemDecoration dividerItemDecoration = new DividerItemDecoration(recyclerView.getContext(), LinearLayoutManager.VERTICAL);
        recyclerView.addItemDecoration(dividerItemDecoration);
        recyclerView.setAdapter(adapter);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        task.cancel(true);
        socket.close();
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        if (checkWiFi())
            new SendTask().execute();
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
                pc.clear();
                new SendTask().execute();
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

    public static class Macro implements Serializable, Parcelable
    {
        public int code;
        public String description;

        public Macro(int x, String y) {
            code = x;
            description = y;
        }

        public Macro(Parcel in) {
            code = in.readInt();
            description = in.readString();
        }

        @Override
        public int describeContents() {return 0;}

        @Override
        public void writeToParcel(Parcel dest, int flags)
        {
            dest.writeInt(code);
            dest.writeString(description);
        }

        public static final Creator<Macro> CREATOR = new Creator<Macro>() {
            @Override
            public Macro createFromParcel(Parcel in) {
                return new Macro(in);
            }

            @Override
            public Macro[] newArray(int size) {
                return new Macro[size];
            }
        };
    }

    public class Pc
    {
        public String ip;
        public ArrayList<Macro> macros;

        public Pc(String x, ArrayList<Macro> y) {
            ip = x;
            macros = y;
        }
    }

    private InetAddress getBroadcastAddress()
    {
        InetAddress addr;
        try
        {
            WifiManager wifi = (WifiManager) getSystemService(WIFI_SERVICE);
            DhcpInfo dhcp = wifi.getDhcpInfo();

            int broadcast = (dhcp.ipAddress & dhcp.netmask) | ~dhcp.netmask;
            byte[] quads = new byte[4];
            for (int k = 0; k < 4; k++)
                quads[k] = (byte) (broadcast >> (k * 8));
            addr = InetAddress.getByAddress(quads);
        } catch (Exception e)
        {
            return null;
        }
        return addr;
    }

    private class SendTask extends AsyncTask<Void, Void, Integer>
    {
        @Override
        protected Integer doInBackground(Void... data)
        {
            byte[] bytes = new byte[1];
            bytes[0] = 0x01;

            try{
                InetAddress addr = getBroadcastAddress();
                if(addr == null)
                    return -1;
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
                int code = (int) data[j - 1];
                String str = "";
                for(int k = j + 1; k <= j + dlen; k++)
                {
                    char c = (char) data[k];
                    str = str.concat(String.valueOf(c));
                }
                Macro macro = new Macro(code, str);
                macList.add(macro);

                j += dlen + 2;
            }
            final Pc pcEntity = new Pc(ip, macList);

            runOnUiThread(new Runnable() {
                @Override
                public void run()
                {
                    boolean found = false;
                    for(Pc item:pc)
                    {
                        if (item.ip.equals(pcEntity.ip))
                        {
                            item.macros = pcEntity.macros;
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                        pc.add(pcEntity);
                    adapter.updateList();
                }
            });
        }

    }

}
