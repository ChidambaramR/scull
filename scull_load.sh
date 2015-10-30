#!/bin/bash

set +e

module="scull"
device="scull"

if grep -q '^staff:' /etc/group; then
	group="staff"
else
	group="wheel"
fi

# Remove the module if already existing
/sbin/rmmod $module
# Install the module
/sbin/insmod ./$module.ko $* || exit 1

major=$(awk -v module="$module" '{if ($2==module) {print $1}}' /proc/devices)


# Remove any device files already existing
rm -f /dev/${device}[0-3]

mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3
