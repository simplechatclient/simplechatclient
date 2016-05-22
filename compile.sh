#!/bin/bash

make_install()
{
	if [ -e /etc/lsb-release ]; then
		if [ "$(lsb_release -i | awk '{print $3}')" = "Ubuntu" ]; then
			sudo make install
		else
			su -c 'make install'
		fi
	else
		su -c 'make install'
	fi
}

cmake . -DCMAKE_INSTALL_PREFIX=/usr && make -j `nproc` && make_install
