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

class MacroDataAdapter extends RecyclerView.Adapter<MacroDataAdapter.ViewHolder> {

    private LayoutInflater inflater;
    private MacrosActivity activity;
    private ArrayList<MainActivity.Macro> macList;

    MacroDataAdapter(Context context, ArrayList<MainActivity.Macro> macList) {
        activity = (MacrosActivity) context;
        this.inflater = LayoutInflater.from(context);
        this.macList = macList;
    }

    public void updateList(ArrayList<MainActivity.Macro> macList)
    {
        this.macList = macList;
        activity.setMacList(macList);
        this.notifyDataSetChanged();
    }

    @Override
    public MacroDataAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {

        View view = inflater.inflate(R.layout.macro_list_item, parent, false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(MacroDataAdapter.ViewHolder holder, int position) {
        MainActivity.Macro item= macList.get(position);
        holder.macView.setText(item.description);
    }

    @Override
    public int getItemCount() {
        return macList.size();
    }

    class ViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {
        final TextView macView;
        ViewHolder(View view){
            super(view);
            macView = (TextView) view.findViewById(R.id.description);
            view.setOnClickListener(this);
        }

        @Override
        public void onClick(View view) {
            int position = getAdapterPosition();
            if (position != RecyclerView.NO_POSITION) {
                activity.sendCode(position);
            }
        }
    }
}

