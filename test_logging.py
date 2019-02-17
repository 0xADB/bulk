#!/usr/bin/env python
from subprocess import Popen, PIPE
import time
import pause
from sys import exit

bulk = Popen(['./bulk','2'], stdin=PIPE)

interval = 1
start = int(time.time()) + 2
commands = [
        "{\n"
        , "1\n"
        , "2\n"
        , "}\n"
        , "3\n"
        , "4\n"
        , "{\n"
        , "5\n"
        , "}\n"
        ]

outputs = [
        "bulk: 1, 2, 3\n"
        , "bulk: 4\n"
        , "bulk: 5\n"
        ]

files = [
        "bulk%s.log" % (start + 1)
        , "bulk%s.log" % (start + 5)
        , "bulk%s.log" % (start + 7)
        ]
print files

stamps = range(start, start + len(commands) * interval, interval)
pause.until(start)

for i in range(0, len(commands)):
    bulk.stdin.write(commands[i])
    pause.until(stamps[i] + interval)
bulk.stdin.close()

for i in range(0, len(files)):
    try:
        print "testing %s" % files[i]
        with open(files[i]) as f:
            s = f.readline()
            if (s != outputs[i]):
                exit("Error: %s:\n\t'%s'\nvs\n\t'%s'\nexpected" % (files[i], s, outputs[i]));
    except IOError:
        exit("%s is not found" % files[i]);
