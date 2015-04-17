// qindiff.js
// adam.gu, sprinfall@gmail.com
// last update: 05/2008

function $(id) { return document.getElementById(id) }

var type = function() {
    return {
        IDT: 0, // identical
        DIF: 1, // different
        ABS: 2, // absent
        ADD: 3, // added
        SEL: 4  // selected
    }
}()

var colors = [
    '#ffffff', // IDT
    '#ffcc66', // DIF
    '#dddddd', // ABS
    '#ffffff', // ADD
    '#ffb6c1'  // SEL
]

var resultX = []
var resultY = []

var undolist = []
var redolist = []

var diffnum = 0

var view = null


function enable(ctrl, enabled) { ctrl.disabled = !enabled }

// TODO: test in Apple Mac & Linux
var CR = '\r'
var LF = '\n'

/*
function browser() {
    var userAgent = window.navigator.userAgent
    alert(userAgent)
    if (userAgent.indexOf('Firefox') >= 0) {
        LF = '\n'
    } else if (userAgent.indexOf('MSIE') >= 0) {
        LF = '\r\n'
    } else if (userAgent.indexOf('Opera') >= 0) {
        LF = '\r\n'
    } else if (userAgent.indexOf('Safari') >= 0) {
        LF = '\n'
    }// TODO
}*/

window.onload = function() {
    extendArray();
    view = new View();
    view.reset();
}

function startDiff() {
    var textX = view.getTextX()
    var textY = view.getTextY()

    if (textX.length > 0 || textY.length > 0) {
        var X = textX.split(LF);
        var Y = textY.split(LF);

        var lcs = new LCSubseq(X, Y);
//        alert('test');
        lcs.createTable();
        lcs.backTrace();
//        alert(lcs.R);

        diff(X, Y, lcs.R, lcs.P, lcs.Q);
//        diff_2(X, Y, lcs.Rx, lcs.Ry, ignore);

        diffnum = 3;//diff.getDiffNum()  TODO
        view.show()
    }
}

function showText() { view.showText() }
function hideText() { view.hideText() }

function select() { view.select(this.id.slice(3)) }
function unselect() { view.unselect(this.id.slice(3)) }


var mergeDir = function() {
    return {
        LTR: 0,
        RTL: 1
    }
}()

function ltrMerge() { view.merge(mergeDir.LTR) }
function rtlMerge() { view.merge(mergeDir.RTL) }
function ltrMergeAll() { view.mergeAll(mergeDir.LTR) }
function rtlMergeAll() { view.mergeAll(mergeDir.RTL) }


function addUndo(cmd) {
    undolist.push(cmd)
    if (undolist.length == 1) { view.enableUndo(true) }
}
function addRedo(cmd) {
    redolist.push(cmd)
    if (redolist.length == 1) { view.enableRedo(true) }
}
function popUndo() {
    if (undolist.length == 1) { view.enableUndo(false) }
    return undolist.pop()
}
function popRedo() {
    if (redolist.length == 1) { view.enableRedo(false) }
    return redolist.pop()
}
function clrUndo() {
    undolist.clear()
    view.enableUndo(false)
}
function clrRedo() {
    redolist.clear()
    view.enableRedo(false)
}

function undo() {
    if (undolist.length == 0) { return }
    var cmd = popUndo()
    cmd.undo()
    addRedo(cmd)
    view.updateMergeAll()
}
function redo() {
    if (redolist.length == 0) { return }
    var cmd = popRedo()
    cmd.exec()
    addUndo(cmd)
    view.updateMergeAll()
}

function LtrMergeCmd(line, dst) {
    this.line = line
    this.text = dst.text
    this.html = dst.html

    this.exec = function() {
        view.ltrMergeLine(this.line)
        --diffnum
    }
    this.undo = function() {
        view.ltrMergeLineUndo(this.line, this.text, this.html)
        ++diffnum
    }
}

function RtlMergeCmd(line, dst) {
    this.line = line
    this.text = dst.text
    this.html = dst.html

    this.exec = function() {
        view.rtlMergeLine(this.line)
        --diffnum
    }
    this.undo = function() {
        view.rtlMergeLineUndo(this.line, this.text, this.html)
        ++diffnum
    }
}

function MergeAllCmd() {
    this.cmds = []

    this.push = function(cmd) {
        this.cmds.push(cmd)
    }
    this.exec = function() {
        for (var i = 0; i < this.cmds.length; ++i) {
            this.cmds[i].exec()
        }
    }
    this.undo = function() {
        for (var i = 0; i < this.cmds.length; ++i) {
            this.cmds[i].undo()
        }
    }
}


