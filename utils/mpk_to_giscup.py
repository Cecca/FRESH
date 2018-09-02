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

import msgpack
import sys
import bz2
import os


def load_data(path):
    if path.endswith('.bz2'):
        fp = bz2.open(path, 'rb')
    else:
        fp = open(path, 'rb')
    for data in msgpack.Unpacker(fp, encoding='utf-8'):
        if 'points' in data:
            yield data
    fp.close()


def write_data(data, path):
    txtfile = path
    txtout = open(txtfile, 'w')
    files_dir = txtfile + ".files"
    assert not os.path.isdir(files_dir)
    os.makedirs(files_dir)
    for curve in data:
        ident = curve['id']
        points = curve['points']
        fname = os.path.join(files_dir, 'file-{}.dat'.format(ident))
        txtout.write(fname)
        txtout.write("\n")
        with open(fname, 'w') as fp:
            fp.write("x y k tid\n")
            k = 0
            for point in points:
                fp.write("{:.32f}".format(point[0]))
                fp.write(" ")
                if len(point) == 1:
                    fp.write("0.0")
                else:
                    fp.write("{:.32f}".format(point[1]))
                fp.write(" {} {}\n".format(k, ident))
                k += 1
    txtout.close()


if __name__ == '__main__':
    inpath = sys.argv[1]
    outpath = sys.argv[2]
    write_data(load_data(inpath), outpath)

