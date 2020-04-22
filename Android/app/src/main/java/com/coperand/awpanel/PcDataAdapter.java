package com.coperand.awpanel;

import android.content.Context;
import android.content.Intent;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

class PcDataAdapter extends RecyclerView.Adapter<PcDataAdapter.ViewHolder> {

    private LayoutInflater inflater;
    private MainActivity activity;
    private ArrayList<MainActivity.Pc> pc;

    PcDataAdapter(Context context) {
        this.activity = (MainActivity) context;
        this.inflater = LayoutInflater.from(context);
        this.pc = activity.getPc();
    }

    public void updateList()
    {
        this.pc = activity.getPc();
        this.notifyDataSetChanged();
    }

    @Override
    public PcDataAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {

        View view = inflater.inflate(R.layout.pc_list_item, parent, false);
        return new ViewHolder(view);
    }


    @Override
    public void onBindViewHolder(PcDataAdapter.ViewHolder holder, int position) {
        MainActivity.Pc item = pc.get(position);
        holder.pcIconView.setImageResource(R.drawable.baseline_desktop_windows_black_18dp);
        String str = "PC "  + position;
        holder.pcView.setText(str);
        holder.ipView.setText(item.ip);
    }

    @Override
    public int getItemCount() {
        return pc.size();
    }

    class ViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {
        final ImageView pcIconView;
        final TextView pcView, ipView;
        ViewHolder(View view){
            super(view);
            pcIconView = (ImageView)view.findViewById(R.id.pc_icon);
            pcView = (TextView) view.findViewById(R.id.pc);
            ipView = (TextView) view.findViewById(R.id.ip);
            view.setOnClickListener(this);
        }

        @Override
        public void onClick(View view) {
            int position = getAdapterPosition();
            if (position != RecyclerView.NO_POSITION) {
                MainActivity.Pc item = pc.get(position);

                Intent intent = new Intent(activity, MacrosActivity.class);
                intent.putExtra("macros", item.macros);
                intent.putExtra("ip", item.ip);
                activity.startActivity(intent);
            }
        }
    }
}
