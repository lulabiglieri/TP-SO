import glob, os
import pandas as pd
import subprocess
from subprocess import Popen, PIPE

executablePath = './../codigo/build/ContarPalabras'
os.chdir("./")
datasets = os.listdir("./data/")

print ("Ejecutando tests")

for dataset in datasets:
	#create output file
	results = './results/%s.csv' %(dataset)
	outputFile = open(results, 'w+')
	outputFile.write("cantThreads,time,algorithm\n")

	args = ""
	for file in os.listdir("./data/" + dataset):
		args += " ./data/" + dataset + "/" + file

	for cantThreads in [1,5,10,15,20,25,30]:
		command = executablePath + " %s %s %s" % (cantThreads, cantThreads, args)
		p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
		p.wait()

		print("Finished executing dataset %s with %s amount of threads" % (dataset, cantThreads))
		output=p.stdout.read().decode("utf-8")
		outputFile.write(output)
	
	outputFile.close()