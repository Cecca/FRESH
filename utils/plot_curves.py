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

import sys
import msgpack
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

def euclidean(pair):
    a = pair[0]
    b = pair[1]
    diff_x = a[0] - b[0]
    diff_y = a[1] - b[1] 
    return np.sqrt(diff_x*diff_x + diff_y*diff_y)

def plot_curves(curves, outpath):
    fig = plt.figure(figsize=(10,10), dpi=150)
    ax = plt.gca()
    ax.set_facecolor('black')
    coll = matplotlib.collections.LineCollection(curves)
    coll.set_color("white")
    coll.set_linewidth(0.01)
    xmin = None
    xmax = None
    ymin = None
    ymax = None
    for curve in curves:
        for x, y in curve:
            ymin = min(y, ymin) if ymin is not None else y
            ymax = max(y, ymax) if ymax is not None else y
            xmax = max(x, xmax) if xmax is not None else x
            xmin = min(x, xmin) if xmin is not None else x

    ax.add_collection(coll, autolim=True)
    ax.set_xlim(xmin, xmax)
    ax.set_ylim(ymin, ymax)
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    plt.tight_layout()
    plt.savefig(outpath, dpi=150)

xmin=116
xmax=117
ymin=39
ymax=40.5
    
def main():
    inpath = sys.argv[1]
    outpath = sys.argv[2]
    curves = []
    with open(inpath, "rb") as fp:
        unpacker = msgpack.Unpacker(fp)
        for obj in unpacker:
            points = []
            for x, y in obj[b'points']:
                if xmin < x < xmax and ymin < y < ymax:
                    points.append((x,y))
            if len(points) > 1:
                avg_dist = sum(map(euclidean, zip(points, points[1:]))) / (len(points)-1)
                print("Appending curve with", len(points), "points, average distance", avg_dist)
                curves.append(points)
    plot_curves(curves, outpath)


if __name__ == '__main__':
    main()
