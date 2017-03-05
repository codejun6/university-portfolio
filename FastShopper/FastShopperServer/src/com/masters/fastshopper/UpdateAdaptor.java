package com.masters.fastshopper;

public abstract class UpdateAdaptor {
	protected String action;
	protected String msg;
	protected int arg;
	public UpdateAdaptor setAction(String action) {
		this.action = action;
		return UpdateAdaptor.this;
	}
	public UpdateAdaptor setMessage(String msg) {
		this.msg = msg;
		return UpdateAdaptor.this;
	}
	public UpdateAdaptor setInt(int arg) {
		this.arg = arg;
		return UpdateAdaptor.this;
	}
	public abstract void update();
}