#!/bin/sh

cat << EOT
---------------------------------------
to bootstrap this package you will need:
autoconf 2.5
automake >=1.6
---------------------------------------
EOT

version_list() {
	for i in "$@";do
		echo "-$i"
	done
	echo "@NOVERSION@"
	echo "-not-found"
}

check_found() {
	return `test ! "$1" = "-not-found"`
}

# bool check_program(char *name, char *version...)
# display found program name
check_program() {
	local program version fprogram
	program=$1
	shift
	for version in `version_list "$@"`;do
		if [ ! "${version}" = "@NOVERSION@" ]; then
			fprogram="${program}${version}"
		else
			fprogram="${program}"
		fi
		fprogram=`type -p ${fprogram} 2> /dev/null`
		if test $? == 0; then
			if test -e ${fprogram} -a -x ${fprogram}; then
				echo ${fprogram}
				break
			fi
		fi
	done
	return `check_found ${version}`
}

declare_indexes() {
# start at 1 because array[non_declared_var] eq array[0]
	local icount=1
	for i in "$@";do
		eval $i=$icount
		icount=$[ $icount + 1]
	done
}

check_all_programs() {
	for program in "$@";do
		name=${program}
		program=\$`echo -n ${program}`
		program=`eval echo ${program}`
		if [ -z "${program}" ]; then
			eerror "bad bootstrap file"
			eerror "${HILITE}${name} must be defined using declare_indexes${NORMAL}"
			eerror "progr_name[\${${name}}] must be defined"
			exit 0
		fi
		if [ -z "${progr_name[${program}]}" ];then
			eerror "bad bootstrap file"
                        eerror "${name} must be defined using declare_indexes"
                        eerror "${HILITE}progr_name[\${${name}}] must be defined${NORMAL}"
                        exit 0
		fi
		echo -n "looking for ${HILITE}${progr_name[${program}]}${NORMAL}..."
	        fprogram=`check_program ${progr_name[${program}]} ${check_versions[${program}]}`
	        if test $? == 0; then
	                eval ${prefix}${progr_name[${program}]}${suffix}=${fprogram}
			echo  " found as `basename ${fprogram}`"
	        else
	                echo  " not found(version checked : ${check_versions[${program}]})"
			exit 1
	        fi
	done							
}


############
# begining #
############

ebegin() {
        echo -n "$@..."
}

eend() {
	if [ $? == 0 ];then
		echo " ok "
	else
	        echo " failed "
		exit 1
	fi
}

ewarn() {
        echo "? $@"
}

eerror() {
        echo "! $@"
}
										

[ -f /etc/init.d/functions.sh ] && source /etc/init.d/functions.sh
esyslog() {
 	:	#prevent syslog use
}

export WANT_AUTOCONF_2_5=1
export WANT_AUTOMAKE=1.7

PROGRAMS="aclocal autoheader libtoolize automake autoconf"

declare_indexes ${PROGRAMS}

progr_name[$aclocal]=aclocal
check_versions[$aclocal]="1.7 1.6"

progr_name[$autoheader]=autoheader
check_versions[$autoheader]="2.5x 2.59 2.58 2.57"

progr_name[$libtoolize]=libtoolize
check_versions[$libtoolize]=""

progr_name[$automake]=automake
check_versions[$automake]="1.7 1.6"

progr_name[$autoconf]=autoconf
check_versions[$autoconf]="2.5x 2.59 2.58 2.57"

check_all_programs  ${PROGRAMS}

echo "[ $libtoolize ]" > bootstrap.log
ebegin "running libtoolize"
$libtoolize
eend $?
echo "[ $aclocal ]" >> bootstrap.log
ebegin "Importing m4 macro"
$aclocal -I m4 >> bootstrap.log 2>&1
eend $?

echo "[ $autoheader ]" >> bootstrap.log
ebegin "Creating config.h.in"
$autoheader >> bootstrap.log 2>&1
eend $?

echo "[ $automake ]" >> bootstrap.log
ebegin "Running automake"
$automake -a >> bootstrap.log 2>&1
eend $?

echo "[ $autoconf ]" >> bootstrap.log
ebegin "Creating configure"
$autoconf >> bootstrap.log 2>&1
eend $?
