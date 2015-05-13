#!/bin/sh
mount -t nfs 192.168.6.10:/home/root/Project /mnt/nfs -o nolock,proto=tcp,nfsvers=3
