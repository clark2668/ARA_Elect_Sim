//basically a c++ recasting of the python code written by c glaser

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <math.h>
#include <vector>
#include <stdio.h>


using namespace std;

int main(int argc, char **argv){
	
	printf("Starting... \n");
	
	int NFOUR = 1024; //number of bins in fourier space
	//in freq domain: NFOUR/2 spectral amplitudes and NFOUR/2 phases
	//in time domain: NFOUR/2 voltages and NFOUR/2 times
	
	double dT = 5E-10; //size of a time step in seconds (this is 0.5 ns)
	double dF = 1./( double(NFOUR)/2 * dT); //frequency step in Hz
	
	vector <double> signal_amp(NFOUR/2);
	vector <double> signal_phs(NFOUR/2);
	
	for(int freq_bin=0; freq_bin<NFOUR/2; freq_bin++){ //loop over all frequency bins
		
		double freq = (double(freq_bin)+0.5) * dF; //the frequency of this freq_bin in Hz
		//the +0.5 puts you in the bin center, specifically to avoid the freq=0 case for the first frequency bin
		
		printf("Frequency bin and frequency: [%d , %.2f]\n",freq_bin,freq*1.e-6); //can print this out in MHz
		
		//the signal in frequency space should be composed of two parts
		//(1) a positive, real valued number corresponding to the spectral amplitude of the signal at frequence "freq"
		//this amplitude will be in units of Volts (this is H_k in numerical recipes)
		//(2) a real valued number between -pi and pi corresponding to the phase at frequency "freq"
		//this phase will be unitless
		
		//in the original implementation of this function
		//the E-Field is returned from signal generation in the time domain in units of V/m
		//it is immediately fourier-transformed by the numerical-recipes-in-c function "realft", putting it in the units of V
		//(remember, the output of the raw "numerical-recipes-in-c realft" routine returns the spectral coefficient H_k, which has units of V)
		//and then it undergoes the system electronics; so the system electronics were coded to apply to V
				
		//a warning that most Askaryan generators (Hanson & Connolly, Alvarez-Muniz, ZHS)
		//return a field in the units of V/m/MHz
		//it would be natural to multiply by MHz before using this function then
		//it's also typically the case that the antenna response has already been applied
		//such that the meters has also already dropped out
		//but that's not strictly required
		
		//you could of course use mV, etc
		//the point is that amplitude needs to have units of *voltage*
		
		ApplyElectronics(freq,signal_amp[freq_bin],signal_phs[freq_bin]);
		
	}
	
	return 0;
}


//takes three parameters
//the frequency of interest in MHz: freq
//the signal amplitude in freq domain, units are Volts: signal_amp
//the signal phase in freq domain, units are dimless: signal_phs in radians
void ApplyElectronics(double freq, double &signal_amp, double &signal_phs){
	double phase_current;
	if(abs(signal_amp)>0.) phase_current = atan2(signal_phs, signal_amp);
	else{
		if(signal_phs>0.) phase_current = M_PI;
		else if(signal_phs<0.) phase_current = -M_PI;
		else phase_current=0.;
	}
	//adjust the amplitude of the signal at this frequency
	double v_amp  = sqrt(signal_amp*signal_amp + signal_phs*signal_phs) * detector->GetElectGain_1D_OutZero( freq ); // apply gain (unitless) to amplitude

	//now adjust the phase
	signal_amp = v_amp * cos( phase_current - detector->GetElectPhase_1D(freq) );
	signal_phs = v_amp * sin( phase_current - detector->GetElectPhase_1D(freq) );
}

void ReadElectronicsGain(string filename){
	ifstream file(filename.c_str());
	
	string line;
	string line2;
	string line3;
	
	int N=-1;
	
	vector <double> xfreq_tmp;
	vector <double> ygain_tmp;
	vector <double> phase_tmp;
	
	int first_time =1;
	
	if(file.is_open()){
		while(file.good()){
			//we need to skip the first three lines
			//because this is header info
			int skipline=3;
			if(first_time==1){
				for(int sl=0; sl<skipline; sl++){
					getline(file,line);
				}
				first_time=0;
			}
			
			getline(file,line);
			N++;
			xfreq_tmp.push_back( atof( line.substr(0, line.find_first_of(",")).c_str() ) );
			cout<<"freq : "<<xfreq_tmp[N]<<"\t";

			line2 = line.substr( line.find_first_of(",")+1);

			ygain_tmp.push_back( atof( line2.substr(0, line2.find_first_of(",")).c_str() ) );
			cout<<"gain : "<<ygain_tmp[N]<<"\t";

			line3 = line2.substr( line2.find_first_of(",")+1);

			phase_tmp.push_back( atof( line3.substr(0).c_str() ) );
			cout<<"phase : "<<phase_tmp[N]<<" N : "<<N<<endl;
		}
		file.close();
	}
	else printf("File cannot be opened!\n");
}
