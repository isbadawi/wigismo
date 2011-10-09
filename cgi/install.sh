#!/bin/bash

CGIBIN=$HOME/public_html/cgi-bin
cp 08rps.py runtime.py $CGIBIN
chmod 755 $CGIBIN/08rps.py $CGIBIN/runtime.py
