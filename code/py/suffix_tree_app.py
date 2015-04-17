# -*- coding: utf-8 -*-
from suffix_tree import SuffixTree

def find(st, sub):
    """Search the first match of the given substring.

    Return the shift of found match or None if no match's found."""

    if not sub: return None

    if sub[0] not in st.root.trans: return None

    i, s = 0, st.root
    while True:
        k, p, s = s.trans[sub[i]]
        len1, len2 = p-k+1, len(sub)-i
        if len1 >= len2:
            if st.text[k:k+len2] == sub[i:]: return k-i
            break
        else:
            if st.text[k:k+len1] == sub[i:i+len1]: i += len1
            else: break

    return None

def get_leaf_depthes(node):
    "Used by find_all: Get depthes of all leaves from 'node'."
    ret = []
    for k, p, s in node.trans.values():
        ret += [i+p-k+1 for i in get_leaf_depthes(s)]
    return ret or [0]

def find_all(st, sub):
    """Search all matches of the given substring.

    Return the shift list of found matches or None if no match's found.
   
    Rationale:
    Once a match is found, the shifts of all suffixes after that match
    state will be the result."""

    if not sub: return None
    if sub[0] not in st.root.trans: return None
   
    found, i, s = False, 0, st.root
    scaned = 0 # length of the scaned
    while True:
        k, p, s = s.trans[sub[i]]
        len1, len2 = p-k+1, len(sub)-i
        if len1 >= len2:
            if st.text[k:k+len2] == sub[i:]:
                found, scaned = True, scaned+len1
            break
        else:
            if st.text[k:k+len1] == sub[i:i+len1]:
                i, scaned = i+len1, scaned+len1
            else: break
    if found:
        # shift_of_suffix = len(st.text) - len(suffix)
        leaf_depthes = get_leaf_depthes(s)
        return [len(st.text)-x-scaned for x in leaf_depthes]

    return None

def __lrs(s, l):
    result = [0, []]
    if len(s.trans) >= 2:
        for k, p, ns in s.trans.values():
            tmp = __lrs(ns, l+p-k+1)
            if tmp[0] > result[0]: result = tmp
            elif tmp[0] == result[0]: result[1] += tmp[1]
        if not result[0]:
            result = [l, [[v[0] for v in s.trans.values()]]]
    return result

def lrs(st):
    """Find the longest repeated substring(s).

    Result is given with the following format:
        [lengh_of_lrs, [(LRS1, shifts1), (LRS2, shifts2), ...]]
    E.g., for text 'maomaoxiaxia#', result is:
        [3, [('mao', [3, 0]), ('xia', [6, 9])]]"""

    length, shifts = __lrs(st.root, 0)
    result = [length, []]
    for shift in shifts:
        lrs_string = st.text[shift[0]-length:shift[0]]
        result[1].append((lrs_string, [x-length for x in shift]))
    return result

if __name__ == '__main__':
    text = 'banana#'
    st = SuffixTree(text)
    st.create()
    st.dump()
    print(find(st, 'na'))
    print(find_all(st, 'na'))
    print(lrs(st))