function View() {
    this.outputDiv = $('outputDiv')
    this.ltrBtn = $('ltrBtn')
    this.rtlBtn = $('rtlBtn')
    this.ltrAllBtn = $('ltrAllBtn')
    this.rtlAllBtn = $('rtlAllBtn')
    this.undoBtn = $('undoBtn')
    this.redoBtn = $('redoBtn')
    this.textDiv = $('textDiv')
    this.textBtn = $('textBtn')
    this.textX = $('textX')
    this.textY = $('textY')

    this.linesX = []
    this.linesY = []

    this.outputX = null
    this.outputY = null

    this.selectedLine = -1

    this.reset = function() {
        this.outputX = $('outputX')
        this.outputY = $('outputY')
        if (this.outputX != null) { this.outputDiv.removeChild(this.outputX) }
        if (this.outputY != null) { this.outputDiv.removeChild(this.outputY) }

        this.outputX = document.createElement('div')
        this.outputY = document.createElement('div')
        this.outputX.id = 'outputX'
        this.outputY.id = 'outputY'
        this.outputX.className = 'outputX'
        this.outputY.className = 'outputY'
        this.outputDiv.appendChild(this.outputX)
        this.outputDiv.appendChild(this.outputY)

        this.linesX.clear()
        this.linesY.clear()
        clrUndo()
        clrRedo()

        this.selectedLine = -1
    }

    this.show = function() {
        this.reset()
        this.enableMerge(false)
        this.updateMergeAll()

        for (var i = 0; i < resultX.length; ++i) {
            var lineX = resultX[i]
            var lineY = resultY[i]

            var lx = document.createElement('div')
            var ly = document.createElement('div')

            lx.id = 'idx' + i
            ly.id = 'idy' + i
            lx.className = 'resultLine'
            ly.className = 'resultLine'
            lx.style.background = colors[lineX.type]
            ly.style.background = colors[lineY.type]
            lx.innerHTML = lineX.html
            ly.innerHTML = lineY.html

            if (lineX.type == type.DIF || lineX.type == type.ABS) {
                lx.onclick = select
                ly.onclick = select
            } else {
                lx.onclick = unselect
                ly.onclick = unselect
            }

            this.linesX.push(lx)
            this.linesY.push(ly)
            this.outputX.appendChild(lx)
            this.outputY.appendChild(ly)
        }
    }


    this.getTextX = function() {
        return this.textX.value
    }
    this.getTextY = function() {
        return this.textY.value
    }

    this.getMergedX = function() {
        if (resultX.length == 0) { return this.textX.value }
        var text = []
        for (var i = 0; i < resultX.length; ++i) {
            if (resultX[i].type != type.ABS
                && this.linesX[i].style.display != 'none') {
                text.push(resultX[i].text);
            }
        }
        return text.join(LF)
    }
    this.getMergedY = function() {
        if (resultY.length == 0) { return this.textY.value }
        var text = []
        for (var i = 0; i < resultY.length; ++i) {
            if (resultY[i].type != type.ABS
                && this.linesY[i].style.display != 'none') {
                text.push(resultY[i].text);
            }
        }
        return text.join(LF)
    }

    this.enableTextBtn = function(enabled) {
        enable(this.textBtn, enabled)
    }

    this.showText = function() {
        this.textX.value = this.getMergedX()
        this.textY.value = this.getMergedY()
        this.textDiv.style.display = 'block'
        this.enableTextBtn(false)
    }
    this.hideText = function() {
        this.textDiv.style.display = 'none'
        this.enableTextBtn(true)
    }


    this.enableUndo = function(enabled) {
        enable(this.undoBtn, enabled)
    }
    this.enableRedo = function(enabled) {
        enable(this.redoBtn, enabled)
    }


    this.selectLine = function(pre, cur) {
        if (pre != -1) {
            this.linesX[pre].style.background = colors[resultX[pre].type]
            this.linesY[pre].style.background = colors[resultY[pre].type]
        }
        if (cur != -1) {
            this.linesX[cur].style.background = colors[type.SEL]
            this.linesY[cur].style.background = colors[type.SEL]
        }
        this.selectedLine = cur
        this.enableMerge(cur != -1)
    }

    this.select = function(line) {
        if (this.selectedLine == line) { return }
        this.selectLine(this.selectedLine, line)
    }

    this.unselect = function(line) {
        if (this.selectedLine == -1 || this.selectedLine == line) { return }
        this.selectLine(this.selectedLine, -1)
    }


    this.enableMerge = function(enabled) {
        enable(this.ltrBtn, enabled)
        enable(this.rtlBtn, enabled)
    }
    this.enableMergeAll = function(enabled) {
        enable(this.ltrAllBtn, enabled)
        enable(this.rtlAllBtn, enabled)
    }
    this.updateMergeAll = function() {
        this.enableMergeAll(diffnum > 0)
    }

    this.mergeLine = function(src, dst, vsrc, vdst) {
        dst.text = src.text
        dst.html = src.html
        vdst.innerHTML = vsrc.innerHTML

        if (src.type == type.ABS) {
            vsrc.style.display = 'none'
            vdst.style.display = 'none'
        } else {
            src.type = type.IDT
            dst.type = dst.type == type.ABS ? type.ADD : type.IDT
            vsrc.onclick = unselect
            vdst.onclick = unselect
        }

        vsrc.style.background = colors[src.type]
        vdst.style.background = colors[dst.type]
    }

    this.mergeLineUndo = function(src, dst, vsrc, vdst, bakText, bakHtml) {
        if (src.type == type.ABS) {
            vsrc.style.display = 'block'
            vdst.style.display = 'block'
        } else {
            src.type = type.DIF
            dst.type = dst.type == type.ADD ? type.ABS: type.DIF
            dst.text = bakText
            dst.html = bakHtml
            vdst.innerHTML = bakHtml
            vsrc.onclick = select
            vdst.onclick = select
        }

        vsrc.style.background = colors[src.type]
        vdst.style.background = colors[dst.type]
    }

    this.ltrMergeLine = function(i) {
        this.mergeLine(resultX[i], resultY[i], this.linesX[i], this.linesY[i])
    }

    this.rtlMergeLine = function(i) {
        this.mergeLine(resultY[i], resultX[i], this.linesY[i], this.linesX[i])
    }

    this.ltrMergeLineUndo = function(i, bakText, bakHtml) {
        this.mergeLineUndo(resultX[i], resultY[i], this.linesX[i], this.linesY[i],
            bakText, bakHtml)
    }

    this.rtlMergeLineUndo = function(i, bakText, bakHtml) {
        this.mergeLineUndo(resultY[i], resultX[i], this.linesY[i], this.linesX[i],
            bakText, bakHtml)
    }

    this.merge = function(dir) {
        var mergeCmd = dir == mergeDir.LTR ? LtrMergeCmd : RtlMergeCmd
        var result = dir == mergeDir.LTR ? resultY : resultX

        var i = this.selectedLine;
        var cmd = new mergeCmd(i, result[i])
        cmd.exec()
        addUndo(cmd)
        clrRedo()
        this.unselect()
        this.updateMergeAll()
    }

    this.mergeAll = function(dir) {
        var mergeCmd = dir == mergeDir.LTR ? LtrMergeCmd : RtlMergeCmd
        var result = dir == mergeDir.LTR ? resultY : resultX

        var allCmd = new MergeAllCmd()
        for (var i = 0; i < result.length; ++i) {
            var dst = result[i]
            if (dst.type != type.IDT && dst.type != type.ADD
                && this.linesX[i].style.display != 'none') {
                allCmd.push(new mergeCmd(i, dst))
            }
        }
        if (allCmd.cmds.length > 0) {
            allCmd.exec()
            addUndo(allCmd)
            clrRedo()
            this.unselect()
            this.updateMergeAll()
        }
    }
}


