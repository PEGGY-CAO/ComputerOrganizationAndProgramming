#!/bin/bash
e=0; [[ $(sha512sum my_malloc.h | cut -d " " -f 1) == 9e5423d0104b2649e09a3dfc46e9aae6353f0e7f9781fc5e32828dd2cb102f1a14ed5d5e61cf52be91605d892878df99af2185ecd58a5492c41d10c080b37b2e ]] || { printf 'error: my_malloc.h was modified! re-download it and try again\n'>&2; e=1; }; exit $e
