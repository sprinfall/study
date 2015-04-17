#!/bin/bash
# burn_ape.sh - 转换 APE 到 WAV 并刻录成 CD
# 需要以下软件：
# mac: http://supermmx.org/linux/mac/
# cuetools & cdrdao
# 最好以 root 账号运行, 这样 cdrdao 就可以实时调度了

# 帮助
if [ -n $1 ] && [ "$1" = -h ]; then
    echo -e "用法: ${0##*/} [文件名]\n参数可选，为不带后缀的 APE 和 CUE 文件名，假定它们文件名相同。"
    exit 0
fi

# APE 和 CUE 不带后缀的文件名，缺省为 CDImage
NAME=${1:-CDImage}

if ! [ -a ${NAME}.ape ] || ! [ -a ${NAME}.cue ]; then
    echo 错误：指定的 APE 或 CUE 文件不存在: ${NAME}.ape, ${NAME}.cue
    exit 1
fi

if ! [ -a ${NAME}.wav ]; then
    # 如下两句等同于 if ! mac ${NAME}.ape ${NAME}.wav -d
    mac ${NAME}.ape ${NAME}.wav -d
    if [ $? -ne 0 ]; then
        echo 错误：使用 mac 转换 APE 到 WAV 失败
        exit 1
    fi
fi

if ! [ -a ${NAME}.toc ]; then
    # 转换 cue 为 toc
    cueconvert -i cue -o toc ${NAME}.cue ${NAME}.toc
    if [ $? -ne 0 ]; then
        echo 错误：使用 cueconvert 转换 cue 到 toc 失败
        exit 1
    fi
fi

# 把 .toc 中的所有 .ape 替换成 .wav
sed -i -e 's/.ape/.wav/g' ${NAME}.toc

# TODO
overburn= #--overburn

# 使用 cdrdao 刻录
cdrdao write $overburn --speed 4 --eject -v 2 --device /dev/cdrom ${NAME}.toc
if [ $? -ne 0 ]; then
    echo 错误：使用 cdrdao 刻录失败
    exit 1
fi

# 清理
rm ${NAME}.wav ${NAME}.toc

echo 刻录成功
exit 0