var CharMap = {
    ' ' : '<span class="mark">&middot;</span>',
    '<' : '&lt;',
    '>' : '&gt;',
    '&' : '&amp;',
    '\t': '<span class="mark">&Tau;&nbsp;&nbsp;&nbsp;</span>' // TODO: tab size ...
}

function Line(text, type) {
    this.text = text
    this.type = type
    this.html = ''

    this.fixChar = function(c) {
        var fc = CharMap[c]
        if (fc == null) { fc = c }
        return fc
    }

    if (this.text.length > 0) {
        var t = []
        for (var i = 0; i < this.text.length; ++i) {
            t.push(this.fixChar(this.text.charAt(i)))
        }
        this.html = t.join('')
    }
    this.html += '<span class="mark">&crarr;</span>'
}

function diff(X, Y, R, P, Q) {

    var M = X.length;
    var N = Y.length;

    resultX = [];
    resultY = [];

    // matching items in the beginning
    for (var i = 0; i < P; ++i) {
        resultX.push(new Line(X[i], type.IDT));
        resultY.push(new Line(Y[i], type.IDT));
    }

    var px = P;
    var py = P;

    for (var i = 0; i < R.length;) {
        var cx = R[i++];
        var cy = R[i++];

        // different
        for (var j = px; j < cx; ++j) {
            resultX.push(new Line(X[j], type.DIF));
        }
        for (var j = py; j < cy; ++j) {
            resultY.push(new Line(Y[j], type.DIF));
        }

        var dx = cx - px;
        var dy = cy - py;
        for (var j = dx; j < dy; ++j) {
            resultX.push(new Line('', type.ABS));
        }
        for (var j = dy; j < dx; ++j) {
            resultY.push(new Line('', type.ABS));
        }

        // identical
        resultX.push(new Line(X[cx], type.IDT));
        resultY.push(new Line(Y[cy], type.IDT));

        px = cx + 1;
        py = cy + 1;
    }

    // different
    for (var i = px; i < M - Q; ++i) {
        resultX.push(new Line(X[i], type.DIF));
    }
    for (var i = py; i < N - Q; ++i) {
        resultY.push(new Line(Y[i], type.DIF));
    }

    // absent
    var lx = M + P - px;
    var ly = N + P - py;
    for (var i = 0; i < ly - lx; ++i) {
        resultX.push(new Line('', type.ABS));
    }
    for (var i = 0; i < lx - ly; ++i) {
        resultY.push(new Line('', type.ABS));
    }

    // matching items in the end
    for (var i = Q; i > 0; --i) {
        resultX.push(new Line(X[M-i], type.IDT));
        resultY.push(new Line(Y[N-i], type.IDT))
    }
//    alert(resultX + "\r\n" + resultY);
}

