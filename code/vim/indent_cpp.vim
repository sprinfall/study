" Vim indent file
" Language:	C++

" Usage:
" Copy this file to ~/vimfiles/indent/cpp.vim

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

" Set the function to do the work.
setlocal indentexpr=GetCppIndent()

" Only define the function once.
if exists("*GetCppIndent")
  finish
endif

function GetCppIndent()
  let indent = cindent(v:lnum)
  if v:lnum < 2
    return indent
  endif

  " Don't indent namespace block.
  let prev = v:lnum - 1
  let pline = getline(prev)
  if pline =~ '^\s*namespace\s\+\a\+\s*{\s*$'
    let indent = indent - &sw
  elseif pline =~ '^\s*{\s*$'
    if getline(prev - 1) =~ '^\s*namespace\s\+\a\+\s*$'
      let indent = indent - &sw
    endif
  endif

  return indent
endfunction

" vi: sw=2 et
