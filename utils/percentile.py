#!/usr/bin/env python

# Copyright 2018 Matteo Ceccarello
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

import numpy as np
import bz2
import glob
import sys


def load_distances(path):
    with bz2.open(path, 'r') as fp:
        distances = []
        for line in fp.readlines():
            line = line.decode('utf-8')
            if 'Skipping' not in line:
                s, t, d = line.split(' ')
                distances.append(float(d))
        return distances

def print_percentile(perc, path):
    p = np.percentile(load_distances(path), perc)
    print(path, p)


if __name__ == '__main__':
    perc = int(sys.argv[1])
    if len(sys.argv) > 2:
        paths = sys.argv[2:]
    else:
        paths = sys.stdin.readlines()
    for p in paths:
        path = p.strip()
        print_percentile(perc, path)

