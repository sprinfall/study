" Vim indent file
" Language:	C++

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

" Adapted from Java indent file.
function! SkipCppBlanksAndComments(startline)
  let lnum = a:startline
  while lnum > 1
    let lnum = prevnonblank(lnum)
    if getline(lnum) =~ '\*/\s*$'
      while getline(lnum) !~ '/\*' && lnum > 1
        let lnum = lnum - 1
      endwhile
      if getline(lnum) =~ '^\s*/\*'
        let lnum = lnum - 1
      else
        break
      endif
    elseif getline(lnum) =~ '^\s*//'
      let lnum = lnum - 1
    else
      break
    endif
  endwhile
  return lnum
endfunction

function GetCppIndent()
  let indent = cindent(v:lnum)
  if v:lnum < 2
    return indent
  endif

  " Don't indent namespace block.
  let prev = SkipCppBlanksAndComments(v:lnum - 1)
  let pline = getline(prev)
  if pline =~ '^\s*namespace\s\+\a\+\s*{\s*$'
    let indent = indent - &sw
  elseif pline =~ '^\s*{\s*$'
    if getline(SkipCppBlanksAndComments(prev - 1)) =~ '^\s*namespace\s\+\a\+\s*$'
      let indent = indent - &sw
    endif
  endif

  return indent
endfunction

" vi: sw=2 et
