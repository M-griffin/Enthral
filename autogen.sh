                                                  #!/bin/bash
have_automake=false
have_aclocal=false
have_autoheader=false
have_autoconf=false

echo "This script will detect (GNU Automake),"
echo "Generate your multi-platform configure script."
echo ""

if automake --version < /dev/null > /dev/null 2>&1 ; then
        automake_version=`automake --version | grep 'automake (GNU automake)' | sed 's/^[^0-9]*\(.*\)/\1/'`
        case $automake_version in
           1.2*|1.3*|1.4|1.4*|1.5*|1.6*|1.7*|1.8*)
                ;;
           *)
                have_automake=true
                ;;
        esac
fi

if $have_automake ; then
        AUTOMAKE="automake"
else
        if automake-1.9 --version < /dev/null > /dev/null 2>&1 ; then
                AUTOMAKE="automake-1.9"
        elif automake19 --version < /dev/null > /dev/null 2>&1 ; then
        AUTOMAKE="automake19"
    else
                echo "automake missing or too old. This requires atleast automake-1.9"
                exit 1
        fi
fi


if aclocal --version < /dev/null > /dev/null 2>&1 ; then
    aclocal_version=`aclocal --version | grep 'automake (GNU automake)' | sed 's/^[^0-9]*\(.*\)/\1/'`
    case aclocal_version in
       1.2*|1.3*|1.4|1.4*|1.5*|1.6*|1.7*|1.8*)
        ;;
       *)
        have_aclocal=true
        ;;
    esac
fi

if $have_aclocal ; then
    ACLOCAL="aclocal"
else
    if aclocal-1.9 --version < /dev/null > /dev/null 2>&1 ; then
        ACLOCAL="aclocal-1.9"
    elif aclocal19 --version < /dev/null > /dev/null 2>&1 ; then
        ACLOCAL="aclocal19"
    else
        echo "aclocal missing or too old. This requires atleast aclocal-1.9"
        exit 1
    fi
fi

echo running $ACLOCAL...
$ACLOCAL --force
if test "$?" != "0"; then
        echo aclocal failed, stopping.
        exit 2
fi

# Uses Normal name, no version number appended.
echo running libtoolize...
libtoolize --force
if test "$?" != "0"; then
        echo libtoolize failed, stopping.
        exit 3
fi

# Detect Standard Autoheader
if autoheader --version < /dev/null > /dev/null 2>&1 ; then
    autoheader_version=`autoheader --version | grep 'Autoconf version' | sed 's/^[^0-9]*\(.*\)/\1/'`
    case autoheader_version in
       1.2*|1.3*|1.4|1.4*|1.5*|1.6*|1.7*|1.8*)
        ;;
       *)
        have_autoheader=true
        ;;
    esac
fi

# Detect Latest and Greatest Version first, then cycle down.
if $have_autoheader ; then
    AUTOHEADER="autoheader"
else
    if autoheader --version < /dev/null > /dev/null 2>&1 ; then
        AUTOHEADER="autoheader"
    elif autoheader-2.62 --version < /dev/null > /dev/null 2>&1 ; then
        AUTOHEADER="autoheader-2.62"
    elif autoheader259 --version < /dev/null > /dev/null 2>&1 ; then
        AUTOHEADER="autoheader259"
    elif autoheader213 --version < /dev/null > /dev/null 2>&1 ; then
        AUTOHEADER="autoheader213"
    else
        echo "autoheader missing."
        exit 1
    fi
fi

echo running autoheader...
$AUTOHEADER --force
if test "$?" != "0"; then
        echo autoheader failed, stopping.
        exit 4
fi

# Make the Makefile(s)
echo running $AUTOMAKE...
$AUTOMAKE -a --foreign -f
if test "$?" != "0"; then
        echo automake failed, stopping.
        exit 5
fi

# Detect Standard Autoheader
if autoconf --version < /dev/null > /dev/null 2>&1 ; then
    autoconf_version=`autoconf --version | grep 'Autoconf version' | sed 's/^[^0-9]*\(.*\)/\1/'`
    case autoconf_version in
       1.2*|1.3*|1.4|1.4*|1.5*|1.6*|1.7*|1.8*)
        ;;
       *)
        have_autoconf=true
        ;;
    esac
fi

# Detect Latest and Greatest Version first, then cycle down.
if $have_autoconf ; then
    AUTOCONF="autoconf"
else
    if autoconf --version < /dev/null > /dev/null 2>&1 ; then
        AUTOCONF="autoconf"
    elif autoheader-2.62 --version < /dev/null > /dev/null 2>&1 ; then
        AUTOCONF="autoconf-2.62"
    elif autoheader259 --version < /dev/null > /dev/null 2>&1 ; then
        AUTOCONF="autoconf259"
    elif autoheader213 --version < /dev/null > /dev/null 2>&1 ; then
        AUTOCONF="autoconf213"
    else
        echo "autoconf missing."
        exit 1
    fi
fi


echo running autoconf...
$AUTOCONF --force
if test "$?" != "0"; then
        echo autoconf failed, stopping.
        exit 6
fi

echo DONE!
echo
echo [You can ignore all warnings]. If no errors occured, run ./configure then make to compile.
exit 0

#autogen.sh generates:
#       aclocal.m4 Makefile.in config.guess config.sub ltmain.sh
#
#configure generates:
#       config.status libtool Makefile.in
