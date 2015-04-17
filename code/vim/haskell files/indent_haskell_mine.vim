" Vim indent file
" Language:     Haskell
" Author:       Chunting Gu <sprinfall@gmail.com>
" Version:      1.0
" Last Change:  2012-01-01

if exists('b:did_indent')
  finish
endif

let b:did_indent = 1

setlocal indentexpr=GetHaskellIndent2()
setlocal indentkeys=!^F,!0<Tab>,o,O,=in,0\|

function NextIndent(indents, cindent)
  let i = index(a:indents, a:cindent)
  if i == (len(a:indents) - 1) || i == -1
    let i = 0
  else
    let i = i + 1
  endif
  return a:indents[i]
endfunction

function PossibleIndents(plnum)
  let m = match(getline(a:plnum), '\<where\s*$')
  let pindent = indent(a:plnum)
  if m != -1
    let indents = [ pindent + &sw, pindent + m, pindent + m + &sw ]
    return indents
  endif
  return [ pindent ]
endfunction

function GetHaskellIndent2()
  let plnum = v:lnum - 1
  let line = getline(v:lnum)
  let pline = getline(plnum)

  let indents = PossibleIndents(plnum)
  let cindent = indent(v:lnum)
  return NextIndent(indents, cindent)

  return -1
endfunction

" TODO: Block comment.
function! IsComment(lnum)
  return getline(a:lnum) =~ '\s*--'
endfunction

" Find a non-blank and non-comment line at or above the current line.
" A blank line is defined as: '\s*'.
function! PrevNonBlankOrComment(lnum)
  let lnum = prevnonblank(a:lnum)
  while lnum > 0 && IsComment(lnum)
    let lnum = prevnonblank(lnum - 1)
  endwhile
  return lnum
endfunction

" Find a non-blank and non-comment line at or below the current line.
function! NextNonBlankOrComment(lnum)
  let lnum = nextnonblank(a:lnum)
  while lnum != 0 && IsComment(lnum)
    let lnum = nextnonblank(lnum + 1)
  endwhile
  return lnum
endfunction

" Return the matched line number & index of the first line at or above {lnum}.
" Stop if a line with top level indent is encountered.
function PrevLineMatch(lnum, pat, toplevel_indent)
  let m = -1
  let lnum = a:lnum
  while lnum > 0
    let m = match(getline(lnum), a:pat)
    if m != -1
      break
    endif
    if indent(lnum) == a:toplevel_indent
      break
    endif
    let lnum = PrevNonBlankOrComment(lnum - 1)
  endwhile
  return [lnum, m]
endfunction

" The top level indent of a Haskell file is the indent of the first
" non-blank and non-comment line.
function! TopLevelIndent()
  let first_line = NextNonBlankOrComment(1)
  return indent(first_line)
endfunction

function! GetHaskellIndent()
  " Handle comments at the very beginning since they will be ignored when
  " handle others.
  let plnum = prevnonblank(v:lnum - 1)
  if IsComments(plnum)
    return indent(plnum)
  endif

  " Then, handle with comments skipped.
  let plnum = PrevNonBlankOrComment(v:lnum - 1)

  " At the start of the file use zero indent.
  if plnum == 0
    return 0
  endif

  let line = getline(v:lnum)
  let pline = getline(plnum)

  " Get the top level indent.
  let toplevel_indent = TopLevelIndent()

  " End with operators. (TODO)
  if pline =~ '[!#$%&*+./<=>?@\\^|~-$' " Removed: ]
    let m = match(pline, '\<\(let\|where\)\s\+\zs')
    if m != -1
      " For indent like this:
      " where bmi = weight /
      "         height ^
      "           2
      return m + &sw
    else
      return indent(plnum) + &sw
    endif
  endif

  " End with do/in/let/where/case/of, indent with an extra shift width.
  if pline =~ '\<\(do\|in\|let\|where\|case\|of\)\s*$'
    " TODO
    let m = match(pline, '\<\(let\|where\)\s\+\zs')
    if m != -1
      return m + &sw
    else
      return indent(plnum) + &sw
    endif
  endif

  " Indent 'in' the same as 'let'.
  if line =~ '\s*in'
    let m = PrevLineMatch(plnum, '\<let\>', toplevel_indent)
    if m[1] != -1 " TODO: else
      return m[1]
    endif
  endif

  " Guard.
  if line =~ '\s*|'
    " If the previous line starts with '|', indent the same as it.
    " Otherwise, add an extra shift width.
    " A guard might be multiple lines, so the previous line might not start
    " with '|'.
    let m = PrevLineMatch(plnum, '\s*\zs|', toplevel_indent)
    if m[1] != -1
      return m[1]
    else
      return indent(plnum) + &sw
    endif
  endif

  " The clauses of let/where must be aligned. If not end with let/where, the
  " first clause is at the same line as let/where. The index of the token
  " after let/where is the indent we want.
  if line =~ '\<=\>'
    let m = PrevLineMatch(plnum, '\<\(let\|where\)\s\+\zs', toplevel_indent)
    if m[1] != -1
      return m[1]
    endif
  endif

  " Return the previous indent if we cannot handle it.
  return indent(plnum)

"  if line =~ '{$'
"    return match(line, '\s*where \zs\|\S') + &shiftwidth
"  endif

"  if line =~ '^\(instance\|class\).*\&.*where$'
"    return &shiftwidth
"  endif

"  if line !~ '\<else\>'
"    let s = match(line, '\<if\>.*\&.*\zs\<then\>')
"    if s > 0
"      return s
"    endif

"  let s = match(line, '\<do\s\+\zs[^{]\|\<where\s\+\zs\w\|\<let\s\+\zs\S\|^\s*\zs|\s')
"  if s > 0
"    return s
"  endif

"  return match(line, '\S')
endfunction
