#!/usr/bin/env python
# coding=utf-8

from subprocess import check_output, call
import sys

if __name__ == '__main__':
    REMOTES_CMD = ['git', 'remote', '-v', '2>nul']
    FETCH_UPSTREAM_CMD = ['git', 'fetch', 'upstream', '2>nul']
    CHECKOUT_MASTER_CMD = ['git', 'checkout', 'master', '2>nul']
    MERGE_UPSTREAM_CMD = ['git', 'merge', 'upstream/master', '2>nul']
	
    print("\nREMOTES:\t")
    call(REMOTES_CMD)
    print("--------------------------------------------------------------------------------")

    print("\nFETCH:")
    call(FETCH_UPSTREAM_CMD)
    print("--------------------------------------------------------------------------------")

    call(CHECKOUT_MASTER_CMD)

    print("\nMERGE UPSTREAM MASTER:")
    call(MERGE_UPSTREAM_CMD)
    print("--------------------------------------------------------------------------------")