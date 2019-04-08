import pandas as pd
import matplotlib.pyplot as plt
import os
import subprocess
def total_vs_hit():
	data = pd.read_csv("table.csv")
	#x = range(0, len(data['total']))
	plt.plot(data['total'], data['hit']+data['miss'])
	plt.plot(data['total'], data['hit'])
	plt.plot(data['total'], data['miss'])
	plt.xticks(rotation=30)
	plt.xlabel('Reference')
	plt.ylabel('hit/miss')
	plt.show()
	#print(data.head(12))
def size_vs_miss():
	table_x = []
	table_y = []
	k = 2
	while k < 2048:
		cmd = "./valtlb379 -p LRU 64 "+str(k)+" < heapsort.txt"
		returned_value = os.popen(cmd).readlines()
		table_x.append(k)
		table_y.append(int(returned_value[0].split(" ")[2]))
		k = k*2
		print(k)
		#print('returned value:', returned_value[0].split(" ")[2])
	#reos.popen(cmd).readlines()
	#table_y.reverse()
	print(table_x)
	print(table_y)
	plt.plot(table_x, table_y)
	plt.show()

size_vs_miss();