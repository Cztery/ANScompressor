#!/usr/bin/python3

import matplotlib.pyplot as plt
import json
import sys

filename = sys.argv[1]
f = open(filename)
data = json.load(f)

probBitsSet = set()
filenameSet = set()

for iStat in data :
  probBitsSet.add(iStat["chunkSize"])
  filenameSet.add(iStat["imgname"])

def plotPerformanceByProbBits(key) :
    avgYvals = [0.0] * len(probBitsSet)
    numOfFiles = 0
    for file in filenameSet :
        YvalsPerFile = []
        numOfFiles += 1
        for probBits in sorted(probBitsSet) :
            for record in data:
                if record.get("chunkSize") == probBits and record.get("imgname") == file :
                    YvalsPerFile.append(record.get(key))
        avgYvals = zip(avgYvals, YvalsPerFile)
        avgYvals = [x + y for (x, y) in avgYvals]

        plt.figure(key)
        plt.plot(sorted(probBitsSet), YvalsPerFile, c='b', lw=0.8, ls=':', marker='o')
    
    avgYvals = [x / numOfFiles for x in avgYvals]
    plt.figure(key)
    plt.ylabel(key)
    plt.xlabel('chunk width')
    plt.xticks(sorted(probBitsSet), sorted(probBitsSet))
    plt.plot(sorted(probBitsSet), avgYvals, 'r-o', label='average')
plotPerformanceByProbBits("encodeSpeed")
plotPerformanceByProbBits("decodeSpeed")
plotPerformanceByProbBits("compressionRate")
plt.show()
