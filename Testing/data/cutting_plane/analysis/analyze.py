import os
from datetime import datetime
from shutil import copyfile
import matplotlib.pyplot as plt
import numpy as np
import csv

logfname = "log.txt"
atts_of_interest = [0, 1, 2, 3]
alpha = 0.2
linewidth = 0.3
normalize_x_axis = True
normalize_y_axis = True
#pattern = "---"
pattern = "-=-"

#atts = ['h(x)', "updated_lambda", "original_lambda", "lagrangian_magnitude", "Grad_E_magnitude"]
atts = ['h(x)', "maximumUpdateAllowed", "m_TimeSteps[dom][k]", "original_gradient_magnitude"]

maindir = datetime.now().strftime('%Y-%m-%d-%H-%M-%S') + '/'

if not os.path.exists(maindir):
    os.makedirs(maindir)
    
copyfile("../cutting_plane.xml", maindir + "cutting_plane.xml")

for att_of_interest in atts_of_interest:

	outdir = maindir + atts[att_of_interest] + '/'

	if not os.path.exists(outdir):
		os.makedirs(outdir)

	logf = open(logfname, 'r')

	entries = []
	split_it = 0
	count = 0

	att_count = 0
	  
	while True: 
		count += 1
	  
		# Get next line from file 
		line = logf.readline() 
	  
		# if line is empty 
		# end of file is reached 
		if not line: 
		    break
		    
		if(line.startswith(pattern)):
			line = line.lstrip(pattern).split()
			line = list(map(float, line))
			att_count = len(line) - 2
			entries.append(line)
		elif(line.startswith('+++') and split_it is 0):
			split_it = len(entries)
			
	logf.close()

	entries = np.array(entries)

	doms = int(entries.max(axis=0)[0] + 1)

	parts_per_dom = int(entries.max(axis=0)[1] + 1)

	print("Creating table...")

	# dim1 domain, dim2 particles, dim3 list of observed variables.
	#table = np.zeros((doms, parts_per_dom, maxcount+1, entries.shape[1]-2))
	table = []

	for i in range(doms):
		l = []
		for j in range(parts_per_dom):
			l.append([])
		table.append(l)
			
	for entry in entries:
		table[int(entry[0])][int(entry[1])].append(list(entry[2:]))

	print("Computing boundaries...")

	max_x = 0
	for i in range(doms):
		for j in range(parts_per_dom):
			if(max_x < len(table[i][j])):
				max_x = len(table[i][j])


	max_y = 0

	for i in range(doms):
		for j in range(parts_per_dom):
			for k in range(len(table[i][j])):
				if(abs(table[i][j][k][att_of_interest]) > max_y):
					max_y = abs(table[i][j][k][att_of_interest])

	print("Max x", max_x)
	print("Max y", max_y)

	print("All particles graph...")
	for i in range(doms):
		if(normalize_x_axis):
			plt.xlim([0, max_x])
		if(normalize_y_axis):
			plt.ylim([-max_y, max_y])
		for j in range(parts_per_dom):
			data = []
			for k in range(len(table[i][j])):
				data.append(table[i][j][k][att_of_interest])
			#values = np.array([range(data.shape[0])]).reshape((data.shape[0],))
			values = np.array([range(max_x - len(data), max_x)]).reshape((len(data),))
			
			plt.plot(values, data, '-o', alpha = alpha, linewidth = linewidth)
		
		plt.savefig(outdir+"dom"+str(i)+".png")
		plt.clf()
		
	startings = []

	print("Per particle graph...")
	for i in range(doms):
		starts = []
		dname = outdir+str(i)+'/'		
		if not os.path.exists(dname):
			os.makedirs(dname)
		for j in range(parts_per_dom):
			if(normalize_x_axis):
				plt.xlim([0, max_x])
			if(normalize_y_axis):
				plt.ylim([-max_y, max_y])
			print("Per particle graph...", i, j)
			data = []
			for k in range(len(table[i][j])):
				data.append(table[i][j][k][att_of_interest])
			values = np.array([range(max_x - len(data), max_x)]).reshape((len(data),))
			starts.append(max_x - len(data))
			
			plt.plot(values, data, '-o', alpha = alpha, linewidth = linewidth)
		
			plt.savefig(dname+"par"+str(j)+".png")
			plt.clf()
		startings.append(starts)
		
	print("Graph Viz Done!")

print("Outputting CSVs...")


for i in range(doms):
	csvout= [["" for horizontal in range((parts_per_dom+1)*att_count)] for vertical in range(1+max_x)]
	print(len(csvout), len(csvout[0]))
	for j in range(parts_per_dom):
	
		for l in range(att_count):
			csvout[ 0][ l + l*parts_per_dom + j] = str(atts[l])
		
		skip = startings[i][j]
		for k in range(len(table[i][j])):
			for l in range(att_count):
				csvout[ 1 + skip +k][ l + l*parts_per_dom + j] = str(table[i][j][k][l])
	
	with open(maindir + "dom" + str(i) +".csv", "w") as f:
		writer = csv.writer(f)
		writer.writerows(csvout)
	
	
		
		
		
		
