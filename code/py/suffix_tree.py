# -*- coding: utf-8 -*-
"""
E. Ukkonen's algorithm to create suffix tree.
The paper [On-line construction of suffix tree] is here:
    [www.cs.helsinki.fi/u/ukkonen/SuffixT1withFigs.pdf]."""

class State:
    "State represents the suffix tree node."
    def __init__(self):
        self.trans = {}   # transition
        self.slink = None # suffix link
    # Get trans.
    def __getitem__(self, key): return self.trans[key]
    # Set/Add trans.
    def __setitem__(self, key, value): self.trans[key] = value

class SuffixTree:

    def __init__(self, text):
        self.text = text
        self.root = State()
        self.bottom = State()
        for i in range(len(self.text)):
            self.bottom[self.text[i]] = [i, i, self.root]
        self.root.slink = self.bottom

    def create(self):
        s, k, i = self.root, 0, 0
        for i in range(len(self.text)):
            s, k = self.__update(s, k, i)
            s, k = self.__canonize(s, k, i)

    def __test_and_split(self, s, k, p, t):
        if k <= p:
            nk, np, ns = s[self.text[k]]
            if t == self.text[nk+p-k+1]:
                return (True, s)
            r = State()
            s[self.text[nk]] = [nk, nk+p-k, r]
            r[self.text[nk+p-k+1]] = [nk+p-k+1, np, ns]
            return (False, r)
        return (t in s.trans, s)

    def __canonize(self, s, k, p):
        if p < k: return (s, k)
        nk, np, ns = s[self.text[k]]
        while np - nk <= p - k:
            k += np - nk + 1
            s = ns
            if k <= p: nk, np, ns = s[self.text[k]]
        return (s, k)

    def __update(self, s, k, i):
        oldr = self.root
        end_point, r = self.__test_and_split(s, k, i-1, self.text[i])
        while not end_point:
            r[self.text[i]] = [i, len(self.text)-1, State()]
            if oldr != self.root: oldr.slink = r
            oldr = r
            s, k = self.__canonize(s.slink, k, i-1)
            end_point, r = self.__test_and_split(s, k, i-1, self.text[i])
        if oldr != self.root: oldr.slink = s
        return (s, k)

    def dump(self):
        ret = "root\n"
        for t in self.root.trans:
            ret += self.__dump(self.root, t, 1)
        print(ret)

    def __dump(self, s, t, depth):
        k, p, ns = s.trans[t]
        ret = ""
        for i in range(depth): ret += "    "
        ret += "%d: %s\n" % (k, self.text[k:p+1])
        for nt in ns.trans:
            ret += self.__dump(ns, nt, depth+1)
        return ret
