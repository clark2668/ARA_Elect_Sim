#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
#include <stdio.h>

using namespace std;

void ReadElectronicsGain(string filename, vector<double> &frequency, vector<double> &gain, vector<double> &phase);

int main(int argc, char **argv){
	
	printf("Starting... \n");
	
	int NFOUR = 1024; //number of bins in fourier space
	//in freq domain: NFOUR/2 spectral amplitudes and NFOUR/2 phases
	//in time domain: NFOUR/2 voltages and NFOUR/2 times
	
	double dT = 5E-10; //size of a time step in seconds (this is 0.5 ns)
	double dF = 1./( double(NFOUR)/2 * dT); //frequency step in Hz
	
	vector <double> signal_amp(NFOUR/2);
	vector <double> signal_phs(NFOUR/2);


	//before we begin looping, we should read in the electronics gain
	//we will store the results in these three vectors
	vector <double> sys_freq;
	vector <double> sys_gain;
	vector <double> sys_phas;
	ReadElectronicsGain("./data/ARA_Electronics_TotalGain_TwoFilters.txt",sys_freq,sys_gain,sys_phas);
	
	for(int freq_bin=0; freq_bin<NFOUR/2; freq_bin++){ //loop over all frequency bins
		
		double freq = (double(freq_bin)+0.5) * dF; //the frequency of this freq_bin in Hz
		//the +0.5 puts you in the bin center, specifically to avoid the freq=0 case for the first frequency bin
		
		//printf("Frequency bin and frequency: [%d , %.2f]\n",freq_bin,freq*1.e-6); //can print this out in MHz
		//ApplyElectronics(freq,signal_amp[freq_bin],signal_phs[freq_bin]);
		
	}
	
	return 0;
}

//this is basically an extraction of "ApplyElect_Tdomain" in the AraSim report class
//takes three parameters
//the frequency of interest in MHz: freq
//the signal amplitude in freq domain, units are Volts: signal_amp
//the signal phase in freq domain, units are dimless: signal_phs in radians

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
/*
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
*/


//! A function to read the ARA electronics resonse.
/*!
	This is basically an extraction from "ReadElectChain" in the AraSim Detector Class

	\param filename a string that is the name of the response files
	\param frequency a vector passed by reference, returning the frequencies
	\param gain a vector passed reference, returned containing the gain
	\param phase a vector passed by reference, returned containing the phase
*/
void ReadElectronicsGain(string filename, vector<double> &frequency, vector<double> &gain, vector<double> &phase){
	
	ifstream file(filename.c_str());
	
	string line;
	string line2;
	string line3;
	
	int N=-1;	
	int first_time =1;
	
	if(file.is_open()){ //while the file is open
		while(file.good()){ //while there is still file left to read
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

			//the data is stored in a csv file in the order "freq,gain,phase"
			//so parse that out

			frequency.push_back( atof( line.substr(0, line.find_first_of(",")).c_str() ) );
			//cout<<"freq : "<<frequency[N]<<"\t";

			line2 = line.substr( line.find_first_of(",")+1);
			gain.push_back( atof( line2.substr(0, line2.find_first_of(",")).c_str() ) );
			//cout<<"gain : "<<gain[N]<<"\t";

			line3 = line2.substr( line2.find_first_of(",")+1);
			phase.push_back( atof( line3.substr(0).c_str() ) );
			//cout<<"phase : "<<phase[N]<<" N : "<<N<<endl;
		}
		file.close();
	}
	else printf("File cannot be opened!\n");
}