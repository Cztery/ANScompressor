import matplotlib.pyplot as plt
import json
import sys

filename = sys.argv[1]
f = open(filename)
data = json.load(f)
xCounts = list(range(0,256))
xCumul = list(range(0,257))

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

plt.subplot(2,2,1)
plt.plot(xCounts, data["counts"], 'b.')
plt.title("symbol counts")
plt.subplot(2,2,2)
plt.plot(xCounts, data["counts_norm"], 'b.')
plt.title("normalized symbol counts")
plt.subplot(2,2,3)
plt.plot(xCumul, data["cumul"], 'b.')
plt.title("cumulative\nsymbol frequencies")
plt.subplot(2,2,4)
plt.plot(xCumul, data["cumul_norm"], 'b.')
plt.title("normalized\ncumulative freqs")

plt.show()