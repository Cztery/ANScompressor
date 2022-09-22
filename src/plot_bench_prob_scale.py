import matplotlib.pyplot as plt
import json
import sys

# filename = sys.argv[1]
filename = "/home/ubu/REPOS/ANScompressor1/build/testjson.json"
f = open(filename)
data = json.load(f)

probBitsSet = set()
filenameSet = set()

for iStat in data :
  probBitsSet.add(iStat["probBits"])
  filenameSet.add(iStat["imgname"])

def plotPerformanceByProbBits(key) :
    avgYvals = [0.0] * len(probBitsSet)
    numOfFiles = 0
    for file in filenameSet :
        YvalsPerFile = []
        numOfFiles += 1
        for probBits in sorted(probBitsSet) :
            for record in data:
                if record.get("probBits") == probBits and record.get("imgname") == file :
                    YvalsPerFile.append(record.get(key))
        avgYvals = zip(avgYvals, YvalsPerFile)
        avgYvals = [x + y for (x, y) in avgYvals]

        plt.figure(key)
        plt.plot(sorted(probBitsSet), YvalsPerFile, 'b-o')

    avgYvals = [x / numOfFiles for x in avgYvals]
    plt.figure(key)
    plt.plot(sorted(probBitsSet), avgYvals, 'r-o')

plotPerformanceByProbBits("encodeSpeed")
plotPerformanceByProbBits("decodeSpeed")
plotPerformanceByProbBits("compressionRate")
plt.show()