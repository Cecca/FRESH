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
import os

path = sys.argv[1]
outpath = sys.argv[2]

outstream = open(outpath, "wb")

with open(path) as ifp:
    curve_id = -1
    for line in ifp.readlines():
        if curve_id < 0:
            curve_id += 1
            continue
        print('processing line', curve_id)
        trj_str = line.split('","')[-1][1:-3]
        if len(trj_str) > 0:
            trj_tokens = trj_str.split('],[')
            curve = []
            for tok in trj_tokens:
                tok = tok.replace(',', ' ').replace('[', '').replace(']', '').replace('"', '')
                try:
                    x, y = tuple(tok.split())
                    curve.append((float(x), float(y)))
                except ValueError as e:
                    print('`',tok, '`', trj_tokens, "LINE:", line)
                    raise e
            curve_obj = {
                'id': curve_id,
                'points': curve
            }
            msgpack.dump(curve_obj, outstream)
        curve_id += 1

outstream.close()
