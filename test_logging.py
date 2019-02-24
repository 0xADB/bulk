#!/usr/bin/env python
from subprocess import Popen, PIPE
import time
import pause
from sys import exit
from os import walk

bulk = Popen(['./bulk','2'], bufsize=1, stdin=PIPE)

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
        "bulk: 1, 2"
        , "bulk: 3, 4"
        , "bulk: 5"
        ]

files = [
        "bulk%s.log" % (start + 1)
        , "bulk%s.log" % (start + 4)
        , "bulk%s.log" % (start + 7)
        ]

stamps = range(start, start + len(commands) * interval, interval)
pause.until(start)

for i in range(0, len(commands)):
    bulk.stdin.write(commands[i])
    pause.until(stamps[i] + interval)
bulk.stdin.close()

logs = []
for (dirpath, dirnames, filenames) in walk("./"):
    for f in filenames:
        if (f.endswith(".log")):
            logs.append(f)
    break

print "Expected log files: ", set(files)
print "Found log files   : ", set(logs)
if (set(files) != set(logs)):
    exit("Error: log lists mismatch")

for i in range(0, len(files)):
    try:
        print "testing %s" % files[i]
        with open(files[i]) as f:
            s = f.readline()
            if (s != outputs[i]):
                exit("Error: %s:\n\t'%s'\nvs\n\t'%s'\nexpected" % (files[i], s, outputs[i]));
            else:
                print s, " - ok"
    except IOError:
        exit("%s is not found" % files[i]);
