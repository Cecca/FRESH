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

"""Given a results file plots curves matching a given query. By
default takes the last experiment from the given file, and a random
query.

If given the path to the dataset, also plots in background the other curves.
"""

import sys
import msgpack
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import argparse
import bz2
from pprint import pprint
import random

def get_limits(curves):
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
    return xmin, xmax, ymin, ymax
            

def plot_curves(curves, ax, color, linewidth):
    coll = matplotlib.collections.LineCollection(curves)
    coll.set_color(color)
    coll.set_linewidth(linewidth)
    
    ax.add_collection(coll, autolim=True)

    
def plot(dataset, queries, query_result, outpath):
    fig = plt.figure(figsize=(10,10), dpi=150)
    ax = plt.gca()
    ax.set_facecolor('black')

    all_curves = [c[b'points'] for c in dataset]
    
    plot_curves(all_curves, ax, 'white', 0.1)

    match_set = set(query_result[b'matching curves'])
    matches = [c[b'points'] for c in dataset
               if c[b'id'] in match_set]

    plot_curves(matches, ax, 'green', 2)
    
    query_id = query_result[b'query id']
    query = None
    for q in queries:
        if query_id == q[b'id']:
            query = q
            break
    if query is not None:
        qpts = query[b'curve'][b'points']
        plot_curves([qpts], ax, 'red', 2)
    else:
        print("WARNING: query curve not in the dataset")

    radius = 0.1
    center_x, center_y = -8.62 , 41.1584136
        
    xmin, xmax, ymin, ymax = get_limits(matches + all_curves)
    xmin = center_x - radius
    xmax = center_x + radius
    ymin = center_y - radius
    ymax = center_y + radius

    ax.set_xlim(xmin, xmax)
    ax.set_ylim(ymin, ymax)
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    plt.tight_layout()
    plt.savefig(outpath, dpi=150)

    
def main():
    argp = argparse.ArgumentParser()
    argp.add_argument("--dataset", metavar="FILE")
    argp.add_argument("--queries", metavar="FILE")
    argp.add_argument("results", metavar="RES.mpk.bz2")
    argp.add_argument("--output", metavar="FILE", default="results.png")

    args = argp.parse_args()

    dataset = []
    with open(args.dataset, "rb") as fp:
        unpacker = msgpack.Unpacker(fp)
        dataset = list(unpacker.__iter__())
    print("Loaded dataset with", len(dataset), "curves from", args.dataset)

    with open(args.queries, "rb") as fp:
        unpacker = msgpack.Unpacker(fp)
        queries = list(unpacker.__iter__())
    
    result = None
    result_cnt = 0
    with bz2.open(args.results, "rb") as fp:
        unpacker = msgpack.Unpacker(fp)
        for obj in unpacker:
            result = obj
            result_cnt += 1
    print("Loaded result with tags")
    pprint(result[b'tags'])
    query_results = result[b'tables'][b'results']
    print("There are", len(query_results), "query results")

    query_result = random.choice(query_results)
    print("Selected query result", query_result[b'query id'])

    plot(dataset, queries, query_result, args.output)

if __name__ == '__main__':
    main()
    