function diff_2(X, Y, Rx, Ry, ignore) {
    resultX = [];
    resultY = [];

    var px = 0; var py = 0;

    for (var i = 0; i < Rx.length; ++i) {
        var cx = Rx[i]; var cy = Ry[i];

        /*
        while (px < cx && py < cy) {
            if (ignore(X[px]) && ignore(Y[py])) {
                resultX.push(new Line(X[px], type.IDT));
                resultY.push(new Line(Y[py], type.IDT));
                ++px; ++py;
            }
            else
                break;
        }

        var k = 0;
        while (cx - k > px && cy -k > py) {
            if (ignore(X[cx-k]) && ignore(Y[cy-k])) {
                ++k;
            }
            else
                break;
        }

        while (px < cx - k && py < cy - k) {
            resultX.push(new Line(X[px], type.DIF));
            resultY.push(new Line(Y[py], type.DIF));
            ++px; ++py;
        }
        while (px < cx - k) {
            resultY.push(new Line('', type.ABS));
            ++px;
        }
        while (py < cy - k) {
            resultX.push(new Line('', type.ABS));
            ++py;
        }*/

        while (px <= cx && py <= cy) {
            resultX.push(new Line(X[px], type.IDT));
            resultY.push(new Line(Y[py], type.IDT));
            ++px; ++py;
        }
    }
}
        

