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

# This script converts datasets in UCR format to our own messagepack format,
# which we use for efficiency.

import sys
import msgpack
import os


def read_ucr_file(path):
    with open(path) as fp:
        points = []
        idx = 0
        for line in fp.readlines():
            coords = line.split(',')[1:] # skip the first element, which is the class, 
                                         # which we are not interested in
            lastx = None
            pts = []
            for c in coords:
                if float(c) != lastx:
                    pts.append([float(c)])
                    lastx = float(c)
            points.append({
                'points': pts,
                'id': idx
            })
            idx += 1
        return points


def read_ucr(path):
    if os.path.isdir(path):
        out = []
        idx = 0
        for f in os.listdir(path):
            for p in read_ucr_file(os.path.join(path,f)):
                p['id'] = idx
                idx += 1
                out.append(p)
        return out
    else:
        return read_ucr_file(path)


def write_points(points, path):
    with open(path, 'wb') as fp:
        # write metadata
        msgpack.pack({'dimensions': '1'}, fp)
        for p in points:
            msgpack.pack(p, fp)


if __name__ == '__main__':
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    write_points(read_ucr(input_path), output_path)
            

