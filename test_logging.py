#!/usr/bin/env python
from subprocess import Popen, PIPE
import time
import pause
from sys import exit

bulk = Popen(['./bulk','2'], stdin=PIPE)

commands = range(0, 9, 1)

interval = 1
start = int(time.time()) + 2
stamps = range(start, start + 9 * interval, interval)
pause.until(start)

for i in commands:
    bulk.stdin.write("%d\n" % i)
    pause.until(stamps[i] + interval)
bulk.stdin.close()

outputs = [
        "bulk: %d, %d\n" % (commands[0], commands[1])
        , "bulk: %d, %d\n" % (commands[2], commands[3])
        , "bulk: %d, %d\n" % (commands[4], commands[5])
        , "bulk: %d, %d\n" % (commands[6], commands[7])
        , "bulk: %d\n" % (commands[8])
        ]

for i in range(0, 5, 1):
    try:
        print "testing bulk%d.log" % stamps[i * 2]
        with open("bulk%d.log" % stamps[i * 2]) as f:
            s = f.readline()
            if (s != outputs[i]):
                exit("Error: bulk%d.log:\n\t'%s'\nvs\n\t'%s'\nexpected" % (stamps[i], s, outputs[i]));
    except IOError:
        exit("bulk%s.log is not found" % stamps[i]);
