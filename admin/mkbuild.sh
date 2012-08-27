#!/bin/bash

PATH=$PATH:/bin:/usr/bin:/usr/local/bin

SCP=${COPASI_SCP:-scp}
AWK=${COPASI_AWK:-gawk}
SORT=${COPASI_SORT:-sort}
PACKAGE=${COPASI_PACKAGE:-$1}

if [ x"$COPASI_UPLOAD" != x ]; then
  function UPLOAD () {
    SRC=""
    while [ x"$2" != x ]; do
      SRC="$SRC $1"
      shift
    done
    echo ${SCP} ${SRC} ${COPASI_UPLOAD}/$1
    ${SCP} ${SRC} ${COPASI_UPLOAD}/$1
  }
else
  function UPLOAD () {
    echo "Skipping upload (environment variable COPASI_UPLOAD not set)."
  }
fi

pushd ../..

if [ x"$#" = x1 ]; then
  major=`${AWK} -- '$2 ~ "VERSION_MAJOR" {print $3}' copasi/CopasiVersion.h`
  minor=`${AWK} -- '$2 ~ "VERSION_MINOR" {print $3}' copasi/CopasiVersion.h`
  build=`${AWK} -- '$2 ~ "VERSION_BUILD" {print $3}' copasi/CopasiVersion.h`
  comment=`${AWK} -- '$3 ~ "VERSION_COMMENT" {print $4}' copasi/CopasiVersion.h | ${SORT} -u`
  buildname=${build}

  if [ x"${comment}" = x\"Snapshot\" ]; then
    buildname=${major}
    [ ${#minor} = 1 ] && buildname=${buildname}0
    buildname=${buildname}${minor}
    [ ${#build} = 1 ] && buildname=${buildname}0
    buildname=${buildname}${build}
  fi

  license="US"

  rm Copasi-${buildname}-${PACKAGE}.*

  case x"$1" in 
  xWIN32)
    . admin/mkWIN32.sh
    ;;

  xDarwin)
    . admin/mkDarwin.sh
    ;;

  xLinux|xSunOS)
    . admin/mkLinux.sh
    ;;
  esac

  UPLOAD Copasi-${buildname}-${PACKAGE}.* \
    $license

else
  echo usage: mkbuild.sh BUILD_OS
fi
popd
