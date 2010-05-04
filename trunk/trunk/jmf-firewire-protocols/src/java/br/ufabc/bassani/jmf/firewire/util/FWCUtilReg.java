package br.ufabc.bassani.jmf.firewire.util;

import java.util.Vector;

import javax.media.PackageManager;

public class FWCUtilReg {

	@SuppressWarnings("unchecked")
	public static void main(String[] args){
		Vector packagePrefix = PackageManager.getProtocolPrefixList();
		String myPackagePrefix = new String("br.ufabc.bassani.jmf.firewire");
		// Add new package prefix to end of the package prefix list. 
		packagePrefix.addElement(myPackagePrefix);
		PackageManager.setProtocolPrefixList(packagePrefix);
		// Save the changes to the package prefix list.
		PackageManager.commitProtocolPrefixList();
	}

}
