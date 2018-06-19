import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
import math
from scipy.fftpack import fft, ifft
from matplotlib.font_manager import FontProperties
from matplotlib import rcParams
from scipy import stats

def main():
	
	data = np.genfromtxt('ARA_Electronics_TotalGain_TwoFilters.txt',delimiter=",",skip_header=3,names=['freq','gain','phase'])
	
	
	fig = plt.figure(figsize=(11.5,8/2))
	ax1 = fig.add_subplot(1,2,1)
	ax2 = fig.add_subplot(1,2,2)
	
	ax1.plot(data['freq'],20.*np.log10(data['gain']),'k',label='Gain')
	ax2.plot(data['freq'],np.unwrap(data['phase']),'k',label='Phase')
	
	#ax1.legend(fontsize=24)
	#size=18
	
	ax1.set_ylabel('Gain (dB)')
	ax1.set_xlabel('Frequency (MHz)')
	ax1.tick_params(axis='both', which='major')
	ax1.grid()
	#ax1.set_xlim([0,1000])
	ax2.set_ylabel('Phase (rad)')
	ax2.set_xlabel('Frequency (MHz)')
	ax2.tick_params(axis='both', which='major')
	ax2.grid()
	#ax1.set_xlim([0,1000])
	
	#ax1.set_ylim([0,30])
	#ax1.set_yticks([5,10,15,20,25,30])
	
	fig.savefig('ara_system_response.pdf',dpi=300,edgecolor='none',bbox_inches="tight") #save the figure
	
main()