/*
function Diff(textX, textY) {

    this.X = textX.split(LF)
    this.Y = textY.split(LF)

    this.M = this.X.length
    this.N = this.Y.length

    this.P = 0
    this.Q = 0

    this.L = null
    this.R = null

    this.offsetX = []
    this.offsetY = []

    this.resultX = []
    this.resultY = []

    this.getDiffNum = function() {
        return this.resultX.length - this.offsetX.length - this.P - this.Q
    }

    this.reduceProblemSet = function() {
        // trim off the matching items at the beginning
        while (this.P < this.M && this.P < this.N
                && this.X[this.P] == this.Y[this.P]) {
            ++this.P
        }
        // trim off the matching items at the end
        while (this.Q < this.M - this.P && this.Q < this.N - this.P
                && this.X[this.M-this.Q-1] == this.Y[this.N-this.Q-1]) {
            ++this.Q
        }
    }

    this.createMatrix = function() {
        var M1 = this.M - this.P - this.Q
        var N1 = this.N - this.P - this.Q

        this.L = new Array(M1 + 1)

        for (var i = 0; i <= M1; ++i) {
            this.L[i] = new Array(N1 + 1)
                this.L[i][N1] = 0
        }
        for (var i = 0; i <= N1; ++i) {
            this.L[M1][i] = 0
        }

        for (var i = M1 - 1; i >= 0; --i) {
            for (var j = N1 - 1; j >= 0; --j) {
                if (this.X[this.P+i] == this.Y[this.P+j])
                    this.L[i][j] = this.L[i+1][j+1] + 1
                else
                    this.L[i][j] = Math.max(this.L[i+1][j], this.L[i][j+1])
            }
        }

        return this.L[0][0]
    }

    this.trackBackAll = function(i, j) {
        if (i == this.M-this.P-this.Q || j == this.N-this.P-this.Q) {
            return [[]]
        }

        if (this.X[this.P+i] == this.Y[this.P+j]) {
            var R = this.trackBackAll(i+1, j+1)
                for (var r = 0; r < R.length; ++r) {
                    R[r].push(this.P+i)
                        R[r].push(this.P+j)
                }
            return R
        }
        else {
            var R = []
            if (this.L[i+1][j] >= this.L[i][j+1])
                R = this.trackBackAll(i+1, j)
            if (this.L[i+1][j] <= this.L[i][j+1])
                R = this.trackBackAll(i, j+1).unite(R, isEqualArray)
                return R
        }
    }

    this.getEmpty = function(i) {
        var offset = this.R[i]
        var num = 0
        for (var j = 0; j < offset.length - 1; j += 2) {
            var line = this.X[offset[j]]
	    // TODO
            if (line == '' || line == CR) { ++num }
        }
        return num
    }

    // get the most prefered LCS
    // the lcs with least empty lines is most prefered
    this.getPrefered = function() {
        var p = 0
        var min = this.getEmpty(0)
        for (var i = 1; i < this.R.length; ++i) {
            var num = this.getEmpty(i)
            if (num < min) {
                min = num
                p = i
            }
        }

        var prefered = this.R[p]
        for (var i = prefered.length - 1; i > 0; i -= 2) {
            this.offsetX.push(prefered[i-1])
            this.offsetY.push(prefered[i])
        }
    }

    this.getResult = function() {
        // matching items in the beginning
        for (var i = 0; i < this.P; ++i) {
            this.resultX.push(new Line(this.X[i], type.IDT))
            this.resultY.push(new Line(this.Y[i], type.IDT))
        }

        var px = this.P
        var py = this.P

        for (var i = 0; i < this.offsetX.length; ++i) {
            var cx = this.offsetX[i]
            var cy = this.offsetY[i]

            // different
            for (var j = px; j < cx; ++j) {
                this.resultX.push(new Line(this.X[j], type.DIF))
            }
            for (var j = py; j < cy; ++j) {
                this.resultY.push(new Line(this.Y[j], type.DIF))
            }

            var dx = cx - px
            var dy = cy - py
            for (var j = dx; j < dy; ++j) {
                this.resultX.push(new Line('', type.ABS))
            }
            for (var j = dy; j < dx; ++j) {
                this.resultY.push(new Line('', type.ABS))
            }

            // identical
            this.resultX.push(new Line(this.X[cx], type.IDT))
            this.resultY.push(new Line(this.Y[cy], type.IDT))

            px = cx + 1
            py = cy + 1
        }

        // different
        for (var i = px; i < this.M-this.Q; ++i) {
            this.resultX.push(new Line(this.X[i], type.DIF))
        }
        for (var i = py; i < this.N-this.Q; ++i) {
            this.resultY.push(new Line(this.Y[i], type.DIF))
        }

        // absent
        var lx = this.M + this.P - px
        var ly = this.N + this.P - py
        for (var i = 0; i < ly - lx; ++i) {
            this.resultX.push(new Line('', type.ABS))
        }
        for (var i = 0; i < lx - ly; ++i) {
            this.resultY.push(new Line('', type.ABS))
        }

        // matching items in the end
        for (var i = this.Q; i > 0; --i) {
            this.resultX.push(new Line(this.X[this.M-i], type.IDT))
            this.resultY.push(new Line(this.Y[this.N-i], type.IDT))
        }
    }

    this.start = function() {
        this.reduceProblemSet()
        this.createMatrix()
        this.R = this.trackBackAll(0, 0)
        this.getPrefered()
        this.getResult()
    }
}
*/

