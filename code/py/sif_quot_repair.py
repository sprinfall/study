"""
sif_quot_repair.py

有一次不小心把《秋天里的春天》所有的引号都搞成了一个方向的，而且还是全角，
所以写了这个东西再把它们转换回来。

注意：输入文件 src.txt 必须为 utf-8 编码。
"""

import os
import codecs

# flag indicating whether the quot is open or closed
closed = True

def char_map(c):
    global closed
    mc = c
    if c == u'＂': 
        closed = not closed # revert
        if closed: mc = u'”'
        else: mc = u'“'
    return mc
    
def filter_line(line):
    closed = True # reset
    line = list(line)
    line = map(char_map, line)
    return ''.join(line)
    
def filter_file(src, dst):
    fi = codecs.open(src, 'r', 'utf-8')
    fo = codecs.open(dst, 'w', 'utf-8')
    
    lines = fi.readlines()
    for line in lines:
        line = line.strip(u'\n')
        line = filter_line(line)
        print >> fo, line

    fi.close()
    fo.close()
        
if __name__ == '__main__':
    print __doc__
    filter_file('src.txt', 'dst.txt')
