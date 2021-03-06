const fs = require('fs')
let arr = {
    max: function (array) {
        return Math.max.apply(null, array);
    },

    min: function (array) {
        return Math.min.apply(null, array);
    },

    range: function (array) {
        return arr.max(array) - arr.min(array);
    },

    midrange: function (array) {
        return arr.range(array) / 2;
    },

    sum: function (array) {
        var num = 0;
        for (var i = 0, l = array.length; i < l; i++) num += array[i];
        return num;
    },

    mean: function (array) {
        return arr.sum(array) / array.length;
    },

    median: function (array) {
        array.sort(function (a, b) {
            return a - b;
        });
        var mid = array.length / 2;
        return mid % 1 ? array[mid - 0.5] : (array[mid - 1] + array[mid]) / 2;
    },
    Quartile_25(data) {
        return this.Quartile(data, 0.25);
    },
    Quartile_75(data) {
        return this.Quartile(data, 0.75);
    },
    Quartile(data, q) {
        data = (data).sort();
        var pos = ((data.length) - 1) * q;
        var base = Math.floor(pos);
        var rest = pos - base;
        if ((data[base + 1] !== undefined)) {
            return data[base] + rest * (data[base + 1] - data[base]);
        } else {
            return data[base];
        }
    },

    modes: function (array) {
        if (!array.length) return [];
        var modeMap = {},
            maxCount = 0,
            modes = [];

        array.forEach(function (val) {
            if (!modeMap[val]) modeMap[val] = 1;
            else modeMap[val]++;

            if (modeMap[val] > maxCount) {
                modes = [val];
                maxCount = modeMap[val];
            }
            else if (modeMap[val] === maxCount) {
                modes.push(val);
                maxCount = modeMap[val];
            }
        });
        return modes;
    },

    variance: function (array) {
        var mean = arr.mean(array);
        return arr.mean(array.map(function (num) {
            return Math.pow(num - mean, 2);
        }));
    },

    standardDeviation: function (array) {
        return Math.sqrt(arr.variance(array));
    },

    meanAbsoluteDeviation: function (array) {
        var mean = arr.mean(array);
        return arr.mean(array.map(function (num) {
            return Math.abs(num - mean);
        }));
    },

    zScores: function (array) {
        var mean = arr.mean(array);
        var standardDeviation = arr.standardDeviation(array);
        return array.map(function (num) {
            return (num - mean) / standardDeviation;
        });
    }
};

try {
    const data = fs.readFileSync('./loglistener.txt', 'utf8')
    commands = []
    currentCommand = null
    data.split("\n").forEach(line => {
        if (line.includes("[INFO: Master")) {
            let match = line.match(/([\d|:|.]*).*ID:(\d+).*LED .*?(\d+)/)
            let [_, time, id, led, seq] = match
            currentCommand = { seq, time, rev: [] }
            commands.push(currentCommand)
        }
        if (line.includes("[INFO: Slave")) {
            let match = line.match(/([\d|:|.]*).*ID:(\d+).*LED .*?(\d+),.*?(\d+)/)
            let [_, time, id, led, seq] = match
            currentCommand.rev.push({ time, id })
        }
    })
    printStats = (a, name) => {
        let r = (i) => Math.round(10000 * i) / 10000
        console.log(`${name},  min: ${r(arr.min(a))},  max: ${r(arr.max(a))},  mean: ${r(arr.mean(a))},  median: ${r(arr.median(a))},  std. dev.: ${r(arr.standardDeviation(a))}, lower Q.: ${r(arr.Quartile_25(a))}, upper Q.: ${r(arr.Quartile_75(a))}`)
    }
    let n = commands.length;
    let nodes = Math.max(...commands.map(c => c.rev.length))
    let reliability = commands.map(c => c.rev.length / nodes);
    let latency = commands.map(c => 1000 * Math.max(...c.rev.map(r =>
        (r.time.split(/[:]/)[0] - c.time.split(/[:]/)[0]) * 60
        + (r.time.split(/[:]/)[1] - c.time.split(/[:]/)[1]))
    ));
    console.log("");
    console.log("nodes: " + nodes + ", n=" + n)
    printStats(reliability, "reliability")
    printStats(latency, "latency")
    console.log("");

} catch (err) {
    console.error(err)
}